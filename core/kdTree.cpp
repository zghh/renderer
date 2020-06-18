#include "kdTree.h"

Node::Node()
{
    this->left = NULL;
    this->right = NULL;
    for (int i = 0; i < 3; i++)
        this->coordinate[i][0] = INF, this->coordinate[i][1] = -INF;
}

Node::Node(double coordinate[][2])
{
    this->left = NULL;
    this->right = NULL;
    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 2; j++)
            this->coordinate[i][j] = coordinate[i][j];
}

Node::~Node()
{
}

void NodeIndex::Init(Parser &parser, int index)
{
    for (int i = 0; i < 3; i++)
        this->coordinate[i][0] = INF, this->coordinate[i][1] = -INF;
    Face face = parser.GetFaces()[index];
    for (int i = 0, len = face.nPoint; i < len; i++)
    {
        Point p = parser.GetPoint(face.points[i].pointIndex);
        double *pAddr = &p.x;
        for (int j = 0; j < 3; j++)
        {
            this->coordinate[j][0] = min(this->coordinate[j][0], pAddr[j]);
            this->coordinate[j][1] = max(this->coordinate[j][1], pAddr[j]);
        }
    }
    this->index = index;
}

bool NodeIndex::operator<(const NodeIndex &x) const
{
    int d = this->d;
    if (eq(this->coordinate[d][1], x.coordinate[d][1]))
        return this->coordinate[d][0] < x.coordinate[d][0];
    else
        return this->coordinate[d][1] < x.coordinate[d][1];
}

KDTree::KDTree(Parser &parser)
{
    this->parser = &parser;
    int nFace = parser.GetNFace();
    NodeIndex *index = new NodeIndex[nFace];
    for (int i = 0; i < nFace; i++)
        index[i].Init(parser, i);
    this->root = new Node();
    this->Build(parser, index, 0, nFace - 1, this->root, 0);
    delete[] index;
}

KDTree::~KDTree()
{
    this->Delete(this->root);
}

bool KDTree::IsHit(const Point &o, const Point &d, Point &p, Point &n, Texture &texture, Material *&material, int num, double tMax)
{
    this->t[num] = INF;
    this->isHit[num] = false;
    this->IsHit(this->root, o, d, num);
    p = this->p[num];
    n = this->n[num];
    texture = this->texture[num];
    material = this->material[num];
    return this->isHit[num] && this->t[num] + EPS < tMax;
}

void KDTree::Build(Parser &parser, NodeIndex *index, int left, int right, Node *node, int depth)
{
    Face *faces = parser.GetFaces();
    for (int i = left; i <= right; i++)
    {
        Face face = faces[index[i].index];
        for (int j = 0, end = face.nPoint; j < end; j++)
        {
            Point p = parser.GetPoint(face.points[j].pointIndex);
            double *pAddr = &p.x;
            for (int k = 0; k < 3; k++)
            {
                node->coordinate[k][0] = min(node->coordinate[k][0], pAddr[k]);
                node->coordinate[k][1] = max(node->coordinate[k][1], pAddr[k]);
            }
        }
        index[i].d = depth % 3;
    }
    // printf("%d %d %.3f %.3f %.3f %.3f %.3f %.3f\n", left, right, node->coordinate[0][0], node->coordinate[0][1],
    //        node->coordinate[1][0], node->coordinate[1][1],
    //        node->coordinate[2][0], node->coordinate[2][1]);

    if (left == right)
    {
        node->face = faces[index[left].index];
        return;
    }

    int mid = (left + right) >> 1;
    nth_element(index + left, index + mid, index + right + 1);

    if (left <= mid)
    {
        Node *leftNode = new Node();
        node->left = leftNode;
        this->Build(parser, index, left, mid, leftNode, depth + 1);
    }
    if (mid + 1 <= right)
    {
        Node *rightNode = new Node();
        node->right = rightNode;
        this->Build(parser, index, mid + 1, right, rightNode, depth + 1);
    }
}

void KDTree::IsHit(Node *node, const Point &o, const Point &d, int num)
{
    if (node->left == NULL && node->right == NULL)
    {
        this->HitFace(&node->face, o, d, num);
        return;
    }
    double tLeft = INF, tRight = INF;
    if (node->left)
    {
        this->HitBox(node->left, o, d, tLeft);
    }
    if (node->right)
    {
        this->HitBox(node->right, o, d, tRight);
    }
    if (tLeft < tRight)
    {
        if (tLeft < this->t[num])
            this->IsHit(node->left, o, d, num);
        if (tRight < this->t[num])
            this->IsHit(node->right, o, d, num);
    }
    else
    {
        if (tRight < this->t[num])
            this->IsHit(node->right, o, d, num);
        if (tLeft < this->t[num])
            this->IsHit(node->left, o, d, num);
    }
}

bool KDTree::HitBox(Node *node, const Point &o, const Point &d, double &t)
{
    // printf("%.3f %.3f %.3f %.3f %.3f %.3f\n", node->coordinate[0][0], node->coordinate[0][1],
    //        node->coordinate[1][0], node->coordinate[1][1],
    //        node->coordinate[2][0], node->coordinate[2][1]);
    double tMin = 0, tMax = INF;

    if (!eq(d.x, 0))
    {
        double x = (node->coordinate[0][0] - o.x) / d.x, y = (node->coordinate[0][1] - o.x) / d.x;
        if (x > y)
            swap(x, y);
        tMin = max(tMin, x), tMax = min(tMax, y);
    }
    else
    {
        if (!(ge(o.x, node->coordinate[0][0]) && le(o.x, node->coordinate[0][1])))
            return false;
    }

    if (!eq(d.y, 0))
    {
        double x = (node->coordinate[1][0] - o.y) / d.y, y = (node->coordinate[1][1] - o.y) / d.y;
        if (x > y)
            swap(x, y);
        tMin = max(tMin, x), tMax = min(tMax, y);
    }
    else
    {
        if (!(ge(o.y, node->coordinate[1][0]) && le(o.y, node->coordinate[1][1])))
            return false;
    }

    if (!eq(d.z, 0))
    {
        double x = (node->coordinate[2][0] - o.z) / d.z, y = (node->coordinate[2][1] - o.z) / d.z;
        if (x > y)
            swap(x, y);
        tMin = max(tMin, x), tMax = min(tMax, y);
    }
    else
    {
        if (!(ge(o.z, node->coordinate[2][0]) && le(o.z, node->coordinate[2][1])))
            return false;
    }

    // printf("%.3f %.3f\n", tMin, tMax);

    bool result = le(tMin, tMax);
    if (result)
        t = tMin;
    return result;
}

void KDTree::HitFace(Face *face, const Point &o, const Point &d, int num)
{
    // for (int j = 0; j < face->nPoint; j++)
    //     printf("%d/%d/%d ", face->points[j].pointIndex, face->points[j].textureIndex, face->points[j].normalDirectionIndex);
    // printf("%.3f %.3f %.3f\n", face->normalDirection.x, face->normalDirection.y, face->normalDirection.z);

    Point pp = this->parser->GetPoint(face->points[0].pointIndex);
    Point n = face->normalDirection;
    double u = dot(d, n);
    if (eq(u, 0))
    {
        return;
    }
    // if (u > 0)
    //     return;
    double t = dot(sub(pp, o), n) / u;
    if (le(t, 0) || t >= this->t[num])
        return;
    Point p = add(mul(d, t), o);
    bool result = this->InFace(p, face);
    if (result)
    {
        this->isHit[num] = true;
        this->t[num] = t;
        this->p[num] = p;
        GetNormalDirection(face, p, this->n[num], this->texture[num]);
        normalization(this->n[num]);
        this->material[num] = face->material;
    }
}

bool KDTree::InFace(const Point &p, const Face *face)
{
    Point u;
    int len = face->nPoint;
    for (int i = 0; i < len; i++)
    {
        Point a = this->parser->GetPoint(face->points[i].pointIndex);
        Point b = this->parser->GetPoint(face->points[(i + 1) % len].pointIndex);
        Point v = mul(sub(a, p), sub(b, p));
        if (eq(v.x, 0) && eq(v.y, 0) && eq(v.z, 0))
        {
            if (le(dot(sub(a, p), sub(b, p)), 0))
                return true;
        }
    }
    for (int i = 0; i < len; i++)
    {
        Point a = this->parser->GetPoint(face->points[i].pointIndex);
        Point b = this->parser->GetPoint(face->points[(i + 1) % len].pointIndex);
        Point v = mul(sub(a, p), sub(b, p));
        if (eq(v.x, 0) && eq(v.y, 0) && eq(v.z, 0))
            return false;
        if (i == 0)
        {
            u = v;
        }
        else
        {
            double d = dot(u, v);
            if (d < 0)
                return false;
        }
    }
    return true;
}

bool KDTree::Intersect(const Point &a, const Point &b, const Point &c, const Point &d)
{
    Point u = mul(sub(a, c), sub(b, c));
    Point v = mul(sub(a, d), sub(b, d));
    if (!le(dot(u, v), 0))
        return false;
    u = mul(sub(c, a), sub(d, a));
    v = mul(sub(c, b), sub(d, b));
    if (!le(dot(u, v), 0))
        return false;
    return true;
}

Point KDTree::GetIntersectPoint(const Point &a, const Point &b, const Point &o, const Point &d)
{
    Point n = sub(b, a);
    Point y = mul(sub(a, o), n);
    Point x = mul(d, n);
    double t = GetProportion(x, y);
    return add(mul(d, t), o);
}

void KDTree::GetNormalDirection(Face *face, const Point &p, Point &n, Texture &texture)
{
    int len = face->nPoint;
    for (int i = 0; i < len; i++)
    {
        Point a = this->parser->GetPoint(face->points[i].pointIndex);
        Point b = this->parser->GetPoint(face->points[(i + 1) % len].pointIndex);
        Point c = mul(sub(a, p), sub(b, p));
        if (eq(c.x, 0) && eq(c.y, 0) && eq(c.z, 0))
        {
            double proportion = GetProportion(sub(b, a), sub(p, a));
            Point aNormalDirection = this->parser->GetNormalDirection(face->points[i].normalDirectionIndex);
            Point bNormalDirection = this->parser->GetNormalDirection(face->points[(i + 1) % len].normalDirectionIndex);
            n = GetPointByProportion(aNormalDirection, bNormalDirection, proportion);
            normalization(n);

            Texture aTexture = this->parser->GetTexture(face->points[i].textureIndex);
            Texture bTexture = this->parser->GetTexture(face->points[(i + 1) % len].textureIndex);
            texture = GetTextureByProportion(aTexture, bTexture, proportion);
            return;
        }
    }
    int firstIndex = 0, secondIndex = 1, lastIndex = 2;
    if (len == 4)
        this->GetIndex(face, p, firstIndex, secondIndex, lastIndex);
    Point firstPoint = this->parser->GetPoint(face->points[firstIndex].pointIndex);
    Point firstNormalDirection = this->parser->GetNormalDirection(face->points[firstIndex].normalDirectionIndex);
    Texture firstTexture = this->parser->GetTexture(face->points[firstIndex].textureIndex);
    Point secondPoint = this->parser->GetPoint(face->points[secondIndex].pointIndex);
    Point secondNormalDirection = this->parser->GetNormalDirection(face->points[secondIndex].normalDirectionIndex);
    Texture secondTexture = this->parser->GetTexture(face->points[secondIndex].textureIndex);
    Point lastPoint = this->parser->GetPoint(face->points[lastIndex].pointIndex);
    Point lastNormalDirection = this->parser->GetNormalDirection(face->points[lastIndex].normalDirectionIndex);
    Texture lastTexture = this->parser->GetTexture(face->points[lastIndex].textureIndex);
    Point dir = sub(firstPoint, secondPoint);

    Point a = GetIntersectPoint(firstPoint, lastPoint, p, dir);
    double proportion = GetProportion(sub(lastPoint, firstPoint), sub(a, firstPoint));
    Point u = GetPointByProportion(firstNormalDirection, lastNormalDirection, proportion);
    Texture tu = GetTextureByProportion(firstTexture, lastTexture, proportion);
    Point b = GetIntersectPoint(secondPoint, lastPoint, p, dir);
    proportion = GetProportion(sub(lastPoint, secondPoint), sub(b, secondPoint));
    Point v = GetPointByProportion(secondNormalDirection, lastNormalDirection, proportion);
    Texture tv = GetTextureByProportion(secondTexture, lastTexture, proportion);
    proportion = GetProportion(sub(b, a), sub(p, a));
    n = GetPointByProportion(u, v, proportion);
    normalization(n);
    texture = GetTextureByProportion(tu, tv, proportion);
}

double KDTree::GetProportion(const Point &a, const Point &b)
{
    double t = 0;
    if (!eq(a.x, 0))
        t = b.x / a.x;
    else if (!eq(a.y, 0))
        t = b.y / a.y;
    else if (!eq(a.z, 0))
        t = b.z / a.z;
    return t;
}

Point KDTree::GetPointByProportion(const Point &a, const Point &b, const double proportion)
{
    Point dir = sub(b, a);
    return add(mul(dir, proportion), a);
}

Texture KDTree::GetTextureByProportion(const Texture &a, const Texture &b, const double proportion)
{
    Texture dir = sub(b, a);
    return add(mul(dir, proportion), a);
}

void KDTree::GetIndex(Face *face, const Point &p, int &firstIndex, int &secondIndex, int &lastIndex)
{
    Point a = this->parser->GetPoint(face->points[0].pointIndex);
    Point b = this->parser->GetPoint(face->points[1].pointIndex);
    Point c = this->parser->GetPoint(face->points[2].pointIndex);
    Point u = mul(sub(a, p), sub(b, p));

    Point v = mul(sub(b, p), sub(c, p));
    if (dot(u, v) < 0)
    {
        firstIndex = 2, secondIndex = 3, lastIndex = 0;
        return;
    }

    v = mul(sub(c, p), sub(a, p));
    if (dot(u, v) < 0)
    {
        firstIndex = 2, secondIndex = 3, lastIndex = 0;
        return;
    }
}

void KDTree::Delete(Node *node)
{
    if (node->left)
        this->Delete(node->left);
    if (node->right)
        this->Delete(node->right);
    delete node;
}
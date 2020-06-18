#include "shader.h"

Shader::Shader(KDTree *kdTree, Parser *parser)
{
    this->areaLight = new AreaLight[10];
    this->nAreaLight = 0;
    Face *faces = parser->GetFaces();
    for (int i = 0, len = parser->GetNFace(); i < len; i++)
    {
        if (faces[i].material->name == "light")
        {
            Point a = parser->GetPoint(faces[i].points[0].pointIndex), b = parser->GetPoint(faces[i].points[1].pointIndex), c = parser->GetPoint(faces[i].points[2].pointIndex), d = parser->GetPoint(faces[i].points[3].pointIndex);
            this->areaLight[this->nAreaLight++].Init(a, b, c, faces[i].material->ke);
            this->areaLight[this->nAreaLight++].Init(c, d, a, faces[i].material->ke);
        }
    }

    this->randomEngine = default_random_engine(1);
    this->lightPosition = Point{100, -100, 100};
    this->maxDepth = 2;

    this->kdTree = kdTree;
}

Shader::~Shader()
{
    delete[] this->areaLight;
}

Point Shader::Render(const Point &p, const Point &n, const Point &d, const Texture &texture, Material *&material, int num, bool isGlobal)
{
    if (!isGlobal)
    {
        Point ka, kd, ks;
        this->GetColor(texture, material, ka, kd, ks);
        if (this->nAreaLight)
        {
            Point result = Point{0, 0, 0};
            for (int i = 0; i < this->nAreaLight; i++)
                result = add(result, this->areaLight[i].GetLight(p, n, d, kd, ks, material->ns, this->kdTree, num, 3));
            result = add(result, dotMul(ka, material->ke));
            limit(result, 1.0);
            return result;
        }
        else
        {
            Point result = add(this->GetLight(p, n, d, this->lightPosition, kd, ks, material->ns, num), dotMul(ka, material->ke));
            limit(result, 1.0);
            return result;
        }
    }
    else
    {
        Point result = Point{0, 0, 0};
        for (int i = 0; i < SSP; i++)
            result = add(result, this->Shade(p, n, d, texture, material, 0, num));
        return div(result, SSP);
    }
}

Point Shader::Shade(const Point &p, const Point &n, const Point &d, const Texture &texture, Material *&material, int depth, int num)
{
    if (depth > this->maxDepth)
        return Point{0, 0, 0};

    Point ka, kd, ks;
    this->GetColor(texture, material, ka, kd, ks);
    Point result = dotMul(ka, material->ke);
    if (this->nAreaLight)
    {
        for (int i = 0; i < this->nAreaLight; i++)
            result = add(result, this->areaLight[i].GetLight(p, n, d, kd, ks, material->ns, this->kdTree, num));
    }
    else
        result = add(result, this->GetLight(p, n, d, this->lightPosition, kd, ks, material->ns, num));

    bool isSpecular = false;
    Point newD = this->GetDirection(n, d, material, isSpecular);

    Point tp, tn;
    Material *tMaterial;
    Texture tTexture;
    if (this->kdTree->IsHit(p, newD, tp, tn, tTexture, tMaterial, num))
    {
        Point dd = Point{-newD.x, -newD.y, -newD.z};
        Point indirect = this->Shade(tp, tn, dd, tTexture, tMaterial, depth + 1, num);
        if (isSpecular)
            result = add(result, dotMul(indirect, ks));
        else
            result = add(result, dotMul(indirect, kd));
    }
    limit(result, 1.0);
    return result;
}

Point Shader::GetLight(const Point &p, const Point &n, const Point &d, const Point &lightPosition, const Point &kd, const Point &ks, const double ns, int num)
{
    Point lightDirection = sub(lightPosition, p);
    Point tp, tn;
    Texture tTexture;
    Material *tMaterial;
    if (!this->kdTree->IsHit(p, lightDirection, tp, tn, tTexture, tMaterial, num, 1.0))
    {
        Point result = Point{0, 0, 0};
        normalization(lightDirection);
        if (le(dot(n, lightDirection), 0) || le(dot(n, d), 0))
            return result;

        if (!eq(kd.x, 0) && !eq(kd.y, 0) && !eq(kd.z, 0))
        {
            double diffuseFactor = max(0.0, dot(n, lightDirection));
            Point diffuse = mul(kd, diffuseFactor);
            result = add(result, diffuse);
        }

        if (!eq(ks.x, 0) && !eq(ks.y, 0) && !eq(ks.z, 0))
        {
            Point r = sub(mul(n, dot(lightDirection, n) * 2), lightDirection);
            normalization(r);
            double reflectiveFactor = max(0.0, dot(r, d));
            double specularFactor = pow(reflectiveFactor, ns);
            Point specular = mul(ks, specularFactor * ns);
            result = add(result, specular);
        }
        limit(result, 1.0);
        return result;
    }
    else
        return Point{0, 0, 0};
}

void Shader::GetColor(const Texture &texture, Material *&material, Point &ka, Point &kd, Point &ks)
{
#ifdef MAP_TEXTURE
    ka = (material->mapKa == NULL) ? material->ka : this->GetColor(material->mapKa, texture);
    kd = (material->mapKd == NULL) ? material->kd : this->GetColor(material->mapKd, texture);
    ks = (material->mapKs == NULL) ? material->ks : this->GetColor(material->mapKs, texture);
#else
    ka = material->ka;
    kd = material->kd;
    ks = material->ks;
#endif
}

#ifdef MAP_TEXTURE
Point Shader::GetColor(QImage *image, const Texture &texture)
{
    int width = image->width(), height = image->height();
    int x = width * texture.u, y = height * (1 - texture.v);
    QColor color = QColor(image->pixel(x, y));
    int red = color.red(), green = color.green(), blue = color.blue();
    return Point{(double)red / 255, (double)green / 255, (double)blue / 255};
}
#endif

Point Shader::GetDirection(const Point &up, const Point &d, Material *material, bool &isSpecular)
{
    Point kd = material->kd, ks = material->ks;
    double pd = kd.x + kd.y + kd.z, ps = ks.x + ks.y + ks.z;
    double pSum = pd + ps;
    if (eq(pSum, 0))
        return up;
    uniform_real_distribution<double> urd(0, pSum);
    if (urd(this->randomEngine) > pd)
    {
        isSpecular = true;
        Point r = sub(mul(up, dot(d, up) * 2), d);
        normalization(r);
        return r;
    }

    uniform_real_distribution<double> u(0, 2 * PI);

    double phi = u(this->randomEngine);
    double theta = (u(this->randomEngine) - PI) / 2;
    Point sample = Point{sin(theta) * cos(phi), cos(theta), sin(theta) * sin(phi)};

    Point front, right;

    if (fabs(up.x) > fabs(up.y))
        front = Point{up.z, 0, -up.x};
    else
        front = Point{0, -up.z, up.y};

    normalization(front);
    right = mul(up, front);

    Point result;
    result.x = right.x * sample.x + up.x * sample.y + front.x * sample.z;
    result.y = right.y * sample.x + up.y * sample.y + front.y * sample.z;
    result.z = right.z * sample.x + up.z * sample.y + front.z * sample.z;
    normalization(result);
    return result;
}
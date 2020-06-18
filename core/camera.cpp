#include "camera.h"

void PrintThread::Init(Point &startPoint, Point &dx, Point &dy, Point &z, Point &o, int width, int height, bool projectionState, bool isGlobal, unsigned int *pixels, KDTree *kdTree, Shader *shader, int num)
{
    this->startPoint = startPoint;
    this->dx = dx;
    this->pdx = mul(dx, NUMBER_OF_THREAD);
    this->dy = dy;
    this->z = z;
    this->o = o;
    this->width = width;
    this->height = height;
    this->projectionState = projectionState;
    this->isGlobal = isGlobal;
    this->pixels = pixels;
    this->kdTree = kdTree;
    this->shader = shader;
    this->num = num;
    this->setDone(false);
}

void PrintThread::Run()
{
    for (int i = 0; i < height; i++)
    {
        Point u = add(startPoint, mul(dx, num));
        for (int j = num; j < width; j += NUMBER_OF_THREAD, u = add(u, pdx))
        {
            Point p, n, d;
            bool isHit;
            Material *material;
            Texture texture;

            if (projectionState)
            {
                // 正交
                Point pp = sub(u, mul(z, 1000));
                d = z;
                isHit = kdTree->IsHit(pp, d, p, n, texture, material, num);
            }
            else
            {
                // 透视
                d = sub(u, o);
                normalization(d);
                isHit = kdTree->IsHit(o, d, p, n, texture, material, num);
            }

            double red = 0, green = 0, blue = 0;
            if (isHit /* && le(dot(n, d), 0)*/)
            {
                d = negative(d);
                Point rgb = shader->Render(p, n, d, texture, material, num, isGlobal);
                red = rgb.x, green = rgb.y, blue = rgb.z;
            }
            int tr = red * 255, tg = green * 255, tb = blue * 255;
            pixels[i * width + j] = static_cast<unsigned int>((tr << 16) | (tg << 8) | tb);
        }
        startPoint = add(startPoint, dy);
    }
    setDone(true);
}

Camera::Camera(KDTree *kdTree, Parser *parser)
{
    this->width = WIDTH;
    this->height = HEIGHT;
    if (!parser->HasCameraInformation())
    {
        this->x = Point{1, 0, 0};
        this->y = Point{0, 0, -1};
        this->z = mul(this->x, this->y);
        normalization(this->z);
        this->f = 5.0;
        this->pixelWidth = 0.01;
        this->o = Point{0.5, -5, 0.5};
        this->projectionState = true;
    }
    else
    {
        this->x = parser->GetCameraX();
        this->y = parser->GetCameraY();
        this->z = parser->GetCameraZ();
        this->f = parser->GetCameraF();
        this->pixelWidth = parser->GetCameraPixelWidth();
        this->o = parser->GetCameraO();
        this->projectionState = parser->GetCameraProjectionState();
    }

    this->kdTree = kdTree;
    this->shader = new Shader(kdTree, parser);
    this->printThread = new PrintThread[NUMBER_OF_THREAD];
}

Camera::~Camera()
{
    delete[] this->printThread;
    delete this->shader;
}

void Camera::GetPicture(unsigned int *pixels, bool isGlobal)
{
#ifdef TEST
    printf("Camera inforamtion\n");
    printf("%.5f %.5f %.5f\n", this->x.x, this->x.y, this->x.z);
    printf("%.5f %.5f %.5f\n", this->y.x, this->y.y, this->y.z);
    printf("%.5f %.5f %.5f\n", this->z.x, this->z.y, this->z.z);
    printf("%.5f %.5f %.5f\n", this->o.x, this->o.y, this->o.z);
    printf("%.5f\n", this->pixelWidth);
    printf("%.5f\n", this->f);
    printf("%d\n", this->projectionState);
#endif
    Point p = add(mul(this->z, this->f), this->o);
    Point dx = mul(this->x, this->pixelWidth), dy = mul(this->y, this->pixelWidth);
    Point startPoint = sub(p, add(mul(dx, this->width / 2), mul(dy, this->height / 2)));
    for (int i = 0; i < NUMBER_OF_THREAD; i++)
    {
        printThread[i].Init(startPoint, dx, dy, this->z, this->o, this->width, this->height, this->projectionState, isGlobal, pixels, this->kdTree, this->shader, i);
        threadPool.AddTask(&printThread[i]);
    }
    while (true)
    {
        bool flag = true;
        for (int i = 0; i < NUMBER_OF_THREAD; i++)
            if (!printThread[i].isDone())
            {
                flag = false;
                break;
            }
        if (flag)
            break;
    }
}

void Camera::Sacle(int d)
{
    double dd = d / 120 * 0.01;
    if (d > 0)
    {
        if (this->pixelWidth <= 0.01)
            this->pixelWidth /= 2;
        else
            this->pixelWidth -= dd;
    }
    else
    {
        if (this->pixelWidth <= 0.01)
            this->pixelWidth *= 2;
        else
            this->pixelWidth -= dd;
    }
    // this->pixelWidth = max(0.01, this->pixelWidth - d / 120 * 0.01);
}

void Camera::Move(int x, int y)
{
    if (x)
        this->o = sub(this->o, mul(this->x, 0.5 * x));
    if (y)
        this->o = sub(this->o, mul(this->y, 0.5 * y));
}

void Camera::Rotate(int xx, int yy)
{
    if (xx)
    {
        xx = -xx;
        double angle = xx * PI / 180;
        Point newO = add(this->o, mul(this->z, this->f));
        double z = -cos(-angle) * this->f, x = -sin(-angle) * this->f;
        this->o = add(add(mul(this->z, z), mul(this->x, x)), newO);
        this->z = sub(newO, this->o);
        this->x = mul(this->y, this->z);
        normalization(this->z);
        normalization(this->x);
    }
    if (yy)
    {
        yy = -yy;
        double angle = yy * PI / 180;
        Point newO = add(this->o, mul(this->z, this->f));
        double z = -cos(-angle) * this->f, y = -sin(-angle) * this->f;
        this->o = add(add(mul(this->z, z), mul(this->y, y)), newO);
        this->z = sub(newO, this->o);
        this->y = mul(this->z, this->x);
        normalization(this->z);
        normalization(this->y);
    }
}

void Camera::ChangeProjectionState()
{
    this->projectionState ^= 1;
}

bool Camera::GetProjectionState()
{
    return this->projectionState;
}

int Camera::GetWidth()
{
    return this->width;
}

int Camera::GetHeight()
{
    return this->height;
}
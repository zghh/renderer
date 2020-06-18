#include "util.h"

Material::Material()
{
    this->ka = Point{0, 0, 0};
    this->ks = Point{0, 0, 0};
    this->kd = Point{0, 0, 0};
    this->ke = Point{0, 0, 0};
    this->tf = Point{0, 0, 0};
    this->ns = 0;
    this->ni = 0;
    this->d = 0;
    this->tr = 0;
    this->illum = 2;
#ifdef MAP_TEXTURE
    this->mapKa = NULL;
    this->mapKd = NULL;
    this->mapKs = NULL;
#endif
}

bool eq(double x, double y)
{
    return fabs(x - y) <= EPS;
}

bool le(double x, double y)
{
    return x <= y + EPS;
}

bool ge(double x, double y)
{
    return x >= y - EPS;
}

void normalization(Point &a)
{
    double l = sqrt(a.x * a.x + a.y * a.y + a.z * a.z);
    a.x /= l;
    a.y /= l;
    a.z /= l;
}

Point negative(const Point &a)
{
    return Point{-a.x, -a.y, -a.z};
}

double dot(const Point &a, const Point &b)
{
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

Point add(const Point &a, const Point &b)
{
    return Point{a.x + b.x,
                 a.y + b.y,
                 a.z + b.z};
}

Texture add(const Texture &a, const Texture &b)
{
    return Texture{a.u + b.u,
                   a.v + b.v};
}

Point sub(const Point &a, const Point &b)
{
    return Point{a.x - b.x,
                 a.y - b.y,
                 a.z - b.z};
}

Texture sub(const Texture &a, const Texture &b)
{
    return Texture{a.u - b.u,
                   a.v - b.v};
}

Point mul(const Point &a, const Point &b)
{
    return Point{a.y * b.z - a.z * b.y,
                 a.z * b.x - a.x * b.z,
                 a.x * b.y - a.y * b.x};
}

Point mul(const Point &a, const double b)
{
    return Point{a.x * b, a.y * b, a.z * b};
}

Texture mul(const Texture &a, const double b)
{
    return Texture{a.u * b, a.v * b};
}

Point dotMul(const Point &a, const Point &b)
{
    return Point{a.x * b.x, a.y * b.y, a.z * b.z};
}

Texture dotMul(const Texture &a, const Texture &b)
{
    return Texture{a.u * b.u, a.v * b.v};
}

Point div(const Point &a, const double b)
{
    return Point{a.x / b, a.y / b, a.z / b};
}

void limit(Point &a, double b)
{
    a.x = min(a.x, b);
    a.y = min(a.y, b);
    a.z = min(a.z, b);
}
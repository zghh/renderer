#pragma once
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <string>
#include "const.h"

#define MAP_TEXTURE

#ifdef MAP_TEXTURE
#include <QImage>
#endif

using namespace std;

struct Point
{
    double x, y, z;
};

struct Material
{
    Point ka, kd, ks, ke, tf;
    double ns, ni, d, tr;
    int illum;
    string name;
    Material();
#ifdef MAP_TEXTURE
    QImage *mapKa, *mapKd, *mapKs;
#endif
};

struct Texture
{
    double u, v;
};

struct FacePoint
{
    int pointIndex, textureIndex, normalDirectionIndex;
};

struct Face
{
    Material *material;
    int nPoint;
    FacePoint points[10];
    Point normalDirection;
};

bool eq(double x, double y);

bool le(double x, double y);

bool ge(double x, double y);

void normalization(Point &a);

Point negative(const Point &a);

double dot(const Point &a, const Point &b);

Point add(const Point &a, const Point &b);

Texture add(const Texture &a, const Texture &b);

Point sub(const Point &a, const Point &b);

Texture sub(const Texture &a, const Texture &b);

Point mul(const Point &a, const Point &b);

Point mul(const Point &a, const double b);

Texture mul(const Texture &a, const double b);

Point dotMul(const Point &a, const Point &b);

Texture dotMul(const Texture &a, const Texture &b);

Point div(const Point &a, const double b);

void limit(Point &a, double b);
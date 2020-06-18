#pragma once
#include <cstdio>
#include <cstdlib>
#include <algorithm>
#include <random>
#include "kdTree.h"
#include "util.h"
#include "const.h"
#include "areaLight.h"

#ifdef MAP_TEXTURE
#include <QColor>
#endif

using namespace std;

#ifndef SHADER_H
#define SHADER_H

class Shader
{
private:
    KDTree *kdTree;
    Point lightPosition;
    default_random_engine randomEngine;
    int maxDepth;
    AreaLight *areaLight;
    int nAreaLight;
    Point Shade(const Point &p, const Point &n, const Point &d, const Texture &texture, Material *&material, int depth, int num);
    Point GetLight(const Point &p, const Point &n, const Point &d, const Point &lightPosition, const Point &kd, const Point &ks, const double ns, int num);
    void GetColor(const Texture &texture, Material *&material, Point &ka, Point &kd, Point &ks);
#ifdef MAP_TEXTURE
    Point GetColor(QImage *image, const Texture &texture);
#endif
    Point GetDirection(const Point &n, const Point &d, Material *material, bool &isSpecular);

public:
    Shader(KDTree *kDTree, Parser *parser);
    ~Shader();
    Point Render(const Point &p, const Point &n, const Point &d, const Texture &texture, Material *&material, int num, bool isGlobal = false);
};

#endif

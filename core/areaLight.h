#pragma once
#include <random>
#include "kdTree.h"
#include "util.h"

using namespace std;

#ifndef AREALIGHT_H
#define AREALIGHT_H

class AreaLight
{
private:
    Point a, b, c, n, dx, dy, color;
    double area;
    int sample;
    default_random_engine randomEngine;

public:
    void Init(Point &a, Point &b, Point &c, Point &color);
    Point GetLight(const Point &p, const Point &n, const Point &d, const Point &kd, const Point &ks, const double ns, KDTree *kdTree, int num, int nSample = 0);
};

#endif
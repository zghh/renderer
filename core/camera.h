#pragma once
#include "util.h"
#include "kdTree.h"
#include "shader.h"
#include "const.h"
#include "parser.h"
#include "threadPool.h"

using namespace std;

#ifndef CAMERA_H
#define CAMERA_H

class PrintThread : public Runnable
{
private:
    Point startPoint, dx, dy, pdx, z, o;
    int width, height;
    bool projectionState;
    bool isGlobal;
    unsigned int *pixels;
    KDTree *kdTree;
    Shader *shader;
    int num;

public:
    void Init(Point &startPoint, Point &dx, Point &dy, Point &z, Point &o, int width, int height, bool projectionState, bool isGlobal, unsigned int *pixels, KDTree *kdTree, Shader *shader, int num);
    void Run();
};

class Camera
{
private:
    Point x, y, z, o;
    double f, pixelWidth;
    KDTree *kdTree;
    Shader *shader;
    int width, height;
    // 投影状态：true为正交投影，false为透视投影
    bool projectionState;

    PrintThread *printThread;
    ThreadPool threadPool;

public:
    Camera(KDTree *kdTree, Parser *parser);
    ~Camera();
    void GetPicture(unsigned int *pixels, bool isGlobal = false);
    void Sacle(int d);
    void Move(int x, int y);
    void Rotate(int xx, int yy);
    void ChangeProjectionState();
    bool GetProjectionState();
    int GetWidth();
    int GetHeight();
};

#endif
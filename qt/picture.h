#pragma once
#include <cstdio>
#include <cstdlib>
#include "../core/camera.h"
#include "ui_window.h"

using namespace std;

#ifndef PICTURE_H
#define PICTURE_H

class Picture
{
private:
    QRgb *pixels, *pixelsWithGlobal;
    QImage *image, *imageWithGlobal;

public:
    Picture();
    ~Picture();
    QRgb *GetPixels();
    QImage *GetImage();
    QRgb *GetPixelsWithGlobal();
    QImage *GetImageWithGlobal();
};

#endif

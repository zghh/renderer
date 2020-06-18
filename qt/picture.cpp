#include "picture.h"
#include "../core/const.h"

Picture::Picture()
{
    int width = WIDTH, height = HEIGHT;
    this->pixels = new QRgb[width * height];
    this->image = new QImage((uchar *)this->pixels, width, height, QImage::Format_RGB32);
    this->pixelsWithGlobal = new QRgb[width * height];
    this->imageWithGlobal = new QImage((uchar *)this->pixelsWithGlobal, width, height, QImage::Format_RGB32);
}

Picture::~Picture()
{
    // delete[] this->pixels;
    // delete this->image;
}

QRgb *Picture::GetPixels()
{
    return this->pixels;
}

QImage *Picture::GetImage()
{
    return this->image;
}

QRgb *Picture::GetPixelsWithGlobal()
{
    return this->pixelsWithGlobal;
}

QImage *Picture::GetImageWithGlobal()
{
    return this->imageWithGlobal;
}

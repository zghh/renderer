#pragma once
#include <cstdio>
#include <cstdlib>
#include <string>
#include <map>
#include <io.h>
#include "util.h"

using namespace std;

#ifndef PARSER_H
#define PARSER_H

class Parser
{
private:
    string objFileName, mtlFileName, folderName, informationFileName;
    int nPoint, nTexture, nNormalDirection, nFace, nMaterial;
    Point *points;
    Texture *textures;
    Point *normalDirections;
    Face *faces;
    map<string, int> materialMap;
    Material *materials;
    char tempString[30];
    bool hasLoaded;
#ifdef MAP_TEXTURE
    map<string, QImage *> textureMap;
#endif
    bool hasCameraInformation;
    Point cameraX, cameraY, cameraZ, cameraO;
    double cameraPixelWidth, cameraF;
    bool cameraProjectionState;
    void LoadCount();
    void LoadData();
    void LoadLine();
    int ReadInt(int &x);
#ifdef MAP_TEXTURE
    QImage *LoadTexture(string name);
#endif

public:
    Parser(string fileName);
    ~Parser();
    Face *GetFaces();
    Point GetPoint(int index);
    Point GetNormalDirection(int index);
    Texture GetTexture(int index);
    int GetNFace();
    int GetNPoint();
    bool HasCameraInformation();
    Point GetCameraX();
    Point GetCameraY();
    Point GetCameraZ();
    Point GetCameraO();
    double GetCameraPixelWidth();
    double GetCameraF();
    bool GetCameraProjectionState();
    void Load();
};

#endif
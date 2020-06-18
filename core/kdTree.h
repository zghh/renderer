#pragma once
#include <cstdio>
#include <cstdlib>
#include <algorithm>
#include "parser.h"
#include "util.h"

using namespace std;

#ifndef KDTREE_H
#define KDTREE_H

const int MAX_THREAD = 8;

struct Node
{
    double coordinate[3][2];
    Node *left, *right;
    Face face;
    Node();
    Node(double coordinate[][2]);
    ~Node();
};

struct NodeIndex
{
    int index, d;
    double coordinate[3][2];
    void Init(Parser &parser, int index);
    bool operator<(const NodeIndex &x) const;
};

class KDTree
{
private:
    Parser *parser;
    Node *root;

    bool isHit[MAX_THREAD];
    Point p[MAX_THREAD], n[MAX_THREAD];
    double t[MAX_THREAD];
    Texture texture[MAX_THREAD];
    Material *material[MAX_THREAD];

    void Build(Parser &parser, NodeIndex *index, int left, int right, Node *node, int depth);
    void IsHit(Node *node, const Point &o, const Point &d, int num);
    bool HitBox(Node *node, const Point &o, const Point &d, double &t);
    void HitFace(Face *face, const Point &o, const Point &d, int num);
    bool InFace(const Point &p, const Face *face);
    bool Intersect(const Point &a, const Point &b, const Point &c, const Point &d);
    Point GetIntersectPoint(const Point &a, const Point &b, const Point &o, const Point &d);
    void GetNormalDirection(Face *face, const Point &p, Point &n, Texture &texture);
    double GetProportion(const Point &a, const Point &b);
    Point GetPointByProportion(const Point &a, const Point &b, const double proportion);
    Texture GetTextureByProportion(const Texture &a, const Texture &b, const double proportion);
    void GetIndex(Face *face, const Point &p, int &firstIndex, int &secondIndex, int &lastIndex);
    void Delete(Node *node);

public:
    KDTree(Parser &parser);
    ~KDTree();
    bool IsHit(const Point &o, const Point &d, Point &p, Point &n, Texture &texture, Material *&material, int num, double tMax = INF);
};

#endif
#pragma once
#ifndef LOOP_SUBDIVISION_STRUCT
#define LOOP_SUBDIVISION_STRUCT
#include "DXUT.h"
#include <vector>
using namespace std;
#define myDefaultColor getRandomColor()
//#define myDefaultColor D3DXVECTOR4(1.0f, 0.0f, 0.0f, 1.0f)
#define PI 3.14159265
struct VertexType
{
	D3DXVECTOR3 position;
	D3DXVECTOR4 color;
	bool operator ==(const VertexType&);
};

struct Edge {
	unsigned int v1;
	unsigned int v2;
	bool operator ==(const Edge&);
};

struct Triangle
{
	Edge *e1;
	Edge *e2;
	Edge *e3;
};
D3DXVECTOR4 getRandomColor();
void doLoopSubdivision(vector<Triangle*> *triangleList, vector<Edge*> *edgeList, vector<VertexType*> *vertexList);

#endif // LOOP_SUBDIVISION_STRUCT

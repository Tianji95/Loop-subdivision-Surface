#include "DXUT.h"
#include "LoopSubdivisionStruct.h"
#include <map>

bool VertexType::operator ==(const VertexType& input) {
	if (this->color == input.color && this->position == input.position) {
		return true;
	}
	else {
		return false;
	}
}
bool Edge::operator ==(const Edge& input) {
	if (this->v1 == input.v1 && this->v2 == input.v2) {
		return true;
	}
	if (this->v2 == input.v1 && this->v1 == input.v2) {
		return true;
	}
	return false;
}

D3DXVECTOR4 getRandomColor() {
	float colorR = (rand() % 1000)*1.0f / 1000.0f;
	float colorG = (rand() % 1000)*1.0f / 1000.0f;
	float colorB = (rand() % 1000)*1.0f / 1000.0f;
	return D3DXVECTOR4(colorR, colorG, colorB, 1.0f);
}


Edge* searchIsinList(vector<Edge*> *edgeList, unsigned int v1, unsigned int v2) {
	for (auto edgeIter = edgeList->begin(); edgeIter != edgeList->end(); edgeIter++) {
		if ((*edgeIter)->v1 == v1 && (*edgeIter)->v2 == v2) {
			return *edgeIter;
		}
		if ((*edgeIter)->v1 == v2 && (*edgeIter)->v2 == v1) {
			return *edgeIter;
		}
	}
	return NULL;
}

void doLoopSubdivision(vector<Triangle*> *triangleList, vector<Edge*> *edgeList, vector<VertexType*> *vertexList) {

	int neighberCount;
	int triaBeforeSize;
	int edgeBeforeSize;
	int vertexBeforeSize;
	triaBeforeSize = triangleList->size();
	edgeBeforeSize = edgeList->size();
	vertexBeforeSize = vertexList->size();
	map<Edge*, int> EdgePointMap;

	vector<Triangle*> newTriangleList;
	newTriangleList.clear();

	//增加边点
	for (auto edgeIter = edgeList->begin(); edgeIter != edgeList->end();edgeIter++) {
		Triangle firstTriangle = **triangleList->begin(), secondTriangle = **triangleList->begin();
		VertexType *firstTopPoint, *secondTopPoint, *firstEdgePoint, *secondEdgePoint;
		firstEdgePoint = (*vertexList)[(*edgeIter)->v1];
		secondEdgePoint = (*vertexList)[(*edgeIter)->v2];
		//找到共边的两个面
		for (auto triIter = triangleList->begin(); triIter != triangleList->end(); triIter++) {	
			if ((*triIter)->e1 == *edgeIter || (*triIter)->e2 == *edgeIter || (*triIter)->e3 == *edgeIter) {
				secondTriangle = firstTriangle;
				firstTriangle = **triIter;
			}
		}
		//找到两个三角形的端点
		if ((*vertexList)[firstTriangle.e1->v1] != firstEdgePoint && (*vertexList)[firstTriangle.e1->v1] != secondEdgePoint) {
			firstTopPoint = (*vertexList)[firstTriangle.e1->v1];
		}
		else if ((*vertexList)[firstTriangle.e1->v2] != firstEdgePoint && (*vertexList)[firstTriangle.e1->v2] != secondEdgePoint) {
			firstTopPoint = (*vertexList)[firstTriangle.e1->v2];
		}
		else if ((*vertexList)[firstTriangle.e2->v1] != firstEdgePoint && (*vertexList)[firstTriangle.e2->v1] != secondEdgePoint) {
			firstTopPoint = (*vertexList)[firstTriangle.e2->v1];
		}
		else {
			firstTopPoint = (*vertexList)[firstTriangle.e2->v2];
		}

		if ((*vertexList)[secondTriangle.e1->v1] != firstEdgePoint && (*vertexList)[secondTriangle.e1->v1] != secondEdgePoint) {
			secondTopPoint = (*vertexList)[secondTriangle.e1->v1];
		}
		else if ((*vertexList)[secondTriangle.e1->v2] != firstEdgePoint && (*vertexList)[secondTriangle.e1->v2] != secondEdgePoint) {
			secondTopPoint = (*vertexList)[secondTriangle.e1->v2];
		}
		else if ((*vertexList)[secondTriangle.e2->v1] != firstEdgePoint && (*vertexList)[secondTriangle.e2->v1] != secondEdgePoint) {
			secondTopPoint = (*vertexList)[secondTriangle.e2->v1];
		}
		else {
			secondTopPoint = (*vertexList)[secondTriangle.e2->v2];
		}

		//生成新的边点
		VertexType *newEdgePoint = new VertexType;
		int newEdgePointIndex = 0;
		float newX, newY, newZ;
		newX = 1.0f / 8.0f*(firstTopPoint->position.x + secondTopPoint->position.x) + 3.0f / 8.0f*(firstEdgePoint->position.x + secondEdgePoint->position.x);
		newY = 1.0f / 8.0f*(firstTopPoint->position.y + secondTopPoint->position.y) + 3.0f / 8.0f*(firstEdgePoint->position.y + secondEdgePoint->position.y);
		newZ = 1.0f / 8.0f*(firstTopPoint->position.z + secondTopPoint->position.z) + 3.0f / 8.0f*(firstEdgePoint->position.z + secondEdgePoint->position.z);
		newEdgePoint->position = D3DXVECTOR3(newX, newY, newZ);
		newEdgePoint->color = myDefaultColor;
		newEdgePointIndex = vertexList->size();
		vertexList->push_back(newEdgePoint);

		EdgePointMap.insert(pair<Edge*, int>(*edgeIter, newEdgePointIndex));
		
	}

	//连接新的顶点，生成新的面
	for(auto trianIter = triangleList->begin(); trianIter!= triangleList->end(); trianIter++){
		
		//连接生成新的边点和原来的两个顶点


		//查找新边是否已经生成了

		Edge *e1Half1 = new Edge;
		Edge *e1Half2 = new Edge;
		Edge *tempEdge = new Edge;
		int e1EdgePointIndex = EdgePointMap[(*trianIter)->e1];
		tempEdge = searchIsinList(edgeList, (*trianIter)->e1->v1, e1EdgePointIndex);
		if (tempEdge!= NULL) {
			e1Half1 = tempEdge;
		}
		else {
			e1Half1->v1 = (*trianIter)->e1->v1;
			e1Half1->v2 = e1EdgePointIndex;
			edgeList->push_back(e1Half1);

		}
		tempEdge = searchIsinList(edgeList, (*trianIter)->e1->v2, e1EdgePointIndex);
		if (tempEdge != NULL) {
			e1Half2 = tempEdge;
		}
		else {
			e1Half2->v1 = (*trianIter)->e1->v2;
			e1Half2->v2 = e1EdgePointIndex;
			edgeList->push_back(e1Half2);
		}




		Edge *e2Half1 = new Edge;
		Edge *e2Half2 = new Edge;
		int e2EdgePointIndex = EdgePointMap[(*trianIter)->e2];
		tempEdge = searchIsinList(edgeList, (*trianIter)->e2->v1, e2EdgePointIndex);
		if (tempEdge != NULL) {
			e2Half1 = tempEdge;
		}
		else {
			e2Half1->v1 = (*trianIter)->e2->v1;
			e2Half1->v2 = e2EdgePointIndex;
			edgeList->push_back(e2Half1);

		}
		tempEdge = searchIsinList(edgeList, (*trianIter)->e2->v2, e2EdgePointIndex);
		if (tempEdge != NULL) {
			e2Half2 = tempEdge;
		}
		else {
			e2Half2->v1 = (*trianIter)->e2->v2;
			e2Half2->v2 = e2EdgePointIndex;
			edgeList->push_back(e2Half2);
		}


		Edge *e3Half1 = new Edge;
		Edge *e3Half2 = new Edge;
		int e3EdgePointIndex = EdgePointMap[(*trianIter)->e3];

		tempEdge = searchIsinList(edgeList, (*trianIter)->e3->v1, e3EdgePointIndex);
		if (tempEdge != NULL) {
			e3Half1 = tempEdge;
		}
		else {
			e3Half1->v1 = (*trianIter)->e3->v1;
			e3Half1->v2 = e3EdgePointIndex;
			edgeList->push_back(e3Half1);

		}
		tempEdge = searchIsinList(edgeList, (*trianIter)->e3->v2, e3EdgePointIndex);
		if (tempEdge != NULL) {
			e3Half2 = tempEdge;
		}
		else {
			e3Half2->v1 = (*trianIter)->e3->v2;
			e3Half2->v2 = e3EdgePointIndex;
			edgeList->push_back(e3Half2);
		}

		//连接生成新的边点和相邻边的边点
		Edge *e1e2ConnectE = new Edge;
		Edge *e1e3ConnectE = new Edge;
		Edge *e2e3ConnectE = new Edge;

		tempEdge = searchIsinList(edgeList, e1EdgePointIndex, e2EdgePointIndex);
		if (tempEdge != NULL) {
			e1e2ConnectE = tempEdge;
		}
		else {
			e1e2ConnectE->v1 = e1EdgePointIndex;
			e1e2ConnectE->v2 = e2EdgePointIndex;
			edgeList->push_back(e1e2ConnectE);
		}
		tempEdge = searchIsinList(edgeList, e2EdgePointIndex, e3EdgePointIndex);
		if (tempEdge != NULL) {
			e2e3ConnectE = tempEdge;
		}
		else {
			e2e3ConnectE->v1 = e2EdgePointIndex;
			e2e3ConnectE->v2 = e3EdgePointIndex;
			edgeList->push_back(e2e3ConnectE);
		}

		tempEdge = searchIsinList(edgeList, e1EdgePointIndex, e3EdgePointIndex);
		if (tempEdge != NULL) {
			e1e3ConnectE = tempEdge;
		}
		else {
			e1e3ConnectE->v1 = e1EdgePointIndex;
			e1e3ConnectE->v2 = e3EdgePointIndex;
			edgeList->push_back(e1e3ConnectE);
		}

		//生成新的面
		Triangle *t1 = new Triangle;
		Triangle *t2 = new Triangle;
		Triangle *t3 = new Triangle;
		Triangle *t4 = new Triangle;
		
		//生成e1e2角对应的面
		if ((*trianIter)->e1->v1 == (*trianIter)->e2->v1) {
			t1->e1 = e1Half1;
			t1->e2 = e2Half1;
			t1->e3 = e1e2ConnectE;

		}
		else if ((*trianIter)->e1->v1 == (*trianIter)->e2->v2) {
			t1->e1 = e1Half1;
			t1->e2 = e2Half2;
			t1->e3 = e1e2ConnectE;
		}
		else if ((*trianIter)->e1->v2 == (*trianIter)->e2->v1) {
			t1->e1 = e1Half2;
			t1->e2 = e2Half1;
			t1->e3 = e1e2ConnectE;
		}
		else if ((*trianIter)->e1->v2 == (*trianIter)->e2->v2) {
			t1->e1 = e1Half2;
			t1->e2 = e2Half2;
			t1->e3 = e1e2ConnectE;
		}
		//生成e1e3角对应的面
		if ((*trianIter)->e1->v1 == (*trianIter)->e3->v1) {
			t2->e1 = e1Half1;
			t2->e2 = e3Half1;
			t2->e3 = e1e3ConnectE;
		}
		else if ((*trianIter)->e1->v1 == (*trianIter)->e3->v2) {
			t2->e1 = e1Half1;
			t2->e2 = e3Half2;
			t2->e3 = e1e3ConnectE;
		}
		else if ((*trianIter)->e1->v2 == (*trianIter)->e3->v1) {
			t2->e1 = e1Half2;
			t2->e2 = e3Half1;
			t2->e3 = e1e3ConnectE;
		}
		else if ((*trianIter)->e1->v2 == (*trianIter)->e3->v2) {
			t2->e1 = e1Half2;
			t2->e2 = e3Half2;
			t2->e3 = e1e3ConnectE;
		}

		//生成e2e3角对应的面
		if ((*trianIter)->e2->v1 == (*trianIter)->e3->v1) {
			t3->e1 = e2Half1;
			t3->e2 = e3Half1;
			t3->e3 = e2e3ConnectE;
		}
		else if ((*trianIter)->e2->v1 == (*trianIter)->e3->v2) {
			t3->e1 = e2Half1;
			t3->e2 = e3Half2;
			t3->e3 = e2e3ConnectE;
		}
		else if ((*trianIter)->e2->v2 == (*trianIter)->e3->v1) {
			t3->e1 = e2Half2;
			t3->e2 = e3Half1;
			t3->e3 = e2e3ConnectE;
		}
		else if ((*trianIter)->e2->v2 == (*trianIter)->e3->v2) {
			t3->e1 = e2Half2;
			t3->e2 = e3Half2;
			t3->e3 = e2e3ConnectE;
		}
		t4->e1 = e1e2ConnectE;
		t4->e2 = e1e3ConnectE;
		t4->e3 = e2e3ConnectE;
		newTriangleList.push_back(t1);
		newTriangleList.push_back(t2);
		newTriangleList.push_back(t3);
		newTriangleList.push_back(t4);

	}
	for (auto trianIter = newTriangleList.begin(); trianIter != newTriangleList.end(); trianIter++) {
		triangleList->push_back(*trianIter);
	}


	//移动原来的顶点
	VertexType *thisVertex;
	float beta, newX, newY, newZ;
	
	
	vector<VertexType*> newVertexList, neighborVertexList;
	for (int verIter = 0; verIter < vertexBeforeSize; verIter++) {
		thisVertex = (*vertexList)[verIter];
		neighberCount = 0;
		neighborVertexList.clear();
		//寻找这个点周围有几个点
		for (int edgeIter = 0; edgeIter < edgeBeforeSize; edgeIter++) {
			if ((*edgeList)[edgeIter]->v1 == verIter) {
				neighberCount++;
				neighborVertexList.push_back((*vertexList)[(*edgeList)[edgeIter]->v2]);
			}
			else if ((*edgeList)[edgeIter]->v2 == verIter) {
				neighberCount++;
				neighborVertexList.push_back((*vertexList)[(*edgeList)[edgeIter]->v1]);
			}
		}

		beta = 1.0f / neighberCount*(5.0f / 8.0f - (3.0f / 8.0f + 1.0f / 4.0f*cos(2.0f*PI / neighberCount))*(3.0f / 8.0f + 1.0f / 4.0f*cos(2.0f*PI / neighberCount)));
		newX = (1.0f - neighberCount * beta) * (thisVertex->position.x);
		newY = (1.0f - neighberCount * beta) * (thisVertex->position.y);
		newZ = (1.0f - neighberCount * beta) * (thisVertex->position.z);
		for (auto neighborIter = neighborVertexList.begin(); neighborIter != neighborVertexList.end(); neighborIter++) {
			newX += (*neighborIter)->position.x * beta;
			newY += (*neighborIter)->position.y * beta;
			newZ += (*neighborIter)->position.z * beta;
		}
		VertexType *newVertex = new VertexType;
		newVertex->position = D3DXVECTOR3(newX, newY, newZ);
		newVertex->color = myDefaultColor;
		newVertexList.push_back(newVertex);
	}
	for (int verIter = 0; verIter < vertexBeforeSize; verIter++) {
		(*vertexList)[verIter]->position = newVertexList[verIter]->position;
	}
	
	//删除原来的面（点已经移动过了）
	for (int trianNum = 0; trianNum < triaBeforeSize; trianNum++) {
		triangleList->erase(triangleList->begin());
	}

}
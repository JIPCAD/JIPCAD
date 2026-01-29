#include "Face.h"
#include "DataStructureFace.h"

void CalculateNormals(Face f) 
{

}
std::vector<Vertex*> vertexList = f->vertices;
// std::vector<Vertex*> vertexList = f->normal;
Vector3 v0 = vertexList.at(0)->position - vertexList.at(2)->position;
Vector3 v1 = vertexList.at(1)->position - vertexList.at(2)->position;
Vector3 n = std::(v0, v1);

return normalize(n);
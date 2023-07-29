#pragma once
#include "Mesh.h"
#include <unordered_map>

namespace Nome::Scene
{
    //Aaron added this class CShellRefiner, modelled off the Offset Refiner
    //to enable JIPCAD to have the shelling capability.
	class CShellRefiner
	{
        public:
            CShellRefiner(DSMesh& _m);
            void Refine(float shellHeight, float shellWidth);
            std::vector<Vertex*> GetVertices() { return shellVertices;
            }
            std::vector<Face*> getFaces() { return shellFaces; }

        private:
            struct ShellVerticesInfo
            {
                Vertex* topVert;
                int topIndex;

                Vertex* currVert;
                int currIndex;
            };
            bool flag = false;
            DSMesh currMesh;
            Face* targetFace;
            float getAngle(Vector3 a, Vector3 b);
            void generateNewVertices(Vertex* vertex, float height);
            Vector3 crossProduct(Vector3 vectorA, Vector3 vectorB);
            void generateNewFacesVertices(Face* face, float height, float width);
            void pushBackFaces(Face* face);
            void closeFace(Face* face);
            Vector3 getEdgeVector(Edge* edge, Vertex* vertex);
            void generateNewFaces(Face* face, bool needGrid, bool needShell);
            std::vector<ShellVerticesInfo> newVertices;

            //Private variables, vectors that denote the newest created Vertices and Faces
            std::vector<Vertex*> shellVertices;
            std::vector<Face*> shellFaces;

            //denoting the number of vertices per face in the original mesh
            std::vector<int> faceVertices; // no of vertices per face
            std::vector<int> vertexEdges; // no of edges per (new) vertex

            //Below, this is the number of faces that are new. This records information on 
            //the new vertices that make up each new face, as well as their information 
            std::vector<std::unordered_map<int, ShellVerticesInfo>> newFaceVertices;

            std::vector<Vertex*> newVertexList;

            Vertex* addPoint(Vector3 vector);

	};
}
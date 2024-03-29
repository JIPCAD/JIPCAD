#include "DataStructureMeshToQGeometry.h"
#ifdef __ARM_ARCH
#include <Qt3DRender>
#include <QBuffer>
#include <Qt3DCore>
#else
#include <Qt3DRender/QBuffer>
#endif
#include <iostream>

namespace Nome
{
#define VERT_COLOR 1.0f, 1.0f, 1.0f // normalized: 255 pixel value coresponds to 1
#define VERT_SEL_COLOR 0, 1.0f, 0

    // Project SwitchDS
CDataStructureMeshToQGeometry::CDataStructureMeshToQGeometry(
    const DSMesh& fromMesh,
    std::array<float, 3>& InstanceColor,
    std::array<float, 3>& InstanceBackColor,
    bool bGenPointGeometry, bool renderBackFace, bool renderFrontFace)
{
    // Per face normal, thus no shared vertices between faces
    struct CVertexData
    {
        std::array<float, 3> Pos; // a float is 4 bytes
        std::array<float, 3> Normal;
        std::array<float, 3> faceColor; // Randy added this on 12/12 to handle face entity coloring

        void SendToBuilder(CGeometryBuilder2& builder) const
        {
            builder.Ingest(Pos[0], Pos[1], Pos[2]);
            builder.Ingest(Normal[0], Normal[1], Normal[2]);
            builder.Ingest(faceColor[0], faceColor[1], faceColor[2]);
        }

    };
    const uint32_t stride = sizeof(CVertexData);
    static_assert(stride == 36, "Vertex data size isn't as expected");
    QByteArray bufferArray;
    #ifdef __ARM_ARCH
    CAttribute2 attrPos { bufferArray, offsetof(CVertexData, Pos), stride,
                         Qt3DCore::QAttribute::Float, 3 };
    CAttribute2 attrNor { bufferArray, offsetof(CVertexData, Normal), stride,
                         Qt3DCore::QAttribute::Float, 3 };
    CAttribute2 attrfaceColor { bufferArray, offsetof(CVertexData, faceColor), stride,
                               Qt3DCore::QAttribute::Float, 3 };
    #else 
    CAttribute2 attrPos { bufferArray, offsetof(CVertexData, Pos), stride,
                         Qt3DRender::QAttribute::Float, 3 };
    CAttribute2 attrNor { bufferArray, offsetof(CVertexData, Normal), stride,
                         Qt3DRender::QAttribute::Float, 3 };
    CAttribute2 attrfaceColor { bufferArray, offsetof(CVertexData, faceColor), stride,
                               Qt3DRender::QAttribute::Float, 3 };
    #endif
    CGeometryBuilder2 builder;
    builder.AddAttribute(&attrPos);
    builder.AddAttribute(&attrNor);
    builder.AddAttribute(&attrfaceColor);

    for (auto fIt = fromMesh.faceList.begin(); fIt < fromMesh.faceList.end(); fIt++)
    {
        CVertexData v0{}, vPrev{}, vCurr{};
        int faceVCount = 0;
        std::array<float, 3> potentialFaceColor = InstanceColor;
        std::array<float, 3> backFaceColor = InstanceBackColor;
        std::array<float, 3> SelectedFaceColor {.7, .7, .7};
        Face* currFace = (*fIt);
        if (!currFace->surfaceName.empty())
        {
            potentialFaceColor = currFace->color;
        }
        if (!currFace->backfaceName.empty())
        {
            backFaceColor = currFace->backcolor;
        }

        Edge* firstEdge = currFace->oneEdge;
        Edge* currEdge = firstEdge;
        Vertex* currVert;
        do
        { // TODO: BUG IS DUE TO NON MANIFOLD FACES MAKE THE ITERATION GET STUCK.
          // TRY COMMENTING OUT TORUS VS COMMENTING OUT TORUS KNOT
            if (currFace == currEdge->fa)
            {
                currVert = currEdge->vb;
                currEdge = currEdge->nextVbFa;
            }
            else if (currFace == currEdge->fb)
            {
                if (currEdge->mobius)
                {
                    currVert = currEdge->vb;
                    currEdge = currEdge->nextVbFb;
                }
                else
                {
                    currVert = currEdge->va;
                    currEdge = currEdge->nextVaFb;
                }
            }
            currVert->position;


            // Below is old stuff, added into do wwhile loop
            const auto& posVec = currVert->position;
            if (faceVCount == 0) // first vert
            {
                const auto& posVec = currVert->position;
                v0.Pos = { posVec.x, posVec.y, posVec.z };
                const auto& fnVec = currFace->normal; // 1/28 i think currFace->normal is not set: currFace->normal; //
                               // fromMesh.normal(*fIter);
                v0.Normal = { fnVec.x, fnVec.y, fnVec.z };
                if (currFace->selected)
                    v0.faceColor = SelectedFaceColor;
                else
                    v0.faceColor = potentialFaceColor;
            }
            else if (faceVCount == 1) // second vert
            {
                const auto& posVec = currVert->position;
                vPrev.Pos = { posVec.x, posVec.y, posVec.z };
                const auto& fnVec = currFace->normal; // fromMesh.normal(*fIter);
                vPrev.Normal = { fnVec.x, fnVec.y, fnVec.z };
                if (currFace->selected)
                    vPrev.faceColor = SelectedFaceColor;
                else
                    vPrev.faceColor = potentialFaceColor;
            }
            else // remaining 3rd, 4th (if a quad face), and any additional polygon vertices. For
                 // the 4th vert and beyond, we send to builder again, creating another triangle.
            {
                // Note: this is how we "triangulate" the meshes. It's just a visual triangulation.
                // The winged data structure was used for the added face, not the triangulated
                // one passed into the shader.
                const auto& posVec = currVert->position;
                vCurr.Pos = { posVec.x, posVec.y, posVec.z };
                const auto& fnVec = currFace->normal; // fromMesh.normal(*fIter);
                vCurr.Normal = { fnVec.x, fnVec.y, fnVec.z };
                if (currFace->selected)
                    vCurr.faceColor = SelectedFaceColor;
                else
                    vCurr.faceColor = potentialFaceColor;
                if (renderFrontFace)
                {
                    v0.SendToBuilder(builder);
                    vPrev.SendToBuilder(builder);
                    vCurr.SendToBuilder(builder);
                }
                if (renderBackFace)
                {
                    auto temp = v0.faceColor;
                    v0.faceColor = backFaceColor;
                    vCurr.faceColor = backFaceColor;
                    vPrev.faceColor = backFaceColor;
                    v0.SendToBuilder(builder);
                    vCurr.SendToBuilder(builder);
                    vPrev.SendToBuilder(builder);
                    v0.faceColor = temp;
                    vCurr.faceColor = temp;
                    vPrev.faceColor = temp;
                }

                vPrev = vCurr;
            }
            faceVCount++;
        } while (currEdge != firstEdge);
    }
    #ifdef __ARM_ARCH
    Geometry = new Qt3DCore::QGeometry();
    auto* buffer = new Qt3DCore::QBuffer(Geometry);
    buffer->setData(bufferArray);

    auto* posAttr = new Qt3DCore::QAttribute(Geometry);
    posAttr->setName(
        Qt3DCore::QAttribute::defaultPositionAttributeName()); // default is vertexPosition. This
                                                                 // is used as input in the .vert
                                                                 // shader
    posAttr->setAttributeType(Qt3DCore::QAttribute::VertexAttribute);
    posAttr->setBuffer(buffer);
    posAttr->setCount(builder.GetVertexCount());
    attrPos.FillInQAttribute(posAttr);
    Geometry->addAttribute(posAttr);

    auto* normAttr = new Qt3DCore::QAttribute(Geometry);
    //std::cout << Qt3DRender::QAttribute::defaultNormalAttributeName().toStdString() << std::endl;
    normAttr->setName(Qt3DCore::QAttribute::defaultNormalAttributeName()); // default is vertexNormal. This is
                                                               // used as input in the .vert shader
    normAttr->setAttributeType(Qt3DCore::QAttribute::VertexAttribute);
    normAttr->setBuffer(buffer);
    normAttr->setCount(builder.GetVertexCount());
    attrNor.FillInQAttribute(normAttr);
    Geometry->addAttribute(normAttr);


    auto* faceColorAttr = new Qt3DCore::QAttribute(Geometry);
    faceColorAttr->setName(Qt3DCore::QAttribute::defaultColorAttributeName());
    faceColorAttr->setAttributeType(Qt3DCore::QAttribute::VertexAttribute);
    faceColorAttr->setBuffer(buffer);
    faceColorAttr->setCount(builder.GetVertexCount());
    attrfaceColor.FillInQAttribute(faceColorAttr);
    Geometry->addAttribute(faceColorAttr);
    #else
    Geometry = new Qt3DRender::QGeometry();
    auto* buffer = new Qt3DRender::QBuffer(Geometry);
    buffer->setData(bufferArray);

    auto* posAttr = new Qt3DRender::QAttribute(Geometry);
    posAttr->setName(
        Qt3DRender::QAttribute::defaultPositionAttributeName()); // default is vertexPosition. This
                                                                 // is used as input in the .vert
                                                                 // shader
    posAttr->setAttributeType(Qt3DRender::QAttribute::VertexAttribute);
    posAttr->setBuffer(buffer);
    posAttr->setCount(builder.GetVertexCount());
    attrPos.FillInQAttribute(posAttr);
    Geometry->addAttribute(posAttr);

    auto* normAttr = new Qt3DRender::QAttribute(Geometry);
    //std::cout << Qt3DRender::QAttribute::defaultNormalAttributeName().toStdString() << std::endl;
    normAttr->setName(Qt3DRender::QAttribute::defaultNormalAttributeName()); // default is vertexNormal. This is
                                                               // used as input in the .vert shader
    normAttr->setAttributeType(Qt3DRender::QAttribute::VertexAttribute);
    normAttr->setBuffer(buffer);
    normAttr->setCount(builder.GetVertexCount());
    attrNor.FillInQAttribute(normAttr);
    Geometry->addAttribute(normAttr);


    auto* faceColorAttr = new Qt3DRender::QAttribute(Geometry);
    faceColorAttr->setName(Qt3DRender::QAttribute::defaultColorAttributeName());
    faceColorAttr->setAttributeType(Qt3DRender::QAttribute::VertexAttribute);
    faceColorAttr->setBuffer(buffer);
    faceColorAttr->setCount(builder.GetVertexCount());
    attrfaceColor.FillInQAttribute(faceColorAttr);
    Geometry->addAttribute(faceColorAttr);
    #endif
    if (bGenPointGeometry)
    {
        #ifdef __ARM_ARCH
        PointGeometry = new Qt3DCore::QGeometry();
        #else 
        PointGeometry = new Qt3DRender::QGeometry();
        #endif

        std::vector<float> pointBufferData;
        uint32_t vertexCount = 0;

        // this is how the vertex is displayed
        std::vector<Vertex*>::iterator vIt;
        for (auto vIt = fromMesh.vertList.begin(); vIt < fromMesh.vertList.end(); vIt++)
        {
            auto currVert = *vIt;
            const auto& pos = currVert->position;

            pointBufferData.push_back(pos.x);
            pointBufferData.push_back(pos.y);
            pointBufferData.push_back(pos.z);

            tc::Vector3 pointColor;
            if (currVert->selected)
            {
                pointColor = { VERT_SEL_COLOR };
            }
            else
            {
                pointColor = { VERT_COLOR };
            }
            pointBufferData.push_back(pointColor.x);
            pointBufferData.push_back(pointColor.y);
            pointBufferData.push_back(pointColor.z);

            vertexCount++;
        }
        QByteArray copyOfBuffer { reinterpret_cast<const char*>(pointBufferData.data()),
                                  static_cast<int>(pointBufferData.size() * sizeof(float)) };
        #ifdef __ARM_ARCH
        auto* buffer = new Qt3DCore::QBuffer(PointGeometry);
        buffer->setData(copyOfBuffer);

        posAttr = new Qt3DCore::QAttribute(PointGeometry);
        posAttr->setName(Qt3DCore::QAttribute::defaultPositionAttributeName());
        posAttr->setAttributeType(Qt3DCore::QAttribute::VertexAttribute);
        posAttr->setBuffer(buffer);
        posAttr->setCount(vertexCount);
        posAttr->setByteOffset(0);
        posAttr->setByteStride(24);
        posAttr->setVertexBaseType(Qt3DCore::QAttribute::Float);
        posAttr->setVertexSize(3);
        PointGeometry->addAttribute(posAttr);

        auto* colorAttr = new Qt3DCore::QAttribute(PointGeometry);
        colorAttr->setName(Qt3DCore::QAttribute::defaultColorAttributeName());
        colorAttr->setAttributeType(Qt3DCore::QAttribute::VertexAttribute);
        colorAttr->setBuffer(buffer);
        colorAttr->setCount(vertexCount);
        colorAttr->setByteOffset(12);
        colorAttr->setByteStride(24);
        colorAttr->setVertexBaseType(Qt3DCore::QAttribute::Float);
        colorAttr->setVertexSize(3);
        PointGeometry->addAttribute(colorAttr);
        #else
        auto* buffer = new Qt3DRender::QBuffer(Qt3DRender::QBuffer::VertexBuffer, PointGeometry);
        buffer->setData(copyOfBuffer);

        posAttr = new Qt3DRender::QAttribute(PointGeometry);
        posAttr->setName(Qt3DRender::QAttribute::defaultPositionAttributeName());
        posAttr->setAttributeType(Qt3DRender::QAttribute::VertexAttribute);
        posAttr->setBuffer(buffer);
        posAttr->setCount(vertexCount);
        posAttr->setByteOffset(0);
        posAttr->setByteStride(24);
        posAttr->setVertexBaseType(Qt3DRender::QAttribute::Float);
        posAttr->setVertexSize(3);
        PointGeometry->addAttribute(posAttr);

        auto* colorAttr = new Qt3DRender::QAttribute(PointGeometry);
        colorAttr->setName(Qt3DRender::QAttribute::defaultColorAttributeName());
        colorAttr->setAttributeType(Qt3DRender::QAttribute::VertexAttribute);
        colorAttr->setBuffer(buffer);
        colorAttr->setCount(vertexCount);
        colorAttr->setByteOffset(12);
        colorAttr->setByteStride(24);
        colorAttr->setVertexBaseType(Qt3DRender::QAttribute::Float);
        colorAttr->setVertexSize(3);
        PointGeometry->addAttribute(colorAttr);
        #endif
    }
}

CDataStructureMeshToQGeometry::~CDataStructureMeshToQGeometry()
{
    if (!Geometry->parent())
        delete Geometry;
    if (PointGeometry && !PointGeometry->parent())
        delete PointGeometry;
}

}
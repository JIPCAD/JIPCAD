#pragma once

#define _USE_MATH_DEFINES
#undef min
#undef max
#include <OpenMesh/Core/Mesh/PolyMesh_ArrayKernelT.hh>
#include "Scene/CustomMeshDataStructure/DataStructureMesh.h" // Project ChangeDS


#include <QByteArray>
#ifdef __ARM_ARCH
#include <Qt3DRender>
#include <Qt3DCore>
#else
#include <Qt3DRender/QAttribute>
#include <Qt3DRender/QGeometry>
#endif
#include <unordered_set>
#include <vector>

namespace Nome
{
typedef OpenMesh::PolyMesh_ArrayKernelT<> CMeshImpl;
typedef Mesh DSMesh; // Project SwitchDS

class CAttribute2
{
public:
    QByteArray& Buffer;
    uint32_t ByteOffset;
    uint32_t ByteStride;
    #ifdef __ARM_ARCH
    Qt3DCore::QAttribute::VertexBaseType Type;
    #else
    Qt3DRender::QAttribute::VertexBaseType Type;
    #endif
    uint32_t Size;
    #ifdef __ARM_ARCH
    CAttribute2(QByteArray& buffer, uint32_t byteOffset, uint32_t byteStride,
               Qt3DCore::QAttribute::VertexBaseType type, uint32_t size)
        : Buffer(buffer)
        , ByteOffset(byteOffset)
        , ByteStride(byteStride)
        , Type(type)
        , Size(size)
    #else
    CAttribute2(QByteArray& buffer, uint32_t byteOffset, uint32_t byteStride,
               Qt3DRender::QAttribute::VertexBaseType type, uint32_t size)
        : Buffer(buffer)
        , ByteOffset(byteOffset)
        , ByteStride(byteStride)
        , Type(type)
        , Size(size)
    #endif
    {
    }

    [[nodiscard]] int GetAbsByteOffset(uint32_t i) const { return ByteOffset + ByteStride * i; }
    #ifdef __ARM_ARCH
    void FillInQAttribute(Qt3DCore::QAttribute* attr) const
    #else
    void FillInQAttribute(Qt3DRender::QAttribute* attr) const
    #endif
    {
        attr->setByteOffset(ByteOffset);
        attr->setByteStride(ByteStride);
        attr->setVertexBaseType(Type);
        attr->setVertexSize(Size);
    }
};

class CGeometryBuilder2
{
public:
    CGeometryBuilder2() = default;

    void AddAttribute(const CAttribute2* attr) { Attributes.push_back(attr); }

    void Ingest(float x, float y, float z)
    {
        auto* target = Attributes[CurrAttrIndex];

        // Type check for target
        #ifdef __ARM_ARCH
        assert(target->Type == Qt3DCore::QAttribute::Float);
        #else
        assert(target->Type == Qt3DRender::QAttribute::Float);
        #endif
        assert(target->Size == 3);

        // Append the buffer pointed to by target
        const int byteLen = sizeof(float) * 3;
        int begOffset = target->GetAbsByteOffset(CurrVertexIndex);
        if (target->Buffer.size() < begOffset + byteLen)
            target->Buffer.resize(begOffset + byteLen);
        auto* beg = &*target->Buffer.begin() + begOffset;
        auto* p = reinterpret_cast<float*>(beg);
        *p++ = x;
        *p++ = y;
        *p = z;

        AdvanceIndices();
    }

    void IngestInt(int x)
    {
        auto* target = Attributes[CurrAttrIndex];

        // Type check for target
        #ifdef __ARM_ARCH
        assert(target->Type == Qt3DCore::QAttribute::Int);
        #else
        assert(target->Type == Qt3DRender::QAttribute::Int);
        #endif
        assert(target->Size == 1);

        // Append the buffer pointed to by target
        const int byteLen = sizeof(int) * 1;
        int begOffset = target->GetAbsByteOffset(CurrVertexIndex);
        if (target->Buffer.size() < begOffset + byteLen)
            target->Buffer.resize(begOffset + byteLen);
        auto* beg = &*target->Buffer.begin() + begOffset;
        auto* p = reinterpret_cast<float*>(beg);
        *p++ = x;

        AdvanceIndices();
    }

    [[nodiscard]] uint32_t GetVertexCount() const { return CurrVertexIndex; }

protected:
    void AdvanceIndices()
    {
        CurrAttrIndex++;
        if (CurrAttrIndex == Attributes.size())
        {
            CurrAttrIndex = 0;
            CurrVertexIndex++;
        }
    }

private:
    std::vector<const CAttribute2*> Attributes;
    uint32_t CurrVertexIndex = 0;
    uint32_t CurrAttrIndex = 0;
};

class CDataStructureMeshToQGeometry
{
public:
    explicit CDataStructureMeshToQGeometry(
        const DSMesh& fromMesh,
        std::array<float, 3>& InstanceColor,
        std::array<float, 3>& InstanceBackColor,
        bool bGenPointGeometry = false,
        bool renderBackFace = false,
        bool renderFrontFace = true);

    ~CDataStructureMeshToQGeometry();

    CDataStructureMeshToQGeometry(const CDataStructureMeshToQGeometry&) = delete;
    CDataStructureMeshToQGeometry(CDataStructureMeshToQGeometry&&) = delete;
    CDataStructureMeshToQGeometry& operator=(const CDataStructureMeshToQGeometry&) = delete;
    CDataStructureMeshToQGeometry& operator=(CDataStructureMeshToQGeometry&&) = delete;
#ifdef __ARM_ARCH
    [[nodiscard]] Qt3DCore::QGeometry* GetGeometry() const { return Geometry; }
    [[nodiscard]] Qt3DCore::QGeometry* GetPointGeometry() const { return PointGeometry; }
private:
    Qt3DCore::QGeometry* Geometry = nullptr;
    Qt3DCore::QGeometry* PointGeometry = nullptr;
#else
    [[nodiscard]] Qt3DRender::QGeometry* GetGeometry() const { return Geometry; }
    [[nodiscard]] Qt3DRender::QGeometry* GetPointGeometry() const { return PointGeometry; }
private:
    Qt3DRender::QGeometry* Geometry = nullptr;
    Qt3DRender::QGeometry* PointGeometry = nullptr;
#endif
};

}
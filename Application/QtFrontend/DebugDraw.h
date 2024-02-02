#pragma once
#include <Qt3DRender/Qt3DRender>
#ifdef __ARM_ARCH
#include <Qt3DCore>
#endif
#include <Scene/RendererInterface.h>
#include <memory>

namespace Nome
{

class CDebugDraw : public Qt3DCore::QNode, public IDebugDraw
{
public:
    explicit CDebugDraw(Qt3DCore::QNode* parent = nullptr);

    void Reset();
    void Commit();
    #ifdef __ARM_ARCH
    [[nodiscard]] Qt3DCore::QGeometry* GetLineGeometry() const { return LineGeometry.get(); }
    #else
    [[nodiscard]] Qt3DRender::QGeometry* GetLineGeometry() const { return LineGeometry.get(); }
    #endif
    /// Draw fresh primitives every frame, fairly costly
    void DrawPoint(tc::Vector3 pos) override;
    void LineSegment(tc::Vector3 from, tc::Vector3 to) override;

    void DrawPoint(tc::Vector3 pos, tc::Color color) override;
    void LineSegment(tc::Vector3 from, tc::Color fromColor, tc::Vector3 to,
                     tc::Color toColor) override;

private:
    QByteArray PointData;
    QByteArray LineData;
    #ifdef __ARM_ARCH
    Qt3DCore::QBuffer* PointBuffer;
    Qt3DCore::QBuffer* LineBuffer;
    std::unique_ptr<Qt3DCore::QGeometry> LineGeometry;
    #else
    Qt3DRender::QBuffer* PointBuffer;
    Qt3DRender::QBuffer* LineBuffer;
    std::unique_ptr<Qt3DRender::QGeometry> LineGeometry;
    #endif
};

}

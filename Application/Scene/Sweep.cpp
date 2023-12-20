#include "Sweep.h"
#include <vector>
#include "SweepControlPoint.h"

#undef M_PI

namespace Nome::Scene
{

DEFINE_META_OBJECT(CSweep)
{
    BindPositionalArgument(&CSweep::CrossSection, 1, 0);
    BindPositionalArgument(&CSweep::Path, 2, 0);
    BindNamedArgument(&CSweep::Azimuth, "azimuth", 0);
    BindNamedArgument(&CSweep::Twist, "twist", 0);
    BindNamedArgument(&CSweep::bBeginCap, "begincap", 0);
    BindNamedArgument(&CSweep::bEndCap, "endcap", 0);
    BindNamedArgument(&CSweep::bReverse, "reverse", 0);
    BindNamedArgument(&CSweep::bMintorsion, "mintorsion", 0);
    BindNamedArgument(&CSweep::bCutBegin, "cutbegin", 0);
    BindNamedArgument(&CSweep::bCutEnd, "cutend", 0);
}

// do cross product with vectorA and vectorB
Vector3 CSweepMath::crossProduct(Vector3 vectorA, Vector3 vectorB)
{
    return Vector3(vectorA.y * vectorB.z - vectorA.z * vectorB.y,
                   vectorA.z * vectorB.x - vectorA.x * vectorB.z,
                   vectorA.x * vectorB.y - vectorA.y * vectorB.x);
}

// get the angle of two vectors (unsigned)
float CSweepMath::getAngle(Vector3 vectorA, Vector3 vectorB)
{
    float value = vectorA.DotProduct(vectorB) / vectorA.Length() / vectorB.Length();
    float epsilon = 1e-4;
    if (fabs(value - 1) < epsilon) { return 0; }
    if (fabs(value + 1) < epsilon) { return (float)tc::M_PI; }
    if (fabs(value) < epsilon) {return (float)tc::M_PI / 2; }

    return acosf(vectorA.DotProduct(vectorB) / vectorA.Length() / vectorB.Length()) ;
}

// get the perpendicular vector from vectorB to vectorA
// i.e. the difference between vectorA and the projection of vectorA onto vectorB
Vector3 CSweepMath::getPerpendicularVector(Vector3 vectorA, Vector3 vectorB)
{
    float epsilon = 1e-4;
    float scale = cosf(getAngle(vectorA, vectorB));

    if (fabs(scale) < epsilon) { return vectorA.Normalized(); }

    return vectorA - vectorB.Normalized() * vectorA.Length() * cosf(getAngle(vectorA, vectorB));
}

// multiply a vector to a matrix
Vector3 CSweepMath::vectorMultiplyMatrix(Vector3 vector, float matrix[3][3])
{
    float x = vector.x * matrix[0][0] + vector.y * matrix[1][0] + vector.z * matrix[2][0];
    float y = vector.x * matrix[0][1] + vector.y * matrix[1][1] + vector.z * matrix[2][1];
    float z = vector.x * matrix[0][2] + vector.y * matrix[1][2] + vector.z * matrix[2][2];

    return Vector3(x, y, z);
}

// calculate the rotation angle from vectorA to vectorB (signed)
float CSweepMath::calculateRotateAngle(Vector3 vectorA, Vector3 vectorB, Vector3 T)
{
    float angle = getAngle(vectorA, vectorB);
    T.Normalize();

    Vector3 perpendicular = getPerpendicularVector(vectorB, vectorA).Normalized();
    Vector3 direction = crossProduct(T, vectorA).Normalized();

    if (direction.DotProduct(perpendicular) >= 0)
        return angle;
    else
        return -angle;
}

// calculate the rotation matrix to get from vectorA to vectorB
Matrix3 CSweepMath::calculateRotationMatrix(Vector3 vectorA, Vector3 vectorB)
{
    Quaternion rot = Quaternion(vectorA, vectorB);
    return rot.RotationMatrix();
}


Vector3 CSweepMath::getDefaultN(Vector3 T)
{
    if (T.Normalized() != Vector3(1, 0, 0) && T.Normalized() != Vector3(-1, 0, 0))
        return crossProduct(T, Vector3(1, 0, 0));
    else
        return crossProduct(T, Vector3(0, 1, 0));
}

bool CSweepMath::isAtSameLine(Vector3 vectorA, Vector3 vectorB)
{
    Vector3 zero = Vector3(0, 0, 0);
    if (vectorA.Equals(zero) || vectorB.Equals(zero))
        return true;

    Vector3 normVA = vectorA.Normalized();
    Vector3 normVB = vectorB.Normalized();

    if (normVA.Equals(normVB) || normVA.Equals(-normVB))
        return true;
    else
        return false;
}

bool CSweepMath::isAtSameDirection(Vector3 vectorA, Vector3 vectorB)
{
    return vectorA.DotProduct(vectorB) >= 0;
}

void CSweep::drawCrossSection(std::vector<Vector3> crossSection, Vector3 center,
                              Vector3 T, Vector3 N, float rotateAngle, float angle,
                              Vector3 controlScale, int index, bool shouldReverse,
                              bool hasReverseFlag, bool shouldCorrectRotation) {

    // Flip rotation orientation
    if (shouldReverse ^ hasReverseFlag) {
        rotateAngle *= -1;
    }

    // Perform cross-section rotation correction
    if (shouldCorrectRotation) {
        rotateAngle += tc::M_PI;
    }

    Vector3 B = Math.crossProduct(T, N);

    N.Normalize();
    B.Normalize();

    float scaleN = 1 / cosf(angle / 2);

    for (size_t i = 0; i < crossSection.size(); i++)
    {
        // make rotations
        Vector3 point = bReverse ^ (shouldReverse ^ hasReverseFlag) ?
                        crossSection[crossSection.size() - i - 1] : crossSection[i];
        float x = point.x * controlScale.x * cosf(rotateAngle) -
                  point.y * controlScale.y * sinf(rotateAngle);
        float y = point.x * controlScale.x * sinf(rotateAngle) +
                  point.y * controlScale.y * cosf(rotateAngle);
        // do transform
        Vector3 transformVector = N * x * scaleN + B * y ;
        // add offset
        Vector3 curVertex = center + transformVector;


        Vertex *vert = AddVertex(
            "v" + std::to_string(index) + "_" + std::to_string(i),
            { curVertex.x, curVertex.y, curVertex.z }
        );

        if (crossSection.size() == 1)
        {
            singlePointLineStrip.push_back(vert);
        }
    }
}

void CSweep::drawCap(std::vector<Vector3> crossSection, int crossIndex,
                     int faceIndex, bool shouldFlip)
{
    size_t size = crossSection.size();
    std::vector<std::string> capFace;

    for (size_t i = 0; i < size; i++) {
        if (shouldFlip)
        {
            capFace.push_back(
                "v" + std::to_string(crossIndex) + "_" +
                std::to_string((crossSection.size() - i - 1) % crossSection.size())
            );
        }
        else
        {
            capFace.push_back(
                "v" + std::to_string(crossIndex) + "_" + std::to_string(i % crossSection.size())
            );
        }
    }
    AddFace("f" + std::to_string(faceIndex), capFace);
}

std::vector<std::vector<Vector3>> CSweep::get_crossSections()
{
    return crossSections;
}

std::vector<Vector3> CSweep::get_controlScales()
{
    return controlScales;
}
bool CSweep::get_bCutBegin()
{
    return bCutBegin;
}

bool CSweep::get_bCutEnd()
{
    return bCutEnd;
}

void CSweep::MarkDirty()
{
    Super::MarkDirty();
    Sweep.MarkDirty();
}

void CSweep::UpdateEntity()
{
    if (!IsDirty())
        return;

    Super::UpdateEntity();

    // the last point is control arguments
    CSweepPathInfo *pathInfo = Path.GetValue(nullptr);
    CSweepPathInfo *crossSectionInfo = CrossSection.GetValue(nullptr);

    if (pathInfo == nullptr || crossSectionInfo == nullptr) { return; }

    // detect if is a closed polyline
    bool isClosed = pathInfo->IsClosed;
    size_t numPoints = pathInfo->Positions.size();
    size_t numCutPoints = 0;
    if (bCutBegin) { numCutPoints++; }
    if (bCutEnd) { numCutPoints++; }

    // if the number of points cannot build a model, exit
    if ((!isClosed && numPoints - numCutPoints < 2) || (isClosed && numPoints - numCutPoints < 3) ||
        crossSectionInfo->Positions.size() < 1) { return; }

    std::vector<Vector3> points;
    // Normal vectors of each paths
    std::vector<Vector3> Ns;
    // Rotation angles of each paths
    std::vector<float> angles;
    // Cross sections
    std::vector<Vector3> crossSection;
    crossSections.clear();
    // Scales from control points
    controlScales.clear();
    // Reverse from control points
    std::vector<bool> controlReverses;
    // Path for single-point crossSection
    singlePointLineStrip.clear();

    std::string name = GetName();

    // Polygon's sides;
    float twist =  Twist.GetValue(0) * (float)tc::M_PI / 180 / (numPoints - 1);
    float azimuth = Azimuth.GetValue(0) * (float)tc::M_PI / 180;

    for (size_t i = 0; i < crossSectionInfo->Positions.size(); i++)
    {
        CVertexInfo *point = crossSectionInfo->Positions[i];
        crossSection.push_back(Vector3(point->Position.x, point->Position.y, point->Position.z));
    }

    for (size_t i = 0; i < numPoints; i++)
    {
        CVertexInfo *point = pathInfo->Positions[i];

        points.push_back(Vector3(point->Position.x, point->Position.y, point->Position.z)); // current point
        crossSections.push_back(crossSection);

        // not mintorsion
        if (!bMintorsion && i > 0)
        {
            Ns.push_back(Math.getDefaultN(points[i] - points[i - 1]));
            angles.push_back(0);
        }
        else if (i > 1)
        {
            Vector3 prevVector = points[i] - points[i - 1];
            Vector3 curVector = points[i - 1] - points[i - 2];

            // Three points in a single line.
            if (Math.isAtSameLine(prevVector, curVector))
            {
                Vector3 N;

                if (i == 2)
                {
                    N = Math.getDefaultN(prevVector);
                    Ns.push_back(N);
                }
                else
                    N = Math.getPerpendicularVector(Ns[i - 2], prevVector);

                angles.push_back(0);
                Ns.push_back(N);
            }
            else
            {
                Vector3 sumVector = prevVector.Normalized() - curVector.Normalized();
                Vector3 curPerpendicular = Math.getPerpendicularVector(sumVector, prevVector);
                Vector3 prevPerpendicular = Math.getPerpendicularVector(sumVector, -curVector);

                /* let the normal vector be the prevPerpendicular vector
                 * in this case, the rotation angle is 0 */
                if (i == 2) { Ns.push_back(prevPerpendicular); }
                // calculate the rotation angle of each joint
                angles.push_back(Math.calculateRotateAngle(Ns[i - 2], prevPerpendicular,
                                                      points[i - 1] - points[i - 2]));
                // set the current normal vector
                Ns.push_back(curPerpendicular);
            }
        }

        controlScales.push_back(Vector3(1.0f, 1.0f, 1.0f));
        controlReverses.push_back(false);
    }

    if (numPoints == 2) { Ns.push_back(Math.getDefaultN(points[1] - points[0])); }

    if (isClosed)
    {
        Vector3 prevVector = points[1] - points[0];
        Vector3 curVector = points[0] - points[numPoints - 2];

        if (!bMintorsion)
        {
            Ns.push_back(Math.getDefaultN(prevVector));
            angles.push_back(0);
        }
        // Three points in a single line.
        else if (Math.isAtSameLine(prevVector, curVector))
        {
            Vector3 N = Math.getPerpendicularVector(Ns[numPoints - 2], prevVector);

            angles.push_back(0);
            Ns.push_back(N);
        }
        else
        {
            Vector3 sumVector = prevVector.Normalized() - curVector.Normalized();
            Vector3 curPerpendicular = Math.getPerpendicularVector(sumVector, prevVector);
            Vector3 prevPerpendicular = Math.getPerpendicularVector(sumVector, -curVector);

            angles.push_back(Math.calculateRotateAngle(Ns[numPoints - 2], prevPerpendicular,
                                                  points[numPoints - 1] - points[numPoints - 2]));
            Ns.push_back(curPerpendicular);
            // add the rotation angle of the closed joint
            angles[0] += Math.calculateRotateAngle(curPerpendicular, Ns[0], prevVector);
        }
    }
    angles.push_back(0);
    angles.push_back(0);

    // get the result rotation angles
    for (size_t i = numPoints - 2; i >= 1; i--) { angles[i - 1] += angles[i]; }

    // add rotation
    for (size_t i = 0; i < numPoints; i++)
    {
        CSweepControlPointInfo* SI = dynamic_cast<CSweepControlPointInfo*>(pathInfo->Positions[i]);

        if (SI != nullptr) {

            controlScales[i].x *= SI->Scale.x;
            controlScales[i].y *= SI->Scale.y;
            controlReverses[i] = SI->Reverse;

            float rotAmount = SI->Rotate.z * tc::M_PI / 180;
            angles[i] += rotAmount;

            if (SI->CrossSection != NULL)
            {
                CSweepPathInfo *sectionInfo = SI->CrossSection;
                std::vector<Vector3> section;
                for (size_t k = 0; k < sectionInfo->Positions.size(); k++)
                {
                    CVertexInfo *point = sectionInfo->Positions[k];
                    section.push_back(Vector3(point->Position.x,
                                point->Position.y, point->Position.z));
                }
                crossSections[i] = section;
            }
        }

        angles[i] += azimuth;
    }

    // add twist to angles
    for (size_t i = 0; i < numPoints; i++) {
        angles[i] += i * twist;
    }

    // the count of drawing segments
    int segmentCount = 0;

    Vector3 T, N;
    bool shouldFlip = controlReverses[0];

    // first point
    if (!bCutBegin)
    {
        if (!isClosed)
        {
            T = points[1] - points[0];
            N = Ns[0];

            float angle = 0;

            if (pathInfo->IsBSpline) {
                angle = Math.getAngle(T, pathInfo->BspTangents[0]);
                Matrix3 rot = Math.calculateRotationMatrix(T, pathInfo->BspTangents[0]);
                N = rot * N;
                T = pathInfo->BspTangents[0];
            }

            // generate points in a circle perpendicular to the curve at the current point
            drawCrossSection(crossSections[0], points[0], T, N, angles[0], angle, controlScales[0],
                             ++segmentCount, shouldFlip, controlReverses[0], false);
        }
        else
        {
            Vector3 prevVector = (points[1] - points[0]).Normalized();
            Vector3 curVector = (points[0] - points[numPoints - 2]).Normalized();
            float angle;

            if (Math.isAtSameLine(prevVector, curVector))
            {
                N = Ns[Ns.size() - 1];
                T = curVector;
                angle = 0;
            }
            else
            {
                N = prevVector - curVector;
                T = prevVector + curVector;
                angle = Math.getAngle(prevVector, curVector);
            }

            drawCrossSection(crossSections[0], points[0], T, N, angles[0], angle, controlScales[0],
                             ++segmentCount, shouldFlip, controlReverses[0], false);
        }
    }

    /**
     * Sweeps with 'reverse' flags may need 180 degree rotations at particular cross-sections.
     * Let's call the reversed point and the points after it (but before the next reversed point)
     * a "reversed section".
     *      e.g. (Sweep 1) A--D--C--B
     *      For this sweep with points defined in order (A B C D), the reversed section is D--C--B.
     * For points in a reversed section, once the angle at a point is not 0 or if the reversal is not colinear,
     * then the need for rotation correction (i.e. shouldCorrectRotation) is flipped from its previous state.
     * Once this change occurs, all subsequent points in the reversed section will also require
     * (or not require) a rotation correction.
     * When the sweep undergoes another reversal and we are in a new reversed section,
     * the canChangeCorrectRotationBool boolean is reset.
     *      e.g. (Sweep 2) A---B
     *                        /
     *                       C
     *      For Sweep 1 above, the reversal at B is colinear.
     *      For Sweep 2 defined with points (A B C), the reversal at B is NOT colinear.
     *
     * Edited by Monica Tang
     */
    bool shouldCorrectRotation = false;
    bool canChangeCorrectRotationBool = false;
    bool isReversalColinear = false;

    for (size_t i = 1; i < numPoints; i++)
    {
        // Flip cross-section for points in an "inside-out" reversed section, but not the reversal point
        shouldFlip ^= controlReverses[i - 1];
        if (controlReverses[i - 1]) {
            canChangeCorrectRotationBool = true;
        }
        // last point
        if (i == numPoints - 1)
        {
            if (!bCutEnd)
            {
                if (isClosed)
                {
                    Vector3 prevVector = (points[1] - points[0]).Normalized();
                    Vector3 curVector = (points[0] - points[numPoints - 2]).Normalized();
                    float angle = (Math.isAtSameLine(prevVector, curVector))
                        ? 0
                        : Math.getAngle(prevVector, curVector);
                    if (canChangeCorrectRotationBool && (angle != 0 || !isReversalColinear)) {
                        shouldCorrectRotation = !shouldCorrectRotation;
                        canChangeCorrectRotationBool = false;
                    }

                    if (Math.isAtSameLine(prevVector, curVector))
                    {
                        N = Ns[i - 1];
                        T = curVector;
                    }
                    else
                    {
                        N = prevVector - curVector;
                        T = prevVector + curVector;
                    }

                    // 0 is perfect.
                    drawCrossSection(crossSections[i], points[i], T, N, angles[i], angle,
                                     controlScales[i], ++segmentCount, shouldFlip, controlReverses[i], shouldCorrectRotation);
                }
                else
                {
                    float angle = 0;
                    N = Ns[i - 1];
                    T = points[i] - points[i - 1];

                    if (pathInfo->IsBSpline) {
                        angle = Math.getAngle(T, pathInfo->BspTangents[1]);
                        Matrix3 rot = Math.calculateRotationMatrix(T, pathInfo->BspTangents[1]);
                        N = rot * N;
                        T = pathInfo->BspTangents[1];
                    }

                    if (canChangeCorrectRotationBool && (angle != 0 || !isReversalColinear)) {
                        shouldCorrectRotation = !shouldCorrectRotation;
                        canChangeCorrectRotationBool = false;
                    }

                    drawCrossSection(crossSections[i], points[i], T, N, angles[i], angle,
                                     controlScales[i], ++segmentCount, shouldFlip, controlReverses[i], shouldCorrectRotation);
                }
            }
        }
        else
        {
            Vector3 prevVector = (points[i + 1] - points[i]).Normalized();
            Vector3 curVector = (points[i] - points[i - 1]).Normalized();
            if (controlReverses[i]) {
                // At the reversal point, negating curVector will generate the cross-section in the correct plane
                curVector = -curVector;
            }
            float angle = (Math.isAtSameLine(prevVector, curVector)) ?
                0 : Math.getAngle(prevVector, curVector);
            if (controlReverses[i]) {
                isReversalColinear = Math.isAtSameLine(prevVector, curVector);
            }
            if (canChangeCorrectRotationBool && (angle != 0 || !isReversalColinear)) {
                shouldCorrectRotation = !shouldCorrectRotation;
                canChangeCorrectRotationBool = false;
            }

            if (Math.isAtSameLine(prevVector, curVector))
            {
                N = Ns[i];
                T = curVector;
            }
            else
            {
                N = prevVector - curVector;
                T = prevVector + curVector;
            }

            drawCrossSection(crossSections[i], points[i], T, N, angles[i], angle,
                             controlScales[i], ++segmentCount, shouldFlip, controlReverses[i], shouldCorrectRotation);
        }
    }

    // Create line curve for single-point crossSection
    if (crossSectionInfo->Positions.size() == 1)
    {
        AddLineStrip("sweep", singlePointLineStrip);
    }
    else
    {
        // Create faces
        for (int k = 0; k < segmentCount - 1; k++)
        {
            for (size_t i = 0; i < crossSection.size() - 1; i++)
            {
                // CCW winding
                // v1_next v1_i
                // v2_next v2_i
                std::vector<std::string> upperFace;

                int next_k = (k + 1) % segmentCount;
                int next = (i + 1) % crossSection.size();
                if (k == 0 && isClosed && shouldFlip)
                {
                    int reverse_i = (2 * crossSection.size() - i - 1) % crossSection.size();
                    int reverse_next = (2 * crossSection.size() - next - 1) % crossSection.size();
                    upperFace = {
                        "v" + std::to_string(next_k + 1) + "_" + std::to_string(reverse_i),
                        "v" + std::to_string(next_k + 1) + "_" + std::to_string(reverse_next),
                        "v" + std::to_string(k + 1) + "_" + std::to_string(next),
                        "v" + std::to_string(k + 1) + "_" + std::to_string(i),
                    };
                }
                else
                {
                    upperFace = {
                        "v" + std::to_string(next_k + 1) + "_" + std::to_string(next),
                        "v" + std::to_string(next_k + 1) + "_" + std::to_string(i),
                        "v" + std::to_string(k + 1) + "_" + std::to_string(i),
                        "v" + std::to_string(k + 1) + "_" + std::to_string(next),
                    };
                }
                AddFace("f" + std::to_string(k) + "_" + std::to_string(i), upperFace);
            }
        }

        // Create caps
        // Cannot create caps when the path is closed
        if (isClosed || crossSectionInfo->Positions.size() < 3)
        {
            Sweep.UpdateValue(this);
            return;
        }
        if (bBeginCap)
        {
            drawCap(crossSections[0], 1, segmentCount++ - 1, true);
        }
        if (bEndCap)
        {
            int index = crossSections.size() - numCutPoints - 1;
            drawCap(crossSections[index], index + 1, segmentCount++ - 1, false);
        }
    }
    Sweep.UpdateValue(this);
}

}

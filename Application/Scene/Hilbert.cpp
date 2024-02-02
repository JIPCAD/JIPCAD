#include "Hilbert.h"

namespace Nome::Scene
{

DEFINE_META_OBJECT(CHilbert)
{
    BindPositionalArgument(&CHilbert::Iteration, 1, 0);
    BindPositionalArgument(&CHilbert::Size, 1, 1);
}

void CHilbert::MarkDirty()
{
    // Mark this entity dirty
    Super::MarkDirty();

    // And also mark the Face output dirty
    Hilbert.MarkDirty();
}

// ALGORITHM FOR HILBERT CURVE TAKEN FROM http://blog.marcinchwedczuk.pl/iterative-algorithm-for-drawing-hilbert-curve

int last2bits(int n)
{
    return n & 3;
}

int * hindex2xy(int hindex, int bigN, int size) {
    int positions[4][2] = {
        {0, 0},
        {0, 1},
        {1, 1},
        {1, 0}
    };
    int tmp [2];
    tmp[0] = positions[last2bits(hindex)][0];
    tmp[1] = positions[last2bits(hindex)][1];
    hindex = hindex >> 2;

    int x = tmp[0];
    int y = tmp[1];

    int temp;

    for (int n = 4; n <= bigN; n *= 2) {
        int n2 = n/2;

        switch (last2bits(hindex)) {
            case 0:
                temp = x;
                x = y;
                y = temp;
                break;
            case 1:
                x = x;
                y = y + n2;
                break;
            case 2:
                x = x + n2;
                y = y + n2;
                break;
            case 3:
                temp = y;
                y = (n2-1) - x;
                x = (n2-1) - temp;
                x = x + n2;
                break;
        }

        hindex = hindex >> 2;
    }

    int * result = new int[2];
    result[0] = x * size;
    result[1] = y * size;
    return result;
}

// Make a Hilbert curve with the given iteration count

// void Hilb(int x, int y, int xi, int xj, int yi, int yj, int n, int i, std::vector<Vertex*> vertArray, std::vector<CVertexInfo *> positions) {
//         if (n <= 0) {
//             // Add the next point
//             Vector3 pos = Vector3((x + (xi  + yi) / 2), (y + (xj + yj) / 2));
//             vertArray.push_back(AddVertex("v" + std::to_string(i), pos));
//             CVertexInfo *point = new CVertexInfo();
//             point->Position = pos;
//             positions.push_back(point);
//             return;
//         } else {
//             Hilb(x, y, yi/2, yj/2, xi/2, xj/2, n - 1, i + 1, vertArray, positions);
//             Hilb(x + xi/2, y + xj/2, xi/2, xj/2, yi/2, yj/2, n - 1, i + 1, vertArray, positions);
//             Hilb(x + xi/2 + yi/2, y+xj/2+yj/2, xi/2, xj/2, yi/2, yj/2, n - 1, i + 1, vertArray, positions);
//             Hilb(x + xi/2+yi, y+xj/2+yj, -yi/2, -yj/2, -xi/2, -xj/2, n - 1, i + 1, vertArray, positions);
//         }
// }

void CHilbert::UpdateEntity()
{
    if (!IsDirty())
        return;
    Super::UpdateEntity();

    std::vector<Vertex*> vertArray;
    std::vector<CVertexInfo *> positions;

    int n = Iteration.GetValue(1.0f);
    int x = 0;
    int y = 0;
    int dx = Size.GetValue(1.0f);
    int dy = Size.GetValue(1.0f);

    // Hilb(0, 0, dx, 0, 0, dy, n, 0, vertArray, positions);

    int prev[2];
    int curr[2];

    for (int i = 0; i < n*n; i++){
        curr[0] = hindex2xy(i, n, dx)[0];
        curr[1] = hindex2xy(i, n, dx)[1];

        Vector3 pos = Vector3(curr[0], curr[1], 0.0f);
        vertArray.push_back(AddVertex("v" + std::to_string(i), pos));
        CVertexInfo *point = new CVertexInfo();
        point->Position = pos;
        positions.push_back(point);

        prev[0] = curr[0];
        prev[1] = curr[1];

    }

    AddLineStrip("hilbert-line", vertArray);

    SI.Positions = positions;
    SI.Name = GetName();
    SI.IsClosed = false;
    Hilbert.UpdateValue(&SI);
    SetValid(true);
}

}
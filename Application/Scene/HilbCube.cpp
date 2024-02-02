#include "HilbCube.h"

namespace Nome::Scene
{

DEFINE_META_OBJECT(CHilbCube)
{
    BindPositionalArgument(&CHilbCube::Iteration, 1, 0);
    BindPositionalArgument(&CHilbCube::Size, 1, 1);
}

void CHilbCube::MarkDirty()
{
    // Mark this entity dirty
    Super::MarkDirty();

    // And also mark the Face output dirty
    HilbCube.MarkDirty();
}

// ALGORITHM FOR HILBERT CURVE TAKEN FROM http://blog.marcinchwedczuk.pl/iterative-algorithm-for-drawing-hilbert-curve

int last3bits(int n)
{
    return n & 7;
}

int decimalToOctal(int n) {
    int remainder; 
    int octal = 0, i = 1;
   
    while(n != 0) {
        remainder = n%8;
        n = n/8;
        octal = octal + (remainder*i);
        i = i*10;
    }
    return octal;
}

// double * index2coords(int index) {
//     int octal = decimalToOctal(index);
//     double x = 0;
//     double y = 0;
//     double z = 0;

//     double temp;
//     double temp2;

//     while (octal != 0) {
//         int lastDigit = octal % 10;
//         if (lastDigit == 0) {
//             x = x/2;
//             temp = y;
//             y = z/2;
//             z = temp/2;
//         } else if (lastDigit == 1) {
//             temp = x;
//             x = z/2;
//             y = y/2 + 0.5;
//             z = temp/2;
//         } else if (lastDigit == 2) {
//             x = x/2 + 0.5;
//             y = y/2 + 0.5;
//             z = z/2;
//         } else if (lastDigit == 3) {
//             temp = x;
//             x = z/2 + 0.5;
//             temp2 = y;
//             y = -temp/2 + 0.5;
//             z = -temp2/2 + 0.5;
//         } else if (lastDigit == 4) {
//             temp = x;
//             x = -z/2 + 1;
//             temp2 = y;
//             y = -temp/2 + 0.5;
//             z = temp2/2 + 0.5;
//         } else if (lastDigit == 5) {
//             x = x/2 + 0.5;
//             y = y/2 + 0.5;
//             z = z/2 + 0.5;
//         } else if (lastDigit == 6) {
//             temp = x;
//             x = -z/2 + 0.5;
//             y = y/2 + 0.5;
//             z = -temp/2 + 1;
//         } else if (lastDigit == 7) {
//             x = x/2;
//             temp = y;
//             y = -z/2 + 0.5;
//             z = -temp/2 + 1;
//         }
//         octal = octal / 10;
//     }

//     double * result = new double[3];
//     result[0] = x;
//     result[1] = y;
//     result[2] = z;
//     return result;
// }

int compact1By2(int x) { // https://fgiesen.wordpress.com/2009/12/13/decoding-morton-codes/
    x &= 0x09249249;                 
    x = (x ^ (x >> 2)) & 0x030c30c3;
    x = (x ^ (x >> 4)) & 0x0300f00f;
    x = (x ^ (x >> 8)) & 0xff0000ff;
    x = (x ^ (x >> 16)) & 0x000003ff;
    return x;
}

int hilberToMortonTable[96] = {
    48, 33, 35, 26, 30, 79, 77, 44,
    78, 68, 64, 50, 51, 25, 29, 63,
    27, 87, 86, 74, 72, 52, 53, 89,
    83, 18, 16,  1,  5, 60, 62, 15,
    0, 52, 53, 57, 59, 87, 86, 66,
    61, 95, 91, 81, 80,  2,  6, 76,
    32,  2,  6, 12, 13, 95, 91, 17,
    93, 41, 40, 36, 38, 10, 11, 31,
    14, 79, 77, 92, 88, 33, 35, 82,
    70, 10, 11, 23, 21, 41, 40,  4,
    19, 25, 29, 47, 46, 68, 64, 34,
    45, 60, 62, 71, 67, 18, 16, 49
};

// changing first number to 8 or 16 works
// second number 56 removes a subcube

int transformCurve(int index, int bits, int lookupTable[]) {
  int out = 0;
  for (int i = 3 * (bits - 1), transform = 0; i >= 0; i -= 3) {
    transform = lookupTable[transform | ((index >> i) & 7)];
    out = (out << 3) | (transform & 7);
    transform &= ~7;
  }
  return out;
}

void CHilbCube::UpdateEntity()
{
    if (!IsDirty())
        return;
    Super::UpdateEntity();

    std::vector<Vertex*> vertArray;
    std::vector<CVertexInfo *> positions;

    int n = Iteration.GetValue(1.0f);
    n = pow(2, n);
    int x = 0;
    int y = 0;
    int z = 0;
    int dx = Size.GetValue(1.0f);
    int dy = Size.GetValue(1.0f);

    // Hilb(0, 0, dx, 0, 0, dy, n, 0, vertArray, positions);

    int prev[3];
    int curr[3];

    // for (int i = 0; i < n; i++){
        
    //     double * coords = index2coords(i);

    //     Vector3 pos = Vector3(coords[0] * dx, coords[1] * dx, coords[2] * dx);
    //     vertArray.push_back(AddVertex("v" + std::to_string(i), pos));
    //     CVertexInfo *point = new CVertexInfo();
    //     point->Position = pos;
    //     positions.push_back(point);
        
    // }

    // mortonIndices = new Uint32Array(n ** 3).map((v, i) => transformCurve(i, order, hilberToMortonTable));
    int order = 5;
    int mortonIndices[n*n*n];
    for(int i = 0; i < pow(n, 3); i++) {
        mortonIndices[i] = transformCurve(i, order, hilberToMortonTable);
    }

    for (int i = 0; i < pow(n, 3); i++) {
        int c = mortonIndices[i];
        x = compact1By2(c >> 0);
        y = compact1By2(c >> 1);
        z = compact1By2(c >> 2);

        Vector3 pos = Vector3(x * dx, y * dx, z * dx);
        vertArray.push_back(AddVertex("v" + std::to_string(i), pos));
        CVertexInfo *point = new CVertexInfo();
        point->Position = pos;
        positions.push_back(point);
    }

    AddLineStrip("hilbert-line", vertArray);

    SI.Positions = positions;
    SI.Name = GetName();
    SI.IsClosed = false;
    HilbCube.UpdateValue(&SI);
    SetValid(true);
}

}
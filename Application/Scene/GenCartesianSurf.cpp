#include "GenCartesianSurf.h"
#include "../Parsing/exprtk.hpp"
#include <cmath>
#include <string>
#include <vector>

#undef M_PI

namespace Nome::Scene
{

DEFINE_META_OBJECT(CGenCartesianSurf)
{
    BindPositionalArgument(&CGenCartesianSurf::x_start, 1, 0);
    BindPositionalArgument(&CGenCartesianSurf::x_end, 1, 1);
    BindPositionalArgument(&CGenCartesianSurf::y_start, 1, 2);
    BindPositionalArgument(&CGenCartesianSurf::y_end, 1, 3);
    BindPositionalArgument(&CGenCartesianSurf::x_segs, 1, 4);
    BindPositionalArgument(&CGenCartesianSurf::y_segs, 1, 5);
}

void CGenCartesianSurf::UpdateEntity()
{
    if (!IsDirty())
        return;

    Super::UpdateEntity();

    double xStart = (double)x_start.GetValue(0.0f);
    double xEnd = (double)x_end.GetValue(0.0f);
    double yStart = (double)y_start.GetValue(0.0f);
    double yEnd = (double)y_end.GetValue(0.0f);
    double xSegs = (double)x_segs.GetValue(0.0f);
    double ySegs = (double)y_segs.GetValue(0.0f);

    std::string funcZ = this->GetFunc(); // ex. funcZ := z(x,y) = "(4*x)+(3*sin(y))"
    funcZ.erase(std::remove(funcZ.begin(), funcZ.end(), '['), funcZ.end());
    funcZ.erase(std::remove(funcZ.begin(), funcZ.end(), ']'), funcZ.end());

    typedef exprtk::symbol_table<double> symbol_table_t;
    typedef exprtk::expression<double>     expression_t;
    typedef exprtk::parser<double>             parser_t;

    double x = 0.0;
    double y = 0.0;

    // Register symbols with the symbol_table
    symbol_table_t symbol_table;
    symbol_table.add_variable("x",x);
    symbol_table.add_variable("y",y);

    // Instantiate expression and register symbol_table
    expression_t expression_z;
    expression_z.register_symbol_table(symbol_table);

    // Instantiate parser and compile the expression
    parser_t parser;
    parser.compile(funcZ,expression_z);

    // set bounds
    double xIncrement = (xEnd - xStart) / xSegs;
    double yIncrement = (yEnd - yStart) / ySegs;

    // add points to vector
    double zi = 0.0;
    int xCounter = 0;
    int yCounter = 0;
    for (double xi = xStart; xi <= xEnd + (xIncrement/5); xi += xIncrement) { // dividing by 5 for rounding error
        x = xi;
        yCounter = 0;
        for (double yi = yStart; yi <= yEnd + (yIncrement/5); yi += yIncrement) { // dividing by 5 for rounding error
            y = yi;
            zi = expression_z.value();
            AddVertex("v_" + std::to_string(xCounter) + "_" + std::to_string(yCounter), // name ex. "v_0_5"
                      { (float)xi, (float)yi, (float)zi } );
            yCounter++;
        }
        xCounter++;
    }

    // add faces
    int faceCounter = 0;
    for (int xi = 0; xi + 1 < xCounter; xi++) {
        for (int yi = 0; yi < yCounter - 1; yi++) {
            std::vector<std::string> face;
            face.push_back("v_" + std::to_string(xi) + "_" + std::to_string(yi));
            face.push_back("v_" + std::to_string(xi+1) + "_" + std::to_string(yi));
            face.push_back("v_" + std::to_string(xi+1) + "_" + std::to_string(yi+1));
            face.push_back("v_" + std::to_string(xi) + "_" + std::to_string(yi+1));

            AddFace("f1_" + std::to_string(faceCounter++), face);
        }
    }
}

}

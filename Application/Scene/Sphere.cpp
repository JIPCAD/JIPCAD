#include "Sphere.h"

#undef M_PI

namespace Nome::Scene
{

DEFINE_META_OBJECT(CSphere)
{
    BindPositionalArgument(&CSphere::Radius, 1, 0);
    BindPositionalArgument(&CSphere::ThetaMax, 1, 1);
    BindPositionalArgument(&CSphere::PhiMin, 1, 2);
    BindPositionalArgument(&CSphere::PhiMax, 1, 3);
    BindPositionalArgument(&CSphere::ThetaSegs, 1, 4);
    BindPositionalArgument(&CSphere::PhiSegs, 1, 5);
    BindPositionalArgument(&CSphere::Type, 1, 6);
}

void CSphere::UpdateEntity()
{
    if (!IsDirty())
        return;

    Super::UpdateEntity();

    int mode = (int)Type.GetValue(1);
    int phiSegs = (int)PhiSegs.GetValue(12);
    int thetaSegs = (int)ThetaSegs.GetValue(24);
    float radius = (int)Radius.GetValue(1.0f);
    float thetaMax = ThetaMax.GetValue(360.0f);
    if (thetaMax != 360.0f && thetaMax > 359.0f) {
      thetaMax = 360.0f;
      std::cout << "[CSphere] thetaMax > 359°, clamped to 360°.";
    }
    float phiMinDeg = PhiMax.GetValue(-90.0f);
    float phiMaxDeg = PhiMin.GetValue(90.0f);


    phiMinDeg = std::clamp(phiMinDeg, -90.0f, 90.0f);
    phiMaxDeg = std::clamp(phiMaxDeg, -90.0f, 90.0f);
    bool fullPhi = (phiMinDeg <= -90.0f && phiMaxDeg >= 90.0f);


    if (phiMinDeg > phiMaxDeg) {
    std::swap(phiMinDeg, phiMaxDeg);
    std::cout << "[CSphere] Warning: phiMinDeg > phiMaxDeg, values swapped.\n";
    }


    if (phiMinDeg != -90.0f && phiMinDeg < -89.0f ) {
      phiMinDeg = -90.0f;
      std::cout << "[CSphere] phiMin < -89°, clamped to -90°.";
    }
    if (phiMaxDeg != 90.0f && phiMaxDeg > 89.0f) {
      phiMaxDeg = 90.0f;
      std::cout << "[CSphere] phiMax > 89°, clamped to 90°.";
    }
    float phiMin = (phiMinDeg + 90.0f) / 180.0f * (float)tc::M_PI;
    float phiMax = (phiMaxDeg + 90.0f) / 180.0f * (float)tc::M_PI;
    bool hasBottomCap = phiMin > 0.0f;
    bool hasTopCap = phiMax < (float)tc::M_PI;

    
    if (mode == 1 || mode == 3) {
        for (int j = 0; j <= thetaSegs; j++) {
            float theta = (float)j / thetaSegs * (thetaMax / 360.0f) * 2.0f * (float)tc::M_PI;
            for (int i = 0; i <= phiSegs; i++) {
                float phi = phiMin + ((float)i / phiSegs) * (phiMax - phiMin);
                float x = radius * sinf(phi) * cosf(theta);
                float y = radius * sinf(phi) * sinf(theta);
                float z = radius * cosf(phi);
                AddVertex("v" + std::to_string(j) + "-" + std::to_string(i), { x, y, z });
            }
        }
    } else if (mode == 2) {
      for (int j = 0; j <= thetaSegs; j++) 
{
    // If not full sphere, skip interior theta segments
    if (thetaMax != 360.0f && j != 0 && j != thetaSegs)
        continue;

    float theta = (float)j / thetaSegs * (thetaMax / 360.0f) * 2.0f * (float)tc::M_PI;
    for (int i = 0; i <= phiSegs; i++) 
    {
        float phi = phiMin + ((float)i / phiSegs) * (phiMax - phiMin);
        float x = radius * sinf(phi) * cosf(theta);
        float y = radius * sinf(phi) * sinf(theta);
        float z = radius * cosf(phi);
        AddVertex("v" + std::to_string(j) + "-" + std::to_string(i), { x, y, z });
    }

    }

        if (hasBottomCap) {
            int i = 0;
            for (int j = 0; j <= thetaSegs; j++) {
                float theta = (float)j / thetaSegs * (thetaMax / 360.0f) * 2.0f * (float)tc::M_PI;
                float phi = phiMin;
                float x = radius * sinf(phi) * cosf(theta);
                float y = radius * sinf(phi) * sinf(theta);
                float z = radius * cosf(phi);
                AddVertex("v" + std::to_string(j) + "-" + std::to_string(i), { x, y, z });
            }
        }

        if (hasTopCap) {
            int i = phiSegs;
            for (int j = 0; j <= thetaSegs; j++) {
                float theta = (float)j / thetaSegs * (thetaMax / 360.0f) * 2.0f * (float)tc::M_PI;
                float phi = phiMax;
                float x = radius * sinf(phi) * cosf(theta);
                float y = radius * sinf(phi) * sinf(theta);
                float z = radius * cosf(phi);
                AddVertex("v" + std::to_string(j) + "-" + std::to_string(i), { x, y, z });
            }
        }
    }

   
    for (int j = 0; j < thetaSegs; j++) {
        for (int i = 0; i < phiSegs; i++) {
            std::vector<std::string> face = {
                "v" + std::to_string(j) + "-" + std::to_string(i),
                "v" + std::to_string(j) + "-" + std::to_string(i + 1),
                "v" + std::to_string(j + 1) + "-" + std::to_string(i + 1),
                "v" + std::to_string(j + 1) + "-" + std::to_string(i),
            };
            if (mode == 1 || mode == 3) {
                AddFace("f" + std::to_string(j) + "-" + std::to_string(i), face);
            }
        }
    }

   if (thetaMax != 360.0f) {
    if (mode == 2 || mode == 3) {
      
        float phiMid = 0.5f * (phiMin + phiMax);
        float z = radius * cosf(phiMid);
        AddVertex("center", { 0.0f, 0.0f, z });
    }


        int j = 0;
        for (int i = 0; i < phiSegs; i++) {
            std::vector<std::string> face = {
                "v" + std::to_string(j) + "-" + std::to_string(i + 1),
                "v" + std::to_string(j) + "-" + std::to_string(i),
                "center"
            };
            if (mode == 2 || mode == 3) {
                AddFace("slice-face-left" + std::to_string(i), face);
            }
        }

        j = thetaSegs;
        for (int i = 0; i < phiSegs; i++) {
            std::vector<std::string> face = {
                "v" + std::to_string(j) + "-" + std::to_string(i),
                "v" + std::to_string(j) + "-" + std::to_string(i + 1),
                "center"
            };
            if (mode == 2 || mode == 3) {
            AddFace("slice-face-right" + std::to_string(i), face);
            }
        }
    }
    
    if (hasBottomCap) {
        float phi = phiMin;
        float z = radius * cosf(phi);
        if (mode == 2 || mode == 3) {
        AddVertex("bottomCapCenter", { 0, 0, z });
        }
        int i = 0;
        for (int j = 0; j < thetaSegs; j++) {
            std::vector<std::string> face = {
                "v" + std::to_string(j) + "-" + std::to_string(i),
                "v" + std::to_string(j + 1) + "-" + std::to_string(i),
                "bottomCapCenter"
            };
            if (mode == 2 || mode == 3) {
            AddFace("bottom-cap-face" + std::to_string(j), face);
            }
        }
    }

    if (hasTopCap) {
        float phi = phiMax;
        float z = radius * cosf(phi);
        if (mode == 2 || mode == 3) {
        AddVertex("topCapCenter", { 0, 0, z });
        }
        int i = phiSegs;
        for (int j = 0; j < thetaSegs; j++) {
            std::vector<std::string> face = {
                "topCapCenter",
                "v" + std::to_string(j + 1) + "-" + std::to_string(i),
                "v" + std::to_string(j) + "-" + std::to_string(i)
            };
            if (mode == 2 || mode == 3) {
            AddFace("top-cap-face" + std::to_string(j), face);
            }
        }
    }

    if (hasBottomCap && thetaMax != 360.0f && (mode == 2 || mode == 3)) {
        AddFace("corner-bottom-left", {
            "bottomCapCenter",
            "center",
            "v0-0"
        });
        AddFace("corner-bottom-right", {
            "center",
            "bottomCapCenter",
            "v" + std::to_string(thetaSegs) + "-0"
        });
    }

    if (hasTopCap && thetaMax != 360.0f && (mode == 2 || mode == 3)) {
        AddFace("corner-top-left", {
            "topCapCenter",
            "v0-" + std::to_string(phiSegs),
            "center"
        });
        AddFace("corner-top-right", {
            "center",
            "v" + std::to_string(thetaSegs) + "-" + std::to_string(phiSegs),
            "topCapCenter"
        });
    }
}

}






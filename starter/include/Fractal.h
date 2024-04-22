//Fractal.h

#ifndef FRACTAL_H
#define FRACTAL_H

#include <string>
#include "viewer_util.h"

namespace starter {

class Fractal {
public:
    Fractal();
    ~Fractal();

    // Sets the Julia set parameters
    void SetParameters(float centerX, float centerY, float zcReal, float zcImag, float range, int nbIterations);

    // Generates the Julia Set
    void GenerateJuliaSet(ImgProc& in);

private:
    ImgProc fractalImage;

    float center_x;
    float center_y;
    float zc_real;
    float zc_imag;
    float range;
    int nb_iterations;

};

}

#endif

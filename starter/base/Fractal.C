// Fractal.C

#include "viewer_util.h"
#include "Fractal.h"
#include <vector>
#include <string>
#include <cmath>
#include <complex>
#include <iostream>
#include <GL/glut.h>
#include <OpenImageIO/imageio.h>

using namespace OIIO;
using namespace starter;

Fractal::Fractal(){}
Fractal::~Fractal(){}

void Fractal::SetParameters(float centerX, float centerY, float zcReal, float zcImag, float range, int nbIterations) {
    center_x = centerX;
    center_y = centerY;
    zc_real = zcReal;
    zc_imag = zcImag;
    this->range = range;
    nb_iterations = nbIterations;
}

void Fractal::GenerateJuliaSet(ImgProc& in) {

    // Color map of colors used in fractal image
    std::vector<std::vector<float>> colorMap = {
      {0.0, 0.0, 0.0},   // Black
      {0.2, 0.2, 0.2},   // Grey
      {1.0, 1.0, 1.0},   // White
      {1.0, 0.0, 0.0}    // Red
    };

    int imageWidth = in.get_width();
    int imageHeight = in.get_height();

    // Iterate through the image pixels to generate the Julia Set
    for (int j = 0; j < imageHeight; ++j) {
        for (int i = 0; i < imageWidth; ++i) {
            // Calculate the complex number 'z' based on pixel coordinates
            float zx = center_x + range * (2.0 * i / imageWidth - 1.0);
            float zy = center_y + range * (1.0 - 2.0 * j / imageHeight);
            std::complex<float> z(zx, zy);

            // Perform iterations to determine the Julia Set
            int iterations = 0;
            while (iterations < nb_iterations) {
              z = z * z + std::complex<float> (zc_real, zc_imag);
              float magnitude_squared = (z.real() * z.real()) + (z.imag() * z.imag());
              if (magnitude_squared > 4.0) {
                  break;
              }
              iterations++;
            }

            // Changes color of pixel based off # of iterations
            int colorIndex = iterations % colorMap.size();
            std::vector<float> pixelColor = colorMap[colorIndex];

            // Sets value of input image as Julia set value at particular pixel
            in.set_value(i, j, pixelColor);
        }
    }

    glutPostRedisplay();
}

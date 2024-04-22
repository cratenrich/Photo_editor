#include "Stencil.h"
#include <vector>
#include <string>
#include <iostream>
#include <random>
#include <GL/glut.h>
#include <OpenImageIO/imageio.h>

using namespace OIIO;
using namespace starter;

//-----------------Stencil object initialization----------------------

Stencil::Stencil(int halfwidth){
  half_width = halfwidth;
  int arraysize = 2 * half_width + 1;
  //Ensures that memory is allocated for the stencil values
  stencil_values = new float[arraysize * arraysize];

  //This is meant to initialize the stencil values during construction
  float sum = 0.0f;
  for (int j = -half_width; j <= half_width; ++j){
    for (int i = -half_width; i <= half_width; ++i){
      int index = computeIndex(i, j);
      float value = 0.0f;

      //Sets rand values between -0.1 and 0.1
      if (i != 0 || j != 0){
        value = static_cast<float>(rand()) / RAND_MAX * 0.2f - 0.1f;
      }
      else{
        value = 1.0f - sum;
      }

      stencil_values[index] = value;
      sum += value;
    }
  }
}
Stencil::~Stencil(){}

//----------------Index computation------------------------------

//Helper function to calculate the index of a given pixel
int Stencil::computeIndex(int i, int j) const {
  int arraysize = 2 * half_width + 1;
  return (i + half_width) + (j + half_width) * arraysize;
}

//Takes in the pixel value
float& Stencil::operator()(int i, int j){
  int index = computeIndex(i, j);
  return stencil_values[index];
}

//Takes in the pixel value
const float& Stencil::operator()(int i, int j) const {
  int index = computeIndex(i, j);
  return stencil_values[index];
}

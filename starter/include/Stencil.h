#ifndef STENCIL
#define STENCIL
#include <string>
#include <vector>
#include <iostream>
#include <random>


namespace starter {
    class Stencil {
    public:
      Stencil(int halfwidth=1);
      ~Stencil();
      int halfwidth() const { return half_width; }
      float& operator()(int i, int j);
      const float& operator()(int i, int j) const;

      int computeIndex(int i, int j) const;

    private:
      int half_width;
      float *stencil_values;
    };
}

#endif

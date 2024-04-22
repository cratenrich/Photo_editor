//  viewer_util.h

#ifndef VIEWER_UTIL
#define VIEWER_UTIL
#include "Stencil.h"
#include <string>
#include <vector>
#include <iostream>


namespace starter {
    class ImgProc {
    public:

      void clear();
      void clear(int nX, int nY, int nC);

      bool load_image(const std::string& file_name);
      void SaveToJPEG(const std::string& file_name);
      void SaveToEXR(const std::string& file_name);
      int get_width() const { return width; }
      int get_height() const { return height; }
      int get_depth() const { return depth; }
      long get_size() const { return Nsize; }
      float* get_image_data(){ return image_data; }
      void checkData();

      void gamma(float s);
      void contrast();
      void EqualizeImage();

      void toneShift(float s);

      void LinearConvolution(const Stencil& stencil,ImgProc& in, ImgProc& out);
      void value(int i, int j, std::vector< float > &pixel);
      void set_value(int i, int j, const std::vector< float > &pixel);
      int index(int i, int j, int c){ return i * depth + j * (depth * width) + c; }


    private:
      int width;
      int height;
      int depth;
      float* image_data;
      long Nsize;

    };
}

#endif

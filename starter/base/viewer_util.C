//  viewer_util.C

#include "viewer_util.h"
#include "Stencil.h"
#include <vector>
#include <string>
#include <cmath>
#include <complex>
#include <iostream>
#include <GL/glut.h>
#include <OpenImageIO/imageio.h>

using namespace OIIO;
using namespace starter;

void ImgProc::clear() {
  if( image_data != 0 ){
    delete[] image_data;
    image_data = 0;
  }
  width = 0;
  height = 0;
  depth = 0;
  Nsize = 0;
}

void ImgProc::clear(int nX, int nY, int nC) {
  clear();
  width = nX;
  height = nY;
  depth = nC;
  Nsize = (long)width * (long)height * (long)depth;
  image_data = new float[Nsize];
  #pragma omp parallel for
  for( long i=0;i<Nsize;i++ ){
    image_data[i] = 0.0;
  };
}

bool ImgProc::load_image(const std::string& file_name) {
    std::unique_ptr<ImageInput> in = ImageInput::open(file_name.c_str());
    if (!in) {
        std::cerr << "Could not open image file: " << file_name << std::endl;
        return false;
    }

    const ImageSpec& spec = in->spec();

    clear(spec.width, spec.height, spec.nchannels);
    std::cout << spec.width << "  " << spec.height << "  " << spec.nchannels << std::endl;

    if (!in->read_image(TypeDesc::FLOAT, image_data)) {
        std::cerr << "Could not read image data: " << file_name << std::endl;
        in->close();
        return false;
    }

    in->close();

    return true;
}

void ImgProc::SaveToJPEG(const std::string& file_name) {
    if (image_data == nullptr) {
        std::cerr << "Image data is not loaded." << std::endl;
        return;
    }

    int channels = depth;
    ImageSpec spec(width, height, channels,TypeDesc::FLOAT);
    std::unique_ptr<ImageOutput> out = ImageOutput::create(file_name);
    if (!out) {
        std::cerr << "Could not create the output image file: " << file_name << std::endl;
        return;
    }

    if (!out->open(file_name, spec)) {
        std::cerr << "Could not open the output image file: " << file_name << std::endl;
        return;
    }

    if (!out->write_image(OIIO::TypeDesc::FLOAT, image_data)) {
        std::cerr << "Could not write image data to: " << file_name << std::endl;
    }

    out->close();
    std::cout << "Image saved to " << file_name << std::endl;
}

// Helper function used to look at the values of the pixels
// at any given moment
void ImgProc::checkData(){
  #pragma omp parallel for
  for( long i=0;i<Nsize;i++ ){
    std::cout << image_data[i] << "\n" << std::endl;
  }
}

void ImgProc::gamma(float s){
  #pragma omp parallel for
  for( long i=0;i<Nsize;i++ ){
    image_data[i] = std::pow(image_data[i], s);
  }
}

/*
-------------------------------- Assignment 4 ----------------------------------
*/

void ImgProc::contrast(){
    std::vector<float> avg(depth, 0.0);
    std::vector<float> rms(depth, 0.0);

    #pragma omp parallel for
    for (long c = 0; c < depth; ++c) {
        for (long i = 0; i < Nsize; ++i) {
            avg[c] += image_data[i+c];
            rms[c] += image_data[i+c] * image_data[i+c];
        }
        avg[c] /= Nsize;
        rms[c] = std::sqrt(rms[c] / Nsize - avg[c] * avg[c]);
    }

    // Contrasts the currently displayed image
    #pragma omp parallel for
    for (long i = 0; i < Nsize; ++i) {
        for (int c = 0; c < depth; ++c) {
            image_data[i+c] = (image_data[i+c] - avg[c]) / rms[c];
        }
    }
}

void ImgProc::EqualizeImage(){
    const int bin_nb = 500;

    // Min value in the image data
    float min = *std::min_element(image_data, image_data + Nsize);
    // Max value in the image data
    float max = *std::max_element(image_data, image_data + Nsize);

    // Histogram initialization. Each will have 500 bins and initially starts with 0
    std::vector<std::vector<int>> histograms(depth, std::vector<int>(bin_nb, 0));

    for (int c = 0; c < depth; ++c){
        for (long i = 0; i < Nsize; ++i){
            int bindex = static_cast<int>((image_data[i * depth + c] - min) / ((max - min) / bin_nb));
            bindex = std::min(std::max(bindex, 0), bin_nb - 1);
            histograms[c][bindex]++;
        }
    }

    // Probability distributions
    std::vector<std::vector<float>> prob(depth, std::vector<float>(bin_nb, 0.0));
    for (int c = 0; c < depth; ++c){
        for (int i = 0; i < bin_nb; ++i){
            prob[c][i] = static_cast<float>(histograms[c][i]) / static_cast<float>(Nsize);
        }
    }

    // CDF calculation
    std::vector<std::vector<float>> cdf(depth, std::vector<float>(bin_nb, 0.0));
    for (int c = 0; c < depth; ++c){
        cdf[c][0] = prob[c][0];
        for (int i = 1; i < bin_nb; ++i){
            cdf[c][i] = cdf[c][i - 1] + prob[c][i];
        }
    }

    // Equalize the image
    #pragma omp parallel for
    for (long i = 0; i < Nsize; ++i){
        for (int c = 0; c < depth; ++c){
            int bindex = static_cast<int>((image_data[i * depth + c] - min) / (max - min) * (bin_nb - 1));
            bindex = std::max(0, std::min(bin_nb - 1, bindex));  // Ensure bin is within bounds
            image_data[i * depth + c] = cdf[c][bindex] * (max - min) + min;
        }
    }
}

void ImgProc::SaveToEXR(const std::string& file_name){
    if (image_data == nullptr) {
        std::cerr << "Image data is not loaded." << std::endl;
        return;
    }

    int channels = depth;
    ImageSpec spec(width, height, channels, TypeDesc::FLOAT);

    std::unique_ptr<ImageOutput> out = ImageOutput::create(file_name);
    if (!out) {
        std::cerr << "Could not create the output EXR file: " << file_name << std::endl;
        return;
    }

    if (!out->open(file_name, spec)) {
        std::cerr << "Could not open the output EXR file: " << file_name << std::endl;
        return;
    }

    if (!out->write_image(TypeDesc::FLOAT, image_data)) {
        std::cerr << "Could not write image data to: " << file_name << std::endl;
    }

    out->close();
    std::cout << "Image saved to " << file_name << " as EXR." << std::endl;
}

/*
-------------------------------- Assignment 5 ----------------------------------
*/

//Modification of Gamma function
//Replaces the image with a +/- tonemapping instead of a power function
void ImgProc::toneShift(float s){
    #pragma omp parallel for
    for( long i=0;i<Nsize;i++ ){
        image_data[i] = 0.5 * (image_data[i] + 1.0);
    }
}

/*
--------------------------------------------------------------------------------
*/

void ImgProc::value( int i, int j, std::vector<float>& pixel){
  pixel.clear();
  if( image_data == nullptr ){ return; }
  if( i<0 || i>=width ){ return; }
  if( j<0 || j>=height ){ return; }
  pixel.resize(depth);
  for( int c=0;c<depth;c++ ){
    pixel[c] = image_data[index(i,j,c)];
  }
  return;
}

void ImgProc::set_value(int i, int j, const std::vector< float > &pixel){
  if( image_data == nullptr ){ return; }
  if( i<0 || i>=width ){ return; }
  if( j<0 || j>=height ){ return; }
  if( depth > (int)pixel.size() ){ return; }
  #pragma omp parallel for
  for( int c=0;c<depth;c++ ){
    image_data[index(i,j,c)] = pixel[c];
  }
  return;
}

void ImgProc::LinearConvolution( const Stencil& stencil, ImgProc& in, ImgProc& out ){
  out.clear( in.get_width(), in.get_height(), in.get_depth() );
  for( int j=0;j<out.get_width();j++){
    int jmin = j - stencil.halfwidth();
    int jmax = j + stencil.halfwidth();

    #pragma omp parallel for
    for(int i=0;i<out.get_width();i++){
      int imin = i - stencil.halfwidth();
      int imax = i + stencil.halfwidth();
      std::vector<float> pixel(out.get_depth(),0.0);
      std::vector<float> sample(in.get_depth(),0.0);
      for(int jj=jmin;jj<=jmax;jj++){
        int stencilj = jj-j;
        int jjj = jj;
        if(jjj < 0 ){
          jjj += out.get_height();
        }
        if(jjj >= out.get_height() ){ jjj -= out.get_height(); }
        for(int ii=imin;ii<=imax;ii++){
          int stencili = ii-i;
          int iii = ii;
          if(iii < 0 ){ iii += out.get_width(); }
          if(iii >= out.get_width() ){ iii -= out.get_width(); }
          const float& stencil_value = stencil(stencili, stencilj);
          in.value(iii,jjj,sample);
          for(size_t c=0;c<sample.size();c++){ pixel[c] += sample[c] * stencil_value; }
        }
      }
      out.set_value(i,j,pixel);
    }
  }
}

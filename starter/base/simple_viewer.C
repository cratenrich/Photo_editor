//simple_viewer.C

#include <vector>
#include <string>
#include "StarterViewer.h"
#include "viewer_util.h"
#include "Stencil.h"
#include <iostream>
#include <OpenImageIO/imageio.h>
#include <GL/glut.h>

using namespace OIIO;
using namespace starter;


int main(int argc, char** argv)
{
  std::string image_file;

  // Command line parsing
  for (int i = 1; i < argc; ++i)
  {
    std::string arg(argv[i]);

    if (arg == "-image" && i + 1 < argc) // Checks for -image flag
    {
      image_file = argv[i + 1]; // Gets image file name
      break;
    }
  }

  if (image_file.empty())
  {
    std::cerr << "Usage: " << argv[0] << " -image <name of image file>\n";
    return 1; // Exit with an error code
  }

  std::cout << "\n\n\nDisplaying " << image_file << std::endl;

  StarterViewer* viewer = CreateViewer();
  viewer->Init({image_file});
  viewer->MainLoop();

  return 0;
}

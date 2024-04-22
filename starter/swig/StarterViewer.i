
%module StarterViewer 
%{
#include "StarterViewer.h"
%}

%include "std_string.i"
%include "std_vector.i"
%template(StringArray) std::vector<std::string>;
%include "StarterViewer.h"

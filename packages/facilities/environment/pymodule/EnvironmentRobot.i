%module EnvironmentRobot

%{
#define SWIG_FILE_WITH_INIT  // specify that the resulting C file should be build as python extension, inserting the module init code
#include <ext/cEnvironmentCommon.hpp>
#include <ext/cEnvironmentRobot.hpp>

%}

%include "ext/cEnvironmentCommon.hpp"
%include "ext/cEnvironmentRobot.hpp"




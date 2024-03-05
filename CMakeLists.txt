cmake_minimum_required(VERSION 3.12)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# set(CMAKE_C_COMPILER /cvmfs/sft.cern.ch/lcg/releases/gcc/11.1.0-e80bf/x86_64-centos7/bin/gcc)
# set(CMAKE_CXX_COMPILER /cvmfs/sft.cern.ch/lcg/releases/gcc/11.1.0-e80bf/x86_64-centos7/bin/g++)

project(GM2SLOWTOYMC)

# Set the ROOT directory
# list(APPEND CMAKE_PREFIX_PATH $ENV{ROOTSYS})
find_package(ROOT REQUIRED)

# boost
find_package(Boost REQUIRED)

# Add source files
include_directories(GSSim/include)

file(GLOB_RECURSE GSSim_sources GSSim/src/*)
file(GLOB_RECURSE GSSim_headers GSSim/include/*)

add_executable(GSSim main.cpp ${GSSim_sources} ${GSSim_headers})

# Link against ROOT libraries
target_link_libraries(GSSim PUBLIC ${ROOT_LIBRARIES})
target_link_libraries(GSSim PUBLIC Boost::boost)

# Include ROOT headers
target_include_directories(GSSim PUBLIC ${ROOT_INCLUDE_DIRS})

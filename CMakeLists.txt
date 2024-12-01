cmake_minimum_required(VERSION 3.29)
project(Tutorial)

set(CMAKE_CXX_STANDARD 20)

# Add OGDF include directory
include_directories(
        /home/max/Desktop/ogdf.v2023.09/OGDF/include
        /home/max/Desktop/ogdf.v2023.09/OGDF/build/include  # Add this to access config_autogen.h
)

# Add OGDF library directory
link_directories(/home/max/Desktop/ogdf.v2023.09/OGDF/build)

# Add your main.cpp file
add_executable(Tutorial main.cpp)

# Link OGDF libraries and other dependencies
target_link_libraries(Tutorial OGDF COIN pthread m dl)

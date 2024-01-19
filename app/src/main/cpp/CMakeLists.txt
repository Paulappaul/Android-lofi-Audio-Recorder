# Sets the minimum CMake version required for this project.
cmake_minimum_required(VERSION 3.22.1)

# Declares the project name.
project("Cosmo388")

# Set C++ standard to C++17
set(CMAKE_CXX_STANDARD 17)

# Creates and names a library, sets it as SHARED, and provides the relative paths to its source code.


# Define the path to the external library with quotes
set(EXTERNAL_LIBRARY_DIR "C:/Users/alcin/Desktop/Convolution/FFTConvolver-non-uniform")


add_library(oboe-lib SHARED
        AudioRecorder.cpp
        AudioRecorderWrapper.cpp
        AudioProcessor.cpp
        ${EXTERNAL_LIBRARY_DIR}/FFTConvolver.cpp
        ${EXTERNAL_LIBRARY_DIR}/AudioFFT.cpp
        ${EXTERNAL_LIBRARY_DIR}/Utilities.cpp)

# Find the Oboe package
find_package(oboe REQUIRED CONFIG)

# Specify the libraries which your native library is dependent on
target_include_directories(oboe-lib PRIVATE
        ${EXTERNAL_LIBRARY_DIR}
        ${JNI_INCLUDE_DIRS}  # Include JNI headers
        ${oboe_INCLUDE_DIRS})

# Specify the libraries which your native library is dependent on
target_link_libraries(oboe-lib PRIVATE android log oboe::oboe)

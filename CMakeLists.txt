cmake_minimum_required(VERSION 3.25)

set(CMAKE_CXX_STANDARD 23)
set(VCPKG_HOST_TRIPLET x64-mingw-static)
set(VCPKG_TARGET_TRIPLET x64-mingw-static)

project(burner_jet_measure_pipeline)

find_package(OpenCV CONFIG REQUIRED)

if (${OpenCV_FOUND})
    add_executable(burner_jet_measure_pipeline src/main.cpp)
    target_include_directories(burner_jet_measure_pipeline PRIVATE ${OpenCV_INCLUDE_DIRS})
    target_link_directories(burner_jet_measure_pipeline PRIVATE ${OpenCV_LIB_DIR})
    target_link_libraries(burner_jet_measure_pipeline PRIVATE ${OpenCV_LIBS})
else ()
    message(FATAL_ERROR "Failed to find OpenCV")
endif ()
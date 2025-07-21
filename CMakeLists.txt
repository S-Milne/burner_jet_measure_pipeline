cmake_minimum_required(VERSION 3.25)

set(CMAKE_CXX_STANDARD 23)
set(VCPKG_HOST_TRIPLET x64-mingw-dynamic)
set(VCPKG_TARGET_TRIPLET x64-mingw-dynamic)

project(burner_jet_measure_pipeline)

find_package(OpenCV CONFIG REQUIRED)
find_package(ffmpeg CONFIG REQUIRED)

add_executable(burner_jet_measure_pipeline src/main.cpp)

if (${OpenCV_FOUND})

    target_include_directories(burner_jet_measure_pipeline PRIVATE ${OpenCV_INCLUDE_DIRS})
    target_link_directories(burner_jet_measure_pipeline PRIVATE ${OpenCV_LIB_DIR})
    target_link_libraries(burner_jet_measure_pipeline PRIVATE ${OpenCV_LIBS})
else ()
    message(FATAL_ERROR "Failed to find OpenCV")
endif ()

if (${ffmpeg_FOUND})
    target_include_directories(burner_jet_measure_pipeline PRIVATE ${ffmpeg_INCLUDE_DIRS})
    target_link_directories(burner_jet_measure_pipeline PRIVATE ${ffmpeg_LIB_DIR})
    target_link_libraries(burner_jet_measure_pipeline PRIVATE ${ffmpeg_LIBS})
else ()
    message(FATAL_ERROR "Failed to find FFMPEG")
endif ()
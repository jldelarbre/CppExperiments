cmake_minimum_required(VERSION 3.18)

project(CppExperiments)

set(SOURCES
    CppExperiments.cpp
    instantiation.cpp
    mutableConst.cpp
)

add_executable(CppExperiments ${SOURCES})
target_compile_features(CppExperiments PRIVATE cxx_std_17)

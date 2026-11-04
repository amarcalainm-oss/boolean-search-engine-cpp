cmake_minimum_required(VERSION 3.15)
project(boolean_search_engine CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(buscador src/main.cpp src/funciones_buscador.cpp)

cmake_minimum_required(VERSION 3.20)

project(school_utils_backend)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Crow CONFIG REQUIRED)

add_executable(server
    main.cpp
    paths.cpp
    frontend.cpp
    M2/Math/quadratic-solver/quadraticSolver.cpp
    M2/Science/planet-weight/planetWeightCalculator.cpp
)

target_link_libraries(server PRIVATE Crow::Crow)

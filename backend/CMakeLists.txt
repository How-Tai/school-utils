cmake_minimum_required(VERSION 3.20)

project(school_utils_backend)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Crow CONFIG REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(PostgreSQL REQUIRED)

add_executable(server
	main.cpp
	paths.cpp
	frontend.cpp
	core/database.cpp
	core/security.cpp
	account/account.cpp
	community/community.cpp
	M2/Math/quadratic-solver/quadraticSolver.cpp
	M2/Science/planet-weight/planetWeightCalculator.cpp
	M2/Science/sig-fig-counting/countingSigFig.cpp
	dschool/attendance/attendanceMarker.cpp
)

target_include_directories(server PRIVATE include ${PostgreSQL_INCLUDE_DIRS})

target_compile_definitions(server PRIVATE CPPHTTPLIB_OPENSSL_SUPPORT)

target_link_libraries(server PRIVATE
	Crow::Crow
	OpenSSL::SSL
	OpenSSL::Crypto
	PostgreSQL::PostgreSQL
)

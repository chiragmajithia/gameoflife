cmake_minimum_required (VERSION 2.6)
project (Life)
SET(CMAKE_CXX_COMPILER mpicxx)
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
find_package(MPI REQUIRED)
include_directories(MPI_INCLUDE_PATH include)

add_executable(life_seq src/life_seq.cpp)
target_link_libraries(life_seq ${INCLUDE_DIRECTORIES})

add_executable(life_seq_opt src/life_seq_opt.cpp)
target_link_libraries(life_seq_opt ${INCLUDE_DIRECTORIES})

add_executable(life_mpi_1d src/life_mpi_1d.cpp)
target_link_libraries(life_mpi_1d ${MPI_LIBRARIES})

add_executable(life_mpi_2d src/life_mpi_2d.cpp)
target_link_libraries(life_mpi_2d ${MPI_LIBRARIES})
# Add a test:
# enable_testing()
# add_test(SimpleTest ${MPIEXEC} ${MPIEXEC_NUMPROC_FLAG} 4 ${CMAKE_CURRENT_BINARY_DIR}/mytest)

# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.0

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:

# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

.SUFFIXES: .hpux_make_needs_suffix_list

# Suppress display of executed commands.
$(VERBOSE).SILENT:

# A target that is always out of date.
cmake_force:
.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /afs/glue.umd.edu/system/1.5/@sys/usr/local/cmake/3.0.2/bin/cmake

# The command to remove a file.
RM = /afs/glue.umd.edu/system/1.5/@sys/usr/local/cmake/3.0.2/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /homes/cmsc714-1bv3/hpc/p1

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /homes/cmsc714-1bv3/hpc/p1/build

# Include any dependencies generated for this target.
include CMakeFiles/life_mpi_1d.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/life_mpi_1d.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/life_mpi_1d.dir/flags.make

CMakeFiles/life_mpi_1d.dir/src/life_mpi_1d.cpp.o: CMakeFiles/life_mpi_1d.dir/flags.make
CMakeFiles/life_mpi_1d.dir/src/life_mpi_1d.cpp.o: ../src/life_mpi_1d.cpp
	$(CMAKE_COMMAND) -E cmake_progress_report /homes/cmsc714-1bv3/hpc/p1/build/CMakeFiles $(CMAKE_PROGRESS_1)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building CXX object CMakeFiles/life_mpi_1d.dir/src/life_mpi_1d.cpp.o"
	mpicxx   $(CXX_DEFINES) $(CXX_FLAGS) -o CMakeFiles/life_mpi_1d.dir/src/life_mpi_1d.cpp.o -c /homes/cmsc714-1bv3/hpc/p1/src/life_mpi_1d.cpp

CMakeFiles/life_mpi_1d.dir/src/life_mpi_1d.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/life_mpi_1d.dir/src/life_mpi_1d.cpp.i"
	mpicxx  $(CXX_DEFINES) $(CXX_FLAGS) -E /homes/cmsc714-1bv3/hpc/p1/src/life_mpi_1d.cpp > CMakeFiles/life_mpi_1d.dir/src/life_mpi_1d.cpp.i

CMakeFiles/life_mpi_1d.dir/src/life_mpi_1d.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/life_mpi_1d.dir/src/life_mpi_1d.cpp.s"
	mpicxx  $(CXX_DEFINES) $(CXX_FLAGS) -S /homes/cmsc714-1bv3/hpc/p1/src/life_mpi_1d.cpp -o CMakeFiles/life_mpi_1d.dir/src/life_mpi_1d.cpp.s

CMakeFiles/life_mpi_1d.dir/src/life_mpi_1d.cpp.o.requires:
.PHONY : CMakeFiles/life_mpi_1d.dir/src/life_mpi_1d.cpp.o.requires

CMakeFiles/life_mpi_1d.dir/src/life_mpi_1d.cpp.o.provides: CMakeFiles/life_mpi_1d.dir/src/life_mpi_1d.cpp.o.requires
	$(MAKE) -f CMakeFiles/life_mpi_1d.dir/build.make CMakeFiles/life_mpi_1d.dir/src/life_mpi_1d.cpp.o.provides.build
.PHONY : CMakeFiles/life_mpi_1d.dir/src/life_mpi_1d.cpp.o.provides

CMakeFiles/life_mpi_1d.dir/src/life_mpi_1d.cpp.o.provides.build: CMakeFiles/life_mpi_1d.dir/src/life_mpi_1d.cpp.o

# Object files for target life_mpi_1d
life_mpi_1d_OBJECTS = \
"CMakeFiles/life_mpi_1d.dir/src/life_mpi_1d.cpp.o"

# External object files for target life_mpi_1d
life_mpi_1d_EXTERNAL_OBJECTS =

life_mpi_1d: CMakeFiles/life_mpi_1d.dir/src/life_mpi_1d.cpp.o
life_mpi_1d: CMakeFiles/life_mpi_1d.dir/build.make
life_mpi_1d: /cell_root/software/openmpi/1.6.5/gnu/4.6.1/threaded/sys/lib/libmpi_cxx.so
life_mpi_1d: /cell_root/software/openmpi/1.6.5/gnu/4.6.1/threaded/sys/lib/libmpi.so
life_mpi_1d: /usr/lib64/libdl.so
life_mpi_1d: /usr/lib64/libm.so
life_mpi_1d: /usr/lib64/libnuma.so
life_mpi_1d: /usr/lib64/librt.so
life_mpi_1d: /usr/lib64/libnsl.so
life_mpi_1d: /usr/lib64/libutil.so
life_mpi_1d: /usr/lib64/libm.so
life_mpi_1d: /usr/lib64/libdl.so
life_mpi_1d: /usr/lib64/libnuma.so
life_mpi_1d: /usr/lib64/librt.so
life_mpi_1d: /usr/lib64/libnsl.so
life_mpi_1d: /usr/lib64/libutil.so
life_mpi_1d: CMakeFiles/life_mpi_1d.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --red --bold "Linking CXX executable life_mpi_1d"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/life_mpi_1d.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/life_mpi_1d.dir/build: life_mpi_1d
.PHONY : CMakeFiles/life_mpi_1d.dir/build

CMakeFiles/life_mpi_1d.dir/requires: CMakeFiles/life_mpi_1d.dir/src/life_mpi_1d.cpp.o.requires
.PHONY : CMakeFiles/life_mpi_1d.dir/requires

CMakeFiles/life_mpi_1d.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/life_mpi_1d.dir/cmake_clean.cmake
.PHONY : CMakeFiles/life_mpi_1d.dir/clean

CMakeFiles/life_mpi_1d.dir/depend:
	cd /homes/cmsc714-1bv3/hpc/p1/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /homes/cmsc714-1bv3/hpc/p1 /homes/cmsc714-1bv3/hpc/p1 /homes/cmsc714-1bv3/hpc/p1/build /homes/cmsc714-1bv3/hpc/p1/build /homes/cmsc714-1bv3/hpc/p1/build/CMakeFiles/life_mpi_1d.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/life_mpi_1d.dir/depend


# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.14

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


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
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/uga/smithcg/Desktop/CurveSurfing/curve_surfing

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/uga/smithcg/Desktop/CurveSurfing/build-curve_surfing-Desktop-Default

# Include any dependencies generated for this target.
include CMakeFiles/glad.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/glad.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/glad.dir/flags.make

CMakeFiles/glad.dir/external/glad/src/glad.c.o: CMakeFiles/glad.dir/flags.make
CMakeFiles/glad.dir/external/glad/src/glad.c.o: /home/uga/smithcg/Desktop/CurveSurfing/curve_surfing/external/glad/src/glad.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/uga/smithcg/Desktop/CurveSurfing/build-curve_surfing-Desktop-Default/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object CMakeFiles/glad.dir/external/glad/src/glad.c.o"
	/usr/lib64/ccache/gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/glad.dir/external/glad/src/glad.c.o   -c /home/uga/smithcg/Desktop/CurveSurfing/curve_surfing/external/glad/src/glad.c

CMakeFiles/glad.dir/external/glad/src/glad.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/glad.dir/external/glad/src/glad.c.i"
	/usr/lib64/ccache/gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/uga/smithcg/Desktop/CurveSurfing/curve_surfing/external/glad/src/glad.c > CMakeFiles/glad.dir/external/glad/src/glad.c.i

CMakeFiles/glad.dir/external/glad/src/glad.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/glad.dir/external/glad/src/glad.c.s"
	/usr/lib64/ccache/gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/uga/smithcg/Desktop/CurveSurfing/curve_surfing/external/glad/src/glad.c -o CMakeFiles/glad.dir/external/glad/src/glad.c.s

# Object files for target glad
glad_OBJECTS = \
"CMakeFiles/glad.dir/external/glad/src/glad.c.o"

# External object files for target glad
glad_EXTERNAL_OBJECTS =

libglad.a: CMakeFiles/glad.dir/external/glad/src/glad.c.o
libglad.a: CMakeFiles/glad.dir/build.make
libglad.a: CMakeFiles/glad.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/uga/smithcg/Desktop/CurveSurfing/build-curve_surfing-Desktop-Default/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking C static library libglad.a"
	$(CMAKE_COMMAND) -P CMakeFiles/glad.dir/cmake_clean_target.cmake
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/glad.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/glad.dir/build: libglad.a

.PHONY : CMakeFiles/glad.dir/build

CMakeFiles/glad.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/glad.dir/cmake_clean.cmake
.PHONY : CMakeFiles/glad.dir/clean

CMakeFiles/glad.dir/depend:
	cd /home/uga/smithcg/Desktop/CurveSurfing/build-curve_surfing-Desktop-Default && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/uga/smithcg/Desktop/CurveSurfing/curve_surfing /home/uga/smithcg/Desktop/CurveSurfing/curve_surfing /home/uga/smithcg/Desktop/CurveSurfing/build-curve_surfing-Desktop-Default /home/uga/smithcg/Desktop/CurveSurfing/build-curve_surfing-Desktop-Default /home/uga/smithcg/Desktop/CurveSurfing/build-curve_surfing-Desktop-Default/CMakeFiles/glad.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/glad.dir/depend


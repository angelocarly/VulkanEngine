# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.15

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
CMAKE_COMMAND = /opt/clion/bin/cmake/linux/bin/cmake

# The command to remove a file.
RM = /opt/clion/bin/cmake/linux/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/magnias/Projects/VulkanEngine

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/magnias/Projects/VulkanEngine/cmake-build-debug

# Include any dependencies generated for this target.
include CMakeFiles/VulkanEngine.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/VulkanEngine.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/VulkanEngine.dir/flags.make

CMakeFiles/VulkanEngine.dir/src/main.cpp.o: CMakeFiles/VulkanEngine.dir/flags.make
CMakeFiles/VulkanEngine.dir/src/main.cpp.o: ../src/main.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/magnias/Projects/VulkanEngine/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/VulkanEngine.dir/src/main.cpp.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/VulkanEngine.dir/src/main.cpp.o -c /home/magnias/Projects/VulkanEngine/src/main.cpp

CMakeFiles/VulkanEngine.dir/src/main.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/VulkanEngine.dir/src/main.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/magnias/Projects/VulkanEngine/src/main.cpp > CMakeFiles/VulkanEngine.dir/src/main.cpp.i

CMakeFiles/VulkanEngine.dir/src/main.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/VulkanEngine.dir/src/main.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/magnias/Projects/VulkanEngine/src/main.cpp -o CMakeFiles/VulkanEngine.dir/src/main.cpp.s

# Object files for target VulkanEngine
VulkanEngine_OBJECTS = \
"CMakeFiles/VulkanEngine.dir/src/main.cpp.o"

# External object files for target VulkanEngine
VulkanEngine_EXTERNAL_OBJECTS =

VulkanEngine: CMakeFiles/VulkanEngine.dir/src/main.cpp.o
VulkanEngine: CMakeFiles/VulkanEngine.dir/build.make
VulkanEngine: /usr/lib/libvulkan.so
VulkanEngine: lib/glfw/src/libglfw3.a
VulkanEngine: libglad.a
VulkanEngine: /usr/lib/librt.so
VulkanEngine: /usr/lib/libm.so
VulkanEngine: /usr/lib/libX11.so
VulkanEngine: CMakeFiles/VulkanEngine.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/magnias/Projects/VulkanEngine/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable VulkanEngine"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/VulkanEngine.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/VulkanEngine.dir/build: VulkanEngine

.PHONY : CMakeFiles/VulkanEngine.dir/build

CMakeFiles/VulkanEngine.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/VulkanEngine.dir/cmake_clean.cmake
.PHONY : CMakeFiles/VulkanEngine.dir/clean

CMakeFiles/VulkanEngine.dir/depend:
	cd /home/magnias/Projects/VulkanEngine/cmake-build-debug && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/magnias/Projects/VulkanEngine /home/magnias/Projects/VulkanEngine /home/magnias/Projects/VulkanEngine/cmake-build-debug /home/magnias/Projects/VulkanEngine/cmake-build-debug /home/magnias/Projects/VulkanEngine/cmake-build-debug/CMakeFiles/VulkanEngine.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/VulkanEngine.dir/depend


# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.13

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
CMAKE_SOURCE_DIR = /home/wushuohan/test/muduo/examples/staticHttpWebServer

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/wushuohan/test/muduo/examples/staticHttpWebServer

# Include any dependencies generated for this target.
include CMakeFiles/staticwebserver.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/staticwebserver.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/staticwebserver.dir/flags.make

CMakeFiles/staticwebserver.dir/serverstart.cpp.o: CMakeFiles/staticwebserver.dir/flags.make
CMakeFiles/staticwebserver.dir/serverstart.cpp.o: serverstart.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/wushuohan/test/muduo/examples/staticHttpWebServer/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/staticwebserver.dir/serverstart.cpp.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/staticwebserver.dir/serverstart.cpp.o -c /home/wushuohan/test/muduo/examples/staticHttpWebServer/serverstart.cpp

CMakeFiles/staticwebserver.dir/serverstart.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/staticwebserver.dir/serverstart.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/wushuohan/test/muduo/examples/staticHttpWebServer/serverstart.cpp > CMakeFiles/staticwebserver.dir/serverstart.cpp.i

CMakeFiles/staticwebserver.dir/serverstart.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/staticwebserver.dir/serverstart.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/wushuohan/test/muduo/examples/staticHttpWebServer/serverstart.cpp -o CMakeFiles/staticwebserver.dir/serverstart.cpp.s

# Object files for target staticwebserver
staticwebserver_OBJECTS = \
"CMakeFiles/staticwebserver.dir/serverstart.cpp.o"

# External object files for target staticwebserver
staticwebserver_EXTERNAL_OBJECTS =

staticwebserver: CMakeFiles/staticwebserver.dir/serverstart.cpp.o
staticwebserver: CMakeFiles/staticwebserver.dir/build.make
staticwebserver: CMakeFiles/staticwebserver.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/wushuohan/test/muduo/examples/staticHttpWebServer/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable staticwebserver"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/staticwebserver.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/staticwebserver.dir/build: staticwebserver

.PHONY : CMakeFiles/staticwebserver.dir/build

CMakeFiles/staticwebserver.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/staticwebserver.dir/cmake_clean.cmake
.PHONY : CMakeFiles/staticwebserver.dir/clean

CMakeFiles/staticwebserver.dir/depend:
	cd /home/wushuohan/test/muduo/examples/staticHttpWebServer && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/wushuohan/test/muduo/examples/staticHttpWebServer /home/wushuohan/test/muduo/examples/staticHttpWebServer /home/wushuohan/test/muduo/examples/staticHttpWebServer /home/wushuohan/test/muduo/examples/staticHttpWebServer /home/wushuohan/test/muduo/examples/staticHttpWebServer/CMakeFiles/staticwebserver.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/staticwebserver.dir/depend


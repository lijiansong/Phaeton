# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.10

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
CMAKE_COMMAND = /Users/lijiansong/work-space/compile/llvm/cmake-3.10.1-Darwin-x86_64/CMake.app/Contents/bin/cmake

# The command to remove a file.
RM = /Users/lijiansong/work-space/compile/llvm/cmake-3.10.1-Darwin-x86_64/CMake.app/Contents/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /Users/lijiansong/work-space/compile/git/phaeton

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /Users/lijiansong/work-space/compile/git/phaeton/build

# Include any dependencies generated for this target.
include tools/ph-translate/CMakeFiles/ph-translate.dir/depend.make

# Include the progress variables for this target.
include tools/ph-translate/CMakeFiles/ph-translate.dir/progress.make

# Include the compile flags for this target's objects.
include tools/ph-translate/CMakeFiles/ph-translate.dir/flags.make

tools/ph-translate/CMakeFiles/ph-translate.dir/ph-translate.cpp.o: tools/ph-translate/CMakeFiles/ph-translate.dir/flags.make
tools/ph-translate/CMakeFiles/ph-translate.dir/ph-translate.cpp.o: ../tools/ph-translate/ph-translate.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/lijiansong/work-space/compile/git/phaeton/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object tools/ph-translate/CMakeFiles/ph-translate.dir/ph-translate.cpp.o"
	cd /Users/lijiansong/work-space/compile/git/phaeton/build/tools/ph-translate && /Library/Developer/CommandLineTools/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/ph-translate.dir/ph-translate.cpp.o -c /Users/lijiansong/work-space/compile/git/phaeton/tools/ph-translate/ph-translate.cpp

tools/ph-translate/CMakeFiles/ph-translate.dir/ph-translate.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/ph-translate.dir/ph-translate.cpp.i"
	cd /Users/lijiansong/work-space/compile/git/phaeton/build/tools/ph-translate && /Library/Developer/CommandLineTools/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Users/lijiansong/work-space/compile/git/phaeton/tools/ph-translate/ph-translate.cpp > CMakeFiles/ph-translate.dir/ph-translate.cpp.i

tools/ph-translate/CMakeFiles/ph-translate.dir/ph-translate.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/ph-translate.dir/ph-translate.cpp.s"
	cd /Users/lijiansong/work-space/compile/git/phaeton/build/tools/ph-translate && /Library/Developer/CommandLineTools/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Users/lijiansong/work-space/compile/git/phaeton/tools/ph-translate/ph-translate.cpp -o CMakeFiles/ph-translate.dir/ph-translate.cpp.s

tools/ph-translate/CMakeFiles/ph-translate.dir/ph-translate.cpp.o.requires:

.PHONY : tools/ph-translate/CMakeFiles/ph-translate.dir/ph-translate.cpp.o.requires

tools/ph-translate/CMakeFiles/ph-translate.dir/ph-translate.cpp.o.provides: tools/ph-translate/CMakeFiles/ph-translate.dir/ph-translate.cpp.o.requires
	$(MAKE) -f tools/ph-translate/CMakeFiles/ph-translate.dir/build.make tools/ph-translate/CMakeFiles/ph-translate.dir/ph-translate.cpp.o.provides.build
.PHONY : tools/ph-translate/CMakeFiles/ph-translate.dir/ph-translate.cpp.o.provides

tools/ph-translate/CMakeFiles/ph-translate.dir/ph-translate.cpp.o.provides.build: tools/ph-translate/CMakeFiles/ph-translate.dir/ph-translate.cpp.o


# Object files for target ph-translate
ph__translate_OBJECTS = \
"CMakeFiles/ph-translate.dir/ph-translate.cpp.o"

# External object files for target ph-translate
ph__translate_EXTERNAL_OBJECTS =

bin/ph-translate: tools/ph-translate/CMakeFiles/ph-translate.dir/ph-translate.cpp.o
bin/ph-translate: tools/ph-translate/CMakeFiles/ph-translate.dir/build.make
bin/ph-translate: lib/libAST.a
bin/ph-translate: lib/libTarget.a
bin/ph-translate: lib/libCodeGen.a
bin/ph-translate: lib/libOpt.a
bin/ph-translate: lib/libLexer.a
bin/ph-translate: lib/libParser.a
bin/ph-translate: lib/libSema.a
bin/ph-translate: tools/ph-translate/CMakeFiles/ph-translate.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/Users/lijiansong/work-space/compile/git/phaeton/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable ../../bin/ph-translate"
	cd /Users/lijiansong/work-space/compile/git/phaeton/build/tools/ph-translate && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/ph-translate.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
tools/ph-translate/CMakeFiles/ph-translate.dir/build: bin/ph-translate

.PHONY : tools/ph-translate/CMakeFiles/ph-translate.dir/build

tools/ph-translate/CMakeFiles/ph-translate.dir/requires: tools/ph-translate/CMakeFiles/ph-translate.dir/ph-translate.cpp.o.requires

.PHONY : tools/ph-translate/CMakeFiles/ph-translate.dir/requires

tools/ph-translate/CMakeFiles/ph-translate.dir/clean:
	cd /Users/lijiansong/work-space/compile/git/phaeton/build/tools/ph-translate && $(CMAKE_COMMAND) -P CMakeFiles/ph-translate.dir/cmake_clean.cmake
.PHONY : tools/ph-translate/CMakeFiles/ph-translate.dir/clean

tools/ph-translate/CMakeFiles/ph-translate.dir/depend:
	cd /Users/lijiansong/work-space/compile/git/phaeton/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /Users/lijiansong/work-space/compile/git/phaeton /Users/lijiansong/work-space/compile/git/phaeton/tools/ph-translate /Users/lijiansong/work-space/compile/git/phaeton/build /Users/lijiansong/work-space/compile/git/phaeton/build/tools/ph-translate /Users/lijiansong/work-space/compile/git/phaeton/build/tools/ph-translate/CMakeFiles/ph-translate.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : tools/ph-translate/CMakeFiles/ph-translate.dir/depend


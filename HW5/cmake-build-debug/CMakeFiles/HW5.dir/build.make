# CMAKE generated file: DO NOT EDIT!
# Generated by "MinGW Makefiles" Generator, CMake Version 3.19

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:


# Disable VCS-based implicit rules.
% : %,v


# Disable VCS-based implicit rules.
% : RCS/%


# Disable VCS-based implicit rules.
% : RCS/%,v


# Disable VCS-based implicit rules.
% : SCCS/s.%


# Disable VCS-based implicit rules.
% : s.%


.SUFFIXES: .hpux_make_needs_suffix_list


# Command-line flag to silence nested $(MAKE).
$(VERBOSE)MAKESILENT = -s

#Suppress display of executed commands.
$(VERBOSE).SILENT:

# A target that is always out of date.
cmake_force:

.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

SHELL = cmd.exe

# The CMake executable.
CMAKE_COMMAND = "C:\Program Files\JetBrains\CLion 2020.3\bin\cmake\win\bin\cmake.exe"

# The command to remove a file.
RM = "C:\Program Files\JetBrains\CLion 2020.3\bin\cmake\win\bin\cmake.exe" -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = "C:\Users\idanp\OneDrive\Documents\University\Operating Systems\Homework\HW5"

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = "C:\Users\idanp\OneDrive\Documents\University\Operating Systems\Homework\HW5\cmake-build-debug"

# Include any dependencies generated for this target.
include CMakeFiles/HW5.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/HW5.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/HW5.dir/flags.make

CMakeFiles/HW5.dir/pcc_client.c.obj: CMakeFiles/HW5.dir/flags.make
CMakeFiles/HW5.dir/pcc_client.c.obj: ../pcc_client.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir="C:\Users\idanp\OneDrive\Documents\University\Operating Systems\Homework\HW5\cmake-build-debug\CMakeFiles" --progress-num=$(CMAKE_PROGRESS_1) "Building C object CMakeFiles/HW5.dir/pcc_client.c.obj"
	C:\PROGRA~1\MINGW-~1\X86_64~1.0-P\mingw64\bin\gcc.exe $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles\HW5.dir\pcc_client.c.obj -c "C:\Users\idanp\OneDrive\Documents\University\Operating Systems\Homework\HW5\pcc_client.c"

CMakeFiles/HW5.dir/pcc_client.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/HW5.dir/pcc_client.c.i"
	C:\PROGRA~1\MINGW-~1\X86_64~1.0-P\mingw64\bin\gcc.exe $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E "C:\Users\idanp\OneDrive\Documents\University\Operating Systems\Homework\HW5\pcc_client.c" > CMakeFiles\HW5.dir\pcc_client.c.i

CMakeFiles/HW5.dir/pcc_client.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/HW5.dir/pcc_client.c.s"
	C:\PROGRA~1\MINGW-~1\X86_64~1.0-P\mingw64\bin\gcc.exe $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S "C:\Users\idanp\OneDrive\Documents\University\Operating Systems\Homework\HW5\pcc_client.c" -o CMakeFiles\HW5.dir\pcc_client.c.s

CMakeFiles/HW5.dir/pcc_server.c.obj: CMakeFiles/HW5.dir/flags.make
CMakeFiles/HW5.dir/pcc_server.c.obj: ../pcc_server.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir="C:\Users\idanp\OneDrive\Documents\University\Operating Systems\Homework\HW5\cmake-build-debug\CMakeFiles" --progress-num=$(CMAKE_PROGRESS_2) "Building C object CMakeFiles/HW5.dir/pcc_server.c.obj"
	C:\PROGRA~1\MINGW-~1\X86_64~1.0-P\mingw64\bin\gcc.exe $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles\HW5.dir\pcc_server.c.obj -c "C:\Users\idanp\OneDrive\Documents\University\Operating Systems\Homework\HW5\pcc_server.c"

CMakeFiles/HW5.dir/pcc_server.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/HW5.dir/pcc_server.c.i"
	C:\PROGRA~1\MINGW-~1\X86_64~1.0-P\mingw64\bin\gcc.exe $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E "C:\Users\idanp\OneDrive\Documents\University\Operating Systems\Homework\HW5\pcc_server.c" > CMakeFiles\HW5.dir\pcc_server.c.i

CMakeFiles/HW5.dir/pcc_server.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/HW5.dir/pcc_server.c.s"
	C:\PROGRA~1\MINGW-~1\X86_64~1.0-P\mingw64\bin\gcc.exe $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S "C:\Users\idanp\OneDrive\Documents\University\Operating Systems\Homework\HW5\pcc_server.c" -o CMakeFiles\HW5.dir\pcc_server.c.s

# Object files for target HW5
HW5_OBJECTS = \
"CMakeFiles/HW5.dir/pcc_client.c.obj" \
"CMakeFiles/HW5.dir/pcc_server.c.obj"

# External object files for target HW5
HW5_EXTERNAL_OBJECTS =

HW5.exe: CMakeFiles/HW5.dir/pcc_client.c.obj
HW5.exe: CMakeFiles/HW5.dir/pcc_server.c.obj
HW5.exe: CMakeFiles/HW5.dir/build.make
HW5.exe: CMakeFiles/HW5.dir/linklibs.rsp
HW5.exe: CMakeFiles/HW5.dir/objects1.rsp
HW5.exe: CMakeFiles/HW5.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir="C:\Users\idanp\OneDrive\Documents\University\Operating Systems\Homework\HW5\cmake-build-debug\CMakeFiles" --progress-num=$(CMAKE_PROGRESS_3) "Linking C executable HW5.exe"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles\HW5.dir\link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/HW5.dir/build: HW5.exe

.PHONY : CMakeFiles/HW5.dir/build

CMakeFiles/HW5.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles\HW5.dir\cmake_clean.cmake
.PHONY : CMakeFiles/HW5.dir/clean

CMakeFiles/HW5.dir/depend:
	$(CMAKE_COMMAND) -E cmake_depends "MinGW Makefiles" "C:\Users\idanp\OneDrive\Documents\University\Operating Systems\Homework\HW5" "C:\Users\idanp\OneDrive\Documents\University\Operating Systems\Homework\HW5" "C:\Users\idanp\OneDrive\Documents\University\Operating Systems\Homework\HW5\cmake-build-debug" "C:\Users\idanp\OneDrive\Documents\University\Operating Systems\Homework\HW5\cmake-build-debug" "C:\Users\idanp\OneDrive\Documents\University\Operating Systems\Homework\HW5\cmake-build-debug\CMakeFiles\HW5.dir\DependInfo.cmake" --color=$(COLOR)
.PHONY : CMakeFiles/HW5.dir/depend


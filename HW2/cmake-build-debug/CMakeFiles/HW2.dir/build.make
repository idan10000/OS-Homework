# CMAKE generated file: DO NOT EDIT!
# Generated by "NMake Makefiles" Generator, CMake Version 3.19

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:


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

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE
NULL=nul
!ENDIF
SHELL = cmd.exe

# The CMake executable.
CMAKE_COMMAND = "C:\Program Files\JetBrains\CLion 2020.3\bin\cmake\win\bin\cmake.exe"

# The command to remove a file.
RM = "C:\Program Files\JetBrains\CLion 2020.3\bin\cmake\win\bin\cmake.exe" -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = "C:\Users\idanp\OneDrive\Documents\University\Operating Systems\Homework\HW2"

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = "C:\Users\idanp\OneDrive\Documents\University\Operating Systems\Homework\HW2\cmake-build-debug"

# Include any dependencies generated for this target.
include CMakeFiles\HW2.dir\depend.make

# Include the progress variables for this target.
include CMakeFiles\HW2.dir\progress.make

# Include the compile flags for this target's objects.
include CMakeFiles\HW2.dir\flags.make

CMakeFiles\HW2.dir\myshell.c.obj: CMakeFiles\HW2.dir\flags.make
CMakeFiles\HW2.dir\myshell.c.obj: ..\myshell.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir="C:\Users\idanp\OneDrive\Documents\University\Operating Systems\Homework\HW2\cmake-build-debug\CMakeFiles" --progress-num=$(CMAKE_PROGRESS_1) "Building C object CMakeFiles/HW2.dir/myshell.c.obj"
	C:\PROGRA~2\MICROS~2\2019\COMMUN~1\VC\Tools\MSVC\1428~1.293\bin\Hostx86\x86\cl.exe @<<
 /nologo $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) /FoCMakeFiles\HW2.dir\myshell.c.obj /FdCMakeFiles\HW2.dir\ /FS -c "C:\Users\idanp\OneDrive\Documents\University\Operating Systems\Homework\HW2\myshell.c"
<<

CMakeFiles\HW2.dir\myshell.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/HW2.dir/myshell.c.i"
	C:\PROGRA~2\MICROS~2\2019\COMMUN~1\VC\Tools\MSVC\1428~1.293\bin\Hostx86\x86\cl.exe > CMakeFiles\HW2.dir\myshell.c.i @<<
 /nologo $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E "C:\Users\idanp\OneDrive\Documents\University\Operating Systems\Homework\HW2\myshell.c"
<<

CMakeFiles\HW2.dir\myshell.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/HW2.dir/myshell.c.s"
	C:\PROGRA~2\MICROS~2\2019\COMMUN~1\VC\Tools\MSVC\1428~1.293\bin\Hostx86\x86\cl.exe @<<
 /nologo $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) /FoNUL /FAs /FaCMakeFiles\HW2.dir\myshell.c.s /c "C:\Users\idanp\OneDrive\Documents\University\Operating Systems\Homework\HW2\myshell.c"
<<

# Object files for target HW2
HW2_OBJECTS = \
"CMakeFiles\HW2.dir\myshell.c.obj"

# External object files for target HW2
HW2_EXTERNAL_OBJECTS =

HW2.exe: CMakeFiles\HW2.dir\myshell.c.obj
HW2.exe: CMakeFiles\HW2.dir\build.make
HW2.exe: CMakeFiles\HW2.dir\objects1.rsp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir="C:\Users\idanp\OneDrive\Documents\University\Operating Systems\Homework\HW2\cmake-build-debug\CMakeFiles" --progress-num=$(CMAKE_PROGRESS_2) "Linking C executable HW2.exe"
	"C:\Program Files\JetBrains\CLion 2020.3\bin\cmake\win\bin\cmake.exe" -E vs_link_exe --intdir=CMakeFiles\HW2.dir --rc=C:\PROGRA~2\WI3CF2~1\10\bin\100183~1.0\x86\rc.exe --mt=C:\PROGRA~2\WI3CF2~1\10\bin\100183~1.0\x86\mt.exe --manifests -- C:\PROGRA~2\MICROS~2\2019\COMMUN~1\VC\Tools\MSVC\1428~1.293\bin\Hostx86\x86\link.exe /nologo @CMakeFiles\HW2.dir\objects1.rsp @<<
 /out:HW2.exe /implib:HW2.lib /pdb:"C:\Users\idanp\OneDrive\Documents\University\Operating Systems\Homework\HW2\cmake-build-debug\HW2.pdb" /version:0.0 /machine:X86 /debug /INCREMENTAL /subsystem:console  kernel32.lib user32.lib gdi32.lib winspool.lib shell32.lib ole32.lib oleaut32.lib uuid.lib comdlg32.lib advapi32.lib 
<<

# Rule to build all files generated by this target.
CMakeFiles\HW2.dir\build: HW2.exe

.PHONY : CMakeFiles\HW2.dir\build

CMakeFiles\HW2.dir\clean:
	$(CMAKE_COMMAND) -P CMakeFiles\HW2.dir\cmake_clean.cmake
.PHONY : CMakeFiles\HW2.dir\clean

CMakeFiles\HW2.dir\depend:
	$(CMAKE_COMMAND) -E cmake_depends "NMake Makefiles" "C:\Users\idanp\OneDrive\Documents\University\Operating Systems\Homework\HW2" "C:\Users\idanp\OneDrive\Documents\University\Operating Systems\Homework\HW2" "C:\Users\idanp\OneDrive\Documents\University\Operating Systems\Homework\HW2\cmake-build-debug" "C:\Users\idanp\OneDrive\Documents\University\Operating Systems\Homework\HW2\cmake-build-debug" "C:\Users\idanp\OneDrive\Documents\University\Operating Systems\Homework\HW2\cmake-build-debug\CMakeFiles\HW2.dir\DependInfo.cmake" --color=$(COLOR)
.PHONY : CMakeFiles\HW2.dir\depend


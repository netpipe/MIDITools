# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 2.8

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
CMAKE_SOURCE_DIR = /home/clayton/Desktop/vmpk-0.5.1

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/clayton/Desktop/vmpk-0.5.1

# Utility rule file for manpages.

# Include the progress variables for this target.
include man/CMakeFiles/manpages.dir/progress.make

man/CMakeFiles/manpages: man/vmpk.1

man/vmpk.1: man/vmpk.xml
	$(CMAKE_COMMAND) -E cmake_progress_report /home/clayton/Desktop/vmpk-0.5.1/CMakeFiles $(CMAKE_PROGRESS_1)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --blue --bold "Generating vmpk.1"
	cd /home/clayton/Desktop/vmpk-0.5.1/man && /usr/bin/xsltproc --nonet --xinclude --xincludestyle --output /home/clayton/Desktop/vmpk-0.5.1/man/vmpk.1 http://docbook.sourceforge.net/release/xsl/current/manpages/docbook.xsl /home/clayton/Desktop/vmpk-0.5.1/man/vmpk.xml

manpages: man/CMakeFiles/manpages
manpages: man/vmpk.1
manpages: man/CMakeFiles/manpages.dir/build.make
.PHONY : manpages

# Rule to build all files generated by this target.
man/CMakeFiles/manpages.dir/build: manpages
.PHONY : man/CMakeFiles/manpages.dir/build

man/CMakeFiles/manpages.dir/clean:
	cd /home/clayton/Desktop/vmpk-0.5.1/man && $(CMAKE_COMMAND) -P CMakeFiles/manpages.dir/cmake_clean.cmake
.PHONY : man/CMakeFiles/manpages.dir/clean

man/CMakeFiles/manpages.dir/depend:
	cd /home/clayton/Desktop/vmpk-0.5.1 && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/clayton/Desktop/vmpk-0.5.1 /home/clayton/Desktop/vmpk-0.5.1/man /home/clayton/Desktop/vmpk-0.5.1 /home/clayton/Desktop/vmpk-0.5.1/man /home/clayton/Desktop/vmpk-0.5.1/man/CMakeFiles/manpages.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : man/CMakeFiles/manpages.dir/depend


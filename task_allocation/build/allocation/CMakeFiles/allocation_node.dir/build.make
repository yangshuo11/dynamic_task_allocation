# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.5

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
CMAKE_SOURCE_DIR = /home/nubot8/dynamic_allocation/task_allocation/src

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/nubot8/dynamic_allocation/task_allocation/build

# Include any dependencies generated for this target.
include allocation/CMakeFiles/allocation_node.dir/depend.make

# Include the progress variables for this target.
include allocation/CMakeFiles/allocation_node.dir/progress.make

# Include the compile flags for this target's objects.
include allocation/CMakeFiles/allocation_node.dir/flags.make

allocation/CMakeFiles/allocation_node.dir/src/main.cpp.o: allocation/CMakeFiles/allocation_node.dir/flags.make
allocation/CMakeFiles/allocation_node.dir/src/main.cpp.o: /home/nubot8/dynamic_allocation/task_allocation/src/allocation/src/main.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/nubot8/dynamic_allocation/task_allocation/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object allocation/CMakeFiles/allocation_node.dir/src/main.cpp.o"
	cd /home/nubot8/dynamic_allocation/task_allocation/build/allocation && /usr/bin/c++   $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/allocation_node.dir/src/main.cpp.o -c /home/nubot8/dynamic_allocation/task_allocation/src/allocation/src/main.cpp

allocation/CMakeFiles/allocation_node.dir/src/main.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/allocation_node.dir/src/main.cpp.i"
	cd /home/nubot8/dynamic_allocation/task_allocation/build/allocation && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/nubot8/dynamic_allocation/task_allocation/src/allocation/src/main.cpp > CMakeFiles/allocation_node.dir/src/main.cpp.i

allocation/CMakeFiles/allocation_node.dir/src/main.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/allocation_node.dir/src/main.cpp.s"
	cd /home/nubot8/dynamic_allocation/task_allocation/build/allocation && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/nubot8/dynamic_allocation/task_allocation/src/allocation/src/main.cpp -o CMakeFiles/allocation_node.dir/src/main.cpp.s

allocation/CMakeFiles/allocation_node.dir/src/main.cpp.o.requires:

.PHONY : allocation/CMakeFiles/allocation_node.dir/src/main.cpp.o.requires

allocation/CMakeFiles/allocation_node.dir/src/main.cpp.o.provides: allocation/CMakeFiles/allocation_node.dir/src/main.cpp.o.requires
	$(MAKE) -f allocation/CMakeFiles/allocation_node.dir/build.make allocation/CMakeFiles/allocation_node.dir/src/main.cpp.o.provides.build
.PHONY : allocation/CMakeFiles/allocation_node.dir/src/main.cpp.o.provides

allocation/CMakeFiles/allocation_node.dir/src/main.cpp.o.provides.build: allocation/CMakeFiles/allocation_node.dir/src/main.cpp.o


allocation/CMakeFiles/allocation_node.dir/src/task_allocation.cpp.o: allocation/CMakeFiles/allocation_node.dir/flags.make
allocation/CMakeFiles/allocation_node.dir/src/task_allocation.cpp.o: /home/nubot8/dynamic_allocation/task_allocation/src/allocation/src/task_allocation.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/nubot8/dynamic_allocation/task_allocation/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object allocation/CMakeFiles/allocation_node.dir/src/task_allocation.cpp.o"
	cd /home/nubot8/dynamic_allocation/task_allocation/build/allocation && /usr/bin/c++   $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/allocation_node.dir/src/task_allocation.cpp.o -c /home/nubot8/dynamic_allocation/task_allocation/src/allocation/src/task_allocation.cpp

allocation/CMakeFiles/allocation_node.dir/src/task_allocation.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/allocation_node.dir/src/task_allocation.cpp.i"
	cd /home/nubot8/dynamic_allocation/task_allocation/build/allocation && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/nubot8/dynamic_allocation/task_allocation/src/allocation/src/task_allocation.cpp > CMakeFiles/allocation_node.dir/src/task_allocation.cpp.i

allocation/CMakeFiles/allocation_node.dir/src/task_allocation.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/allocation_node.dir/src/task_allocation.cpp.s"
	cd /home/nubot8/dynamic_allocation/task_allocation/build/allocation && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/nubot8/dynamic_allocation/task_allocation/src/allocation/src/task_allocation.cpp -o CMakeFiles/allocation_node.dir/src/task_allocation.cpp.s

allocation/CMakeFiles/allocation_node.dir/src/task_allocation.cpp.o.requires:

.PHONY : allocation/CMakeFiles/allocation_node.dir/src/task_allocation.cpp.o.requires

allocation/CMakeFiles/allocation_node.dir/src/task_allocation.cpp.o.provides: allocation/CMakeFiles/allocation_node.dir/src/task_allocation.cpp.o.requires
	$(MAKE) -f allocation/CMakeFiles/allocation_node.dir/build.make allocation/CMakeFiles/allocation_node.dir/src/task_allocation.cpp.o.provides.build
.PHONY : allocation/CMakeFiles/allocation_node.dir/src/task_allocation.cpp.o.provides

allocation/CMakeFiles/allocation_node.dir/src/task_allocation.cpp.o.provides.build: allocation/CMakeFiles/allocation_node.dir/src/task_allocation.cpp.o


# Object files for target allocation_node
allocation_node_OBJECTS = \
"CMakeFiles/allocation_node.dir/src/main.cpp.o" \
"CMakeFiles/allocation_node.dir/src/task_allocation.cpp.o"

# External object files for target allocation_node
allocation_node_EXTERNAL_OBJECTS =

devel/lib/allocation/allocation_node: allocation/CMakeFiles/allocation_node.dir/src/main.cpp.o
devel/lib/allocation/allocation_node: allocation/CMakeFiles/allocation_node.dir/src/task_allocation.cpp.o
devel/lib/allocation/allocation_node: allocation/CMakeFiles/allocation_node.dir/build.make
devel/lib/allocation/allocation_node: devel/lib/libworld_model.a
devel/lib/allocation/allocation_node: /opt/ros/kinetic/lib/libdynamic_reconfigure_config_init_mutex.so
devel/lib/allocation/allocation_node: /opt/ros/kinetic/lib/libmessage_filters.so
devel/lib/allocation/allocation_node: /opt/ros/kinetic/lib/libroscpp.so
devel/lib/allocation/allocation_node: /usr/lib/x86_64-linux-gnu/libboost_signals.so
devel/lib/allocation/allocation_node: /usr/lib/x86_64-linux-gnu/libboost_filesystem.so
devel/lib/allocation/allocation_node: /opt/ros/kinetic/lib/librosconsole.so
devel/lib/allocation/allocation_node: /opt/ros/kinetic/lib/librosconsole_log4cxx.so
devel/lib/allocation/allocation_node: /opt/ros/kinetic/lib/librosconsole_backend_interface.so
devel/lib/allocation/allocation_node: /usr/lib/x86_64-linux-gnu/liblog4cxx.so
devel/lib/allocation/allocation_node: /usr/lib/x86_64-linux-gnu/libboost_regex.so
devel/lib/allocation/allocation_node: /opt/ros/kinetic/lib/libxmlrpcpp.so
devel/lib/allocation/allocation_node: /opt/ros/kinetic/lib/libroscpp_serialization.so
devel/lib/allocation/allocation_node: /opt/ros/kinetic/lib/librostime.so
devel/lib/allocation/allocation_node: /opt/ros/kinetic/lib/libcpp_common.so
devel/lib/allocation/allocation_node: /usr/lib/x86_64-linux-gnu/libboost_system.so
devel/lib/allocation/allocation_node: /usr/lib/x86_64-linux-gnu/libboost_thread.so
devel/lib/allocation/allocation_node: /usr/lib/x86_64-linux-gnu/libboost_chrono.so
devel/lib/allocation/allocation_node: /usr/lib/x86_64-linux-gnu/libboost_date_time.so
devel/lib/allocation/allocation_node: /usr/lib/x86_64-linux-gnu/libboost_atomic.so
devel/lib/allocation/allocation_node: /usr/lib/x86_64-linux-gnu/libpthread.so
devel/lib/allocation/allocation_node: /usr/lib/x86_64-linux-gnu/libconsole_bridge.so
devel/lib/allocation/allocation_node: /opt/Qt5.9.1/5.9.1/gcc_64/lib/libQt5Widgets.so.5.9.1
devel/lib/allocation/allocation_node: /opt/Qt5.9.1/5.9.1/gcc_64/lib/libQt5Gui.so.5.9.1
devel/lib/allocation/allocation_node: /opt/Qt5.9.1/5.9.1/gcc_64/lib/libQt5Core.so.5.9.1
devel/lib/allocation/allocation_node: allocation/CMakeFiles/allocation_node.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/nubot8/dynamic_allocation/task_allocation/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Linking CXX executable ../devel/lib/allocation/allocation_node"
	cd /home/nubot8/dynamic_allocation/task_allocation/build/allocation && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/allocation_node.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
allocation/CMakeFiles/allocation_node.dir/build: devel/lib/allocation/allocation_node

.PHONY : allocation/CMakeFiles/allocation_node.dir/build

allocation/CMakeFiles/allocation_node.dir/requires: allocation/CMakeFiles/allocation_node.dir/src/main.cpp.o.requires
allocation/CMakeFiles/allocation_node.dir/requires: allocation/CMakeFiles/allocation_node.dir/src/task_allocation.cpp.o.requires

.PHONY : allocation/CMakeFiles/allocation_node.dir/requires

allocation/CMakeFiles/allocation_node.dir/clean:
	cd /home/nubot8/dynamic_allocation/task_allocation/build/allocation && $(CMAKE_COMMAND) -P CMakeFiles/allocation_node.dir/cmake_clean.cmake
.PHONY : allocation/CMakeFiles/allocation_node.dir/clean

allocation/CMakeFiles/allocation_node.dir/depend:
	cd /home/nubot8/dynamic_allocation/task_allocation/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/nubot8/dynamic_allocation/task_allocation/src /home/nubot8/dynamic_allocation/task_allocation/src/allocation /home/nubot8/dynamic_allocation/task_allocation/build /home/nubot8/dynamic_allocation/task_allocation/build/allocation /home/nubot8/dynamic_allocation/task_allocation/build/allocation/CMakeFiles/allocation_node.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : allocation/CMakeFiles/allocation_node.dir/depend


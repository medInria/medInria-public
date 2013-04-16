# DO NOT USE DIRECTLY THIS FILE!!
# Copy it into a local directory, on the machine you want the dartboard server to run.
# Edit the file and replace the values with the ones that match your system.
# Edit also the file experimentalwin.bat, and create a task with the task manager.

SET (CTEST_SOURCE_DIRECTORY "D:/dashboardbuilds/vtkINRIA3D/experimental/src")
SET (CTEST_BINARY_DIRECTORY "D:/dashboardbuilds/vtkINRIA3D/experimental/VC++")

SET (CTEST_CVS_COMMAND   "C:/Applis/cygwin/bin/svn.exe")
SET (CTEST_CVS_CHECKOUT  "${CTEST_CVS_COMMAND} co svn://scm.gforge.inria.fr/svn/vtkinria3d ${CTEST_SOURCE_DIRECTORY}")

# which ctest command to use for running the dashboard
SET (CTEST_COMMAND "D:/CMake 2.4/bin/ctest.exe -D Experimental")

# what cmake command to use for configuring this dashboard
SET (CTEST_CMAKE_COMMAND "D:/CMake 2.4/bin/cmake.exe")


####################################################################
# The values in this section are optional you can either
# have them or leave them commented out
####################################################################

# should ctest wipe the binary tree before running
SET (CTEST_START_WITH_EMPTY_BINARY_DIRECTORY TRUE)

# this is the initial cache to use for the binary tree, be careful to escape
# any quotes inside of this string if you use it
SET (CTEST_INITIAL_CACHE "
CMAKE_MAKE_PROGRAM:FILEPATH=devenv
CMAKE_GENERATOR:INTERNAL=Visual Studio 7 .NET 2003
BUILDNAME:STRING=Win32-VS7.NET-2003
SITE:STRING=pare
SVNCOMMAND:FILEPATH=C:/Applis/cygwin/bin/svn.exe
VTK_DIR:STRING=D:/VTK/VTK-5.0.3/VC++
vtkINRIA3D_USE_ITK:BOOL=ON
ITK_DIR:STRING=D:/ITK/ITK-3.2.0/VC++
vtkINRIA3D_USE_KWWIDGETS:BOOL=ON
vtkINRIA3D_USE_WXWINDOWS:BOOL=ON
wxWidgets_LIB_DIR:STRING=D:/wxWidgets/wxMSW-2.8.3/lib/vc_lib
wxWidgets_ROOT_DIR:STRING=D:/wxWidgets/wxMSW-2.8.3
KWWidgets_DIR:STRING=D:/KWWidgets/VC++
")
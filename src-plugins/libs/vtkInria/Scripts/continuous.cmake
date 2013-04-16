SET (CTEST_SOURCE_DIRECTORY "/u/sylvius/home/dashboardBuilds/vtkINRIA3D/continuous/src")
SET (CTEST_BINARY_DIRECTORY "/u/sylvius/home/dashboardBuilds/vtkINRIA3D/continuous/linux-build")

SET (CTEST_CVS_COMMAND   "svn")
SET (CTEST_CVS_CHECKOUT  "${CTEST_CVS_COMMAND} co svn://scm.gforge.inria.fr/svn/vtkinria3d ${CTEST_SOURCE_DIRECTORY}")

# which ctest command to use for running the dashboard
SET (CTEST_COMMAND "/proj/asclepios/home/import/CMake/testing/install/linux_FC3/bin/ctest -D Continuous")

# what cmake command to use for configuring this dashboard
SET (CTEST_CMAKE_COMMAND "/proj/asclepios/home/import/CMake/testing/install/linux_FC3/bin/cmake")


####################################################################
# The values in this section are optional you can either
# have them or leave them commented out
####################################################################

# should ctest wipe the binary tree before running
SET (CTEST_START_WITH_EMPTY_BINARY_DIRECTORY TRUE)


SET (CTEST_CONTINUOUS_DURATION 600)
SET (CTEST_CONTINUOUS_MINIMUM_INTERVAL 10)
SET (CTEST_START_WITH_EMPTY_BINARY_DIRECTORY_ONCE 1)


# this is the initial cache to use for the binary tree, be careful to escape
# any quotes inside of this string if you use it
SET (CTEST_INITIAL_CACHE "
SITE:STRING=PleaseChangeSite
VTK_DIR:STRING=/proj/asclepios/home/import/VTK/VTK/release/install/linux_FC3/lib/vtk-5.0
vtkINRIA3D_USE_ITK:BOOL=ON
ITK_DIR:STRING=/proj/asclepios/home/import/ITK/Insight/release/install/linux_FC3/lib/InsightToolkit
vtkINRIA3D_USE_KWWIDGETS:BOOL=ON
vtkINRIA3D_USE_WXWINDOWS:BOOL=ON
wxWidgets_CONFIG_EXECUTABLE:STRING=/proj/asclepios/home/import/wxWidgets/release/install/linux_FC3/bin/wx-config
KWWidgets_DIR:STRING=/proj/asclepios/home/import/KWWidgets/install/linux_FC3/lib/KWWidgets
")

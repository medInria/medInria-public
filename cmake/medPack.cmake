#add dependancy on dtk for debian packages:
set (CPACK_DEBIAN_PACKAGE_DEPENDS "dtk (>= ${${PROJECT_NAME}_VERSION_MAJOR}.${${PROJECT_NAME}_VERSION_MINOR})") 

#
# this module looks for GTK-2.0 (http://www.gtk.org)
#
# it will search for the following values
#
# PKGCONFIG      = Package configuration program  
#                  (http://www.freedesktop.org/Software/pkgconfig)
#
# and set those variables :
#
# GTK_CFLAGS      = Compilation flags and include directories. 
#                     Ex: ADD_DEFINITIONS(${GTK_CFLAGS})
# GTK_INCLUDE_DIRS= Include directories. 
#                     Ex: INCLUDE_DIRECTORIES(${GTK_INCLUDE_DIRS})
# GTK_LINK_LIBS   = Libraries to link with. 
#                     Ex: LINK_LIBRARIES(${GTK_LINK_LIBS})
# GTK_LINK_DIRS   = Path to find the libs to link with. 
#                     Ex: LINK_DIRECTORIES(${GTK_LINK_DIRS})
#
#
FIND_PROGRAM(PKGCONFIG pkg-config DOC "Full path to the ${PKGCONFIG} tool")

IF (PKGCONFIG)

  EXEC_PROGRAM (${PKGCONFIG} ARGS --cflags gtk+-2.0 # gtkglext-1.0
	OUTPUT_VARIABLE PKG_CFLAGS)

  EXEC_PROGRAM (${PKGCONFIG} ARGS --cflags-only-I gtk+-2.0 # gtkglext-1.0
	OUTPUT_VARIABLE PKG_INCLUDE_DIRS)

  EXEC_PROGRAM (${PKGCONFIG} ARGS --libs-only-L gtk+-2.0 # gtkglext-1.0
	OUTPUT_VARIABLE PKG_LINK_DIRS)
  EXEC_PROGRAM (${PKGCONFIG} ARGS --libs-only-l gtk+-2.0 # gtkglext-1.0
	OUTPUT_VARIABLE PKG_LINK_LIBS)
  
  STRING(REGEX REPLACE "^-I[^ ]*| -I[^ ]*" "" REX_CFLAGS "${PKG_CFLAGS}")

  STRING(REGEX REPLACE " -I" ";" REX "${PKG_INCLUDE_DIRS}")
  STRING(REGEX REPLACE "^-I" "" REX_INCLUDE_DIRS "${REX}")

  STRING(REGEX REPLACE " -L" ";" REX "${PKG_LINK_DIRS}")
  STRING(REGEX REPLACE "^-L" "" REX_LINK_DIRS "${REX}")
  
  STRING(REGEX REPLACE " -l" ";" REX "${PKG_LINK_LIBS}")
  STRING(REGEX REPLACE "^-l" "" REX_LINK_LIBS "${REX}")
  
  #MESSAGE(PKG_LINK_LIBS= "${PKG_LINK_LIBS}")
  #MESSAGE(REX= "${REX}")
  #MESSAGE(REX_LINK_LIBS= "${REX_LINK_LIBS}")

  SET(GTK_CFLAGS "${REX_CFLAGS}" CACHE STRING 
	"GTK compilation flags" )
  SET(GTK_INCLUDE_DIRS "${REX_INCLUDE_DIRS}" CACHE STRING 
	"Path to find the GTK headers" )
  SET(GTK_LINK_DIRS "${REX_LINK_DIRS}" CACHE STRING 
	"GTK path to find the libs to link against" )
  SET(GTK_LINK_LIBS "${REX_LINK_LIBS}" CACHE STRING 
	"GTK libraries to link with" )

  
  #MESSAGE (GTK_CFLAGS="${GTK_CFLAGS}")
  #MESSAGE (GTK_LINK_DIRS="${GTK_LINK_DIRS}")
  #MESSAGE (GTK_LINK_LIBS="${GTK_LINK_LIBS}")

  MARK_AS_ADVANCED(GTK_CFLAGS GTK_INCLUDE_DIRS GTK_LINK_DIRS GTK_LINK_LIBS)

  SET(GTK-2_FOUND ON CACHE INTERNAL "GTK-2 found" FORCE)
ELSE (PKGCONFIG)
  SET(GTK-2_FOUND OFF CACHE INTERNAL "GTK-2 not found" FORCE)
ENDIF (PKGCONFIG)



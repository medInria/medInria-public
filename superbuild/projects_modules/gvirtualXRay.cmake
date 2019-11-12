function(gvirtualXRay_project) 

set(ep gvirtualXRay)

## #############################################################################
## List the dependencies of the project
## #############################################################################

list(APPEND ${ep}_dependencies "xcom")

## #############################################################################
## Prepare the project
## ############################################################################# 

EP_Initialisation(${ep}
  USE_SYSTEM OFF
  BUILD_SHARED_LIBS ON
  REQUIRED_FOR_PLUGINS OFF
  )

if (NOT USE_SYSTEM_${ep})

set(LIB_EXT "dylib")  
if(UNIX)
	set(LIB_EXT "so") 
endif() 

## #############################################################################
## Add specific cmake arguments for configuration step of the project
## #############################################################################

# set compilation flags
if (UNIX OR APPLE)
  if (CMAKE_COMPILER_IS_GNUCC AND CMAKE_C_COMPILER_VERSION VERSION_LESS 5.4)
    set(${ep}_c_flags "${${ep}_c_flags} -std=c99")
  else()
    set(${ep}_c_flags "${${ep}_c_flags}")
  endif()
  set(${ep}_cxx_flags "${${ep}_cxx_flags}")
endif()


message(${CMAKE_CURRENT_SOURCE_DIR})

set(cmake_args
        ${ep_common_cache_args}
        -DCMAKE_C_FLAGS:STRING=${${ep}_c_flags}
        -DCMAKE_CXX_FLAGS:STRING=${${ep}_cxx_flags}
    -DCMAKE_SHARED_LINKER_FLAGS:STRING=${${ep}_shared_linker_flags}
    -DCMAKE_INSTALL_PREFIX:PATH=<INSTALL_DIR>
    -DBUILD_TUTORIALS:bool=OFF
    -DBUILD_EXAMPLES:bool=OFF 
    #-DNDEBUG
-DGLEW_INCLUDE_DIR=${EP_PATH_SOURCE}/glew/include/
-DZLIB_INCLUDE_DIR=/usr/include/
-U HAS_ITK
-DITK_DIR=/  
-DXCOM_PATH=${EP_PATH_SOURCE}/../build/XCOM
-DCMAKE_INSTALL_PREFIX:PATH=${EP_PATH_SOURCE}/../build/gvirtualXRay
)


if(UNIX) 
	set(cmake_args ${cmake_args} 
	    -G "Unix Makefiles"
            -DGLEW_LIBRARY_DIR="${EP_PATH_SOURCE}/../build/glew/build/lib/"
	    -DCMAKE_BUILD_TYPE=RELEASE
            -DZLIB_LIBRARY_RELEASE="/lib/x86_64-linux-gnu/libz.so.1"
	    -DZLIB_LIBRARY_DEBUG="/lib/x86_64-linux-gnu/libz.so.1")
 else() 
	set(cmake_args ${cmake_args}
              -DCMAKE_BUILD_TYPE=RELEASE 
              -DZLIB_LIBRARY_RELEASE="/usr/lib/libz.1.${LIB_EXT}"
              -DZLIB_LIBRARY_DEBUG="/usr/lib/libz.1.${LIB_EXT}")
endif()


 
## #############################################################################
## Add external-project
## ############################################################################# 

message("CMAKE CURRENT SOURCE DIR",${CMAKE_CURRENT_SOURCE_DIR}) 

if(APPLE)
  set (cmake_args
  ${cmake_args}
  -DCMAKE_MACOSX_RPATH=OFF)
endif(APPLE)

## cd ${CMAKE_CURRENT_SOURCE_DIR}/build/gvirtualXRay-prefix 
set(patch_dir ${EP_PATH_SOURCE}/../../medInria-public/superbuild/patches)
set(source_dir ${EP_PATH_SOURCE}/gVirtualXRay-1.0.0-Source)
set(build_dir ${EP_PATH_SOURCE}/../build/gvirtualXRay)  


ExternalProject_Add(gvirtualXRay
        PREFIX ${EP_PATH_SOURCE}
	SOURCE_DIR ${EP_PATH_SOURCE}/${ep} 	
	CMAKE_GENERATOR ${gen}
        CMAKE_GENERATOR_PLATFORM ${CMAKE_GENERATOR_PLATFORM}
        CMAKE_ARGS ${cmake_args}

	DEPENDS ${${ep}_dependencies} 
	DOWNLOAD_COMMAND curl -Lo gVirtualXRay-1.0.0-Source.zip https://sourceforge.net/projects/gvirtualxray/files/1.0/gVirtualXRay-1.0.0-Source.zip/download
	DOWNLOAD_NAME	gVirtualXRay-1.0.0-Source.zip
	PATCH_COMMAND 	unzip   ${EP_PATH_SOURCE}/src/gVirtualXRay-1.0.0-Source.zip -d ${EP_PATH_SOURCE}/ && 
	                cd ${source_dir} && 
			patch ./cmake/CMakeLists.txt < ${patch_dir}/patchgVirtualXRay.txt  &&  
			patch -i ${patch_dir}/polygonMeshHeaderPatch.txt  ${source_dir}/include/gVirtualXRay/PolygonMesh.h && 
			patch -i ${patch_dir}/polygonMeshHeaderInlPatch.txt ${source_dir}/include/gVirtualXRay/PolygonMesh.inl    
	CONFIGURE_COMMAND mkdir -p ${build_dir} && 
	                  cd ${build_dir} && 
	                  cmake  ${cmake_args} ${source_dir}/cmake/
	BUILD_COMMAND  cd ${build_dir} &&  
	               make install && 
		       touch ${source_dir}/include/gVirtualXRay/gVirtualXRayConfig.h    ## might have to put 
  	INSTALL_COMMAND ""
  	UPDATE_COMMAND ""
	)


## #############################################################################
## Set variable to provide infos about the project
## #############################################################################
ExternalProject_Get_Property(${ep} binary_dir)
set(${ep}_DIR ${binary_dir} PARENT_SCOPE)

endif()


endfunction()



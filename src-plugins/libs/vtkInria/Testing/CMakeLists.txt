set(TEST 
  TestSynchronizedViews
)

set(EXE_NAME ${TEST})
add_executable(${EXE_NAME} ${TEST}.cxx)

target_link_libraries(${EXE_NAME}
  vtkRenderingAddOn
)







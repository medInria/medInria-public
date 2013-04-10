set(TEST 
  TestSynchronizedViews
)

set(EXE_NAME ${TEST})
ADD_EXECUTABLE(${EXE_NAME} ${TEST}.cxx)

target_link_libraries(${EXE_NAME}
  vtkRenderingAddOn
)







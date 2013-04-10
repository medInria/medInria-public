set(TEST 
  TestSynchronizedViews
)

set(EXE_NAME ${TEST})
ADD_EXECUTABLE(${EXE_NAME} ${TEST}.cxx)

TARGET_LINK_LIBRARIES(${EXE_NAME}
  vtkRenderingAddOn
)







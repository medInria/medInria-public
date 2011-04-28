
INCLUDE_DIRECTORIES("..")


add_executable(itkGenerateThumbnails
  itkGenerateThumbnails.cxx)
  
target_link_libraries(itkGenerateThumbnails
  ${ITK_LIBRARIES})

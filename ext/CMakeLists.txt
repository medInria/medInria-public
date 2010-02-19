option(med_EXT_ITK "Build the ITK-Medular extensuib" OFF)
if (med_EXT_ITK)
  add_subdirectory(medItk)
endif (med_EXT_ITK)


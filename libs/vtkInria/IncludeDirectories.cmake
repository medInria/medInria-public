#-----------------------------------------------------------------------------
# Include directories for other projects installed on the system.
SET(vtkInria_INCLUDE_DIRS_SYSTEM "")


#-----------------------------------------------------------------------------
# Include directories from the build tree.
SET(vtkInria_INCLUDE_DIRS_BUILD_TREE
${vtkInria_BINARY_DIR}
${vtkInria_BINARY_DIR}/Shaders/
)


# These directories are always needed.
SET(vtkInria_INCLUDE_DIRS_SOURCE_TREE
${vtkInria_SOURCE_DIR}
${vtkInria_SOURCE_DIR}/HWShading/
${vtkInria_SOURCE_DIR}/HWShading/Helpers/
${vtkInria_SOURCE_DIR}/HWShading/Shaders/
${vtkInria_SOURCE_DIR}/wxAddOn/
${vtkInria_SOURCE_DIR}/vtkVisuManagement/
${vtkInria_SOURCE_DIR}/vtkWidgetsAddOn/
${vtkInria_SOURCE_DIR}/vtkDataManagement/
${vtkInria_SOURCE_DIR}/wxVtk/
${vtkInria_SOURCE_DIR}/KWAddOn/
${vtkInria_SOURCE_DIR}/KWAddOn/GenericToolBox/
${vtkInria_SOURCE_DIR}/vtkItk/
${vtkInria_SOURCE_DIR}/itkAddOn/
${vtkInria_SOURCE_DIR}/itkAddOn/Registration/
${vtkInria_SOURCE_DIR}/itkAddOn/IO/
${vtkInria_SOURCE_DIR}/wxSamples/HelloWorld/
${vtkInria_SOURCE_DIR}/wxSamples/ImageViewer/
${vtkInria_SOURCE_DIR}/vtkRenderingAddOn/
${vtkInria_SOURCE_DIR}/vtkImageView/
${vtkInria_SOURCE_DIR}/SphericalHarmonicsVisu/
)

IF(Boost_FOUND)
  MESSAGE("Boost Include DIRS " ${Boost_INCLUDE_DIRS})
  SET(vtkInria_INCLUDE_DIRS_SOURCE_TREE
  ${vtkInria_INCLUDE_DIRS_SOURCE_TREE}
  ${Boost_INCLUDE_DIRS}
  )
ENDIF( Boost_FOUND )

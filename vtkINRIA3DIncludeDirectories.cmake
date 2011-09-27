#-----------------------------------------------------------------------------
# Include directories for other projects installed on the system.
SET(vtkINRIA3D_INCLUDE_DIRS_SYSTEM "")


#-----------------------------------------------------------------------------
# Include directories from the build tree.
SET(vtkINRIA3D_INCLUDE_DIRS_BUILD_TREE
${vtkINRIA3D_BINARY_DIR}
${vtkINRIA3D_BINARY_DIR}/Shaders/
)


# These directories are always needed.
SET(vtkINRIA3D_INCLUDE_DIRS_SOURCE_TREE
${vtkINRIA3D_SOURCE_DIR}
${vtkINRIA3D_SOURCE_DIR}/HWShading/
${vtkINRIA3D_SOURCE_DIR}/HWShading/Helpers/
${vtkINRIA3D_SOURCE_DIR}/HWShading/Shaders/
${vtkINRIA3D_SOURCE_DIR}/wxAddOn/
${vtkINRIA3D_SOURCE_DIR}/vtkVisuManagement/
${vtkINRIA3D_SOURCE_DIR}/vtkWidgetsAddOn/
${vtkINRIA3D_SOURCE_DIR}/vtkDataManagement/
${vtkINRIA3D_SOURCE_DIR}/wxVtk/
${vtkINRIA3D_SOURCE_DIR}/KWAddOn/
${vtkINRIA3D_SOURCE_DIR}/KWAddOn/GenericToolBox/
${vtkINRIA3D_SOURCE_DIR}/vtkItk/
${vtkINRIA3D_SOURCE_DIR}/itkAddOn/
${vtkINRIA3D_SOURCE_DIR}/itkAddOn/Registration/
${vtkINRIA3D_SOURCE_DIR}/itkAddOn/IO/
${vtkINRIA3D_SOURCE_DIR}/wxSamples/HelloWorld/
${vtkINRIA3D_SOURCE_DIR}/wxSamples/ImageViewer/
${vtkINRIA3D_SOURCE_DIR}/vtkRenderingAddOn/
${vtkINRIA3D_SOURCE_DIR}/vtkImageView/
${vtkINRIA3D_SOURCE_DIR}/SphericalHarmonicsVisu/
)

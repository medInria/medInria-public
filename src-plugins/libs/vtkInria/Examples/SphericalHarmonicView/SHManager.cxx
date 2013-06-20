/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <vtkViewImage3D.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkImageData.h>
#include <vtkPolyData.h>

#include "vtkSphericalHarmonicManager.h"
#include "itkSphericalHarmonicITKToVTKFilter.h"

#include <itkVectorImage.h>
#include <itkImageFileReader.h>

typedef itk::VectorImage<float, 3>    SHImageTypeFloat;
typedef SHImageTypeFloat::PixelType SHTypeFloat;
typedef SHImageTypeFloat::Pointer SHImagePointerFloat;

int main (int argc, char*argv[])
{
    int sampleRate = 1;
    int glyphResolution = 6;
    int tesselationBasis = 0;
    int sliceVisibility[3] = {1,0,0};
    int dims[3] = {0,0,0};

    int flipGlyphAxis[3] = {0,0,1};
    bool glyphColouring  = true;
    int tesselationType = 1;//TODO it is actually not workin only icosahedro works
    const float glyphScale = .5;

    if (argc<2) {
        std::cout << "Usage: " << std::endl;
        std::cout << "\t" << argv[0]
                  << " <SH file> SH FILE FLOAT ITKVECTOR::IMAGE 15 28 COEF " << std::endl;
        std::cout << "Example: " << std::endl;
        std::cout << "\t" << argv[0] << " [vtkINRIA3D_DATA_DIR]/odf.nrrd" << std::endl;
        exit (-1);
    }

    /**
       In this example, we illustrate the use of the vtkSphericalHarmonicVisuManager.
       We first set up a vtkImageView3D to display an image.
    */

    vtkViewImage3D*            view = vtkViewImage3D::New();
    vtkRenderWindowInteractor* iren = vtkRenderWindowInteractor::New();
    vtkRenderWindow*           rwin = vtkRenderWindow::New();
    vtkRenderer*           renderer = vtkRenderer::New();

    iren->SetRenderWindow(rwin);
    rwin->AddRenderer(renderer);
    view->SetRenderWindow(rwin);
    view->SetRenderer(renderer);

    double color[3] = {0.0,0.0,0.0};
    view->SetTextColor (color);
    view->SetRenderingModeToPlanar();
    view->SetCubeVisibility(1);
    view->SetAboutData ("Powered by vtkINRIA3D");

    /**
         We read the SH coefficients == typedef itk::VectorImage<float, 3>  with SH data.
     */
    typedef itk::ImageFileReader<SHImageTypeFloat> ReaderType;
    ReaderType::Pointer reader = ReaderType::New();
    reader->SetFileName(argv[1]);
    try {
        reader->Update();
    } catch (itk::ExceptionObject &e) {
        std::cerr << e.GetDescription();
        return false;
    }

    /**
         We convert the SH coefficinets typedef itk::VectorImage<float, 3>  with SH data to
         vtkImageData. ITK image is keep and used in this example in order to keep
         the orientation of the image that can not be held by the vtkImageData.
     */
    itk::SphericalHarmonicITKToVTKFilter<SHImageTypeFloat>::Pointer filterFloat;
    filterFloat = itk::SphericalHarmonicITKToVTKFilter<SHImageTypeFloat>::New();
    filterFloat->SetInput(reader->GetOutput());
    filterFloat->Update();

    /**
         We create the vtkSphericalHarmonicManager, feed it with SH data, set the rendering
         properties, the interactor and renderer and finally update it .
     */
    vtkSphericalHarmonicManager       *manager;
    manager = vtkSphericalHarmonicManager::New();

    vtkImageData *shs = filterFloat->GetVTKSphericalHarmonic();
    vtkMatrix4x4 *matrix = filterFloat->GetDirectionMatrix();
    manager->SetInput(shs);
    manager->SetMatrixT(matrix);

    int number = reader->GetOutput()->GetNumberOfComponentsPerPixel();
    int Order = (int)(-1.5+std::sqrt((float)(0.25+2*number)));
    manager->SetOrder(Order);

    // Rendering  properties
    manager->SetTesselationType(tesselationType);
    manager->SetTesselationBasis(tesselationBasis);
    manager->SetSampleRate(sampleRate, sampleRate, sampleRate);
    manager->SetGlyphResolution(glyphResolution);
    manager->SetGlyphScale((float)glyphScale);
    manager->SetCurrentPosition((int*)dims);
    manager->SetAxialSliceVisibility(sliceVisibility[0]);
    manager->SetCoronalSliceVisibility(sliceVisibility[1]);
    manager->SetSagittalSliceVisibility(sliceVisibility[2]);
    manager->FlipX(flipGlyphAxis[0]);
    manager->FlipY(flipGlyphAxis[1]);
    manager->FlipZ(flipGlyphAxis[2]);
    manager->ColorGlyphs(glyphColouring);

    manager->SetRenderWindowInteractor( iren, renderer );
    manager->Update();

    rwin->Render();
    iren->Start();

    view->Delete();
    iren->Delete();
    rwin->Delete();
    renderer->Delete();
    manager->Delete();

    return 0;
}

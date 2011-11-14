//   Authors:	   Maxime Descoteaux, Jaime Garcia Guevara, Theodore Papadopoulo.
//
//   Description:  scale and orient glyph according to spherical harmonic representation
//   vtkSphericalHarmonicGlyph is a filter that copies a SH representation (specified
//   as polygonal data) to every input point.
//
//   Copyright (c) 2007-2011, INRIA Sophia Antipolis, France, groups Odyssee, Athena.
//   Please see the copyright notice included in this distribution for full details.

#include <vtkViewImage3D.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkStructuredPointsReader.h>
#include <vtkStructuredPoints.h>
#include <vtkPolyData.h>

#include "vtkSphericalHarmonicManager.h"
#include "itkSphericalHarmonicITKToVTKFilter.h"

#include <itkVectorImage.h>
#include <itkImageFileReader.h>

typedef itk::VectorImage<float, 3>    ShImageTypeFloat;
typedef ShImageTypeFloat::PixelType ShTypeFloat;
typedef ShImageTypeFloat::Pointer ShImagePointerFloat;

int main (int argc, char*argv[])
{
    int sampleRate = 1;
    int glyphResolution = 6;
    int tesselationBasis = 0;
    int sliceVisibility[3] = {1,0,1};
    int dims[3] = {0,0,0};

    int flipGlyphAxis[3] = {0,0,0};
    bool glyphColouring  = true;
    int tesselationType = 2;
    const float glyphScale = 0.3;

    if( argc<2 )
    {
        std::cout << "Usage: " << std::endl;
        std::cout << "\t" << argv[0] << " <SH file> SH FILE FLOAT ITKVECTOR::IMAGE 15 28 COEF " << std::endl;
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

    iren->SetRenderWindow ( rwin );
    rwin->AddRenderer ( renderer );
    view->SetRenderWindow ( rwin );
    view->SetRenderer ( renderer );

    double color[3] = {0.0,0.0,0.0};
    view->SetTextColor (color);
    view->SetRenderingModeToPlanar();
    view->SetCubeVisibility(1);
    view->SetAboutData ("Powered by vtkINRIA3D");

    /**
         We read the SH coefficinets == typedef itk::VectorImage<float, 3>  with SH data.
      */

    typedef itk::ImageFileReader<ShImageTypeFloat> ReaderType;
    ReaderType::Pointer reader = ReaderType::New();
    reader->SetFileName ( argv[1] );
    try {
        reader->Update();
    }
    catch (itk::ExceptionObject &e) {
        std::cerr << e.GetDescription();
        return false;
    }

    /**
         We convert the SH coefficinets typedef itk::VectorImage<float, 3>  with SH data to
         vtkStructuredPoints. ITK image is keep and used in this example in order to keep
         the orientation of the image that can not be held by the vtkStructuredPoints.
      */
    itk::SphericalHarmonicITKToVTKFilter<ShImageTypeFloat>::Pointer filterFloat;
    filterFloat = itk::SphericalHarmonicITKToVTKFilter<ShImageTypeFloat>::New();
    filterFloat->SetInput(reader->GetOutput());
    filterFloat->Update();

    /**
         We create the vtkSphericalHarmonicManager, feed it with SH data, set the rendering
         properties, the interactor and renderer and finally update it .
      */
    vtkSphericalHarmonicManager       *manager;
    manager = vtkSphericalHarmonicManager::New();

    vtkStructuredPoints *shs = filterFloat->GetVTKSphericalHarmonic();
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

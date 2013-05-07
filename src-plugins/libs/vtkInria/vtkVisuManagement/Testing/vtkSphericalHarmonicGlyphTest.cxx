/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <vtkMatrix4x4.h>
#include <vtkSmartPointer.h>
#include <vtkImageData.h>
#include <vtkPolyData.h>
#include <vtkExtractVOI.h>
#include <vtkPolyDataReader.h>
#include <vtkViewImage3D.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkViewImage3D.h>

#include <itkVectorImage.h>
#include <itkImageFileReader.h>

#include "vtkSphericalHarmonicManager.h"
#include "itkSphericalHarmonicITKToVTKFilter.h"
#include "vtkSphericalHarmonicSource.h"
#include "vtkSphericalHarmonicGlyph.h"

#include <cmath>
#include <cstdlib>
#include <iostream>

typedef itk::VectorImage<float, 3>    ShImageTypeFloat;

int Compare( vtkPoints* verticesA, vtkPoints* verticesB)
{
  double pointA[3], pointB[3];
  int nPointsA = verticesA->GetNumberOfPoints();
  int nPointsB = verticesB->GetNumberOfPoints();

  const double tolerance = 0.0000000000001;
  int found=0;

  for (int i = 0; i < nPointsA; i++)
  {
    verticesA->GetPoint(i, pointA);
    for (int j = i; j < nPointsB; j++)// Since they were computed in the same order this comparison should sufice
    {
      verticesA->GetPoint(j, pointB);
      if (std::abs(pointB[0]-pointA[0])<tolerance && std::abs(pointB[1]-pointA[1])<tolerance && std::abs(pointB[2]-pointA[2])<tolerance)
      {
        found=1;
        break;
      }
    }

    if (found==0)
    {
      std::cerr << "The " << i << "th point " << pointA[0] << ", " << pointA[1] << ", " << pointA[2] << "is not inside the mesh" << std::endl;
      break;
    }
    else
    {
      if (i<=nPointsA-2)
        found = 0;
    }
  }

  return found;
}

int vtkSphericalHarmonicGlyphTest(int argc, char *argv[])
{
  int status = EXIT_FAILURE;
 std::cout << "HOLA " << argc;
  if( argc<3 )
  {
    std::cout << "Usage: " << std::endl;
    std::cout << "\t" << argv[0]
              << " <SH file> SH FILE FLOAT ITKVECTOR::IMAGE 15 28 COEF "
              << " <SH mesh> SH vtk Mesh" << std::endl;
    std::cout << "Example: " << std::endl;
    std::cout << "\t" << argv[0] << " [vtkINRIA3D_DATA_DIR]/odf.nrrd" << " [vtkINRIA3D_DATA_DIR]/SHTest.vtk" <<std::endl;
    exit (-1);
  }

  int sampleRate = 1;
  int glyphResolution = 2;
  int tesselationBasis = 1;
  int sliceVisibility[3] = {1,0,0};
  int dimsPosition[3] = {0,0,0};

  int flipGlyphAxis[3] = {0,0,1};
  bool glyphColouring  = true;
  int tesselationType = 2;//TODO it is actually not workin only icosahedro seem to work
  const float glyphScale = .5;

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
           vtkImageData. ITK image is keep and used in this example in order to keep
           the orientation of the image that can not be held by the vtkImageData.
       */
  itk::SphericalHarmonicITKToVTKFilter<ShImageTypeFloat>::Pointer filterFloat;
  filterFloat = itk::SphericalHarmonicITKToVTKFilter<ShImageTypeFloat>::New();
  filterFloat->SetInput(reader->GetOutput());
  filterFloat->Update();

  /**
           We create the vtkSphericalHarmonicManager, feed it with SH data, set the rendering
           properties, the interactor and renderer and finally update it .
       */

  vtkImageData *shs = filterFloat->GetVTKSphericalHarmonic();
  vtkMatrix4x4 *matrix = filterFloat->GetDirectionMatrix();

  int number = reader->GetOutput()->GetNumberOfComponentsPerPixel();
  int Order = (int)(-1.5+std::sqrt((float)(0.25+2*number)));

  vtkSphericalHarmonicSource* SHSource;
  vtkSphericalHarmonicGlyph*  SHGlyph;
  vtkExtractVOI*              VOI;

  SHSource = vtkSphericalHarmonicSource::New();
  SHGlyph  = vtkSphericalHarmonicGlyph::New();
  VOI    = vtkExtractVOI::New();

  SHGlyph->SetTMatrix(matrix);

  SHSource->SetNumberOfSphericalHarmonics( number );
  SHSource->SetOrder(Order);

  SHSource->SetTesselationType( tesselationType );
  SHSource->SetTesselationBasis( tesselationBasis );
  SHSource->FlipXOff();
  SHSource->FlipYOff();
  SHSource->FlipZOn();

  SHSource->SetTesselation( glyphResolution/2 );
  SHSource->UpdateSphericalHarmonicSource();

  SHGlyph->SetSource ( SHSource->GetOutput() );
  SHGlyph->SetScaleFactor( glyphScale );
  SHGlyph->SetSphericalHarmonicSource( SHSource );
  SHGlyph->SetColorModeToDirections();
  SHGlyph->GetOutput()->GetPointData()
      ->SetActiveScalars(vtkSphericalHarmonicGlyph::GetRGBArrayName());

  VOI->ReleaseDataFlagOn();
  VOI->SetInput ( shs );

  int* dims =  shs->GetDimensions();

  // clamping only one slice that matches the one in the mesh SHTest.vtk
  int ZZ = dimsPosition[2]>(dims[2]-1)?(dims[2]-1):dimsPosition[2];
  // synchronize with VOI
  VOI->SetVOI (0, dims[0]-1, 0, dims[1]-1, ZZ, ZZ);
  VOI->SetSampleRate(sampleRate, sampleRate, sampleRate);

  SHGlyph->SetInput( VOI->GetOutput() );

  if( glyphColouring )
    SHGlyph->SetColorGlyphs(true);
  else
    SHGlyph->SetColorGlyphs(false);

  SHGlyph->Update();

  vtkPolyDataReader * shReaderB = vtkPolyDataReader::New();
  shReaderB->SetFileName(argv[2]);
  shReaderB->Update();

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
  We create the vtkSphericalHarmonicManager, feed it with SH data, set the rendering
  properties, the interactor and renderer and finally update it .
*/
vtkSphericalHarmonicManager       *manager;
manager = vtkSphericalHarmonicManager::New();


manager->SetInput(shs);
manager->SetMatrixT(matrix);

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

renderer->ResetCamera();
rwin->Render();
//iren->Start();

  if (Compare(SHGlyph->GetOutput()->GetPoints(), shReaderB->GetOutput()->GetPoints()))
    status = EXIT_SUCCESS ;

  view->Delete(); view=NULL;
  iren->Delete(); iren =NULL;
  rwin->Delete();  rwin =NULL;
  renderer->Delete();  renderer =NULL;
  manager->Delete();   manager =NULL;

  SHGlyph->Delete();
  SHSource->Delete();
  VOI->Delete();
  shReaderB->Delete();

  return status;
}




/*============================================================================

Program:   vtkINRIA3D
Module:    $Id: HWShading 480 2007-11-21 08:48:21Z $
Language:  C++
Author:    $Author: Tim Peeters $
Date:      $Date: 2007-11-21 09:48:21 +0100 (Wed, 21 Nov 2007) $
Version:   $Revision: 480 $

The Hardware Shading (HWShading) module of vtkINRIA3D is protected by the
following copyright:

Copyright (c) 2007 Biomedical Image Analysis (BMIA) - Department of
Biomedical Engineering - Eindhoven University of Technology.
All rights reserved. See Copyright.txt for details.

The HWShading implementation was originally written by Tim Peeters (BMIA - TUe)
and published at the "11th International Fall Workshop on Vision, Modeling,
and Visualization 2006" (VMV'06):
"Visualization of the Fibrous Structure of the Heart", by T. Peeters et al.
See http://timpeeters.com/research/vmv2006 for more information.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notices for more information.

============================================================================*/
/**
 * texturemap.cxx
 * by Tim Peeters
 *
 * 2005-05-10	Tim Peeters
 * - First version
 *
 * 2006-01-30	Tim Peeters
 * - Use VTK OpenGL extension manager instead of glew.
 */

#include "vtkBMIAShaderProgramReader.h"
#include "vtkBMIAShaderProgram.h"
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
//#include "vtkTensorDataReader.h"
//#include "vtkTensorData.h"
#include <vtkCamera.h>
#include <vtkJPEGReader.h>
#include <vtkTexture.h>
#include <vtkCylinderSource.h>
#include <vtkCubeSource.h>
#include <vtkPlaneSource.h>
#include <vtkSphereSource.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkExtractVOI.h>
#include "vtkDoubleToUCharFilter.h"
//#include "vtkShaderManager.h"
#include <vtkOpenGLRenderWindow.h>
#include "ExtensionInitialize.h"
#include <vtkImageData.h>

int texturemap(int argc, char *argv[]) {
  cout<<"======================================================================"<<endl;

  if (argc < 2) {
    cout<<"Usage: "<<argv[0]<<" JPEG FILE "<<endl;
    exit(0);
  }
  
  vtkRenderer* r = vtkRenderer::New();
  vtkRenderWindow* rw = vtkRenderWindow::New();
  rw->AddRenderer(r);
  vtkRenderWindowInteractor* rwi = vtkRenderWindowInteractor::New();
  rwi->SetRenderWindow(rw);

  r->SetBackground(0, 0, 0.4);

  vtkJPEGReader* reader = vtkJPEGReader::New();
  reader->SetFileName(argv[1]);

  vtkTexture* texture = vtkTexture::New();
  texture->SetInput(reader->GetOutput());
  texture->InterpolateOn();

  vtkPlaneSource* source = vtkPlaneSource::New();
  double point1[3]; double point2[3]; double origin[3];
  source->GetPoint1(point1);
  source->GetPoint2(point2);
  source->GetOrigin(origin);
  cout<<"origin = "<<origin[0]<<", "<<origin[1]<<", "<<origin[2]<<endl;
  cout<<"point1 = "<<point1[0]<<", "<<point1[1]<<", "<<point1[2]<<endl;
  cout<<"point2 = "<<point2[0]<<", "<<point2[1]<<", "<<point2[2]<<endl;
  source->SetOrigin(0,0,0);
  source->SetPoint1(1701, 0, 0);
  source->SetPoint2(0,710,0);
  vtkPolyDataMapper* mapper = vtkPolyDataMapper::New();
  mapper->SetInput(source->GetOutput());
  source->Delete(); source = NULL;
  vtkActor* actor = vtkActor::New();
  actor->SetMapper(mapper);
  mapper->Delete(); mapper = NULL;
  
  actor->SetTexture(texture);
  texture->Delete(); texture = NULL;

  rw->SetSize(80,60);
  rwi->Initialize();
  //glewInit();
  InitializeExtensions(rw);

  //vtkShaderManager* sm = vtkShaderManager::New();
  //sm->Initialize();
  //sm->UseShaderFromFile("texture.prog");
  vtkBMIAShaderProgramReader* sreader = vtkBMIAShaderProgramReader::New();
  sreader->SetFileName("texture.prog");
  sreader->Execute();
  vtkBMIAShaderProgram* shader = sreader->GetOutput();
  shader->Activate();

  r->AddActor(actor);

  actor->Delete(); actor = NULL;
  r->MakeCamera();
  // vtkCamera* cam = r->GetActiveCamera();

  rwi->Render();

  shader->Deactivate();
  reader->Delete();
  //sm->Delete();
  r->Delete();
  rw->Delete();
  rwi->Delete();

  return 0;
}

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
 * fibermapper.cxx
 * by Tim Peeters
 *
 * 2005-06-20	Tim Peeters
 * - First version. Reads fibers from VTK file and visualizes using standard
 *   vtkPolyDataMapper.
 *
 * 2005-06-22	Tim Peeters
 * - Use my own vtkOpenGLFiberMapper instead of vtkPolyDataMapper.
 *
 * 2005-07-04	Tim Peeters
 * - Read data from file specified on command-line.
 *
 * 2005-12-07	Tim Peeters
 * - Added command-line options:
 *   --help
 *   --no-shading
 *
 * 2006-01-30	Tim Peeters
 * - Call InitializeExtensions() instead of using glew.
 *
 * 2006-02-22	Tim Peeters
 * - Remove InitializeExtensions again. This stuff is now dealt with in the
 *   mapper itself.
 */

#include "vtkFiberMapper.h"

#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkPolyDataReader.h>
#include <vtkActor.h>
#include <vtkProperty.h>

#include "vtkAnisotropicLightingSP.h"
#include <assert.h>

int fibermapper(int argc, char *argv[]) {
  cout << "============================================================"<<endl;

  cout<<argc<<" arguments."<<endl;
  bool printhelp = false;
  for (int i=0; i < argc; i++)
    {
    if (strcmp(argv[i], "--help") == 0) printhelp = true;
    cout<<"-- argument "<<i<<" == "<<argv[i]<<endl;
    }

  if (argc < 2) printhelp = true;

  if (printhelp) {
    cout<<"Usage: "<<argv[0]<<" INPUT_FILE [--no-shading] [--no-shadows]"<<endl;
    exit(0);
  }

  bool shading = true;
  bool shadows = true;
  for (int i=2; i < argc; i++)
    {
    if (strcmp(argv[i], "--no-shading") == 0) shading = false;
    if (strcmp(argv[i], "--no-shadows") == 0) shadows = false;

    }

  const char* filename = argv[1];

  vtkRenderer* r = vtkRenderer::New();
  //r->DebugOn();
  vtkRenderWindow* rw = vtkRenderWindow::New();
 // rw->AddRenderer(r);
  vtkRenderWindowInteractor* rwi = vtkRenderWindowInteractor::New();
  rwi->SetRenderWindow(rw);
  rwi->SetInteractorStyle(vtkInteractorStyleTrackballCamera::New());

  vtkPolyDataReader* reader = vtkPolyDataReader::New();
  reader->SetFileName(filename);
  vtkFiberMapper* mapper = vtkFiberMapper::New();
//  if (shading)
//    {
    //vtkFiberMapper* fibermapper = vtkFiberMapper::New();
    if (!shadows)
      {
      mapper->ShadowingOff();
      }

    if (!shading)
      {
      mapper->LightingOff();
      }


//    }
//  else // no shading
//    {
//    mapper = vtkPolyDataMapper::New();

    // disable scalar visibility. Otherwise all fibers are blue (?) 
//    mapper->ScalarVisibilityOff();
//    }

  //mapper->GetShaderProgram()->SetDiffuseContribution(0.5);
  //mapper->GetShaderProgram()->SetSpecularContribution(0.5);
  //mapper->GetShaderProgram()->SetSpecularPower(20);
  //mapper->DebugOn();
  cout<<"Reading data..."<<endl;
  assert(reader->GetOutput());
  reader->GetOutput()->Update();
  cout<<"Done!"<<endl;
  mapper->SetInput(reader->GetOutput());
  mapper->ScalarVisibilityOff();
  reader->Delete(); reader = NULL;
  //mapper->LightingOff();


  /*
  mapper->SetAmbientContribution (0.7);
  mapper->SetDiffuseContribution (0.3);
  mapper->SetSpecularContribution(0.6);
  mapper->SetSpecularPower(50);
  */
  
  /*
  mapper->SetAmbientContribution (0.1);
  mapper->SetDiffuseContribution (0.6);
  mapper->SetSpecularContribution(0.5);
  mapper->SetSpecularPower(40);
  */

  mapper->SetAmbientContributionShadow(0.0);
  mapper->SetDiffuseContributionShadow(0.6);
  mapper->SetSpecularContributionShadow(0.0);
  
  std::cout << "Color prop: ";
  std::cout << mapper->GetAmbientContribution() << " ";
  std::cout << mapper->GetDiffuseContribution() << " ";
  std::cout << mapper->GetSpecularContribution() << " ";
  std::cout << mapper->GetSpecularPower() << " ";
  std::cout << mapper->GetAmbientContributionShadow() << " ";
  std::cout << mapper->GetDiffuseContributionShadow() << " ";
  std::cout << mapper->GetSpecularContributionShadow() << std::endl;
  
  vtkActor* actor = vtkActor::New();
  actor->SetMapper(mapper);
  

  r->AddActor(actor);
  actor->GetProperty()->SetColor(1.0, 0.9, 0.2);
  actor->GetProperty()->SetDiffuse(1.0);
  actor->GetProperty()->SetSpecular(0.5);
  actor->GetProperty()->SetSpecularPower(20);
  

  r->SetBackground(0, 0, 0);
  rw->SetSize(800, 600);

  rwi->Initialize();
  //glewInit();
  //InitializeExtensions(rw);
  rw->AddRenderer(r);

  r->ResetCamera();
  
  rwi->Render();

  actor->SetMapper(NULL); // if not present, seems to make things crash when detaching the shaders
  mapper->Delete(); mapper = NULL;
  actor->Delete(); actor = NULL;
  rwi->Delete(); rwi = NULL;
  rw->Delete(); rw = NULL;
  r->Delete(); r = NULL;

  return 0;
}

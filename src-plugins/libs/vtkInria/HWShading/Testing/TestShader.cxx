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
 * TestShader.cxx
 * by Tim Peeters
 *
 * 2005-05-09	Tim Peeters
 * - First version
 *
 * 2006-01-30	Tim Peeters
 * - Remove glew stuff.
 * - Use InitializeExtensions function to setup OpenGL2 context.
 */

#include <vtkShaderBase.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>

#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkProperty.h>
#include <vtkCylinderSource.h>
//#include <vtkConeSource.h> // don't use this for shader testing because it has no normals defined.
#include <vtkSphereSource.h>

#include "vtkBMIAShaderProgramReader.h"
#include "vtkBMIAShaderProgram.h"
#include "ExtensionInitialize.h"


int TestShader(int argc, char *argv[]) {
  cout<<"======================================================================"<<endl;

  cout<<argc<<" arguments."<<endl;
  for (int i=0; i < argc; i++)
    {
    cout<<"-- argument "<<i<<" == "<<argv[i]<<endl;
    }

  if (argc < 2) {
    cout<<"Usage: "<<argv[0]<<" SHADERFILE "<<endl;
    exit(0);
  }

  //assert(argv[1] != NULL);

  const char* shaderfile = argv[1];

  vtkRenderer* r = vtkRenderer::New();
  vtkRenderWindow* rw = vtkRenderWindow::New();
  rw->AddRenderer(r);
  vtkRenderWindowInteractor* rwi = vtkRenderWindowInteractor::New();
  rwi->SetRenderWindow(rw);

  vtkCylinderSource * s1 = vtkCylinderSource::New();
  vtkSphereSource* s2 = vtkSphereSource::New();
  s1->SetResolution(15);
  s2->SetThetaResolution(15);
  s2->SetPhiResolution(15);
  vtkPolyDataMapper * m1 = vtkPolyDataMapper::New();
  vtkPolyDataMapper * m2 = vtkPolyDataMapper::New();
  m1->SetInput(s1->GetOutput());
  m2->SetInput(s2->GetOutput());
  s1->Delete(); s2->Delete();
  vtkActor* a1 = vtkActor::New();
  vtkActor* a2 = vtkActor::New();
  a1->SetMapper(m1);
  a2->SetMapper(m2);
  a2->SetPosition(1.5,0,0);
  m1->Delete(); m2->Delete();
  r->AddActor(a1);
  r->AddActor(a2);
  a1->Delete(); a2->Delete();
  r->SetBackground(0,0,0.5);

  rw->SetSize(400,400);
  rwi->Initialize();
  //glewInit();
  InitializeExtensions(rw);

  vtkBMIAShaderProgramReader* spreader = vtkBMIAShaderProgramReader::New();
  spreader->SetFileName(shaderfile);
  spreader->Execute();
  spreader->GetOutput()->Activate();

  rwi->Render();

  spreader->GetOutput()->Deactivate();
  spreader->Delete();
  
  r->Delete();
  rw->Delete();
  rwi->Delete();

  return 0;
}

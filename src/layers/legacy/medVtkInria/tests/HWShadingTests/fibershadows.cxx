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
 * fibershadows.cxx
 * by Tim Peeters
 *
 * 2005-11-17	Tim Peeters
 * - First version, based on fibermapper.cxx from vtkTensorData
 *
 * 2006-01-30	Tim Peeters
 * - Call InitializeExtensions() instead of using glew.
 */

#include <vtkPolyDataMapper.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkPolyData.h>
#include <vtkPolyDataReader.h>
#include <vtkActor.h>

#include "vtkShadowRenderer.h"
#include <assert.h>
#include <vtkCellArray.h>
#include "ExtensionInitialize.h"

int fibershadows(int argc, char *argv[]) {
  cout << "============================================================"<<endl;

  cout<<argc<<" arguments."<<endl;
  for (int i=0; i < argc; i++)
    {
    cout<<"-- argument "<<i<<" == "<<argv[i]<<endl;
    }

  if (argc < 2) {
    cout<<"Usage: "<<argv[0]<<" INPUT_FILE"<<endl;
    exit(0);
  }

  const char* filename = argv[1];

//  vtkRenderer* r = vtkRenderer::New();
  vtkShadowRenderer* r = vtkShadowRenderer::New();
  r->ShadowsOn();
  r->ShowShadowMapOn();
  //r->DebugOn();
  vtkRenderWindow* rw = vtkRenderWindow::New();
//  rw->AddRenderer(r);
  vtkRenderWindowInteractor* rwi = vtkRenderWindowInteractor::New();
  rwi->SetRenderWindow(rw);
  rwi->SetInteractorStyle(vtkInteractorStyleTrackballCamera::New());

  vtkPolyDataReader* reader = vtkPolyDataReader::New();
  //reader->SetFileName("/home/tim/data/data-fibers-anna/fibers.fbs");
  //reader->SetFileName("/data/TPeeters/data/few.fbs");
  reader->SetFileName(filename);
//  vtkFiberMapper* mapper = vtkFiberMapper::New();
  vtkPolyDataMapper* mapper = vtkPolyDataMapper::New();
  //mapper->DebugOn();
  cout<<"Reading data..."<<endl;
  assert(reader->GetOutput());
  reader->GetOutput()->Update();
  cout<<"Done!"<<endl;
  cout<<"Read "<<reader->GetOutput()->GetVerts()->GetNumberOfCells()
	<<" vertices and "<<reader->GetOutput()->GetLines()->GetNumberOfCells()
	<<" fibers!."<<endl;
  vtkCellArray* carr = reader->GetOutput()->GetLines();
  carr->InitTraversal();
  int numcells = 0;
  int id = 1;
  vtkIdType npts = 0;
  vtkIdType* pts;
  while (id != 0)
    { 
    id = carr->GetNextCell(npts, pts);
    numcells += npts;
    }
  cout<<"Total number of "<<numcells<<" lines in dataset."<<endl;

  mapper->SetInput(reader->GetOutput());
  reader->Delete(); reader = NULL;
  //mapper->LightingOff();

//  assert(mapper->GetShaderProgram());
  //vtkAnisotropicLightingSP* ShaderProgram = mapper->GetShaderProgram();
//  vtkAnisotropicLightingSP* ShaderProgram;
  //vtkAnisoLiShadowMapSP* ShaderProgram;
//  ShaderProgram = mapper->GetShaderProgram();

//  ShaderProgram->SetDiffuseContribution(0.5);
//  ShaderProgram->SetSpecularContribution(0.5);
//  ShaderProgram->SetSpecularPower(20);

  vtkActor* actor = vtkActor::New();
  actor->SetMapper(mapper);
  mapper->Delete(); mapper = NULL;

  r->AddActor(actor);
  actor->Delete(); actor = NULL;

  r->SetBackground(0.3, 0.3, 0.5);
  rw->SetSize(800, 550);

  //vtkShaderManager *sm = vtkShaderManager::New();

  rwi->Initialize();
  //sm->Initialize();
  //glewInit();
  InitializeExtensions(rw);

  rw->AddRenderer(r);
  r->ResetCamera();

  rwi->Render();

  rw->RemoveRenderer(r);

  //sm->Delete(); sm = NULL;
  r->Delete(); r = NULL;
  rw->Delete(); rw = NULL;
  rwi->Delete(); rwi = NULL;

  return 0;
}

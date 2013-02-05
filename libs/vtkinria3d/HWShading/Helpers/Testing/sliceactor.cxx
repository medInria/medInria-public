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
 * sliceactor.cxx
 * by Tim Peeters
 *
 * 2006-05-02	Tim Peeters
 * - First version.
 */

#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkImageData.h>
#include "vtkImageSliceActor.h"
#include "vtkImageOrthogonalSlicesActor.h"
//#include "vtkDTIComponentReader.h"

#include <vtkStructuredPointsReader.h>
#include <vtkStructuredPoints.h>
#include <vtkVolume16Reader.h>

int sliceactor(int argc, char *argv[]) {
  cout<<"======================================================================"<<endl;

  if (argc < 2) {
    cout<<"Usage: "<<argv[0]<<" INPUT_FILE"<<endl;
    exit(0);
  }
  // const char* filename = argv[1];

  vtkRenderer* r = vtkRenderer::New();
  vtkRenderWindow* rw = vtkRenderWindow::New();
  rw->AddRenderer(r);
  vtkRenderWindowInteractor* rwi = vtkRenderWindowInteractor::New();
  rwi->SetRenderWindow(rw);

  // Create the reader for the data
//  vtkDTIReader2* reader = vtkDTIReader2::New();
//  reader->SetFileName(filename);

/*
  vtkDTIComponentReader* reader = vtkDTIComponentReader::New();
  reader->SetDataScalarTypeToFloat();
  reader->SetFileName(filename);
*/

  /*

  vtkVolume16Reader *reader = vtkVolume16Reader::New();
    reader->SetDataDimensions (64,64);
    reader->SetImageRange (1,93);
    reader->SetDataByteOrderToLittleEndian();
    reader->SetFilePrefix (argv[1]);
    reader->SetDataSpacing (3.2, 3.2, 1.5);
  */

  vtkStructuredPointsReader* reader = vtkStructuredPointsReader::New();
  reader->SetFileName(argv[1]);
  reader->GetOutput()->Update();
  
  int extent[6];

  reader->GetOutput()->GetExtent(extent);
  //reader->Delete(); reader = NULL;
  cout<<"Extent == "<<extent[0]<<", "<<extent[1]<<", "
  	  <<extent[2]<<", "<<extent[3]<<", "<<extent[4]
  	  <<", "<<extent[5]<<".\n";


  vtkImageSliceActor* sliceActor = vtkImageSliceActor::New();
  sliceActor->SetInputConnection(reader->GetOutputPort());
  vtkImageOrthogonalSlicesActor* slicesActor = vtkImageOrthogonalSlicesActor::New();
  slicesActor->SetInputConnection(reader->GetOutputPort());
//  slicesActor->SetInput(reader->GetOutput());
//  r->AddActor(sliceActor);
  r->AddActor(slicesActor);
  sliceActor->SetSliceOrientationToYZ();

  rw->SetSize(400,400);

  rwi->Initialize();
  slicesActor->UpdateInput();
  slicesActor->CenterSlices();

  r->ResetCamera(); 
  rwi->Render();

  sliceActor->Delete();
  slicesActor->Delete();
  r->Delete();
  rw->Delete();
  rwi->Delete();

  return 0;
}


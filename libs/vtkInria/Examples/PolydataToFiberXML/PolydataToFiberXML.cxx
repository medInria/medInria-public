/*=========================================================================

Program:   vtkINRIA3D
Module:    $Id: FibersManager.cxx 917 2008-08-27 10:37:34Z filus $
Language:  C++
Author:    $Author: filus $
Date:      $Date: 2008-08-27 12:37:34 +0200 (Wed, 27 Aug 2008) $
Version:   $Revision: 917 $

Copyright (c) 2007 INRIA - Asclepios Project. All rights reserved.
See Copyright.txt for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include <vtkPolyDataReader.h>
#include <vtkPolyData.h>

#include <vtkFiberDataSet.h>
#include <vtkXMLFiberDataSetWriter.h>

int main (int argc, char*argv[])
{

  if( argc<3 )
  {
    std::cout << "Usage: " << std::endl;
    std::cout << "\t" << argv[0] << " <polydata file> <xml fiber file>" << std::endl;
    return -1;
  }

  vtkObject::GlobalWarningDisplayOn();

  vtkPolyDataReader *reader = vtkPolyDataReader::New();
  reader->SetFileName (argv[1]);
  reader->Update();

  vtkPolyData *vtkFibers = reader->GetOutput();
  vtkFibers->Update();
  
  vtkFiberDataSet *fiberDataSet = vtkFiberDataSet::New();
  fiberDataSet->SetFibers (vtkFibers);
  
  vtkXMLFiberDataSetWriter *writer = vtkXMLFiberDataSetWriter::New();
  writer->SetFileName (argv[2]);
  writer->SetInput ( fiberDataSet );
  writer->SetDataModeToBinary();
  writer->Update();

  reader->Delete();
  fiberDataSet->Delete();
  writer->Delete();

  return 0;
}

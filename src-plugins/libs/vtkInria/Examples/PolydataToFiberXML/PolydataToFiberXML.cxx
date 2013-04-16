/*=========================================================================

 MedInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

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

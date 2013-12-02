/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include "vtkXMLFiberDataSetReader.h"
#include "vtkFiberDataSet.h"

int main (int narg, const char *args[])
{

  if (narg<2)
    return -1;
  
  vtkXMLFiberDataSetReader *reader = vtkXMLFiberDataSetReader::New();
  reader->SetFileName (args[1]);
  reader->Update();


  vtkFiberDataSet *fibers = reader->GetOutput();

  std::cout << *fibers;
   

  reader->Delete();
  
  return 0;
}

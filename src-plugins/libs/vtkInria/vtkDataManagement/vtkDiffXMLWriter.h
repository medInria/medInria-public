/*=========================================================================

 MedInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/







#pragma once

#include "vtkObject.h"

#include <string>

class vtkDataManager;
class vtkMetaDataSet;
class vtkDiffXMLWriterInternals;


class vtkDiffXMLWriter : public vtkObject
{
public:
  static vtkDiffXMLWriter* New();
  vtkTypeRevisionMacro(vtkDiffXMLWriter,vtkObject);
  void PrintSelf(ostream& os, vtkIndent indent);  

  // Description:
  // Set/Get an input of this algorithm. You should not override these
  // methods because they are not the only way to connect a pipeline
  void SetInput(vtkDataManager *);
  vtkDataManager *GetInput();

  // see algorithm for more info
  virtual void Update();
  
  void SetFileName (const char* file)
  { this->FileName = file; }
  const char* GetFileName()
  { return this->FileName.c_str(); }
	
  
protected:
  vtkDiffXMLWriter();
  ~vtkDiffXMLWriter();
  
private:
  vtkDiffXMLWriter(const vtkDiffXMLWriter&);  // Not implemented.
  void operator=(const vtkDiffXMLWriter&);  // Not implemented.

  vtkDataManager* Input;
  //BTX
  std::string FileName;
  //ETX
};



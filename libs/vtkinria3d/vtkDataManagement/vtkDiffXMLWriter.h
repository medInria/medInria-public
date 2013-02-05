/*=========================================================================

Program:   vtkINRIA3D
Module:    $Id: vtkDiffXMLWriter.h 801 2008-04-17 16:29:56Z ntoussaint $
Language:  C++
Author:    $Author: ntoussaint $
Date:      $Date: 2008-04-17 18:29:56 +0200 (Thu, 17 Apr 2008) $
Version:   $Revision: 801 $

Copyright (c) 2007 INRIA - Asclepios Project. All rights reserved.
See Copyright.txt for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
// .NAME vtkXMLWriter - Superclass for VTK's XML file writers.
// .SECTION Description
// vtkXMLWriter provides methods implementing most of the
// functionality needed to write VTK XML file formats.  Concrete
// subclasses provide actual writer implementations calling upon this
// functionality.

#ifndef __vtkDiffXMLWriter_h
#define __vtkDiffXMLWriter_h

#include "vtkINRIA3DConfigure.h"

#include "vtkObject.h"

#include <string>

class vtkDataManager;
class vtkMetaDataSet;
class vtkDiffXMLWriterInternals;


class VTK_DATAMANAGEMENT_EXPORT vtkDiffXMLWriter : public vtkObject
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
  virtual void Update (void);
  
  void SetFileName (const char* file)
  { this->FileName = file; }
  const char* GetFileName (void)
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

#endif

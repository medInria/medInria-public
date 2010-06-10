/*=========================================================================

Program:   vtkINRIA3D
Module:    $Id: vtkDiffXMLWriter.cxx 1298 2009-10-22 12:58:07Z filus $
Language:  C++
Author:    $Author: filus $
Date:      $Date: 2009-10-22 14:58:07 +0200 (Thu, 22 Oct 2009) $
Version:   $Revision: 1298 $

Copyright (c) 2007 INRIA - Asclepios Project. All rights reserved.
See Copyright.txt for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include "vtkDiffXMLWriter.h"

#include <vtksys/SystemTools.hxx>
#include <vtksys/ios/sstream>
#include <vtkstd/string>
#include <vtkstd/vector>
#include "vtkSmartPointer.h"
#include <string>
#include "vtkDataManager.h"
#include "vtkMetaDataSetSequence.h"
#include "vtkDataSet.h"
#include "vtkObjectFactory.h"
//----------------------------------------------------------------------------
vtkStandardNewMacro(vtkDiffXMLWriter);
vtkCxxRevisionMacro(vtkDiffXMLWriter, "$Revision: 1298 $");

//----------------------------------------------------------------------------
vtkDiffXMLWriter::vtkDiffXMLWriter()
{

  this->Input = 0;
  this->FileName = "";
}

//----------------------------------------------------------------------------
vtkDiffXMLWriter::~vtkDiffXMLWriter()
{
}


//----------------------------------------------------------------------------
void vtkDiffXMLWriter::PrintSelf(ostream& os, vtkIndent indent)
{
}

//----------------------------------------------------------------------------
void vtkDiffXMLWriter::SetInput (vtkDataManager* manager)
{
  this->Input = manager;
}

//----------------------------------------------------------------------------
vtkDataManager* vtkDiffXMLWriter::GetInput (void)
{
  return this->Input;
}



//----------------------------------------------------------------------------
void vtkDiffXMLWriter::Update(void)
{
  std::cout<<"writing XML"<<std::endl;
}


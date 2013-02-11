/*=========================================================================

Program:   vtkINRIA3D
Module:    $Id: vtkDataManagerReader.h 765 2008-03-31 17:27:00Z ntoussaint $
Language:  C++
Author:    $Author: ntoussaint $
Date:      $Date: 2008-03-31 19:27:00 +0200 (Mon, 31 Mar 2008) $
Version:   $Revision: 765 $

Copyright (c) 2007 INRIA - Asclepios Project. All rights reserved.
See Copyright.txt for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
// .NAME vtkXMLReader - Superclass for VTK's XML format readers.
// .SECTION Description
// vtkXMLReader uses vtkXMLDataParser to parse a VTK XML input file.
// Concrete subclasses then traverse the parsed file structure and
// extract data.

#ifndef __vtkDataManagerReader_h
#define __vtkDataManagerReader_h

#include "vtkXMLReader.h"
#include "vtkDataManagementExport.h"

class vtkDataManager;
/* class vtkMultiGroupDataSet; */
class vtkMetaDataSet;

//BTX
struct vtkDataManagerReaderInternals;
//ETX

class VTK_DATAMANAGEMENT_EXPORT vtkDataManagerReader : public vtkXMLReader
{
public:
  static vtkDataManagerReader* New();
  vtkTypeRevisionMacro(vtkDataManagerReader,vtkXMLReader);
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // Get the output data object for a port on this algorithm.
  vtkDataManager* GetOutput();

protected:
  vtkDataManagerReader();
  ~vtkDataManagerReader();  

  // Get the name of the data set being read.
  virtual const char* GetDataSetName();

  virtual void ReadXMLData();
  virtual int ReadPrimaryElement(vtkXMLDataElement* ePrimary);

  // Setup the output with no data available.  Used in error cases.
  virtual void SetupEmptyOutput();

  virtual int FillOutputPortInformation(int, vtkInformation* info);

  // Create a default executive.
  virtual vtkExecutive* CreateDefaultExecutive();

  vtkXMLReader* GetReaderOfType(const char* type);

/*   virtual void HandleDataSet( */
/*     vtkXMLDataElement* ds, int group, int dsId,  */
/*     vtkMultiGroupDataSet* output, vtkDataSet* data); */

  virtual int RequestInformation(vtkInformation*, 
                                 vtkInformationVector**, 
                                 vtkInformationVector*);

/*   virtual void ProvideDataManagerFromMultiGroup (vtkMultiGroupDataSet* group); */

  virtual vtkMetaDataSet* CreateMetaDataSetFromDataSet (vtkDataSet* dataset, const char* name);
  virtual void RestoreMetaDataSetInformation(vtkXMLDataElement* element);

  virtual vtkMetaDataSet* CreateMetaDataSetFromXMLElement (vtkXMLDataElement* element);
  
  
private:
  vtkDataManagerReader(const vtkDataManagerReader&);  // Not implemented.
  void operator=(const vtkDataManagerReader&);  // Not implemented.

  vtkDataManagerReaderInternals* Internal;

  
  vtkDataManager* Output;
/*   vtkMultiGroupDataSet* MultiGroup;   */
  
  
};

#endif

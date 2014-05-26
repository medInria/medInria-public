/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <vtkXMLReader.h>
#include <medVtkInriaExport.h>

class vtkDataManager;
/* class vtkMultiGroupDataSet; */
class vtkMetaDataSet;

//BTX
struct vtkDataManagerReaderInternals;
//ETX

class MEDVTKINRIA_EXPORT vtkDataManagerReader : public vtkXMLReader
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



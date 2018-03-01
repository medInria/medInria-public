/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2018. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once
#include <medVtkFibersDataPluginExport.h>
#include <vtkXMLCompositeDataReader.h>

class vtkFiberDataSet;

class MEDVTKFIBERSDATAPLUGIN_EXPORT vtkXMLFiberDataSetReader : public vtkXMLCompositeDataReader
{
public:
  static vtkXMLFiberDataSetReader* New();
  vtkTypeMacro(vtkXMLFiberDataSetReader,vtkXMLCompositeDataReader);
  void PrintSelf(ostream& os, vtkIndent indent);
  
  // Description:
  // Get the output data object for a port on this algorithm.
  vtkFiberDataSet* GetOutput();
  vtkFiberDataSet* GetOutput(int);

protected:
  vtkXMLFiberDataSetReader();
  ~vtkXMLFiberDataSetReader();  

  // Read the XML element for the subtree of a the composite dataset.
  // dataSetIndex is used to rank the leaf nodes in an inorder traversal.
  virtual void ReadComposite(vtkXMLDataElement* element, 
    vtkCompositeDataSet* composite, const char* filePath, 
    unsigned int &dataSetIndex);

  // Get the name of the data set being read.
  virtual const char* GetDataSetName();

  virtual int FillOutputPortInformation(int, vtkInformation* info);

  virtual int RequestDataObject(
                                  vtkInformation* request,
                                  vtkInformationVector** inputVector,
                                  vtkInformationVector* outputVector );

private:
  vtkXMLFiberDataSetReader(const vtkXMLFiberDataSetReader&);  // Not implemented.
  void operator=(const vtkXMLFiberDataSetReader&);  // Not implemented.
};



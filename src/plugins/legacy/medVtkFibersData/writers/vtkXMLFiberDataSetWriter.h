/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <vtkXMLCompositeDataWriter.h>
#include <medVtkFibersDataPluginExport.h>

class MEDVTKFIBERSDATAPLUGIN_EXPORT vtkXMLFiberDataSetWriter : public vtkXMLCompositeDataWriter
{
public:
  static vtkXMLFiberDataSetWriter* New();
  vtkTypeMacro(vtkXMLFiberDataSetWriter, vtkXMLCompositeDataWriter);
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // Get the default file extension for files written by this writer.
  virtual const char* GetDefaultFileExtension()
    { return "fds"; }

//BTX
protected:
  vtkXMLFiberDataSetWriter();
  ~vtkXMLFiberDataSetWriter();

  virtual int FillInputPortInformation(int port, vtkInformation* info);

  // Internal method called recursively to create the xml tree for the children
  // of compositeData.
  virtual int WriteComposite(vtkCompositeDataSet* compositeData, 
    vtkXMLDataElement* parent, int &writerIdx);

private:
  vtkXMLFiberDataSetWriter(const vtkXMLFiberDataSetWriter&); // Not implemented.
  void operator=(const vtkXMLFiberDataSetWriter&); // Not implemented.
//ETX
};





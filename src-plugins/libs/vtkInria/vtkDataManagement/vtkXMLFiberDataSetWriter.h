#pragma once

#include "vtkXMLCompositeDataWriter.h"
#include "vtkDataManagementExport.h"

class VTK_DATAMANAGEMENT_EXPORT vtkXMLFiberDataSetWriter : public vtkXMLCompositeDataWriter
{
public:
  static vtkXMLFiberDataSetWriter* New();
  vtkTypeRevisionMacro(vtkXMLFiberDataSetWriter, vtkXMLCompositeDataWriter);
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





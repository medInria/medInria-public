#ifndef __vtkXMLFiberDataSetReader_h
#define __vtkXMLFiberDataSetReader_h

#include "vtkXMLCompositeDataReader.h"

#include "vtkINRIA3DConfigure.h"

class vtkFiberDataSet;

class VTK_DATAMANAGEMENT_EXPORT vtkXMLFiberDataSetReader : public vtkXMLCompositeDataReader
{
public:
  static vtkXMLFiberDataSetReader* New();
  vtkTypeRevisionMacro(vtkXMLFiberDataSetReader,vtkXMLCompositeDataReader);
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

#endif

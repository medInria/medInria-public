/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include "vtkXMLFiberDataSetReader.h"

#include "vtkCompositeDataPipeline.h"
#include "vtkDataSet.h"
#include "vtkInformation.h"
#include "vtkFiberDataSet.h"
#include "vtkPolyData.h"
#include "vtkPointData.h"
#include "vtkObjectFactory.h"
#include "vtkSmartPointer.h"
#include "vtkXMLDataElement.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"

vtkCxxRevisionMacro(vtkXMLFiberDataSetReader, "$Revision: $");
vtkStandardNewMacro(vtkXMLFiberDataSetReader);

//----------------------------------------------------------------------------
vtkXMLFiberDataSetReader::vtkXMLFiberDataSetReader()
{
}

//----------------------------------------------------------------------------
vtkXMLFiberDataSetReader::~vtkXMLFiberDataSetReader()
{
}

//----------------------------------------------------------------------------
void vtkXMLFiberDataSetReader::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}

//----------------------------------------------------------------------------
int vtkXMLFiberDataSetReader::FillOutputPortInformation(
  int vtkNotUsed(port), vtkInformation* info)
{
  info->Set(vtkDataObject::DATA_TYPE_NAME(), "vtkFiberDataSet");
  return 1;
}

//----------------------------------------------------------------------------
const char* vtkXMLFiberDataSetReader::GetDataSetName()
{
  return "vtkFiberDataSet";
}

//----------------------------------------------------------------------------
vtkFiberDataSet *vtkXMLFiberDataSetReader::GetOutput()
{
  return this->GetOutput (0);
}

//----------------------------------------------------------------------------
vtkFiberDataSet *vtkXMLFiberDataSetReader::GetOutput(int port)
{
  return vtkFiberDataSet::SafeDownCast(this->GetOutputDataObject(port));
}

//----------------------------------------------------------------------------
void vtkXMLFiberDataSetReader::ReadComposite(vtkXMLDataElement* element,
  vtkCompositeDataSet* composite, const char* filePath, 
  unsigned int &dataSetIndex)
{
  vtkFiberDataSet* fiberDataSet = vtkFiberDataSet::SafeDownCast(composite);
  if (!fiberDataSet)
    {
    vtkErrorMacro("Unsuported composite dataset.");
    return;
    }

  if (this->GetFileMajorVersion() < 1)
    {
    vtkErrorMacro("Deprecated file format, unsupported.");
    return;
    }

  unsigned int maxElems = element->GetNumberOfNestedElements();
  for (unsigned int cc=0; cc < maxElems; ++cc)
    {
    vtkXMLDataElement* childXML = element->GetNestedElement(cc);
    if (!childXML || !childXML->GetName())
      {
      continue;
      }

    int index = 0;
    if (!childXML->GetScalarAttribute("index", index))
    // if index not in the structure file, then 
    // set up to add at the end
      {
      index = fiberDataSet->GetNumberOfBlocks();
      }

    // child is a leaf node, read and insert.
    const char* tagName = childXML->GetName();
    if (strcmp(tagName, "Fibers") == 0)
      {
      vtkSmartPointer<vtkDataSet> childDS;
      // if (this->ShouldReadDataSet(dataSetIndex)) // CountLeaves return erroneous values
        {
        // Read
        childDS.TakeReference(this->ReadDataset(childXML, filePath));
        }
      // insert
      fiberDataSet->SetFibers ( vtkPolyData::SafeDownCast (childDS) );
      }
    else if (strcmp(tagName, "Bundle") == 0)
      {
      vtkSmartPointer<vtkDataSet> childDS;
      const char *bundleName = childXML->GetAttribute ("name");
      double color[3] = {0.0, 0.0, 0.0};
      childXML->GetScalarAttribute ("red",   color[0]);
      childXML->GetScalarAttribute ("green", color[1]);
      childXML->GetScalarAttribute ("blue",  color[2]);
      // if (this->ShouldReadDataSet(dataSetIndex)) // CountLeaves return erroneous values
        {
        // Read
        childDS.TakeReference(this->ReadDataset(childXML, filePath));
        }
      // insert
      vtkPolyData *bundle = vtkPolyData::SafeDownCast (childDS);
      if (fiberDataSet->GetFibers())
      {
	bundle->SetPoints(fiberDataSet->GetFibers()->GetPoints());
	bundle->GetPointData()->SetScalars(fiberDataSet->GetFibers()->GetPointData()->GetScalars());
        for( int i=0; i<fiberDataSet->GetFibers()->GetPointData()->GetNumberOfArrays(); i++)
        {
          bundle->GetPointData()->AddArray ( fiberDataSet->GetFibers()->GetPointData()->GetArray (i) );
        }
      }
      fiberDataSet->AddBundle(bundleName, bundle, color);
      }
    else
      {
      vtkErrorMacro("Syntax error in file.");
      return;
      }
    }
}

//----------------------------------------------------------------------------
int vtkXMLFiberDataSetReader::RequestDataObject(vtkInformation* vtkNotUsed(request),
						vtkInformationVector** vtkNotUsed(inputVector),
						vtkInformationVector* outputVector )
{
  //RequestDataObject (RDO) is an earlier pipeline pass.
  //During RDO, each filter is supposed to produce an empty data object of the proper type
  
  vtkInformation* outInfo = outputVector->GetInformationObject(0);
  vtkFiberDataSet* output = vtkFiberDataSet::SafeDownCast(
							  outInfo->Get( vtkDataObject::DATA_OBJECT() ) );
  
  if ( ! output )
    {
      output = vtkFiberDataSet::New();
      outInfo->Set( vtkDataObject::DATA_OBJECT(), output );
      output->FastDelete();
 
      this->GetOutputPortInformation(0)->Set(
					     vtkDataObject::DATA_EXTENT_TYPE(), output->GetExtentType() );
    }
  return 1;
}

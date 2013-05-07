/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include "vtkXMLFiberDataSetWriter.h"

#include "vtkCompositeDataIterator.h"
#include "vtkInformation.h"
#include "vtkPolyData.h"
#include "vtkPointData.h"
#include "vtkFiberDataSet.h"
#include "vtkObjectFactory.h"
#include "vtkSmartPointer.h"
#include "vtkXMLDataElement.h"

vtkStandardNewMacro(vtkXMLFiberDataSetWriter);
vtkCxxRevisionMacro(vtkXMLFiberDataSetWriter, "$Revision: 1.5 $");
//----------------------------------------------------------------------------
vtkXMLFiberDataSetWriter::vtkXMLFiberDataSetWriter()
{
}

//----------------------------------------------------------------------------
vtkXMLFiberDataSetWriter::~vtkXMLFiberDataSetWriter()
{
}

//----------------------------------------------------------------------------
int vtkXMLFiberDataSetWriter::FillInputPortInformation(
  int vtkNotUsed(port), vtkInformation* info)
{
  info->Set(vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkFiberDataSet");
  return 1;
}

//----------------------------------------------------------------------------
int vtkXMLFiberDataSetWriter::WriteComposite(vtkCompositeDataSet* compositeData, 
    vtkXMLDataElement* parent, int &writerIdx)
{
  if (!compositeData->IsA("vtkFiberDataSet"))
    {
    vtkErrorMacro("Unsupported composite dataset type: "
                  << compositeData->GetClassName() << ".");
    return 0;
    }

  vtkFiberDataSet *fiberDataSet = vtkFiberDataSet::SafeDownCast(compositeData);
  if (!fiberDataSet)
    vtkErrorMacro("Cannot cast composite dataset to supported type");

  int index = 0;
  int RetVal = 0;
  
  // first, write the fibers
  {
    vtkPolyData *data = fiberDataSet->GetFibers();
    vtkXMLDataElement* datasetXML = vtkXMLDataElement::New();
    datasetXML->SetName("Fibers");
    datasetXML->SetIntAttribute("index", index);
    vtkStdString fileName = this->CreatePieceFileName(writerIdx);
    if (this->WriteNonCompositeData( data, datasetXML, writerIdx, 
				     fileName.c_str()))
    {
      parent->AddNestedElement(datasetXML);
      RetVal = 1;
    }
    datasetXML->Delete();
    index++;
  }

  // then, write each bundle
  vtkFiberDataSet::vtkFiberBundleListType bundleList = fiberDataSet->GetBundleList();
  vtkFiberDataSet::vtkFiberBundleListType::iterator it = bundleList.begin();

  while (it!=bundleList.end())
  {
    std::string bundleName = (*it).first;
    vtkPolyData *data = (*it).second.Bundle;

    vtkPolyData *copyData = vtkPolyData::New();
    copyData->SetLines ( data->GetLines() );
    
    vtkXMLDataElement* datasetXML = vtkXMLDataElement::New();
    datasetXML->SetName("Bundle");
    datasetXML->SetIntAttribute("index", index);
    datasetXML->SetAttribute ("name", bundleName.c_str());
    datasetXML->SetDoubleAttribute ("red",   (*it).second.Red);
    datasetXML->SetDoubleAttribute ("green", (*it).second.Green);
    datasetXML->SetDoubleAttribute ("blue",  (*it).second.Blue);
    vtkStdString fileName = this->CreatePieceFileName(writerIdx);
    if (this->WriteNonCompositeData( copyData, datasetXML, writerIdx, 
				     fileName.c_str()))
    {
      parent->AddNestedElement(datasetXML);
      RetVal = 1;
    }
    datasetXML->Delete();

    copyData->Delete();
    
    index++;
    ++it;
  }
  
  return RetVal;
}


//----------------------------------------------------------------------------
void vtkXMLFiberDataSetWriter::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}


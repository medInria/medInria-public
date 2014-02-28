/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <map>
#include <string>
#include <vector>

#include "vtkDataManagerReader.h"

#include "vtkCompositeDataPipeline.h"
#include "vtkDataSet.h"
// #include "vtkMultiGroupDataSet.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkInstantiator.h"
#include "vtkObjectFactory.h"
#include "vtkSmartPointer.h"
#include "vtkXMLDataElement.h"
#include "vtkXMLImageDataReader.h"
#include "vtkXMLPolyDataReader.h"
#include "vtkXMLRectilinearGridReader.h"
#include "vtkXMLStructuredGridReader.h"
#include "vtkXMLUnstructuredGridReader.h"

#include <vtkDataSetReader.h>

#include <vtkUnstructuredGrid.h>
#include <vtkPolyData.h>
#include <vtkRectilinearGrid.h>
#include <vtkStructuredGrid.h>

#include <vtkDataManager.h>
#include <vtkMetaImageData.h>
#include <vtkMetaSurfaceMesh.h>
#include <vtkMetaVolumeMesh.h>
#include <vtkMetaDataSetSequence.h>
#include <vtkDataSet.h>

#include <vtkDataManager.h>

// #include <vtkMultiGroupDataSet.h>


vtkCxxRevisionMacro(vtkDataManagerReader, "$Revision: 1087 $");
vtkStandardNewMacro(vtkDataManagerReader);

struct vtkDataManagerReaderEntry
{
  const char* extension;
  const char* name;
};

struct vtkDataManagerReaderInternals
{
  vtkXMLDataElement* MetaDataInformation;
  std::vector<vtkXMLDataElement*> MetaDataSets;
  typedef std::map<std::string, vtkSmartPointer<vtkXMLReader> > ReadersType;
  ReadersType Readers;
  static const vtkDataManagerReaderEntry ReaderList[];
};

//----------------------------------------------------------------------------
vtkDataManagerReader::vtkDataManagerReader()
{
  this->Internal = new vtkDataManagerReaderInternals;
//   this->MultiGroup = vtkMultiGroupDataSet::New();
  this->Output = vtkDataManager::New();
}

//----------------------------------------------------------------------------
vtkDataManagerReader::~vtkDataManagerReader()
{
//   this->MultiGroup->Delete();
  this->Output->Delete();
  
  delete this->Internal;
}

//----------------------------------------------------------------------------
void vtkDataManagerReader::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}


//----------------------------------------------------------------------------
void vtkDataManagerReader::SetupEmptyOutput()
{
}

//----------------------------------------------------------------------------
const char* vtkDataManagerReader::GetDataSetName()
{
  return "vtkDataManager";
}

//----------------------------------------------------------------------------
int vtkDataManagerReader::FillOutputPortInformation(
  int vtkNotUsed(port), vtkInformation* info)
{
  info->Set(vtkDataObject::DATA_TYPE_NAME(), "vtkDataObject");
  return 1;
}

//----------------------------------------------------------------------------
vtkExecutive* vtkDataManagerReader::CreateDefaultExecutive()
{
  return vtkCompositeDataPipeline::New();
}

//----------------------------------------------------------------------------
vtkDataManager* vtkDataManagerReader::GetOutput()
{
  return this->Output;
}

//----------------------------------------------------------------------------
int vtkDataManagerReader::ReadPrimaryElement(vtkXMLDataElement* ePrimary)
{
  if(!this->Superclass::ReadPrimaryElement(ePrimary)) { return 0; }

  int numNested = ePrimary->GetNumberOfNestedElements();
  int i;
  this->Internal->MetaDataSets.clear();
  for(i=0; i < numNested; ++i)
  {
    vtkXMLDataElement* eNested = ePrimary->GetNestedElement(i);
    if(strcmp(eNested->GetName(), "vtkMetaDataSet") == 0) 
    { 
      this->Internal->MetaDataSets.push_back(eNested);
    }
  }
    

  return 1;
}

template<typename T>
vtkDataSet * FileToDataSet_helper(const std::string& filename)
{
    T* reader = T::New();
    reader->SetFileName(filename.c_str());
    reader->Update();
    vtkDataSet * output = reader->GetOutputAsDataSet();
    vtkDataSet * outputCopy = output->NewInstance();
    outputCopy->DeepCopy(output);
    reader->Delete();
    return outputCopy;
}

template<typename T>
vtkDataSet * FileToDataSet_helper2(const std::string& filename)
{
    T* reader = T::New();
    reader->SetFileName(filename.c_str());
    reader->Update();
    vtkDataSet * output = reader->GetOutput();
    vtkDataSet * outputCopy = output->NewInstance();
    outputCopy->DeepCopy(output);
    reader->Delete();
    return outputCopy;
}

//----------------------------------------------------------------------------
vtkDataSet* vtkDataManagerReader::FileToDataSet(const char* type, const std::string& filename)
{
  vtkDataSet* output = 0;
  if (strcmp(type, "vtkXMLImageDataReader") == 0) {
      output = FileToDataSet_helper<vtkXMLImageDataReader>(filename);
  } else if (strcmp(type,"vtkXMLUnstructuredGridReader") == 0) {
      output = FileToDataSet_helper<vtkXMLUnstructuredGridReader>(filename);
  } else if (strcmp(type,"vtkXMLPolyDataReader") == 0) {
      output = FileToDataSet_helper<vtkXMLPolyDataReader>(filename);
  } else if (strcmp(type,"vtkXMLRectilinearGridReader") == 0) {
      output = FileToDataSet_helper<vtkXMLRectilinearGridReader>(filename);
  } else if (strcmp(type,"vtkXMLStructuredGridReader") == 0) {
      output = FileToDataSet_helper<vtkXMLStructuredGridReader>(filename);
  } else if (strcmp(type,"vtkDataSetReader") == 0) {
      output = FileToDataSet_helper2<vtkDataSetReader>(filename);
  }

  return output;
}

//----------------------------------------------------------------------------
vtkMetaDataSet* vtkDataManagerReader::CreateMetaDataSetFromXMLElement (vtkXMLDataElement* element)
{
  vtkMetaDataSet* metadataset = 0;

  int type = 0;
  const char* name;
  double time = 0;
  const char* tag;

  std::string filePath = this->FileName;
  std::string::size_type pos = filePath.find_last_of("/\\");
  if(pos != filePath.npos)
  {
    filePath = filePath.substr(0, pos);
  }
  else
  {
    filePath = "";
  }

  if (!element->GetScalarAttribute("type", type))
  {
    vtkErrorMacro (<<"no type for this dataset : cannot handle"<<endl);
    return NULL;
  }

  int numberofnested = element->GetNumberOfNestedElements();
  std::vector<vtkXMLDataElement*> frames;
  std::vector<vtkXMLDataElement*> flags;
  vtkXMLDataElement* data=NULL;
  
  for(int i=0; i < numberofnested; ++i)
  {
    vtkXMLDataElement* eNested = element->GetNestedElement(i);
    if(strcmp(eNested->GetName(), "vtkMetaDataSet") == 0) 
    { 
      frames.push_back (eNested);
    }
    else if(strcmp(eNested->GetName(), "MetaData") == 0)
    {
      flags.push_back (eNested);
    }
    else if(strcmp(eNested->GetName(), "DataSet") == 0)
    {
      data = eNested;
    }
    
  }

  if (!frames.size())
  {
    switch(type)
    {
      case vtkMetaDataSet::VTK_META_IMAGE_DATA:
	metadataset = vtkMetaImageData::New();
	break;
      case vtkMetaDataSet::VTK_META_VOLUME_MESH:
	metadataset = vtkMetaVolumeMesh::New();
	break;
      case vtkMetaDataSet::VTK_META_SURFACE_MESH:	    
	metadataset = vtkMetaSurfaceMesh::New();
	break;
      default:
	vtkWarningMacro (<<"dataset type not handled"<<endl);
	break;
    }
  }
  else
  {
    metadataset = vtkMetaDataSetSequence::New();
  }  

  name = element->GetAttribute("name");
  if (name)
    metadataset->SetName (name);
  if (element->GetScalarAttribute("time", time))
    metadataset->SetTime (time);
  tag = element->GetAttribute("tag");
  if (tag)
    metadataset->SetTag (tag);

  for (unsigned int j=0; j<flags.size(); j++)
  {
    const char* key = flags[j]->GetAttribute("key");
    if (!key)
      continue;    
    double val;
    flags[j]->GetScalarAttribute("value", val);
    metadataset->SetMetaData<double>(key, val);
  }  
    
  if (!frames.size())
  {
    // Construct the name of the internal file.
    std::string fileName;
    const char* file = data->GetAttribute("file");
    if(!(file[0] == '/' || file[1] == ':'))
    {
      fileName = filePath;
      if(fileName.length())
      {
	fileName += "/";
      }
    }
    fileName += file;
    
    // Get the file extension.
    std::string ext;
    std::string::size_type pos2 = fileName.rfind('.');
    if(pos2 != fileName.npos)
    {
      ext = fileName.substr(pos2+1);
    }
    
    // Search for the reader matching this extension.
    const char* rname = 0;
    for(const vtkDataManagerReaderEntry* r = 
	  this->Internal->ReaderList;
	!rname && r->extension; ++r)
    {
      if(ext == r->extension)
      {
	rname = r->name;
      }
    }
    vtkDataSet* output = this->FileToDataSet(rname, fileName);
    if (!output)
    {
      vtkErrorMacro("Output is not a dataset for  " << rname);
      return NULL;
    }

    metadataset->SetDataSet (output);
  }
  else
  {
    vtkMetaDataSetSequence* sequence = vtkMetaDataSetSequence::SafeDownCast (metadataset);
    for (unsigned int i=0; i<frames.size(); i++)
    {
      vtkMetaDataSet* frame = this->CreateMetaDataSetFromXMLElement (frames[i]);
	  if (frame)
		sequence->AddMetaDataSet (frame);

      frame->Delete();
    }
  }

  return metadataset;
}



//----------------------------------------------------------------------------
void vtkDataManagerReader::ReadXMLData()
{
  // vtkExecutive* exec = this->GetExecutive();
  // vtkInformation* info = exec->GetOutputInformation(0);

  //unsigned int updatePiece = static_cast<unsigned int>(
  //  info->Get(vtkStreamingDemandDrivenPipeline::UPDATE_PIECE_NUMBER()));
  //unsigned int updateNumPieces =  static_cast<unsigned int>(
  //  info->Get(vtkStreamingDemandDrivenPipeline::UPDATE_NUMBER_OF_PIECES()));

  // Find the path to this file in case the internal files are
  // specified as relative paths.
  std::string filePath = this->FileName;
  std::string::size_type pos = filePath.find_last_of("/\\");
  if(pos != filePath.npos)
  {
    filePath = filePath.substr(0, pos);
  }
  else
  {
    filePath = "";
  }
  
  std::vector<vtkXMLDataElement*>::iterator d;
  
  for(d=this->Internal->MetaDataSets.begin();
      d != this->Internal->MetaDataSets.end(); ++d)
  {
    vtkMetaDataSet* metadataset = this->CreateMetaDataSetFromXMLElement (*d);
    if (metadataset)
    {   
      this->GetOutput()->AddMetaDataSet (metadataset);
      metadataset->Delete();
    }
  }
}



//----------------------------------------------------------------------------
int vtkDataManagerReader::RequestInformation(
  vtkInformation *request,
  vtkInformationVector **inputVector,
  vtkInformationVector *outputVector)
{
  this->Superclass::RequestInformation(request, inputVector, outputVector);
  vtkInformation* info = outputVector->GetInformationObject(0);
  info->Set(
    vtkStreamingDemandDrivenPipeline::MAXIMUM_NUMBER_OF_PIECES(), -1);

  return 1;
}

// //----------------------------------------------------------------------------
// void vtkDataManagerReader::HandleDataSet(vtkXMLDataElement*,
//                                                int group, int dsId, 
//                                                vtkMultiGroupDataSet* output,
//                                                vtkDataSet* data)
// {
//   output->SetDataSet(group, dsId, data);
// }

//----------------------------------------------------------------------------
const vtkDataManagerReaderEntry
vtkDataManagerReaderInternals::ReaderList[] =
{
  {"vtp", "vtkXMLPolyDataReader"},
  {"vtu", "vtkXMLUnstructuredGridReader"},
  {"vti", "vtkXMLImageDataReader"},
  {"vtr", "vtkXMLRectilinearGridReader"},
  {"vts", "vtkXMLStructuredGridReader"},
  {"vtk", "vtkDataSetReader"},
  {0, 0}
};


//----------------------------------------------------------------------------
vtkMetaDataSet* vtkDataManagerReader::CreateMetaDataSetFromDataSet (vtkDataSet* dataset, const char* name)
{
  vtkMetaDataSet* metadataset = 0;
  
  switch (dataset->GetDataObjectType())
  {
      case VTK_IMAGE_DATA:
	metadataset = vtkMetaImageData::New();
	break;
      case VTK_UNSTRUCTURED_GRID:
	metadataset = vtkMetaVolumeMesh::New();
	break;
      case VTK_POLY_DATA:	    
	metadataset = vtkMetaSurfaceMesh::New();
	break;
      default:
	vtkWarningMacro (<<"dataset type not handled"<<endl);
	break;
  }

  metadataset->SetDataSet (dataset);
  metadataset->SetName (name);
  return metadataset;
}


//----------------------------------------------------------------------------
void vtkDataManagerReader::RestoreMetaDataSetInformation (vtkXMLDataElement* element)
{
  int NumberOfMetaDataSets = element->GetNumberOfNestedElements();
  int i;
  for(i=0; i < NumberOfMetaDataSets; ++i)
  {
    vtkMetaDataSet* metadataset = this->GetOutput()->GetMetaDataSet (i);
    if (!metadataset)
      continue;

    vtkXMLDataElement* metadatasetelement = element->GetNestedElement(i);
    const char* name = metadatasetelement->GetAttribute("name");
    const char* tag = metadatasetelement->GetAttribute("tag");
    int type = 0;
    double time;
    metadatasetelement->GetScalarAttribute("type", type);
    metadatasetelement->GetScalarAttribute("time", time);
    
    metadataset->SetName (name);
    metadataset->SetTime (time);
    metadataset->SetTag (tag);
    int NumberOfFlags = metadatasetelement->GetNumberOfNestedElements();
    for (int j=0; j<NumberOfFlags; j++)
    {
      vtkXMLDataElement* flagelement = metadatasetelement->GetNestedElement(j);
      const char* key = flagelement->GetAttribute("key");
      double val;
      flagelement->GetScalarAttribute("value", val);
      metadataset->SetMetaData<double>(key, val);
    }
    
    
  }
  
}


  

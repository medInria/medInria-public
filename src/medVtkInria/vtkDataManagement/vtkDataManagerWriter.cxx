/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <string>
#include <vector>

#include "vtkDataManagerWriter.h"

#include "vtkAMRBox.h"
#include "vtkCallbackCommand.h"
#include "vtkCompositeDataPipeline.h"
#include "vtkExecutive.h"
#include "vtkErrorCode.h"
#include "vtkGarbageCollector.h"

#include "vtkImageData.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkObjectFactory.h"
#include "vtkPolyData.h"
#include "vtkRectilinearGrid.h"
#include "vtkStreamingDemandDrivenPipeline.h"
#include "vtkStructuredGrid.h"
#include "vtkUnstructuredGrid.h"
#include "vtkXMLImageDataWriter.h"
#include "vtkXMLPDataWriter.h"
#include "vtkXMLPImageDataWriter.h"
#include "vtkXMLPPolyDataWriter.h"
#include "vtkXMLPRectilinearGridWriter.h"
#include "vtkXMLPStructuredGridWriter.h"
#include "vtkXMLPUnstructuredGridWriter.h"
#include "vtkXMLPolyDataWriter.h"
#include "vtkXMLRectilinearGridWriter.h"
#include "vtkXMLStructuredGridWriter.h"
#include "vtkXMLUnstructuredGridWriter.h"
#include "vtkXMLWriter.h"

#include <vtksys/SystemTools.hxx>
#include <vtksys/ios/sstream>
#include "vtkSmartPointer.h"
#include <string>
#include "vtkDataManager.h"
#include "vtkMetaDataSetSequence.h"
#include "vtkDataSet.h"
//----------------------------------------------------------------------------
vtkStandardNewMacro(vtkDataManagerWriter);
vtkCxxRevisionMacro(vtkDataManagerWriter, "$Revision: 1298 $");



class vtkDataManagerWriterInternals
{
public:
  
  std::string FilePath;
  std::string FilePrefix;
  std::vector<std::string> Entries;
  std::string CreatePieceFileName(int index1, int index2, unsigned int type);

};



//----------------------------------------------------------------------------
vtkDataManagerWriter::vtkDataManagerWriter()
{
  this->Internal = new vtkDataManagerWriterInternals;
  this->Piece = 0;
  this->NumberOfPieces = 1;
  this->GhostLevel = 0;
  this->WriteMetaFileInitialized = 0;
  this->WriteMetaFile = 0;
  
  // Setup a callback for the internal writers to report progress.
  this->ProgressObserver = vtkCallbackCommand::New();
  this->ProgressObserver->SetCallback(&vtkDataManagerWriter::ProgressCallbackFunction);
  this->ProgressObserver->SetClientData(this);

  this->InputInformation = 0;

  this->Input = 0;
  
  this->SetWriteMetaFile (1);
  
  
}

//----------------------------------------------------------------------------
vtkDataManagerWriter::~vtkDataManagerWriter()
{
  this->ProgressObserver->Delete();

  delete this->Internal;
}


//----------------------------------------------------------------------------
void vtkDataManagerWriter::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
  os << indent << "GhostLevel: " << this->GhostLevel << endl;
  os << indent << "NumberOfPieces: " << this->NumberOfPieces<< endl;
  os << indent << "Piece: " << this->Piece<< endl;
  os << indent << "WriteMetaFile: " << this->WriteMetaFile<< endl;
}

//----------------------------------------------------------------------------
int vtkDataManagerWriter::ProcessRequest(
  vtkInformation* request,
  vtkInformationVector** inputVector,
  vtkInformationVector* outputVector)
{
  if(request->Has(vtkStreamingDemandDrivenPipeline::REQUEST_UPDATE_EXTENT()))
    {
    return this->RequestUpdateExtent(request, inputVector, outputVector);
    }
  if(request->Has(vtkDemandDrivenPipeline::REQUEST_DATA()))
    {
    return this->RequestData(request, inputVector, outputVector);
    }

  return this->Superclass::ProcessRequest(request, inputVector, outputVector);
}

//----------------------------------------------------------------------------
void vtkDataManagerWriter::SetWriteMetaFile(int flag)
{
  this->WriteMetaFileInitialized = 1;
  vtkDebugMacro(<< this->GetClassName() << " ("
                << this << "): setting WriteMetaFile to " << flag);
  if(this->WriteMetaFile != flag)
    {
    this->WriteMetaFile = flag;
    this->Modified();
    }
}

//----------------------------------------------------------------------------
int vtkDataManagerWriter::RequestUpdateExtent(
  vtkInformation* vtkNotUsed(request),
  vtkInformationVector** vtkNotUsed(inputVector),
  vtkInformationVector* vtkNotUsed(outputVector))
{
  return 1;
}



//----------------------------------------------------------------------------
int  vtkDataManagerWriter::WriteMetaDataSet (vtkMetaDataSet* metadataset, unsigned int groupid, unsigned int datasetid)
{

  if (!metadataset)
  {
    return 0;
  }  

  vtkMetaDataSetSequence* sequence = vtkMetaDataSetSequence::SafeDownCast (metadataset);
  if (sequence)
  {
    for (int i=0; i < sequence->GetNumberOfMetaDataSets(); i++)
    {
      int ret = this->WriteMetaDataSet (sequence->GetMetaDataSet (i), groupid, i);
      if (!ret)
      {
	return ret;
      }
    }
  }
  else
  {

    std::string filename = this->Internal->FilePath;
    // Set the file name.
    std::string datasetname = this->Internal->CreatePieceFileName(groupid, datasetid, metadataset->GetType());
    filename += datasetname;

    vtkXMLWriter* writer = this->CreateWriter (metadataset);
    if (!writer)
    {
      vtkWarningMacro (<<"writer for metadataset "<<metadataset->GetName()<<" failed at creation !"<<endl);
      return 0;
    }
    writer->SetFileName(filename.c_str());
    writer->SetInput (metadataset->GetDataSet());
    
    // Write the data.
    writer->AddObserver(vtkCommand::ProgressEvent, this->ProgressObserver);
    if( ! writer->Write()) {
        vtkErrorMacro (<<"Writing file failed! "<<writer->GetErrorCode()<<endl);
        writer->Delete();
        return 0;
    }
    writer->RemoveObserver(this->ProgressObserver);

    if (writer->GetErrorCode() == vtkErrorCode::OutOfDiskSpaceError)
    {
      vtkErrorMacro (<<"Out Of Memory Error !"<<endl);
      this->SetErrorCode(vtkErrorCode::OutOfDiskSpaceError);
      writer->Delete();
      return 0;
    }
    writer->Delete();
  }

  return 1;
}



//----------------------------------------------------------------------------
int vtkDataManagerWriter::RequestData(vtkInformation*,
				      vtkInformationVector** inputVector,
				      vtkInformationVector*)
{
  vtkInformation* inInfo = inputVector[0]->GetInformationObject(0);

  this->InputInformation = inInfo;

  if (!this->GetInput()) 
  {
    vtkErrorMacro("No input has been provided. Cannot write");
    this->InputInformation = 0;
    return 0;
  }

  if (!this->Input) 
  {
    vtkErrorMacro("No input has been provided. Cannot write");
    this->InputInformation = 0;
    return 0;
  }
  
  this->SetErrorCode(vtkErrorCode::NoError);
  
  // Make sure we have a file to write.
  if(!this->Stream && !this->FileName)
  {
    vtkErrorMacro("Writer called with no FileName set.");
    this->SetErrorCode(vtkErrorCode::NoFileNameError);
    this->InputInformation = 0;
    return 0;
  }

  // We are just starting to write.  Do not call
  // UpdateProgressDiscrete because we want a 0 progress callback the
  // first time.
  this->UpdateProgress(0);
  
  // Initialize progress range to entire 0..1 range.
  float wholeProgressRange[2] = {0,1};
  this->SetProgressRange(wholeProgressRange, 0, 1);
  
  // Prepare file prefix for creation of internal file names.
  this->SplitFileName();
  
  // Decide whether to write the collection file.
  int writeCollection = 0;
  if(this->WriteMetaFileInitialized)
  {
    writeCollection = this->WriteMetaFile;
  }
  else if(this->Piece == 0)
  {
    writeCollection = 1;
  }
  
  float progressRange[2] = {0,0};
  this->GetProgressRange(progressRange);
  
  // Create the subdirectory for the internal files.
  std::string subdir = this->Internal->FilePath;
  subdir += this->Internal->FilePrefix;
  this->MakeDirectory(subdir.c_str());
  
  this->DeleteAllEntries();

  // Write each input.
  for (unsigned int i=0; i<this->Input->GetNumberOfMetaDataSet(); i++)
  {
    vtkMetaDataSet* metadataset = this->Input->GetMetaDataSet (i);
    this->WriteMetaDataSet (metadataset, i, 0);

    if (this->GetErrorCode() == vtkErrorCode::OutOfDiskSpaceError )
    {
      // remove the subdirectory for the internal files, and the xml file.
      subdir = this->Internal->FilePath;
      subdir += this->Internal->FilePrefix;
      this->RemoveADirectory (subdir.c_str());
      this->DeleteAFile ();
      this->InputInformation = 0;
      return 0;
    }
      
    this->SetProgressRange(progressRange, i, this->Input->GetNumberOfMetaDataSet()+writeCollection);
  }
  
  // Write the collection file if requested.
  if(writeCollection)
  {
    this->SetProgressRange(progressRange, this->Input->GetNumberOfMetaDataSet(),
                           this->Input->GetNumberOfMetaDataSet() + writeCollection);
    int retVal = this->WriteMetaFileIfRequested();
    this->InputInformation = 0;
    this->UpdateProgressDiscrete(1);
    return retVal;
  }

  // We have finished writing.
  this->UpdateProgressDiscrete(1);

  this->InputInformation = 0;

  return 1;
}

  

//----------------------------------------------------------------------------
std::string vtkDataManagerWriter::CreateMetaDataSetEntry(vtkMetaDataSet* metadataset, vtkIndent indent, unsigned int groupid, unsigned int datasetid)
{
  std::ostringstream os;
  std::ostringstream first_st;
  std::ostringstream metadata_st;
  std::ostringstream frames_st;
  std::ostringstream dataset_st;

  
  os << indent
	   << "<vtkMetaDataSet "
	   << "   type=\"" << metadataset->GetType()
	   << "\" name=\"" << metadataset->GetName()
	   << "\" time=\"" << metadataset->GetTime()
	   << "\" tag=\"" << metadataset->GetTag()
	   << "\">" << std::endl;
  
  std::vector<std::string> flaglist = metadataset->GetMetaDataKeyList();
  for (unsigned int i=0; i<flaglist.size(); i++)
  {
    double val = 0;
    std::string key = flaglist[i].c_str();
    bool valid = metadataset->GetMetaData<double>(key, val);
    if (valid)
      os << indent.GetNextIndent()
		  << "<metadata key=\""<< key
		  << "\" value=\""<< val << "\"/>" << std::endl;
  }

  vtkMetaDataSetSequence* sequence = vtkMetaDataSetSequence::SafeDownCast (metadataset);
  
  if (sequence)
  {
    for (int i=0; i<sequence->GetNumberOfMetaDataSets(); i++)
    {
      std::string frame_entry = this->CreateMetaDataSetEntry (sequence->GetMetaDataSet (i), indent.GetNextIndent(), groupid, i);
      os << frame_entry.c_str();
    }  
  }
  else
  {
    // Set the file name.
    // std::string fname = this->Internal->CreatePieceFileName(i);
    std::string fname = this->Internal->CreatePieceFileName(groupid, datasetid, metadataset->GetType());
    
    os << indent.GetNextIndent()
	    << "<DataSet group=\"" << groupid << "\" dataset=\"" << datasetid << "\""
	    << " file=\"" << fname.c_str() << "\"/>" << std::endl;
  }  
    
  os << indent << "</vtkMetaDataSet>" << std::endl;
  std::string os_str = os.str().c_str();
  return os.str();
}

//----------------------------------------------------------------------------
int vtkDataManagerWriter::WriteData()
{
  // Write the collection file.
  this->StartFile();
  vtkIndent indent = vtkIndent().GetNextIndent();
  
  // Open the primary element.
  ostream& os = *(this->Stream);
  os << indent << "<" << this->GetDataSetName() << ">\n";

  for (unsigned int i=0; i<this->Input->GetNumberOfMetaDataSet(); i++)
  {
	
    std::string metadataset_entry = this->CreateMetaDataSetEntry (this->Input->GetMetaDataSet(i), indent.GetNextIndent(), i);
    os << metadataset_entry.c_str() << std::endl;
  }

  
  // Close the primary element.
  
  os << indent << "</" << this->GetDataSetName() << ">\n";
  return this->EndFile();
}

//----------------------------------------------------------------------------
int vtkDataManagerWriter::WriteMetaFileIfRequested()
{
  // Decide whether to write the collection file.
  int writeCollection = 0;
  if(this->WriteMetaFileInitialized)
    {
    writeCollection = this->WriteMetaFile;
    }
  else if(this->Piece == 0)
    {
    writeCollection = 1;
    }
  
  if(writeCollection)
    {
    if(!this->Superclass::WriteInternal()) { return 0; }
    }
  return 1;
}

//----------------------------------------------------------------------------
void vtkDataManagerWriter::MakeDirectory(const char* name)
{
  if( !vtksys::SystemTools::MakeDirectory(name) )
  {
    vtkErrorMacro( << "Sorry unable to create directory: " << name 
                   << endl << "Last systen error was: " 
                   << vtksys::SystemTools::GetLastSystemError().c_str() );
  }
}

//----------------------------------------------------------------------------
void vtkDataManagerWriter::RemoveADirectory(const char* name)
{
  if( !vtksys::SystemTools::RemoveADirectory(name) )
  {
    vtkErrorMacro( << "Sorry unable to remove a directory: " << name 
                   << endl << "Last systen error was: " 
                   << vtksys::SystemTools::GetLastSystemError().c_str() );
  }
  
}

//----------------------------------------------------------------------------
const char* vtkDataManagerWriter::GetDefaultFileExtension()
{
  return "dmf";
}

//----------------------------------------------------------------------------
const char* vtkDataManagerWriter::GetDataSetName()
{
  if (!this->InputInformation)
    {
    return "vtkDataManager";
    }
  vtkDataManager *manager = this->GetInput();
  
  if (!manager) 
  {
    return 0;
  }
  return "vtkDataManager";
}

//----------------------------------------------------------------------------
vtkXMLWriter* vtkDataManagerWriter::CreateWriter(vtkMetaDataSet* metadataset)
{

  vtkXMLWriter* writer = NULL;
  
  if (vtkMetaDataSetSequence::SafeDownCast (metadataset))
    return NULL;

  if (!metadataset || !metadataset->GetDataSet())
    return NULL;
    
  
  switch (metadataset->GetType())
  {
      case vtkMetaDataSet::VTK_META_SURFACE_MESH :
	writer = vtkXMLPolyDataWriter::New();
	break;
	
      case vtkMetaDataSet::VTK_META_VOLUME_MESH :
	writer = vtkXMLUnstructuredGridWriter::New();
	break;
	
      case vtkMetaDataSet::VTK_META_IMAGE_DATA :
	writer = vtkXMLImageDataWriter::New();
	break;
	
      default:
	vtkErrorMacro(<<"dataset type not handled"<<endl);
	break;
  }

  return writer;
  
}

//----------------------------------------------------------------------------
void vtkDataManagerWriter::SplitFileName()
{
  std::string fileName = this->FileName;
  std::string name;
  
  // Split the file name and extension from the path.
  std::string::size_type pos = fileName.find_last_of("/\\");
  if(pos != fileName.npos)
    {
    // Keep the slash in the file path.
    this->Internal->FilePath = fileName.substr(0, pos+1);
    name = fileName.substr(pos+1);
    }
  else
    {
    this->Internal->FilePath = "./";
    name = fileName;
    }
  
  // Split the extension from the file name.
  pos = name.find_last_of(".");
  if(pos != name.npos)
    {
    this->Internal->FilePrefix = name.substr(0, pos);
    }
  else
    {
    this->Internal->FilePrefix = name;
    
    // Since a subdirectory is used to store the files, we need to
    // change its name if there is no file extension.
    this->Internal->FilePrefix += "_data";
    }
}

//----------------------------------------------------------------------------
const char* vtkDataManagerWriter::GetFilePrefix()
{
  return this->Internal->FilePrefix.c_str();
}

//----------------------------------------------------------------------------
const char* vtkDataManagerWriter::GetFilePath()
{
  return this->Internal->FilePath.c_str();
}

//----------------------------------------------------------------------------
void vtkDataManagerWriter::ProgressCallbackFunction(vtkObject* caller,
                                                        unsigned long,
                                                        void* clientdata,
                                                        void*)
{
  vtkAlgorithm* w = vtkAlgorithm::SafeDownCast(caller);
  if(w)
    {
    reinterpret_cast<vtkDataManagerWriter*>(clientdata)->ProgressCallback(w);
    }
}

//----------------------------------------------------------------------------
void vtkDataManagerWriter::ProgressCallback(vtkAlgorithm* w)
{
  float width = this->ProgressRange[1]-this->ProgressRange[0];
  float internalProgress = w->GetProgress();
  float progress = this->ProgressRange[0] + internalProgress*width;
  this->UpdateProgressDiscrete(progress);
  if(this->AbortExecute)
    {
    w->SetAbortExecute(1);
    }
}

//----------------------------------------------------------------------------
void vtkDataManagerWriter::AppendEntry(const char* entry)
{
  this->Internal->Entries.push_back(entry);
}

//----------------------------------------------------------------------------
void vtkDataManagerWriter::DeleteAllEntries()
{
  this->Internal->Entries.clear();
}

//----------------------------------------------------------------------------
std::string vtkDataManagerWriterInternals::CreatePieceFileName(int index1, int index2, unsigned int type)
{
  
  std::string ext;
  switch(type)
  {
      case vtkMetaDataSet::VTK_META_IMAGE_DATA :
	ext = "vti";
	break;
      case vtkMetaDataSet::VTK_META_SURFACE_MESH :
	ext = "vtp";
	break;
      case vtkMetaDataSet::VTK_META_VOLUME_MESH :
	ext = "vtu";
	break;
  }
  
  std::string fname;
  vtksys_ios::ostringstream fn_with_warning_C4701;
  fn_with_warning_C4701
    << this->FilePrefix.c_str() << "/"
    << this->FilePrefix.c_str() << "_" << index1 << "_" << index2 << "."
    << ext;
  fname = fn_with_warning_C4701.str();
  return fname;
}

//----------------------------------------------------------------------------
void vtkDataManagerWriter::ReportReferences(vtkGarbageCollector* collector)
{
  this->Superclass::ReportReferences(collector);
}

//----------------------------------------------------------------------------
vtkExecutive* vtkDataManagerWriter::CreateDefaultExecutive()
{
  return vtkCompositeDataPipeline::New();
}

//----------------------------------------------------------------------------
int vtkDataManagerWriter::FillInputPortInformation(
  int vtkNotUsed(port), vtkInformation* info)
{
  info->Set(vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkDataObject");
  return 1;
}

//----------------------------------------------------------------------------
void vtkDataManagerWriter::SetInput (vtkDataManager* manager)
{
  this->Input = manager;
  if (manager->GetMetaDataSet((unsigned int)0) && manager->GetMetaDataSet((unsigned int)0)->GetDataSet())
    this->SetInputConnection(0, manager->GetMetaDataSet((unsigned int)0)->GetDataSet()->GetProducerPort());
}

//----------------------------------------------------------------------------
vtkDataManager* vtkDataManagerWriter::GetInput()
{
  return this->Input;
}


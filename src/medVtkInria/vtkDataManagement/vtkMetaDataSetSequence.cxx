/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <vtkMetaDataSetSequence.h>
#include "vtkObjectFactory.h"


#include <vtkMetaDataSet.h>
#include <vtkMetaImageData.h>
#include <vtkMetaSurfaceMesh.h>
#include <vtkMetaVolumeMesh.h>
#include <vtkDataSet.h>
#include <vtkPointData.h>
#include <vtkImageData.h>
#include <vtkPolyData.h>
#include <vtkUnstructuredGrid.h>
#include <vtkImageData.h>
#include <vtkMapper.h>
#include <vtkActorCollection.h>
#include <vtkActor.h>
#include <vtkColorTransferFunction.h>
#include <vtkCellData.h>
#include <vtksys/SystemTools.hxx>
#include <vtkDirectory.h>
#include <vtkErrorCode.h>
#include <vtkLookupTable.h>
#include <vtkCellArray.h>
#include <vtkDoubleArray.h>
#include <vtkDataArrayCollection.h>

#include <itkPhilipsRECImageIO.h>
#include <itkPhilipsPAR.h>

#include <sstream>
#include <algorithm> // for sort algorithm

//----------------------------------------------------------------------------
vtkStandardNewMacro( vtkMetaDataSetSequence );
vtkCxxRevisionMacro(vtkMetaDataSetSequence, "$Revision: 1370 $");

//----------------------------------------------------------------------------
vtkMetaDataSetSequence::vtkMetaDataSetSequence()
{

  this->SequenceDuration = 2.0;
  this->CurrentId = -1;
  this->Type = vtkMetaDataSet::VTK_META_UNKNOWN;
  this->SameGeometryFlag = true;
  this->ParseAttributes = true;
  
}

//----------------------------------------------------------------------------
vtkMetaDataSetSequence::~vtkMetaDataSetSequence()
{
  for (unsigned int i=0; i<this->MetaDataSetList.size(); i++)
    this->MetaDataSetList[i]->Delete();
  this->MetaDataSetList.clear();
}


//----------------------------------------------------------------------------
void vtkMetaDataSetSequence::Initialize()
{
  this->Superclass::Initialize();
  
//   if (this->DataSet)
//     this->DataSet->GetPointData()->CopyScalarsOn();

}




//----------------------------------------------------------------------------
unsigned int vtkMetaDataSetSequence::CanReadFile (const char* filename)
{
  /*
    As their might be more than one volume in an image, we read 4D
    images, and then split them into 3D volumes if the 4th dimension
    is greater than 1.
  */
  itk::ImageFileReader<vtkMetaImageData::FloatImage4DType>::Pointer reader4d 
    = itk::ImageFileReader<vtkMetaImageData::FloatImage4DType>::New();
  reader4d->SetFileName (filename);
  
  try
  {
    reader4d->GenerateOutputInformation();
  }
  catch (itk::ExceptionObject)
  {
    return 0;
  }  
  
  // here we look for the 4th dimension:
  vtkMetaImageData::FloatImage4DType::Pointer image4d = reader4d->GetOutput();
  vtkMetaImageData::FloatImage4DType::SizeType size = image4d->GetLargestPossibleRegion().GetSize();
  unsigned int nVolumes = size[3];
  
  if( nVolumes<2 )
  {
    return 0;
  }

  return 1;
}





//----------------------------------------------------------------------------
void vtkMetaDataSetSequence::Read(const char* dirname)
{

  if (!vtksys::SystemTools::FileIsDirectory (dirname))
  {
  
    try
    {

      // first read the image component type
      typedef itk::Image<short,4> d_ImageType;
      typedef itk::ImageFileReader<d_ImageType> d_ReaderType;
      d_ReaderType::Pointer d_reader = d_ReaderType::New();
      d_reader->SetFileName(dirname);
      try
      {
	d_reader->GenerateOutputInformation();
      }
      catch(itk::ExceptionObject &e)
      {
	std::cerr << e;
	throw vtkErrorCode::CannotOpenFileError;
      }
      
      // check if the image can be read
      itk::ImageIOBase::IOComponentType componenttype = d_reader->GetImageIO()->GetComponentType();
      
      if (componenttype == itk::ImageIOBase::UNKNOWNCOMPONENTTYPE)
      {
	std::cerr << "Unknown component type !" << std::endl;
	throw vtkErrorCode::CannotOpenFileError;
      }
      if(d_reader->GetImageIO()->GetPixelType() != itk::ImageIOBase::SCALAR)
      {
	std::cerr << "Only scalar images can be read" << std::endl;
	throw vtkErrorCode::CannotOpenFileError;
      }

      
      // finally read the image : try all types
      if (componenttype == itk::ImageIOBase::UCHAR)
	this->ReadSingleFile<unsigned char>(dirname);  
      else if (componenttype ==  itk::ImageIOBase::CHAR)
	this->ReadSingleFile<char>(dirname);
      else if (componenttype ==  itk::ImageIOBase::USHORT)
	this->ReadSingleFile<unsigned short>(dirname);
      else if (componenttype ==  itk::ImageIOBase::SHORT)
	this->ReadSingleFile<short>(dirname);
      else if (componenttype ==  itk::ImageIOBase::UINT)
	this->ReadSingleFile<unsigned int>(dirname);
      else if (componenttype ==  itk::ImageIOBase::INT)
	this->ReadSingleFile<int>(dirname);
      else if (componenttype ==  itk::ImageIOBase::ULONG)
	this->ReadSingleFile<unsigned long>(dirname);
      else if (componenttype ==  itk::ImageIOBase::LONG)
	this->ReadSingleFile<long>(dirname);
      else if (componenttype ==  itk::ImageIOBase::FLOAT)
	this->ReadSingleFile<float>(dirname);
      else if (componenttype ==  itk::ImageIOBase::DOUBLE)
	this->ReadSingleFile<double>(dirname);

    } 
    catch (vtkErrorCode::ErrorIds error)
    {
      throw error;
    }
    this->SetFilePath (dirname);  
    return;
  }
  

  
  vtkDirectory* directory = vtkDirectory::New();
  int ret = directory->Open(dirname);
  if (!ret)
  {
    vtkErrorMacro(<<"Cannot open directory"<<dirname<<endl);
    throw vtkErrorCode::CannotOpenFileError;
  }

  
  std::string directoryname = dirname;
  std::string::size_type slashpos;
  
  // check slash position
#ifdef WIN32
  slashpos = directoryname.rfind ("\\"); // last occurence of "/"
  if (slashpos != directoryname.size()-1)
    directoryname+="\\";
#else
  slashpos = directoryname.rfind ("/"); // last occurence of "/"
  if (slashpos != directoryname.size()-1)
    directoryname+="/";
#endif
  
  std::vector<std::string> list;
  std::string firstfilename;
  
  if (directory->GetNumberOfFiles() < 2)
  {
    vtkErrorMacro(<<"not enougth files in directory \n cannot build sequence !"<<endl);
    throw vtkErrorCode::UserError;
  }
  
  firstfilename = "";
  
  for (int i=0; i<directory->GetNumberOfFiles(); i++)
  {
    if ( (strcmp(directory->GetFile (i), ".") == 0 )
	 || (strcmp(directory->GetFile (i), "..") == 0 ))
      continue;

    if (!firstfilename.size())
      firstfilename = directoryname+directory->GetFile (i);
    
    list.push_back((directoryname+directory->GetFile (i)));
  }

  std::sort(list.begin(), list.end());

  double duration = this->SequenceDuration;
  
  if (duration <= 0)
  {
    duration = 2.5;
  }
  
  for (unsigned int i = 0; i < list.size(); i++)
  {
    double time = (double)(i)*duration/(double)(list.size());
    
    try
    {
      this->ReadAndAddFile (list[i].c_str(), time);
    }
    catch (vtkErrorCode::ErrorIds)
    {
      // we don't throw exception as we want to continue scanning the directory
      std::cerr<<"skipping file "<<list[i].c_str()<<std::endl;
    }    
  }
  
  this->SetFilePath (dirname);  

  directory->Delete();

}

//----------------------------------------------------------------------------
void vtkMetaDataSetSequence::ReadAndAddFile (const char* filename, double time)
{
  vtkMetaDataSet* metadataset = NULL;

  if (this->GetType() == vtkMetaDataSet::VTK_META_UNKNOWN)
  {
    
    if (vtkMetaSurfaceMesh::CanReadFile (filename))
      this->Type = vtkMetaDataSet::VTK_META_SURFACE_MESH;
    else if (vtkMetaVolumeMesh::CanReadFile (filename))
      this->Type = vtkMetaDataSet::VTK_META_VOLUME_MESH;
    else if (vtkMetaImageData::CanReadFile (filename))
      this->Type = vtkMetaDataSet::VTK_META_IMAGE_DATA;
    else
    {
      vtkErrorMacro(<<"unable to open file : "<<filename<<endl);
      throw vtkErrorCode::CannotOpenFileError;
    }
  }
  

  switch(this->GetType())
  {
      case vtkMetaDataSet::VTK_META_SURFACE_MESH:
	if (!vtkMetaSurfaceMesh::CanReadFile (filename))
	{
	  vtkErrorMacro(<<"wrong file type : "<<filename<<endl);
	  throw vtkErrorCode::CannotOpenFileError;
	}
	
	metadataset = vtkMetaSurfaceMesh::New();
	break;
      case vtkMetaDataSet::VTK_META_VOLUME_MESH:
	if (!vtkMetaVolumeMesh::CanReadFile (filename))
	{
	  vtkErrorMacro(<<"wrong file type : "<<filename<<endl);
	  throw vtkErrorCode::CannotOpenFileError;
	}
	
	metadataset = vtkMetaVolumeMesh::New();
	break;
      case vtkMetaDataSet::VTK_META_IMAGE_DATA:
	if (!vtkMetaImageData::CanReadFile (filename))
	{
	  vtkErrorMacro(<<"wrong file type : "<<filename<<endl);
	  throw vtkErrorCode::CannotOpenFileError;
	}
	
	metadataset = vtkMetaImageData::New();
	break;
  }
  
  try
  {
    
    metadataset->Read(filename);
    metadataset->SetTime(time);
    metadataset->SetName(vtksys::SystemTools::GetFilenameWithoutExtension(vtksys::SystemTools::GetFilenameName (filename).c_str()).c_str());
    this->AddMetaDataSet(metadataset);
    metadataset->Delete();
  }
  catch (vtkErrorCode::ErrorIds)
  {
    vtkErrorMacro(<<"error when reading file "<<filename<<endl);
    throw vtkErrorCode::CannotOpenFileError;
  }


}


//----------------------------------------------------------------------------
void vtkMetaDataSetSequence::Write(const char* dirname)
{


  
  if (!vtksys::SystemTools::FileIsDirectory (dirname) && (this->GetType() == vtkMetaDataSet::VTK_META_IMAGE_DATA))
  {
    std::string filename = dirname;
    std::string extension = vtksys::SystemTools::GetFilenameLastExtension(dirname);
    if (extension.size() < 2)
      filename+=".mha";

    try
    {

      vtkImageData* imagedata = vtkImageData::SafeDownCast (this->GetDataSet());
      if (!imagedata)
      {
	 vtkErrorMacro(<<"Cannot retreive image !"<<endl);
	 throw vtkErrorCode::UserError;
      }
      
      unsigned int componenttype = imagedata->GetScalarType();

      if (componenttype == VTK_UNSIGNED_CHAR)
	this->WriteSingleFile<unsigned char>(filename.c_str()); 
      else if (componenttype == VTK_CHAR)
	this->WriteSingleFile<char>(filename.c_str()); 
      else if (componenttype == VTK_UNSIGNED_SHORT)
	this->WriteSingleFile<unsigned short>(filename.c_str()); 
      else if (componenttype == VTK_SHORT)
	this->WriteSingleFile<short>(filename.c_str()); 
      else if (componenttype == VTK_UNSIGNED_INT)
	this->WriteSingleFile<unsigned int>(filename.c_str()); 
      else if (componenttype == VTK_INT)
	this->WriteSingleFile<int>(filename.c_str()); 
      else if (componenttype == VTK_UNSIGNED_LONG)
	this->WriteSingleFile<unsigned long>(filename.c_str()); 
      else if (componenttype == VTK_LONG)
	this->WriteSingleFile<long>(filename.c_str()); 
      else if (componenttype == VTK_FLOAT)
	this->WriteSingleFile<float>(filename.c_str()); 
      else if (componenttype == VTK_DOUBLE)
	this->WriteSingleFile<double>(filename.c_str()); 
      else
	this->WriteSingleFile<float>(filename.c_str());
    } 
    catch (vtkErrorCode::ErrorIds error)
    {
      throw error;
    }
    this->SetFilePath (dirname);  

    return;
  }
  

  std::string directoryname = dirname;
  std::string::size_type slashpos;
  
  // check slash position
#ifdef WIN32
  slashpos = directoryname.rfind ("\\"); // last occurence of "/"
  if (slashpos != directoryname.size()-1)
    directoryname+="\\";
#else
  slashpos = directoryname.rfind ("/"); // last occurence of "/"
  if (slashpos != directoryname.size()-1)
    directoryname+="/";
#endif


  std::string extension;
  if (this->GetType() == vtkMetaDataSet::VTK_META_IMAGE_DATA)
    extension = ".mha";
  else
    extension = ".vtk";

  int N = this->GetNumberOfMetaDataSets();
  

  for (int i=0; i<N; i++)
  {
    try
    {
      vtkMetaDataSet* metadataset = this->GetMetaDataSet (i);
      std::ostringstream os;
      os << directoryname << this->GetName() << "-";
      if (N > 99)
      {
	if (i < 10)
	  os << "0";
	if (i <100)
	  os << "0";
      }
      else if (N > 10)
	if (i < 10)
	  os << "0";
      os << i << extension.c_str();      
      std::string filename = os.str().c_str();
      
      metadataset->Write (filename.c_str());
    } 
    catch (vtkErrorCode::ErrorIds error)
    {
      throw error;
    }
  }

  this->SetFilePath (dirname);  

}



//----------------------------------------------------------------------------
void vtkMetaDataSetSequence::BuildSequenceFromGeometry(const char* filename, unsigned int Niter)
{

  unsigned long type = vtkMetaDataSet::VTK_META_UNKNOWN;
  
  if (vtkMetaSurfaceMesh::CanReadFile (filename))
    type = vtkMetaDataSet::VTK_META_SURFACE_MESH;
  else if (vtkMetaVolumeMesh::CanReadFile (filename))
    type = vtkMetaDataSet::VTK_META_VOLUME_MESH;
  else
  {
    vtkErrorMacro(<<"unknown file type (should be a geometry) : "<<filename<<endl);
    throw vtkErrorCode::CannotOpenFileError;
  }

  vtkMetaDataSet* mainmetadataset = NULL;
  
  switch(type)
  {
      case vtkMetaDataSet::VTK_META_SURFACE_MESH:
	mainmetadataset = vtkMetaSurfaceMesh::New();
	break;
      case vtkMetaDataSet::VTK_META_VOLUME_MESH:
	mainmetadataset = vtkMetaVolumeMesh::New();
	break;
  }

  
  try
  {
    
    mainmetadataset->Read(filename);
    mainmetadataset->SetTime (0.0);
    mainmetadataset->SetName(vtksys::SystemTools::GetFilenameWithoutExtension(vtksys::SystemTools::GetFilenameName (filename).c_str()).c_str());
    this->BuildMetaDataSetFromMetaDataSet(mainmetadataset);
    mainmetadataset->Delete();
  }
  catch (vtkErrorCode::ErrorIds error)
  {
    mainmetadataset->Delete();
    vtkErrorMacro(<<"error in reading."<<endl);
    throw error;
  }
  
  vtkPointSet* mainpointset = vtkPointSet::SafeDownCast (this->DataSet);
  double duration = this->SequenceDuration;
  for (unsigned int i=0; i<Niter; i++)
  {
    
    vtkMetaDataSet* metadataset = NULL;
    vtkPointSet* pointset = NULL;
    double time = (double)(i)*duration/(double)(Niter);
    
    switch(type)
    {
	case vtkMetaDataSet::VTK_META_SURFACE_MESH:
	  metadataset = vtkMetaSurfaceMesh::New();
	  pointset = vtkPolyData::New();
	  break;
	case vtkMetaDataSet::VTK_META_VOLUME_MESH:
	  metadataset = vtkMetaVolumeMesh::New();
	  pointset = vtkUnstructuredGrid::New();
	  break;
    }
    std::ostringstream os;
    os << this->GetName() << "#" << i+1;
    metadataset->SetName (os.str().c_str());
    metadataset->SetTime (time);
    
    pointset->CopyStructure (mainpointset);
    metadataset->SetDataSet(pointset);
    try
    {
      this->AddMetaDataSet (metadataset);
    }
    catch (vtkErrorCode::ErrorIds)
    {
      pointset->Delete();
      metadataset->Delete();
      vtkWarningMacro(<<"Error while adding dataset, skipping..."<<endl);
      continue;
    }
  
    pointset->Delete();
    metadataset->Delete();
  }
  
}


//----------------------------------------------------------------------------
void vtkMetaDataSetSequence::AddMetaDataSet (vtkMetaDataSet* metadataset)
{
  if (!metadataset)
  {  
    vtkErrorMacro(<<"NULL object !"<<endl);
    throw vtkErrorCode::UserError;
  }
  
  if (this->Type == vtkMetaDataSet::VTK_META_UNKNOWN)
    this->Type = metadataset->GetType();
  
  if ( metadataset->GetType() != this->Type)
  {
    vtkErrorMacro(<<"Cannot add heterogeneous type datasets to sequence !"<<endl);
    throw vtkErrorCode::UserError;
  }

  vtkMetaImageData* metaimage1 = NULL;
  metaimage1 = vtkMetaImageData::SafeDownCast (metadataset);
  if (metaimage1)
  {
    vtkImageData* image1 = metaimage1->GetImageData();
    vtkImageData* image2 = vtkImageData::SafeDownCast (this->GetDataSet());
    if (image1 && image2)
    {
      if ((image1->GetDimensions()[0] != image2->GetDimensions()[0]) ||
	  (image1->GetDimensions()[1] != image2->GetDimensions()[1]) ||
	  (image1->GetDimensions()[2] != image2->GetDimensions()[2])
	  )
      {
	vtkErrorMacro(<<"Images must have the same dimensions !"<<endl);
	throw vtkErrorCode::UserError;
      }
    }
  }


  std::vector<vtkMetaDataSet*>::iterator it;
  bool inserted = false;

  for (it = this->MetaDataSetList.begin(); it != this->MetaDataSetList.end(); it++)
  {
    if ((*it)->GetTime() > metadataset->GetTime())
    {
      this->MetaDataSetList.insert(it, metadataset);
      inserted = true;
    }
  }
  if (!inserted)
  {  
    this->MetaDataSetList.push_back (metadataset);
  }
  metadataset->Register(this);  
  
  try
  {
    if (!this->GetDataSet())
    {
      this->Time = metadataset->GetTime();
      this->CurrentId = this->MetaDataSetList.size() - 1;
      this->BuildMetaDataSetFromMetaDataSet (metadataset);
    }
    this->ComputeSequenceDuration();
  }
  catch (vtkErrorCode::ErrorIds error)
  {
    throw error;
  }    
}



//----------------------------------------------------------------------------
void vtkMetaDataSetSequence::RemoveMetaDataSet (unsigned int id)
{
  if (id >= this->MetaDataSetList.size())
    return;
  
  std::vector<vtkMetaDataSet*> templist = this->MetaDataSetList;
  this->MetaDataSetList.clear();
  
  for (unsigned int i=0; i<templist.size(); i++)
  {
    if (i != id)
      this->MetaDataSetList.push_back (templist[i]);
    else
    {
      templist[i]->UnRegister(this);
    }
  }
  
  this->ComputeSequenceDuration();

  if (this->MetaDataSetList.size() == 0)
  {    
    this->Type = vtkMetaDataSet::VTK_META_UNKNOWN;
    if (this->DataSet)
    {
      this->DataSet->Delete();
    }
  }

}


//----------------------------------------------------------------------------
void vtkMetaDataSetSequence::RemoveMetaDataSet (vtkMetaDataSet* metadataset)
{
  for (unsigned int i=0; i<this->MetaDataSetList.size(); i++)
  {
    if (this->MetaDataSetList[i] == metadataset)
    {
      this->RemoveMetaDataSet(i);
      return;
    }
  }
}

//----------------------------------------------------------------------------
void vtkMetaDataSetSequence::RemoveAllMetaDataSets()
{
  for (unsigned int i=0; i<this->MetaDataSetList.size(); i++)  
    this->MetaDataSetList[i]->UnRegister(this);
  this->MetaDataSetList.clear();
  this->ComputeSequenceDuration();
  this->Type = vtkMetaDataSet::VTK_META_UNKNOWN;

  if (this->DataSet)
  {
    this->DataSet->Delete();
  }

}

  

  
//----------------------------------------------------------------------------
vtkMetaDataSet* vtkMetaDataSetSequence::GetMetaDataSet (unsigned int i)
{
  if (i>=this->MetaDataSetList.size())
    return NULL;
  return this->MetaDataSetList[i];
}



//----------------------------------------------------------------------------
bool vtkMetaDataSetSequence::HasMetaDataSet (vtkMetaDataSet* metadataset)
{
  for (unsigned int i=0; i<this->MetaDataSetList.size(); i++)
  {
    if (this->MetaDataSetList[i] == metadataset)
      return true;
  }
  return false;
}


//----------------------------------------------------------------------------
vtkMetaDataSet* vtkMetaDataSetSequence::FindMetaDataSet (const char* name)
{
  
  for (unsigned int i=0; i<this->MetaDataSetList.size(); i++)
  {
    if (strcmp (this->MetaDataSetList[i]->GetName(), name) == 0)
      return this->MetaDataSetList[i];
  }
  return NULL;

}

  
//----------------------------------------------------------------------------
vtkMetaDataSet* vtkMetaDataSetSequence::FindMetaDataSet (double time, unsigned int &id)
{

  double distance = VTK_DOUBLE_MAX;
  double framedistance;
  vtkMetaDataSet* ret = 0;
  unsigned int i;
  
  for (i=0; i<this->MetaDataSetList.size(); i++)
  {
    framedistance = fabs (time - this->MetaDataSetList[i]->GetTime());
    if (framedistance < distance)
    {
      ret = this->MetaDataSetList[i];
      distance = framedistance;
      id = i;
    }    
  }
  
  return ret;
}

  

//----------------------------------------------------------------------------
double vtkMetaDataSetSequence::GetRelativeTime (double time)
{
  if (this->MetaDataSetList.size() == 0)
    return 0.0;
  
  if (this->SequenceDuration <= 0 )
    return 0;
  double t = time;
  while (t > (this->SequenceDuration + 0.000001) )
  {
    t -= this->SequenceDuration;
  }
  return (t);
  
}



//----------------------------------------------------------------------------
void vtkMetaDataSetSequence::BuildMetaDataSetFromMetaDataSet (vtkMetaDataSet* metadataset)
{

  this->CopyInformation(metadataset);
  this->SetProperty (metadataset->GetProperty());
  
  vtkDataSet* dataset = NULL;

  switch (metadataset->GetDataSet()->GetDataObjectType())
  {
      case VTK_IMAGE_DATA :
	dataset = vtkImageData::New();
	break;
      case VTK_POLY_DATA :
	dataset = vtkPolyData::New();
	break;
      case VTK_UNSTRUCTURED_GRID :
	dataset = vtkUnstructuredGrid::New();
	break;
      default:
	vtkErrorMacro(<<"Unknown type !"<<endl);
	throw vtkErrorCode::UnrecognizedFileTypeError;
  }
  
  
  dataset->DeepCopy (metadataset->GetDataSet());
  try
  {
    this->SetDataSet(dataset);
  }
  catch (vtkErrorCode::ErrorIds &e)
  {
    throw e;
  }

  dataset->Delete();
  
}

//----------------------------------------------------------------------------
double vtkMetaDataSetSequence::GetMinTime() const
{
  double ret = 0;
  for (unsigned int i=0; i<this->MetaDataSetList.size(); i++)
    if ( ret > this->MetaDataSetList[i]->GetTime())
      ret = this->MetaDataSetList[i]->GetTime();
  return ret;
  
}

//----------------------------------------------------------------------------
double vtkMetaDataSetSequence::GetMaxTime() const
{
  double ret = 0;
  for (unsigned int i=0; i<this->MetaDataSetList.size(); i++)
    if ( ret < this->MetaDataSetList[i]->GetTime())
      ret = this->MetaDataSetList[i]->GetTime();
  return ret;
  
}


//----------------------------------------------------------------------------
double vtkMetaDataSetSequence::GetTimeResolution()
{
  if (!this->MetaDataSetList.size())
    return 0;
  
  this->ComputeSequenceDuration();
  
  double ret = this->SequenceDuration / (double)this->GetNumberOfMetaDataSets();
  
  return ret;
}


//----------------------------------------------------------------------------
void vtkMetaDataSetSequence::ComputeSequenceDuration()
{
  if (this->MetaDataSetList.size() < 2)
  {
    this->SequenceDuration = this->GetMaxTime();
    return;
  }
  
  double step = (this->GetMaxTime() - this->GetMinTime())/(double)(this->GetNumberOfMetaDataSets()-1);
  
  this->SequenceDuration = this->GetMaxTime() - this->GetMinTime() + step;
}




//----------------------------------------------------------------------------
void vtkMetaDataSetSequence::UpdateToTime (double time)
{
  unsigned int id = 0;
  if (this->FindMetaDataSet (time, id))
  {
    this->UpdateToIndex (id);
  }
  
  this->SetTime (time);
}

//----------------------------------------------------------------------------
void vtkMetaDataSetSequence::UpdateToIndex (unsigned int id)
{
  if ((int)id == this->CurrentId)
    return;

  if (id >= this->MetaDataSetList.size())
    return;

  vtkDataSet* datasettoshow  = this->MetaDataSetList[id]->GetDataSet();

  vtkPolyData* polydatatoshow   = vtkPolyData::SafeDownCast (datasettoshow);
  vtkPolyData* polydatatochange = vtkPolyData::SafeDownCast (this->GetDataSet());

  if (polydatatoshow && polydatatochange) {
      polydatatochange->ShallowCopy (polydatatoshow);
  } else {
      vtkUnstructuredGrid* unstructuredgridtoshow   = vtkUnstructuredGrid::SafeDownCast (datasettoshow);
      vtkUnstructuredGrid* unstructuredgridtochange = vtkUnstructuredGrid::SafeDownCast (this->GetDataSet());
      if (unstructuredgridtoshow && unstructuredgridtochange) {
          unstructuredgridtochange->ShallowCopy (unstructuredgridtoshow);
      }
  }

  if (this->ParseAttributes)
  {
    vtkDataArray* pdscalars = this->GetDataSet()->GetPointData()->GetScalars();
    vtkDataArray* cdscalars = this->GetDataSet()->GetCellData()->GetScalars();
    vtkDataArray* pdtensors = this->GetDataSet()->GetPointData()->GetTensors();
    vtkDataArray* cdtensors = this->GetDataSet()->GetCellData()->GetTensors();

    if (pdscalars)
    {
      const char* c_name = pdscalars->GetName();
      if ( c_name && (*c_name) )
      {
    std::string name = c_name;
    vtkDataArray* array = datasettoshow->GetPointData()->GetArray(name.c_str());
    if (array)
    {
// 	  pdscalars->SetVoidArray (array);
      pdscalars->DeepCopy (array);
    }
      }
      else
      {
    this->GetDataSet()->GetPointData()->SetScalars(datasettoshow->GetPointData()->GetScalars());
      }

      this->GetDataSet()->GetPointData()->Modified();
      this->GetDataSet()->Modified();
    }

    if (cdscalars)
    {
      const char* c_name = cdscalars->GetName();
      if ( c_name && (*c_name) )
      {
    std::string name = c_name;
    vtkDataArray* array = datasettoshow->GetCellData()->GetArray(name.c_str());

    if (array)
    {
// 	  cdscalars->SetVoidArray (array);
      cdscalars->DeepCopy (array);
    }

      }
      else
      {
    this->GetDataSet()->GetCellData()->SetScalars(datasettoshow->GetCellData()->GetScalars());
      }
      this->GetDataSet()->GetCellData()->Modified();

    }
    if (pdtensors)
    {

      const char* c_name = pdtensors->GetName();
      if ( c_name && (*c_name) )
      {
    std::string name = c_name;
    vtkDataArray* array = datasettoshow->GetPointData()->GetArray(name.c_str());
    if (array)
    {
// 	  pdscalars->SetVoidArray (array);
      pdtensors->DeepCopy (array);
    }
      }
      else
      {
    this->GetDataSet()->GetPointData()->SetTensors(datasettoshow->GetPointData()->GetTensors());
      }

      this->GetDataSet()->GetPointData()->Modified();
    }

    if (cdtensors)
    {
      const char* c_name = cdtensors->GetName();
      if ( c_name && (*c_name) )
      {
    std::string name = c_name;
    vtkDataArray* array = datasettoshow->GetCellData()->GetArray(name.c_str());

    if (array)
    {
// 	  cdscalars->SetVoidArray (array);
      cdtensors->DeepCopy (array);
    }

      }
      else
      {
    this->GetDataSet()->GetCellData()->SetTensors(datasettoshow->GetCellData()->GetTensors());
      }
      this->GetDataSet()->GetCellData()->Modified();

    }

  }

  // index showing how much frames we lost
  // vtkDebugMacro(<<"sequence losts "<< ((int)id-(int)(this->CurrentId)) << "frames");
  // updating the current id
  this->CurrentId   = id;
}




//----------------------------------------------------------------------------
double* vtkMetaDataSetSequence::GetCurrentScalarRange()
{
  static double * val = new double[2];
  val[0] = VTK_DOUBLE_MAX;
  val[1] = VTK_DOUBLE_MIN;
  
  for (unsigned int i=0; i<this->MetaDataSetList.size(); i++)
  {
    double* range = this->MetaDataSetList[i]->GetCurrentScalarRange();
    
    if (val[0] > range[0])
      val[0] = range[0];
    if (val[1] < range[1])
      val[1] = range[1];
  }

  return val;
}


//----------------------------------------------------------------------------
void vtkMetaDataSetSequence::ColorByArray(vtkDataArray* array)
{
  this->CurrentScalarArray = array;

  if (!array)
    return;  
  
  if (!this->MetaDataSetList.size())
    return;

  if (!this->DataSet)
    return;
  
  bool array_is_in_points = false;

  if (this->DataSet->GetPointData()->HasArray (array->GetName()))
      array_is_in_points = true;
  
  double min = 0, max = 0;

  vtkLookupTable* lut = array->GetLookupTable();
  
  for (unsigned int i=0; i<this->MetaDataSetList.size(); i++)
  {
    vtkDataArray* junk;
    vtkDataSetAttributes* attributes;
    
    if (array_is_in_points)
    {
      junk = this->MetaDataSetList[i]->GetDataSet()->GetPointData()->GetArray (array->GetName());
      attributes = this->MetaDataSetList[i]->GetDataSet()->GetPointData();
    }
    else
    {  
      junk = this->MetaDataSetList[i]->GetDataSet()->GetCellData()->GetArray (array->GetName());
      attributes = this->MetaDataSetList[i]->GetDataSet()->GetCellData();
    }
    
    if (!junk)
      continue;
    
    if (min > junk->GetRange()[0])
      min = junk->GetRange()[0];
    if (max < junk->GetRange()[1])
      max = junk->GetRange()[1];

    
    if (lut)
      junk->SetLookupTable (lut);

    attributes->SetActiveScalars(array->GetName());

    this->MetaDataSetList[i]->SetCurrentActiveArray (junk);
  }

  if (lut)
    lut->SetRange (min, max);
  
    
  if (array_is_in_points)
    this->DataSet->GetPointData()->SetActiveScalars (array->GetName());
  else
    this->DataSet->GetCellData()->SetActiveScalars (array->GetName());
    

  for (int i=0; i<this->ActorList->GetNumberOfItems(); i++)
  {
    vtkActor* actor = this->GetActor (i);
    if (!actor)
      continue;
    vtkMapper* mapper = actor->GetMapper();
    
    if (!array_is_in_points)
      mapper->SetScalarModeToUseCellFieldData();
    else
      mapper->SetScalarModeToUsePointFieldData();    
    
    if (lut)
    {
//       mapper->SetLookupTable (lut);
      mapper->UseLookupTableScalarRangeOn();
    }
    
    //mapper->SetScalarRange (min, max);
    mapper->SelectColorArray (array->GetName());
  }

  

  //this->GetDataSet()->Modified();
  
}



//----------------------------------------------------------------------------
void vtkMetaDataSetSequence::SetScalarVisibility(bool val)
{
  this->Superclass::SetScalarVisibility (val);
  this->SetParseAttributes (val);
}


//----------------------------------------------------------------------------
void vtkMetaDataSetSequence::ReadData (const char* filename)
{
  std::ifstream file (filename );

  if(file.fail())
  {
    vtkWarningMacro(<<"File not found"<<endl);
    file.close();
    throw vtkErrorCode::FileNotFoundError;
  }
  
  // read the first line to get the number of keywords
  std::string keywordline = "";
  vtksys::SystemTools::GetLineFromStream(file, keywordline);
  std::vector<std::string> keywordlist;
  std::istringstream is2;
  is2.str (keywordline);
  for (unsigned int i=0; i<20; i++)
  {
    std::string tmp;
    is2 >> tmp;
    if (tmp.size())
      keywordlist.push_back (tmp);
  }

  if (keywordlist.size() > 1)
  {
    // here we are in the case that says that
    // the data we are reading are global parameters
    // for the sequences (ex. : *.cvgpar files)
    file.close();
    this->ReadDataAdvanced (filename);
    return;
  }

  // this line will show us the type
  // of this new attributes
  // 1 corresponds to a point attribute
  // 2 corresponds to a cell (triangles/tetras) attibute
  std::string line = "";
  vtksys::SystemTools::GetLineFromStream(file, line);
  int dataType = -1;
  std::istringstream is1;
  is1.str (line);
  is1 >> dataType;

  if (dataType < 0)
  {
    vtkWarningMacro(<<"wrong type of data, cannot read this file"<<endl);
    file.close();
    return;
  }
  
  unsigned long Nb = 0;
  unsigned int Dim = 0;
  int CycleId = -1;
  int FrameId = -1;
  
  vtksys::SystemTools::GetLineFromStream(file, line);
  std::istringstream is3;
  is3.str (line);
  is3 >> Nb >> Dim >> CycleId >> FrameId;
  
  file.close();

  // if frame id is not provided
  // this means that this file does not correspond to
  // a sequence attribute, we load it as the superclass would load it
  // otherwise this is a frame attribute
  if (FrameId >= 0)
    this->ReadFrameData (filename, FrameId);
  else
    this->Superclass::ReadData (filename);

  // for the first frame, we have to
  // load this attribute in our current dataset
  if (FrameId == 0)
    this->Superclass::ReadData (filename);
}


//----------------------------------------------------------------------------
void vtkMetaDataSetSequence::ReadDataAdvanced (const char* filename)
{
  std::ifstream file (filename );

  if(file.fail())
  {
    vtkWarningMacro(<<"File not found"<<endl);
    file.close();
    throw vtkErrorCode::FileNotFoundError;
  }
  
  // read the first line to get the number of keywords
  std::string keywordline = "";
  vtksys::SystemTools::GetLineFromStream(file, keywordline);
  std::vector<std::string> keywordlist;
  std::istringstream is2;
  is2.str (keywordline);
  for (unsigned int i=0; i<20; i++)
  {
    std::string tmp;
    is2 >> tmp;
    if (tmp.size())
      keywordlist.push_back (tmp);
  }

  vtkDataArrayCollection* ArrayList = vtkDataArrayCollection::New();

  std::cout<<"reading advanced data : ";
  for (unsigned int j=0; j<keywordlist.size(); j++)
  {
    if (!(keywordlist[j].size()))
      continue;
    std::cout<<keywordlist[j].c_str()<<", ";

    vtkDoubleArray* array = vtkDoubleArray::New();
    array->SetName (keywordlist[j].c_str());
    ArrayList->AddItem(array);
    array->Delete();
  }

  
  std::string line = "";
  while(!file.fail())
  {
    bool isvalid = vtksys::SystemTools::GetLineFromStream(file, line);
    if (!isvalid)
      break;
    
    std::istringstream is;
    is.str (line);
    for (unsigned int j=0; j<keywordlist.size(); j++)
    {
      if (!keywordlist.size())
	continue;
      double val = 0.0;
      is >> val;

      vtkDoubleArray* array = vtkDoubleArray::SafeDownCast (ArrayList->GetItemAsObject(j));
      if (array)
	array->InsertNextValue(val);
      
    }
  }
  

  for (int i=0; i<ArrayList->GetNumberOfItems(); i++)
  {
    vtkDoubleArray* array = vtkDoubleArray::SafeDownCast (ArrayList->GetItemAsObject(i));
    if (array)
      this->AddArray (array);
  }
  
  ArrayList->Delete();

  file.close();
  
}




//----------------------------------------------------------------------------
void vtkMetaDataSetSequence::ReadFrameData (const char* filename, unsigned int iter)
{

  if (!this->GetDataSet())
    return;
  
  if (iter >= (unsigned int)this->GetNumberOfMetaDataSets())
  {

    unsigned int NMetaDataSets = this->GetNumberOfMetaDataSets();
    
    for (unsigned int i = this->MetaDataSetList.size(); i<=iter; i++)
    {
      
      vtkMetaDataSet* metadataset = NULL;
      vtkDataSet* dataset = NULL;
      
      switch(this->GetType())
      {
	  case vtkMetaDataSet::VTK_META_SURFACE_MESH:
	    metadataset = vtkMetaSurfaceMesh::New();
	    dataset = vtkPolyData::New();
	    break;
	  case vtkMetaDataSet::VTK_META_VOLUME_MESH:
	    metadataset = vtkMetaVolumeMesh::New();
	    dataset = vtkUnstructuredGrid::New();
	    break;
	  case vtkMetaDataSet::VTK_META_IMAGE_DATA:
	    metadataset = vtkMetaImageData::New();
	    dataset = vtkImageData::New();
	    break;
      }
       
      metadataset->CopyInformation(this);
      metadataset->SetProperty (this->GetProperty());
      
      if (this->GetSameGeometryFlag())
      {
	vtkPolyData* polydata = vtkPolyData::SafeDownCast (dataset);
	vtkPolyData* lastpolydata = vtkPolyData::SafeDownCast (this->GetDataSet());
	if (this->GetNumberOfMetaDataSets())
	  lastpolydata = vtkPolyData::SafeDownCast (this->GetMetaDataSet (NMetaDataSets-1)->GetDataSet());
	
	vtkUnstructuredGrid* unstructuredgrid = vtkUnstructuredGrid::SafeDownCast (dataset);
	vtkUnstructuredGrid* lastunstructuredgrid = vtkUnstructuredGrid::SafeDownCast (this->GetDataSet());
	if (this->GetNumberOfMetaDataSets())
	  lastunstructuredgrid = vtkUnstructuredGrid::SafeDownCast (this->GetMetaDataSet (NMetaDataSets-1)->GetDataSet());
	
	if (polydata && lastpolydata)
	{
	  vtkPoints* points = vtkPoints::New();
	  points->DeepCopy (lastpolydata->GetPoints());
	  polydata->SetPoints (points);
	  polydata->SetPolys (lastpolydata->GetPolys());
	  points->Delete();
	}
	else if (unstructuredgrid && lastunstructuredgrid)
	{
	  vtkPoints* points = vtkPoints::New();
	  points->DeepCopy (lastunstructuredgrid->GetPoints());
	  unstructuredgrid->SetPoints (points);
	  int t = lastunstructuredgrid->GetCellType (0);
	  unstructuredgrid->SetCells(t, lastunstructuredgrid->GetCells());
	  points->Delete();
	}
      }
      else
      {
	dataset->DeepCopy (this->GetDataSet());
      }

      metadataset->SetDataSet(dataset);
      
      std::ostringstream o_name;
      o_name<<this->GetName()<<"("<<iter<<")";
      metadataset->SetName(o_name.str().c_str());
      
      metadataset->SetTime(this->GetMaxTime() + this->GetTimeResolution());
      
      this->AddMetaDataSet (metadataset);
      
      metadataset->Delete();
      dataset->Delete();
    }
  }
  
  try
  {
    this->MetaDataSetList[iter]->ReadData (filename);
  }
  catch (vtkErrorCode::ErrorIds error)
  {
    throw error;
  }

  //this->UpdateToIndex (0);
  
  
}


//----------------------------------------------------------------------------
void vtkMetaDataSetSequence::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
  os << indent << "name \t: " << this->GetName() << endl;
  os << indent << "delay \t: " << this->GetTimeResolution() << endl;
  os << indent << "duration \t: " << this->SequenceDuration << endl;
  os << indent << "type \t: " << this->Type << endl;
  os << indent << "number of items \t: " << this->GetNumberOfMetaDataSets() << endl;
}



//----------------------------------------------------------------------------
vtkDoubleArray* vtkMetaDataSetSequence::GenerateFollowerTimeTable(const char* arrayname, unsigned int idtofollow)
{
  vtkDoubleArray* ret = vtkDoubleArray::New();
  ret->Allocate(this->GetNumberOfMetaDataSets());

  ret->SetName (arrayname);

  //unsigned int hasarray = this->GetNumberOfMetaDataSets();
  unsigned int canfollow = this->GetNumberOfMetaDataSets();
  
  for (int i=0; i<this->GetNumberOfMetaDataSets(); i++)
  {
    double val;
    
    vtkDataArray* array = this->GetMetaDataSet (i)->GetArray (arrayname);
    
    if (!array)
    {
      ret->InsertNextValue (0);
      continue;
    }

    if ((int)idtofollow > array->GetNumberOfTuples())
    {
      canfollow--;
      ret->InsertNextValue (0);
      continue;
    }


    double* temp = array->GetTuple (idtofollow);
    if (temp)
      val = temp[0];
    else
      val = -1;
    
    ret->InsertNextValue (val);
  }

  if ((double)(ret->GetNumberOfTuples()) < (double)(this->GetNumberOfMetaDataSets())/2.0 )
  {
    
    vtkWarningMacro(<<"array "<<arrayname<<" not found in all sequence instances"<<endl);
    ret->Delete();
    return NULL;
  }
  

  return ret;  
}

//----------------------------------------------------------------------------
vtkDoubleArray* vtkMetaDataSetSequence::GenerateMetaDataTimeTable(const char* metadatakey)
{

  vtkDoubleArray* ret = vtkDoubleArray::New();
  ret->Allocate(this->GetNumberOfMetaDataSets());

  ret->SetName (metadatakey);
  
  for (int i=0; i<this->GetNumberOfMetaDataSets(); i++)
  {
    double val = 0.0;
    
    bool isvalid = this->GetMetaDataSet (i)->GetMetaData<double>(metadatakey, val);
    if (!isvalid)
    {
      vtkWarningMacro(<<"metadata "<<metadatakey<<" not found in all sequence frames"<<endl);
      ret->Delete();
      return NULL;
    }
    ret->InsertNextValue (val);
  }

  return ret;
}

  


//----------------------------------------------------------------------------
void vtkMetaDataSetSequence::CopyInformation (vtkMetaDataSet* metadataset)
{
  this->Superclass::CopyInformation(metadataset);
  
  vtkMetaDataSetSequence* sequence = vtkMetaDataSetSequence::SafeDownCast (metadataset);
  
  if (!sequence)
    return;

  this->SameGeometryFlag = sequence->GetSameGeometryFlag();
  this->ParseAttributes = sequence->GetParseAttributes();
  
}




//----------------------------------------------------------------------------
void vtkMetaDataSetSequence::ComputeTimesFromDuration()
{
  for (int i=0; i<this->GetNumberOfMetaDataSets(); i++)
  {
      this->GetMetaDataSet(i)->SetTime((double)i * (double) ( SequenceDuration )/ (double)(this->GetNumberOfMetaDataSets() ) );
  }
}

//----------------------------------------------------------------------------
vtkMetaDataSetSequence::ShortImageType::PointType vtkMetaDataSetSequence::ExtractPARRECImageOrigin (const char* filename, ShortDirectionType direction)
{

  typedef ShortImageType::PointType PointType;
  PointType nullorigin;
  nullorigin[0] = nullorigin[1] = nullorigin[2] = 0.0;

  itk::PhilipsRECImageIO::Pointer philipsIO = itk::PhilipsRECImageIO::New();

  philipsIO->SetFileName(filename);
  try
  {
    philipsIO->ReadImageInformation();
  }
  catch(itk::ExceptionObject &e)
  {
    std::cerr << e;
    throw vtkErrorCode::CannotOpenFileError;
  }

  itk::MetaDataDictionary PARheader = philipsIO->GetMetaDataDictionary();
  
  typedef itk::PhilipsRECImageIO::OffCentreMidSliceType OffCentreType;
  
  OffCentreType offcenter;
  
  bool valid = itk::ExposeMetaData<OffCentreType>(PARheader, "PAR_OffCentreMidSlice", offcenter);
  if (!valid)
  {
    std::cerr<<"cannot find off-center information in PAR header, no correction"<<std::endl;
    return nullorigin;
  }

  double dimensions[3];
  dimensions[0] = philipsIO->GetDimensions (0);
  dimensions[1] = philipsIO->GetDimensions (1);
  dimensions[2] = philipsIO->GetDimensions (2);
  
  ShortImageType::SpacingType midoffset;
  midoffset[0] = philipsIO->GetSpacing (0) * (dimensions[0] - 1) / 2.0;
  midoffset[1] = philipsIO->GetSpacing (1) * (dimensions[1] - 1) / 2.0;
  midoffset[2] = philipsIO->GetSpacing (2) * (dimensions[2] - 1) / 2.0;
  midoffset = direction * midoffset;
  
  PointType offcenterpoint;
  offcenterpoint[0] = offcenter[0];
  offcenterpoint[1] = offcenter[1];
  offcenterpoint[2] = offcenter[2];
  
  ShortDirectionType AFRtoLPS;
  AFRtoLPS.Fill (0);
  AFRtoLPS[0][2] = 1;
  AFRtoLPS[1][0] = 1;
  AFRtoLPS[2][1] = 1;
  
  offcenterpoint = AFRtoLPS * offcenterpoint;
  offcenterpoint -= midoffset;
  
  return offcenterpoint;
}



//----------------------------------------------------------------------------
vtkMetaDataSetSequence::ShortDirectionType vtkMetaDataSetSequence::ExtractPARRECImageOrientation (const char* filename)
{

  typedef ShortDirectionType DirectionType;

  DirectionType eyedir;
  eyedir.SetIdentity();
    
  itk::PhilipsRECImageIO::Pointer philipsIO = itk::PhilipsRECImageIO::New();

  philipsIO->SetFileName(filename);
  try
  {
    philipsIO->ReadImageInformation();
  }
  catch(itk::ExceptionObject &e)
  {
    std::cerr << e;
    throw vtkErrorCode::CannotOpenFileError;
  }

  itk::MetaDataDictionary PARheader = philipsIO->GetMetaDataDictionary();

  typedef itk::PhilipsRECImageIO::AngulationMidSliceType AngulationType;
  
  AngulationType angulation;
  int sliceorientation = 0;
  
  bool valid = itk::ExposeMetaData<AngulationType>(PARheader, "PAR_AngulationMidSlice", angulation);
  if (!valid)
  {
    std::cerr<<"cannot find angulation in PAR header, no correction"<<std::endl;
    return eyedir;
  }
  
  valid = itk::ExposeMetaData<int>(PARheader, "PAR_SliceOrientation", sliceorientation);
  if (!valid)
  {
    std::cerr<<"cannot find slice orientation in PAR header, no correction"<<std::endl;
    return eyedir;
  }
  
  ShortDirectionType AFRtoLPS;
  AFRtoLPS.Fill (0);
  AFRtoLPS[0][2] = 1;
  AFRtoLPS[1][0] = 1;
  AFRtoLPS[2][1] = 1;

  ShortDirectionType magicmatrix;
  magicmatrix.Fill (0);
  magicmatrix [0][0] = -1;
  magicmatrix [1][2] = 1;
  magicmatrix [2][1] = -1;

  ShortDirectionType TRA;
  TRA.Fill (0);
  TRA [0][1] = 1;
  TRA [1][0] = -1;
  TRA [2][2] = -1;
  ShortDirectionType SAG;
  SAG.Fill (0);
  SAG [0][0] = -1;
  SAG [1][2] = 1;
  SAG [2][1] = -1;
  ShortDirectionType COR;
  COR.Fill (0);
  COR [0][1] = 1;
  COR [1][2] = 1;
  COR [2][0] = -1;

  ShortDirectionType Torientation;
  
  switch(sliceorientation)
  {
    
    case PAR_SLICE_ORIENTATION_TRANSVERSAL: 
      // Transverse - the REC data appears to be stored as right-left, 
      // anterior-posterior, and inferior-superior.
      // Verified using a marker on right side of brain.
      Torientation = TRA;      
      break;
    case PAR_SLICE_ORIENTATION_SAGITTAL: 
      // Sagittal - the REC data appears to be stored as anterior-posterior, 
      // superior-inferior, and right-left.
      // Verified using marker on right side of brain.
      Torientation = SAG;
      break;
    case PAR_SLICE_ORIENTATION_CORONAL: 
      // Coronal - the REC data appears to be stored as right-left, 
      // superior-inferior, and anterior-posterior.
      // Verified using marker on right side of brain.
      // fall thru
    default:
      Torientation = COR;
  
  }


  double ap = angulation[0] * vnl_math::pi / 180.0;
  double fh = angulation[1] * vnl_math::pi / 180.0;
  double rl = angulation[2] * vnl_math::pi / 180.0;

  ShortDirectionType Tap;
  Tap.Fill (0);
  Tap[0][0] = 1;
  Tap[1][1] = std::cos (ap);
  Tap[1][2] = - std::sin (ap);
  Tap[2][1] = std::sin (ap);
  Tap[2][2] = std::cos (ap);

  ShortDirectionType Tfh;
  Tfh.Fill (0);
  Tfh[1][1] = 1;
  Tfh[0][0] = std::cos (fh);
  Tfh[0][2] = std::sin (fh);
  Tfh[2][0] = - std::sin (fh);
  Tfh[2][2] = std::cos (fh);

  ShortDirectionType Trl;
  Trl.Fill (0);
  Trl[2][2] = 1;
  Trl[0][0] = std::cos (rl);
  Trl[0][1] = - std::sin (rl);
  Trl[1][0] = std::sin (rl);
  Trl[1][1] = std::cos (rl);
  
  ShortDirectionType TR = AFRtoLPS * Trl * Tap * Tfh * magicmatrix.GetTranspose() * Torientation.GetTranspose();
  DirectionType retval;
  retval.SetIdentity();
  
  for (unsigned int i=0; i<3; i++)
    for (unsigned int j=0; j<3; j++)
      retval[i][j] = TR[i][j];

  return retval;
}


/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <vtkMetaImageData.h>
#include "vtkObjectFactory.h"


#include <vtkImageData.h>
#include <vtkImageCast.h>


//----------------------------------------------------------------------------
vtkStandardNewMacro( vtkMetaImageData );
vtkCxxRevisionMacro(vtkMetaImageData, "$Revision: 1370 $");


#include <itkMatrix.h>
#include <itkImage.h>

#include <itkPhilipsRECImageIO.h>
#include <itkPhilipsPAR.h>

#include <vtkPNGReader.h>
#include <vtkJPEGReader.h>
#include <vtkTIFFReader.h>
#include <vtkMatrix4x4.h>
  

//----------------------------------------------------------------------------
vtkMetaImageData::vtkMetaImageData()
{
  this->m_ItkImage  = 0;
  this->Type = vtkMetaDataSet::VTK_META_IMAGE_DATA;

  this->OrientationMatrix = vtkMatrix4x4::New();
  this->OrientationMatrix->Identity();
  
}

//----------------------------------------------------------------------------
vtkMetaImageData::~vtkMetaImageData()
{
  this->OrientationMatrix->Delete();
}

//----------------------------------------------------------------------------
void vtkMetaImageData::SetDataSet(vtkDataSet* dataset)
{
  if (!vtkImageData::SafeDownCast(dataset))
    vtkWarningMacro(<<"data type is not image"<<endl);
  this->Superclass::SetDataSet (dataset);
}

//----------------------------------------------------------------------------
vtkImageData* vtkMetaImageData::GetImageData() const
{
  if (!this->DataSet)
    return NULL;
  return vtkImageData::SafeDownCast (this->DataSet);
}

//----------------------------------------------------------------------------
bool vtkMetaImageData::IsColorExtension (const char* ext)
{
  if (strcmp (ext, ".png") == 0 ||
      strcmp (ext, ".jpg") == 0 ||
      strcmp (ext, ".tiff") == 0)
    return true;
  return false;
}


//----------------------------------------------------------------------------
bool vtkMetaImageData::IsImageExtension (const char* ext)
{
  if (strcmp (ext, ".hdr") == 0 ||
      strcmp (ext, ".gipl") == 0 ||
      strcmp (ext, ".gipl.gz") == 0 ||
      strcmp (ext, ".mha") == 0 ||
      strcmp (ext, ".mhd") == 0 ||
      strcmp (ext, ".vtk") == 0 ||
      strcmp (ext, ".nrrd") == 0 ||
      strcmp (ext, ".nhdr") == 0 ||
      strcmp (ext, ".nii") == 0 ||
      strcmp (ext, ".nii.gz") == 0 ||
      strcmp (ext, ".png") == 0 ||
      strcmp (ext, ".jpg") == 0 ||
      strcmp (ext, ".tiff") == 0 ||
      strcmp (ext, ".inr") == 0 ||
      strcmp (ext, ".inr.gz") == 0      
      )
    return true;
  return false;
}

//----------------------------------------------------------------------------
unsigned int vtkMetaImageData::CanReadFile (const char* filename)
{
  
  /*
    As their might be more than one volume in an image, we read 4D
    images, and then split them into 3D volumes if the 4th dimension
    is greater than 1.
  */
  itk::ImageFileReader<FloatImage4DType>::Pointer reader4d 
    = itk::ImageFileReader<FloatImage4DType>::New();
  reader4d->SetFileName (filename);
  //reader4d->GetImageIO()->SetFileName (filename);
  
  try
  {
    
    reader4d->UpdateOutputInformation();
  }
  catch (itk::ExceptionObject &e)
  {
    std::cerr << e;
    return 0;
  }  
  
  // here we look for the 4th dimension:
  FloatImage4DType::Pointer image4d = reader4d->GetOutput();
  FloatImage4DType::SizeType size = image4d->GetLargestPossibleRegion().GetSize();
  unsigned int nVolumes = size [3];
  
  if( nVolumes!=1 )
  {
    std::cerr<<"Careful, there is more than one image in this file "<<filename<<std::endl;
    return 0;
  }

  return 1;
}


unsigned long vtkMetaImageData::GetComponentType()
{
  if (!this->GetImageData())
    return 0;
  
  return this->GetImageData()->GetScalarType();
}




itk::ImageBase<3>* vtkMetaImageData::GetItkImage()
{
  if (!this->GetImageData())
  {
    return NULL;
  }

  if (m_ItkImage.IsNull())
  {
    unsigned int componenttype = this->GetComponentType();
  
    if (componenttype == VTK_UNSIGNED_CHAR)
      this->ConvertImage<unsigned char>(this->GetImageData(), this->m_ItkImage, this->m_Converter); 
    else if (componenttype == VTK_CHAR)
      this->ConvertImage<char>(this->GetImageData(), this->m_ItkImage, this->m_Converter); 
    else if (componenttype == VTK_UNSIGNED_SHORT)
      this->ConvertImage<unsigned short>(this->GetImageData(), this->m_ItkImage, this->m_Converter); 
    else if (componenttype == VTK_SHORT)
      this->ConvertImage<short>(this->GetImageData(), this->m_ItkImage, this->m_Converter); 
    else if (componenttype == VTK_UNSIGNED_INT)
      this->ConvertImage<unsigned int>(this->GetImageData(), this->m_ItkImage, this->m_Converter);
    else if (componenttype == VTK_INT)
      this->ConvertImage<int>(this->GetImageData(), this->m_ItkImage, this->m_Converter); 
    else if (componenttype == VTK_UNSIGNED_LONG)
      this->ConvertImage<unsigned long>(this->GetImageData(), this->m_ItkImage, this->m_Converter); 
    else if (componenttype == VTK_LONG)
      this->ConvertImage<long>(this->GetImageData(), this->m_ItkImage, this->m_Converter); 
    else if (componenttype == VTK_FLOAT)
      this->ConvertImage<float>(this->GetImageData(), this->m_ItkImage, this->m_Converter); 
    else if (componenttype == VTK_DOUBLE)
      this->ConvertImage<double>(this->GetImageData(), this->m_ItkImage, this->m_Converter); 

    // However by doing so, we don't feed the Direction of the itk::Image (not in vtk-image)
    // therefore we copy it from the OrientationMatrix field :
    vtkMatrix4x4* matrix = this->GetOrientationMatrix();
    DirectionType direction;
    for (unsigned int x=0; x<3; x++) for (unsigned int y=0; y<3; y++) direction[x][y] = matrix->GetElement (x,y);
    this->m_ItkImage->SetDirection (direction);
  }

  return this->m_ItkImage;
}


void vtkMetaImageData::CopyInformation (vtkMetaDataSet* metadataset)
{
  this->Superclass::CopyInformation(metadataset);
  vtkMetaImageData* imagedata = vtkMetaImageData::SafeDownCast (metadataset);
  
  if (!imagedata)
    return;
  
  this->SetDicomDictionary (imagedata->GetDicomDictionary());
  this->SetOrientationMatrix (imagedata->GetOrientationMatrix());
}






//----------------------------------------------------------------------------
void vtkMetaImageData::ReadColorImage (const char* filename)
{
  std::string ext = vtksys::SystemTools::GetFilenameLastExtension(filename).c_str();

  vtkPNGReader* reader1 = vtkPNGReader::New();

  if (reader1->CanReadFile (filename))
  {
    reader1->SetFileName (filename);
    reader1->Update();
    this->SetDataSet (reader1->GetOutput());
    reader1->Delete();
    return;
  }

  reader1->Delete();
  
  vtkJPEGReader* reader2 = vtkJPEGReader::New();

  if (reader2->CanReadFile (filename))
  {

    reader2->SetFileName (filename);
    reader2->Update();
    this->SetDataSet (reader2->GetOutput());
    reader2->Delete();
    return;
  }

  reader2->Delete();
  
  vtkTIFFReader* reader3 = vtkTIFFReader::New();

  if (reader3->CanReadFile (filename))
  {

    reader3->SetFileName (filename);
    reader3->Update();
    this->SetDataSet (reader3->GetOutput());
    reader3->Delete();
    return;
  }

  reader3->Delete();
  
  throw vtkErrorCode::CannotOpenFileError;
}




//----------------------------------------------------------------------------
void vtkMetaImageData::Read (const char* filename)
{

  // first read the image component type
  typedef itk::Image<short,3> d_ImageType;
  typedef itk::ImageFileReader<d_ImageType> d_ReaderType;
  d_ReaderType::Pointer d_reader = d_ReaderType::New();
  d_reader->SetFileName(filename);
  try
  {
    d_reader->UpdateOutputInformation();
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
    vtkErrorMacro (<<"Unknown component type !"<<endl);
    throw vtkErrorCode::CannotOpenFileError;
  }

  if ( (d_reader->GetImageIO()->GetPixelType() == itk::ImageIOBase::RGB) ||
       (d_reader->GetImageIO()->GetPixelType() == itk::ImageIOBase::RGBA) )
  {
    try
    {
      this->ReadColorImage(filename);
    }
    catch(vtkErrorCode::ErrorIds e)
    {
      throw e;
    }
  }
  else
  {
    
    
    if(d_reader->GetImageIO()->GetPixelType() != itk::ImageIOBase::SCALAR)
    {
      vtkWarningMacro (<<"The image type is not scalar, "<<endl<<"restricting reading to the first component !"<<endl);
    }
    
    // finally read the image : try all types
    if (componenttype == itk::ImageIOBase::UCHAR)
      this->ReadFile<unsigned char>(filename);  
    else if (componenttype ==  itk::ImageIOBase::CHAR)
      this->ReadFile<char>(filename);
    else if (componenttype ==  itk::ImageIOBase::USHORT)
      this->ReadFile<unsigned short>(filename);
    else if (componenttype ==  itk::ImageIOBase::SHORT)
      this->ReadFile<short>(filename);
    else if (componenttype ==  itk::ImageIOBase::UINT)
      this->ReadFile<unsigned int>(filename);
    else if (componenttype ==  itk::ImageIOBase::INT)
      this->ReadFile<int>(filename);
    else if (componenttype ==  itk::ImageIOBase::ULONG)
      this->ReadFile<unsigned long>(filename);
    else if (componenttype ==  itk::ImageIOBase::LONG)
      this->ReadFile<long>(filename);
    else if (componenttype ==  itk::ImageIOBase::FLOAT)
      this->ReadFile<float>(filename);
    else if (componenttype ==  itk::ImageIOBase::DOUBLE)
      this->ReadFile<double>(filename);
  }
    
  // remember the path of the file
  this->SetFilePath (filename);
  
}

//----------------------------------------------------------------------------
void vtkMetaImageData::Write (const char* filename)
{

  // check if there is actually any image to save
  if (!this->GetImageData())
  {
    std::cerr << "no image in metadataset !" <<std::endl;
    throw vtkErrorCode::UserError;
  }

  //   this->WriteFile<unsigned char>(filename);
  //   return;
  
  unsigned int componenttype = this->GetComponentType();
  
  if (componenttype == VTK_UNSIGNED_CHAR)
    this->WriteFile<unsigned char>(filename); 
  else if (componenttype == VTK_CHAR)
    this->WriteFile<char>(filename); 
  else if (componenttype == VTK_UNSIGNED_SHORT)
    this->WriteFile<unsigned short>(filename); 
  else if (componenttype == VTK_SHORT)
    this->WriteFile<short>(filename); 
  else if (componenttype == VTK_UNSIGNED_INT)
    this->WriteFile<unsigned int>(filename); 
  else if (componenttype == VTK_INT)
    this->WriteFile<int>(filename); 
  else if (componenttype == VTK_UNSIGNED_LONG)
    this->WriteFile<unsigned long>(filename); 
  else if (componenttype == VTK_LONG)
    this->WriteFile<long>(filename); 
  else if (componenttype == VTK_FLOAT)
    this->WriteFile<float>(filename); 
  else if (componenttype == VTK_DOUBLE)
    this->WriteFile<double>(filename); 
  else
    this->WriteFile<float>(filename);

  // remember the path of the file
  this->SetFilePath (filename);
  
}

//----------------------------------------------------------------------------
void vtkMetaImageData::SetOrientationMatrix (vtkMatrix4x4* matrix)
{
  if (this->OrientationMatrix == matrix)
  {
    return;
  }
  if (this->OrientationMatrix)
  {
    this->OrientationMatrix->UnRegister(this);
  }
  this->OrientationMatrix = matrix;
  
  if (this->OrientationMatrix)
  {
    this->OrientationMatrix->Register(this);
  }

  this->Modified();

}


//----------------------------------------------------------------------------
vtkMetaImageData::FloatImageType::PointType vtkMetaImageData::ExtractPARRECImageOrigin (const char* filename, DirectionType direction)
{

  typedef FloatImageType::PointType PointType;
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
  
  FloatImageType::SpacingType midoffset;
  midoffset[0] = philipsIO->GetSpacing (0) * (dimensions[0] - 1) / 2.0;
  midoffset[1] = philipsIO->GetSpacing (1) * (dimensions[1] - 1) / 2.0;
  midoffset[2] = philipsIO->GetSpacing (2) * (dimensions[2] - 1) / 2.0;
  midoffset = direction * midoffset;
  
  PointType offcenterpoint;
  offcenterpoint[0] = offcenter[0];
  offcenterpoint[1] = offcenter[1];
  offcenterpoint[2] = offcenter[2];
  
  DirectionType AFRtoLPS;
  AFRtoLPS.Fill (0);
  AFRtoLPS[0][2] = 1;
  AFRtoLPS[1][0] = 1;
  AFRtoLPS[2][1] = 1;
  
  offcenterpoint = AFRtoLPS * offcenterpoint;
  offcenterpoint -= midoffset;
  
  return offcenterpoint;
}



//----------------------------------------------------------------------------
vtkMetaImageData::DirectionType vtkMetaImageData::ExtractPARRECImageOrientation (const char* filename)
{

  typedef DirectionType DirectionType;

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
  
  DirectionType AFRtoLPS;
  AFRtoLPS.Fill (0);
  AFRtoLPS[0][2] = 1;
  AFRtoLPS[1][0] = 1;
  AFRtoLPS[2][1] = 1;

  DirectionType magicmatrix;
  magicmatrix.Fill (0);
  magicmatrix [0][0] = -1;
  magicmatrix [1][2] = 1;
  magicmatrix [2][1] = -1;

  DirectionType TRA;
  TRA.Fill (0);
  TRA [0][1] = 1;
  TRA [1][0] = -1;
  TRA [2][2] = -1;
  DirectionType SAG;
  SAG.Fill (0);
  SAG [0][0] = -1;
  SAG [1][2] = 1;
  SAG [2][1] = -1;
  DirectionType COR;
  COR.Fill (0);
  COR [0][1] = 1;
  COR [1][2] = 1;
  COR [2][0] = -1;

  
  DirectionType Torientation;
  
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

  DirectionType Tap;
  Tap.Fill (0);
  Tap[0][0] = 1;
  Tap[1][1] = std::cos (ap);
  Tap[1][2] = - std::sin (ap);
  Tap[2][1] = std::sin (ap);
  Tap[2][2] = std::cos (ap);

  DirectionType Tfh;
  Tfh.Fill (0);
  Tfh[1][1] = 1;
  Tfh[0][0] = std::cos (fh);
  Tfh[0][2] = std::sin (fh);
  Tfh[2][0] = - std::sin (fh);
  Tfh[2][2] = std::cos (fh);

  DirectionType Trl;
  Trl.Fill (0);
  Trl[2][2] = 1;
  Trl[0][0] = std::cos (rl);
  Trl[0][1] = - std::sin (rl);
  Trl[1][0] = std::sin (rl);
  Trl[1][1] = std::cos (rl);
  
  DirectionType TR = AFRtoLPS * Trl * Tap * Tfh * magicmatrix.GetTranspose() * Torientation.GetTranspose();
  DirectionType retval;
  retval.SetIdentity();
  
  for (unsigned int i=0; i<3; i++)
    for (unsigned int j=0; j<3; j++)
      retval[i][j] = TR[i][j];

  return retval;
}


/*=========================================================================

Program:   vtkINRIA3D
Module:    $Id: vtkMetaImageData.cxx 1370 2009-12-01 11:59:17Z ntoussaint $
Language:  C++
Author:    $Author: ntoussaint $
Date:      $Date: 2009-12-01 12:59:17 +0100 (Tue, 01 Dec 2009) $
Version:   $Revision: 1370 $

Copyright (c) 2007 INRIA - Asclepios Project. All rights reserved.
See Copyright.txt for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include <vtkMetaImageData.h>
#include "vtkObjectFactory.h"


#include <vtkImageData.h>
#include <vtkImageCast.h>


//----------------------------------------------------------------------------
vtkStandardNewMacro( vtkMetaImageData );
vtkCxxRevisionMacro(vtkMetaImageData, "$Revision: 1370 $");


#ifdef vtkINRIA3D_USE_ITK
#include <itkMatrix.h>
#include <itkImage.h>
#include <itkDicomTagManager.h>
#endif

#ifdef ITK_USE_REVIEW
#include <itkPhilipsRECImageIO.h>
#include <itkPhilipsPAR.h>
#endif



#include <vtkPNGReader.h>
#include <vtkJPEGReader.h>
#include <vtkTIFFReader.h>
#include <vtkMatrix4x4.h>
  

//----------------------------------------------------------------------------
vtkMetaImageData::vtkMetaImageData()
{
#ifdef vtkINRIA3D_USE_ITK
  this->ComponentType = itk::ImageIOBase::UNKNOWNCOMPONENTTYPE;
  m_ItkImage = NULL;
  m_ItkConverter = ItkConverterType::New();
#endif
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
void vtkMetaImageData::Initialize (void)
{
  //nothing to do
  this->Superclass::LinkFilters();
  this->LinkFilters();
  
}

//----------------------------------------------------------------------------
void vtkMetaImageData::SetDataSet(vtkDataSet* dataset)
{
  // TO DO
  // THINK OF A GOOD STRATEGY
  // CONCERNING PIXEL-TYPE HANDLING 
  vtkImageData* image = vtkImageData::SafeDownCast(dataset);

  if (!image || !(image->GetScalarType() == VTK_FLOAT))
  {
    // this is OK now as we can handle any type of scalar !!
    //vtkWarningMacro(<<"this pixel type is not float, \nand might induce some errors in further process"<<endl);
  }
  this->Superclass::SetDataSet (dataset);
  
      
  
}

  

//----------------------------------------------------------------------------
vtkImageData* vtkMetaImageData::GetImageData (void) const
{
  if (!this->DataSet)
    return NULL;
  return vtkImageData::SafeDownCast (this->DataSet);
}

// //----------------------------------------------------------------------------
// void vtkMetaImageData::Read (const char* filename)
// {

// #ifndef vtkINRIA3D_USE_ITK
//   vtkErrorMacro(<<"cannot read file without ITK"<<endl);
//   throw vtkErrorCode::UserError;
// #else

//   /*
//     we assume here that the user did called "CanReadFile" before entering
//     this prevent us from running the 4F image test once more.
//   */

//   // Read image in float type
//   typedef FloatImageType RealImageType;
//   typedef itk::ImageFileReader<RealImageType> RealReaderType;		
// //   typedef itk::ImageFileReader<itk::Image<unsigned char, 3> > RealReaderType;		
//   RealReaderType::Pointer reader = RealReaderType::New();		
//   reader->SetFileName(filename);					
//   try
//   {
//     std::cout<<"reading "<<filename<<"... ";			
//     reader->Update();
//     std::cout<<"done."<<std::endl;
//   }
//   catch(itk::ExceptionObject &e)  {
//     std::cerr << e;
//     vtkErrorMacro(<<"cannot read file : "<<filename<<endl);
//     throw vtkErrorCode::CannotOpenFileError;
//   }
//   try
//   {
//     this->SetItkImage<unsigned char>(reader->GetOutput());    
// //     this->SetDataSetAsItkImage(reader->GetOutput());    
//   }
//   catch (vtkErrorCode::ErrorIds)
//   {
//     throw vtkErrorCode::UserError;
//   }
  
//   this->LinkFilters();

//   // remember the path of the file
//   this->SetFilePath ((vtksys::SystemTools::GetFilenamePath (filename)).c_str());
// #endif
// }


// //----------------------------------------------------------------------------
// void vtkMetaImageData::Write (const char* filename)
// {

// #ifndef vtkINRIA3D_USE_ITK
//   vtkErrorMacro(<<"cannot write file without ITK"<<endl);
//   throw vtkErrorCode::UserError;
// #else

//   if (this->GetItkImage().IsNull())
//   {
//     vtkErrorMacro(<<"No data to save");
//     throw vtkErrorCode::UserError;
//   }

//   std::string realname = filename;
  
//   std::string ext = vtksys::SystemTools::GetFilenameLastExtension(filename);
//   if (!ext.size())
//     realname += ".hdr";
  
//   // ITK stuff to be able to write analyze format
  
//   itk::Matrix<double,3,3> cosines;
//   cosines[0][0]= 1;
//   cosines[0][1]= 0;
//   cosines[0][2]= 0;
//   cosines[1][0]= 0;
//   cosines[1][1]=-1;
//   cosines[1][2]= 0;
//   cosines[2][0]= 0;
//   cosines[2][1]= 0;
//   cosines[2][2]= 1;
//   this->GetItkImage()->SetDirection(cosines);
  
//   typedef FloatImageType RealImageType;
//   typedef itk::ImageFileWriter<RealImageType> RealWriterType;
//   RealWriterType::Pointer writer = RealWriterType::New();
//   writer->SetFileName(realname.c_str());
//   writer->SetInput(this->GetItkImage());

//   try
//   {
//     std::cout<<"writing "<<realname.c_str()<<"... ";
//     writer->Write();
//     std::cout<<"done."<<std::endl;
//   }
//   catch(itk::ExceptionObject &e)
//   {
//     std::cerr << e;
//     vtkErrorMacro(<<"cannot write file : "<<realname.c_str()<<endl);
//     throw vtkErrorCode::UserError;
//   }
  

//   // remember the path of the file
//   this->SetFilePath ((vtksys::SystemTools::GetFilenamePath (realname.c_str())).c_str());
// #endif
// }


void vtkMetaImageData::LinkFilters (void)
{
  this->Superclass::LinkFilters();
  
#ifdef vtkINRIA3D_USE_ITK

  vtkImageData* c_image = this->GetImageData();
  if (c_image)
  {
    m_ItkConverter->SetInput (c_image);
  }
#endif
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
  
#ifndef vtkINRIA3D_USE_ITK
  std::cerr<<"cannot write file without ITK"<<std::endl;
  
  return 0;
#else

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
    
    reader4d->GenerateOutputInformation();
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

#endif  

  return 0;
}

  
#ifdef vtkINRIA3D_USE_ITK
void vtkMetaImageData::SetDataSetAsItkImage(vtkMetaImageData::FloatImageType::Pointer image)
{


  if (image.IsNull())
    return;

  
  typedef itk::ImageToVTKImageFilter<FloatImageType> ConverterType;
  ConverterType::Pointer converter = ConverterType::New();  
  converter->SetInput (image);

  try
  {
    converter->Update();
  }
  catch(itk::ExceptionObject &e)
  {
    std::cerr << e;
    vtkErrorMacro(<<"error when linking filters"<<endl);
    throw vtkErrorCode::UserError;
  }

  // we have to copy the image
  vtkImageData* input = vtkImageData::New();
  input->DeepCopy(converter->GetOutput());  
  this->SetDataSet (input);
  input->Delete();
  this->LinkFilters();

  this->m_ItkImage = image;

  
}

vtkMetaImageData::FloatImageType::Pointer vtkMetaImageData::GetItkImage()
{
  if (!this->GetDataSet())
  {
    return NULL;
  }

  if (m_ItkImage.IsNull())
  {
    
    vtkImageCast* caster = vtkImageCast::New();
    caster->SetOutputScalarTypeToFloat ();
    caster->SetInput (this->GetImageData());
    caster->Update();

    this->m_ItkConverter->SetInput (caster->GetOutput());
    
    try
    {
      this->m_ItkConverter->Update();
    }
    catch(itk::ExceptionObject &e)
    {
      std::cerr << e;
      vtkErrorMacro(<<"error when converting"<<endl);
      caster->Delete();
      return NULL;
    }

    this->m_ItkImage = this->m_ItkConverter->GetOutput();
    caster->Delete();
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

#endif  




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

#ifndef vtkINRIA3D_USE_ITK
  std::cerr<<"ITK not used : cannot read any image..."<<std::endl;
  return;

#else

  // first read the image component type
  typedef itk::Image<short,3> d_ImageType;
  typedef itk::ImageFileReader<d_ImageType> d_ReaderType;
  d_ReaderType::Pointer d_reader = d_ReaderType::New();
  d_reader->SetFileName(filename);
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
  
  
#endif
  
  // remember the path of the file
  this->SetFilePath (filename);
  
}

//----------------------------------------------------------------------------
void vtkMetaImageData::Write (const char* filename)
{

#ifndef vtkINRIA3D_USE_ITK
  std::cout<<"ITK not used : cannot write any image..."<<std::endl;
  throw vtkErrorCode::UserError;
#else

  // check if there is actually any image to save
  if (!this->GetImageData())
  {
    std::cerr << "no image in metadataset !" <<std::endl;
    throw vtkErrorCode::UserError;
  }

//   this->WriteFile<unsigned char>(filename);
//   return;
  

  
  unsigned int componenttype = this->GetImageData()->GetScalarType();

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

  
#endif
  
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





#ifdef vtkINRIA3D_USE_ITK


//----------------------------------------------------------------------------
vtkMetaImageData::ShortDirectionType vtkMetaImageData::ExtractPARRECImageOrientation (const char* filename)
{

  ShortDirectionType eyedir;
  eyedir.SetIdentity();

#ifndef ITK_USE_REVIEW
  std::cerr<<"cannot correct for PAR-REC angulation without ITK_USE_REVIEW to ON"<<std::endl;
  
  return eyedir;

  
#else
  
  typedef itk::Image<short,3> ImageType;
  typedef itk::ImageFileReader<ImageType> ReaderType;
  typedef ImageType::DirectionType DirectionType;
  
  ReaderType::Pointer reader = ReaderType::New();
  itk::PhilipsRECImageIO::Pointer philipsIO = itk::PhilipsRECImageIO::New();

  reader->SetImageIO (philipsIO);  
  reader->SetFileName(filename);

  try
  {
    reader->GenerateOutputInformation();
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

  return TR;

  
#endif
  
  
}

#endif

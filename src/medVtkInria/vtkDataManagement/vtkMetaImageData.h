/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <vtkErrorCode.h>
#include <vtksys/SystemTools.hxx>
#include "medVtkInriaExport.h"
#include <vtkMetaDataSet.h>
#include <vtkMatrix4x4.h>

// ITK includes for I/O only
#include <itkImageFileReader.h>
#include <itkImageFileWriter.h>
#include <itkImageToVTKImageFilter.h>
#include <itkVTKImageToImageFilter.h>
#include <itkImage.h>
#include <itkCastImageFilter.h>

/**
   The origin in ITK pipeline is taken into account in a different
   way than in the VTK equivalent.
   A first hack would be to force the vtkImageData instance to have
   a null origin, and put the ITK origin in the 4th column of the
   OrientationMatrix instance. BUT, when the ITK pipeline is updated,
   then the ITK origin is put back in place in the vtkImageData instance.
   
   Therefore we need to keep the vtkImageData's origin the same as the
   ITK one. And, we need to correct for this misbehaviour through a hack
   in the OrientationMatrix 4th column, a sort of corrected origin.
*/
#define MEDVTKINRIA_CORRECT_IMAGE_ORIENTATION 1


class vtkImageData;
class vtkVolumeProperty;

/**
   \class vtkMetaImageData vtkMetaImageData.h "vtkMetaImageData.h"
   \brief Concrete implementation of vtkMetaDataSet for Image handling
   \author Nicolas Toussaint
   
   This class is a powerfull vtk Addon class that helps handling a vtkDataSet.
   You can use it as a tool for handling an image, modify it, convert it into itkImage, etc
   Use the Read function to read any type of itk and vtk image file formats.
   Use the Write function to write it into a previous formats file.
   And you can associate to the metaimage a specific vtkVolumeProperty

   A DICOM dictionary is provided to carry DICOM information
   
   
   Note : This class will be able to read and write files only if ITK is used
   
   
   \see
   vtkMetaDataSet vtkMetaSurfaceMesh vtkMetaVolumeMesh
   vtkMetaDataSetSequence vtkDataManager 
*/



class MEDVTKINRIA_EXPORT vtkMetaImageData: public vtkMetaDataSet
{
 public:


  
  static vtkMetaImageData* New();
  vtkTypeRevisionMacro(vtkMetaImageData,vtkMetaDataSet);

  vtkGetObjectMacro (OrientationMatrix, vtkMatrix4x4);
  virtual void SetOrientationMatrix (vtkMatrix4x4* matrix);
  
  

  //BTX
  typedef float ImageComponentType;
  typedef itk::Image<ImageComponentType, 3> FloatImageType;
  typedef itk::Image<ImageComponentType, 4> FloatImage4DType;
  typedef itk::MetaDataDictionary DictionaryType;
  typedef std::pair <std::string, std::string> DicomEntry;
  typedef std::vector<DicomEntry> DicomEntryList;

  typedef FloatImageType::DirectionType DirectionType;
  
  
  /**
     Access directly to the itk image in float scalar component type
     For other use please refer to templated method GetItkImage()
  */
  itk::ImageBase<3>* GetItkImage();
  /**
     Access to the DICOM dictionary of this image
     Note that this dictionary is not filled automatically when ITK image is set.
     You have to explicitally call SetDicomDictionary() for that
  */
  DictionaryType GetDicomDictionary()
  { return this->DicomDictionary; }
  /**
     Set the DICOM dictionary of this image.
     Use this method after a DICOM import process.
     If you use itkGDCMImporter, the output volumes contain the right dictionary
     that could be used to feed this metaimagedata
  */
  void SetDicomDictionary (DictionaryType dictionary)
  { this->DicomDictionary = dictionary; }
  /**
     Access to the DICOM tag list of this image
     Note that this dictionary is not filled automatically when ITK image is set.
     You have to explicitally call SetDicomDictionary() for that
  */
  DicomEntryList GetDicomEntryList()
  { return this->DicomTagList; }
  /**
     Set the DICOM tag list of this image.
     Use this method after a DICOM import process.
     If you use itkGDCMImporter, the output volumes contain the right dictionary
     that could be used to feed this metaimagedata
  */
  void SetDicomEntryList (DicomEntryList list)
  { this->DicomTagList = list; }
  /**
     Reads a file and creates a image of a given scalar component type.
     Use with care. Please prefer using Read()
  */
  template <typename type> inline void ConvertImage (vtkImageData* vtkimage,
						     itk::ImageBase<3>::Pointer& itkimage,
						     itk::ProcessObject::Pointer& itkconverter)
  {
    typedef typename itk::Image<type, 3> ImageType;
    typedef typename itk::VTKImageToImageFilter<ImageType> ConverterType;

    typename ConverterType::Pointer converter = ConverterType::New();
    converter->SetInput (vtkimage);
    try
    {
      converter->Update();
    }
    catch(itk::ExceptionObject &e)
    {
      std::cerr << e;
      vtkErrorMacro(<<"cannot convert image "<<endl);
    }

    itkconverter = converter;
    itkimage = converter->GetOutput();

  }
  
  /**
     Sets the dataset as an ITK image.
     template type is the image scalar component type (i.e. unsigned char, float, etc)
     This method converts the itk image into a corresponding type of vtkImageData
     so that there is no unusefull memory stored.
  */
  template <typename type> inline void SetItkImage (typename itk::Image<type, 3>::Pointer input)
  {
    typedef typename itk::Image<type, 3> ImageType;

    typename ImageType::DirectionType direction = input->GetDirection();
    typedef typename itk::Point<typename ImageType::DirectionType::ValueType, ImageType::ImageDimension> PointType;
    PointType origin = input->GetOrigin();
    
    vtkMatrix4x4* matrix = vtkMatrix4x4::New();
    matrix->Identity();
    for (unsigned int x=0; x<3; x++)
      for (unsigned int y=0; y<3; y++)
	matrix->SetElement (x,y,direction[x][y]);

#if MEDVTKINRIA_CORRECT_IMAGE_ORIENTATION
    /**
       The origin in ITK pipeline is taken into account in a different
       way than in the VTK equivalent.
       A first hack would be to force the vtkImageData instance to have
       a null origin, and put the ITK origin in the 4th column of the
       OrientationMatrix instance. BUT, when the ITK pipeline is updated,
       then the ITK origin is put back in place in the vtkImageData instance.

       Therefore we need to keep the vtkImageData's origin the same as the
       ITK one. And, we need to correct for this misbehaviour through a hack
       in the OrientationMatrix 4th column, a sort of corrected origin.
    */
    double v_origin[4], v_origin2[4];
    for (int i=0; i<3; i++)
      v_origin[i] = origin[i];
    v_origin[3] = 1.0;
    matrix->MultiplyPoint (v_origin, v_origin2);
    for (int i=0; i<3; i++)
      matrix->SetElement (i, 3, v_origin[i]-v_origin2[i]);
    /**
       The matrix instance is from now on corrected.
    */
#else
    for (int i=0; i<3; i++)
      matrix->SetElement (i, 3, origin[i]);
    
    /**
       The matrix instance is from now on corrected.
    */
#endif

    this->SetOrientationMatrix (matrix);
 
    matrix->Delete();
    
    typedef typename itk::ImageToVTKImageFilter< ImageType > ConverterType;
    typename ConverterType::Pointer converter = ConverterType::New();
    converter->SetInput (input);
    converter->Update();

#if MEDVTKINRIA_CORRECT_IMAGE_ORIENTATION
    /**
       The origin in ITK pipeline is taken into account in a different
       way than in the VTK equivalent.
       A first hack would be to force the vtkImageData instance to have
       a null origin, and put the ITK origin in the 4th column of the
       OrientationMatrix instance. BUT, when the ITK pipeline is updated,
       then the ITK origin is put back in place in the vtkImageData instance.

       Therefore we need to keep the vtkImageData's origin the same as the
       ITK one. And, we need to correct for this misbehaviour through a hack
       in the OrientationMatrix 4th column, a sort of corrected origin.
    */
    this->SetDataSet (converter->GetOutput());
#else
    vtkImageData* vtkinput = vtkImageData::New();
    vtkinput->DeepCopy(converter->GetOutput());
    vtkinput->SetOrigin (0,0,0);
    this->SetDataSet (vtkinput);
    vtkinput->Delete();
#endif    

    this->m_ItkImage = input;
    this->m_Converter = converter;
    
    this->Modified();
    
  }
  /**
     Reads a file and creates a image of a given scalar component type.
     Use with care. Please prefer using Read()
  */
  template <typename type> inline void ReadFile (const char* filename)
  {
    typedef typename itk::Image<type, 3> ImageType;
    typedef typename itk::ImageFileReader<ImageType> ReaderType;
    typename ReaderType::Pointer reader = ReaderType::New();		
    reader->SetFileName(filename);					
    try
    {
      std::cout<<"reading "<<filename<<"... ";			
      reader->Update();
      std::cout<<"done."<<std::endl;
    }
    catch(itk::ExceptionObject &e)  {
      std::cerr << e;
      vtkErrorMacro(<<"cannot read file : "<<filename<<endl);
      throw vtkErrorCode::CannotOpenFileError;
    }
    
    if ( !strcmp (reader->GetImageIO()->GetNameOfClass(), "PhilipsRECImageIO"))
    {
      DirectionType correctdirection = this->ExtractPARRECImageOrientation(filename);
      reader->GetOutput()->SetDirection (correctdirection);
      FloatImageType::PointType correctorigin = this->ExtractPARRECImageOrigin (filename, correctdirection);
      reader->GetOutput()->SetOrigin (correctorigin);
    }
    
    try
    {
      this->SetItkImage<type>(reader->GetOutput());
    }
    catch (vtkErrorCode::ErrorIds)
    {
      throw vtkErrorCode::UserError;
    }

    this->SetFilePath ((vtksys::SystemTools::GetFilenamePath (filename)).c_str());
  }


  /**
     Writes a file of a given scalar component type.
     Use with care. Please prefer using Write() 
  */
  template <typename type> inline void WriteFile (const char* filename)
  {
    typedef typename itk::Image<type, 3> ImageType;
    typedef typename itk::ImageFileWriter<ImageType> WriterType;
    typename WriterType::Pointer writer = WriterType::New();		
    
    typename ImageType::Pointer itkimage;
    bool valid = this->GetItkImage<type>(itkimage);
    if (!valid)
    {
      vtkErrorMacro(<<"cannot convert image !"<<endl);
      throw vtkErrorCode::UserError;
    }

    std::string realname = filename;
    std::string ext = vtksys::SystemTools::GetFilenameLastExtension(filename);
    if (!ext.size())
      realname += ".mha";
    writer->SetFileName(realname.c_str());

    writer->SetInput(itkimage);
    
    try
    {
      std::cout<<"writing "<<realname.c_str()<<"... ";			
      writer->Write();
      std::cout<<"done."<<std::endl;
    }
    catch(itk::ExceptionObject &e)  {
      std::cerr << e;
      vtkErrorMacro(<<"cannot write file : "<<realname.c_str()<<endl);
      throw vtkErrorCode::CannotOpenFileError;
    }

    // remember the path of the file
    this->SetFilePath ((vtksys::SystemTools::GetFilenamePath (realname.c_str())).c_str());

  }
  
  /**
     Access to the dataset as an ITK image.
     template type is the image scalar component type (i.e. unsigned char, float, etc)
     This method converts the vtk image into a corresponding type of vtkImageData
     so that there is no unusefull memory stored.
     However, you will have to know which type to use. For that use GetImageData() and recover its scalar type
     A switch on the scalar type will be necessary for optimal use.
  */
  template <typename type> inline bool GetItkImage (typename itk::Image<type, 3>::Pointer& ret)
  {
    if (!this->GetImageData())
      return false;
    
    typedef typename itk::Image<type, 3> ImageType;
    typedef typename ImageType::DirectionType DirectionType;
    
    typename ImageType::Pointer goodtypedimage = dynamic_cast<ImageType*>(this->GetItkImage());

    if (!goodtypedimage)
    {
      try
      {
	// If the dynamic cast failed, we need to provide a conversion of the vtk-imagedata
	// to the output :
	this->ConvertImage<type>(this->GetImageData(), this->m_ItkImage, this->m_Converter);
	// However by doing so, we don't feed the Direction of the itk::Image (not in vtk-image)
	// therefore we copy it from the OrientationMatrix field :
	vtkMatrix4x4* matrix = this->GetOrientationMatrix();
	DirectionType direction;
	for (unsigned int x=0; x<3; x++) for (unsigned int y=0; y<3; y++) direction[x][y] = matrix->GetElement (x,y);
	this->m_ItkImage->SetDirection (direction);
      }
      catch (itk::ExceptionObject &e)
      {
      	std::cerr << e;
      	vtkErrorMacro(<<"error catched in conversion."<<endl);
      	return false;
      }

      goodtypedimage = dynamic_cast<ImageType*>(this->GetItkImage());
    }

    if (!goodtypedimage)
    {
      std::cerr<<"something is blocking conversion, quit"<<std::endl;
      return false;
    }
    
    ret = goodtypedimage;
    return true;
  }


  DirectionType ExtractPARRECImageOrientation (const char* filename);
  FloatImageType::PointType ExtractPARRECImageOrigin (const char* filename, DirectionType direction);
  
  //ETX
  
  /**
     Copy some informations from a given metadataset
     It corresponds basically to all metadataset characteristics unless the vtkDataSet
     name, time, metadata dictionary...
  */  
  virtual void CopyInformation (vtkMetaDataSet* metadataset);

  /**
     Overwridden methods for read and write images
     These methods could only be used if ITK is set to ON
     Note that the read and write process are build in order
     to reduce the unusefull memory storage. A unsigned int image file will be opened
     as so, and stored in memory as so.
  */
  virtual void Read (const char* filename);
  virtual void Write (const char* filename);

  virtual void ReadColorImage (const char* filename);
  
  /**
     Set the dataset associated with the metadataset
  */
  virtual void SetDataSet(vtkDataSet* dataset);
  /**
     Get mehtod to get the vtkDataSet as an vtkImageData
  */
  vtkImageData* GetImageData() const;

  /**
     Get the type of the metadataset as string
  */
  virtual const char* GetDataSetType() const
  {
    return "ImageData";
  }

  static bool IsColorExtension (const char* ext);
  static bool IsImageExtension (const char* ext);
  
  static unsigned int CanReadFile (const char* filename);

  unsigned long GetComponentType();
  
 protected:
  vtkMetaImageData();
  ~vtkMetaImageData();

  vtkVolumeProperty* VolumeProperty;
  
 private:
  
  vtkMetaImageData(const vtkMetaImageData&);      // Not implemented.
  void operator=(const vtkMetaImageData&);        // Not implemented.

  vtkMatrix4x4* OrientationMatrix;
  
  //BTX
  itk::ImageBase<3>::Pointer m_ItkImage;
  itk::ProcessObject::Pointer m_Converter;
  
  DictionaryType DicomDictionary;
  DicomEntryList DicomTagList;
  //ETX


};




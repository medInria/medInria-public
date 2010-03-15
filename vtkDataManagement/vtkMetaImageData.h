/*=========================================================================

Program:   vtkINRIA3D
Module:    $Id: vtkMetaImageData.h 1452 2010-01-27 19:15:29Z ntoussaint $
Language:  C++
Author:    $Author: ntoussaint $
Date:      $Date: 2010-01-27 20:15:29 +0100 (Wed, 27 Jan 2010) $
Version:   $Revision: 1452 $

Copyright (c) 2007 INRIA - Asclepios Project. All rights reserved.
See Copyright.txt for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef _vtkMetaImageData_h_
#define _vtkMetaImageData_h_

#include "vtkINRIA3DConfigure.h"

#include <vtkErrorCode.h>
#include <vtksys/SystemTools.hxx>

#include <vtkMetaDataSet.h>
#include <vtkMatrix4x4.h>

#ifdef vtkINRIA3D_USE_ITK
// ITK includes for I/O only
#include <itkImageFileReader.h>
#include <itkImageFileWriter.h>
#include <itkImageToVTKImageFilter.h>
#include <itkVTKImageToImageFilter.h>
#include <itkImage.h>
#include <itkCastImageFilter.h>
#endif



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



class VTK_DATAMANAGEMENT_EXPORT vtkMetaImageData: public vtkMetaDataSet
{
 public:


  
  static vtkMetaImageData* New();
  vtkTypeRevisionMacro(vtkMetaImageData,vtkMetaDataSet);

  vtkGetObjectMacro (OrientationMatrix, vtkMatrix4x4);
  virtual void SetOrientationMatrix (vtkMatrix4x4* matrix);
  
  
#ifdef vtkINRIA3D_USE_ITK

  //BTX
  typedef float ImageComponentType;
  typedef itk::Image<ImageComponentType, 3> FloatImageType;
  typedef itk::Image<ImageComponentType, 4> FloatImage4DType;
  typedef itk::VTKImageToImageFilter<FloatImageType> ItkConverterType;
  typedef itk::MetaDataDictionary DictionaryType;

  typedef FloatImageType::DirectionType ShortDirectionType;
  
  
  /**
     Access directly to the itk image in float scalar component type
     For other use please refer to templated method GetItkImage()
  */
  FloatImageType::Pointer GetItkImage (void);
  /**
     Not used anymore : use SetItkImage instead
  */
  void SetDataSetAsItkImage(FloatImageType::Pointer image);
  /**
     Access to the DICOM dictionary of this image
     Note that this dictionary is not filled automatically when ITK image is set.
     You have to explicitally call SetDicomDictionary() for that
  */
  DictionaryType GetDicomDictionary (void)
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
    for (unsigned int x=0; x<3; x++)
      matrix->SetElement (x,3,origin[x]);

    this->SetOrientationMatrix (matrix);
 
    matrix->Delete();
    
    typedef typename itk::ImageToVTKImageFilter< ImageType > ConverterType;
    typename ConverterType::Pointer converter = ConverterType::New();
    converter->SetInput (input);
    converter->Update();
    vtkImageData* vtkinput = vtkImageData::New();
    vtkinput->DeepCopy(converter->GetOutput());

    /**
       FIX ME:
       In the new rendering system, this origin is taken into consideration
       directly in the 4x4 OrientationMatrix as its translation part.
       However in the classic rendering one, this matrix is not taken into
       consideration hence the origin has to be contained in the vtkImageData
       object. I need more coherence here ;-) !
    */
    vtkinput->SetOrigin (0,0,0);
    //#endif
    
    this->SetDataSet (vtkinput);
    vtkinput->Delete();
    this->LinkFilters();

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
      ShortDirectionType correctdirection = this->ExtractPARRECImageOrientation(filename);
      reader->GetOutput()->SetDirection (correctdirection);
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

/*     // ITK stuff to be able to write analyze format */
/*     itk::Matrix<double,3,3> cosines; */
/*     cosines[0][0]= 1; */
/*     cosines[0][1]= 0; */
/*     cosines[0][2]= 0; */
/*     cosines[1][0]= 0; */
/*     cosines[1][1]=-1; */
/*     cosines[1][2]= 0; */
/*     cosines[2][0]= 0; */
/*     cosines[2][1]= 0; */
/*     cosines[2][2]= 1; */
/*     itkimage->SetDirection(cosines); */
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
    typedef typename itk::CastImageFilter<FloatImageType, ImageType> CastFilterType;

    typename CastFilterType::Pointer myCast = CastFilterType::New();
    myCast->SetInput (this->GetItkImage());

    try
    {
      myCast->Update();
    }
    catch(itk::ExceptionObject &e)
    {
      std::cerr << e;
      vtkErrorMacro(<<"error when converting"<<endl);
      return false;
    }
    ret = myCast->GetOutput();

    typedef typename itk::Point<typename ImageType::DirectionType::ValueType, ImageType::ImageDimension> PointType;
    typename ImageType::DirectionType direction;
    PointType origin;
    vtkMatrix4x4* matrix = this->GetOrientationMatrix();

    for (unsigned int x=0; x<3; x++)
      for (unsigned int y=0; y<3; y++)
	direction[x][y] = matrix->GetElement (x, y);
    for (unsigned int x=0; x<3; x++)
      origin[x] = matrix->GetElement (x, 3);
    
    ret->SetOrigin(origin);
    ret->SetDirection(direction);
    
    return true;
  }


  ShortDirectionType ExtractPARRECImageOrientation (const char* filename);
  

  
  //ETX
  
  /**
     Copy some informations from a given metadataset
     It corresponds basically to all metadataset characteristics unless the vtkDataSet
     name, time, metadata dictionary...
  */  
  virtual void CopyInformation (vtkMetaDataSet* metadataset);

#endif  
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
  vtkImageData* GetImageData (void) const;

  /**
     Get the type of the metadataset as string
  */
  virtual const char* GetDataSetType (void) const
  {
    return "ImageData";
  }

  static bool IsColorExtension (const char* ext);
  static bool IsImageExtension (const char* ext);
  
  static unsigned int CanReadFile (const char* filename);

  vtkGetMacro (ComponentType, unsigned long);
  vtkSetMacro (ComponentType, unsigned long);
  

  
 protected:
  vtkMetaImageData();
  ~vtkMetaImageData();

  /**
     Method called everytime the dataset changes for connexion updates
  */
  virtual void LinkFilters (void);
  /**
     Method called everytime the dataset changes for initialization
   */
  virtual void Initialize (void);

  vtkVolumeProperty* VolumeProperty;


  
  
 private:
  
  vtkMetaImageData(const vtkMetaImageData&);      // Not implemented.
  void operator=(const vtkMetaImageData&);        // Not implemented.

  unsigned long ComponentType;

  vtkMatrix4x4* OrientationMatrix;
  
#ifdef vtkINRIA3D_USE_ITK
  //BTX
  FloatImageType::Pointer m_ItkImage;
  ItkConverterType::Pointer m_ItkConverter;
  DictionaryType DicomDictionary;
  //ETX

  
#endif
  

};

#endif


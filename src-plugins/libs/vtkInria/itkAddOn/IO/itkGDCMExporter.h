/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <cstddef> // For ITK 3.20 that does not define correctly ptrdiff_t

#include <itkProcessObject.h>
#include <itkImage.h>
#include <itkImageFileReader.h>
#include <itkImageSeriesWriter.h>
#include <itkGDCMSeriesFileNames.h>
#include <itkFlipImageFilter.h>
#include "itkExtractImageFilter.h"
#include "itkGDCMImageIO.h"
#include <itksys/ios/sstream>
#include <itksys/Base64.h>

#include "itkDicomTagManager.h"
#include "itkGDCMImporter.h"
#include <string>
#include <vector>

namespace itk
{


  /**
     
     \class GDCMExporter
     \brief itkProcessObject to write DICOM files from a set of volume images
     It needs to be feed by a Output directory SetOutputDirectory(), where the dicom files will be written
     Input images are set with AddVolume()
     and write all files with WriteAll()
     
     \author Nicolas Toussaint

     \see GDCMExporter  GDCMImporter ImageSeriesWriter
     \ingroup   ProcessObject
     
  */
 class ITK_EXPORT GDCMExporter : public ProcessObject
  {

  public:

    
    /** generic typedefs */
    typedef GDCMExporter Self;
    typedef ProcessObject            Superclass;
    typedef SmartPointer<Self>       Pointer;
    typedef SmartPointer<const Self> ConstPointer;
    
    /** generic constructors */
    itkNewMacro (Self);
    itkTypeMacro (GDCMExporter, ProcessObject);

    /** GDCMExporter's typedef, use GDCMExporter::FloatImageType as outputimage be match the templates */
    typedef GDCMImporter::ImageComponentType ImageComponentType;
    typedef itk::Image<ImageComponentType, 2>    ImageSliceType;
    typedef itk::Image<ImageComponentType, 3>    ImageType;
    typedef itk::Image< short, 2>                ShortImageSliceType;
    typedef itk::Image< short, 3>                ShortImageType;
    
    typedef itk::ImageFileWriter<ImageType>      ImageWriterType;
    typedef itk::ImageFileWriter<ShortImageSliceType> ImageSliceWriterType;
    typedef itk::MetaDataDictionary              DictionaryType;

    typedef itk::ExtractImageFilter< ImageType, ImageSliceType > ExtractType;
    typedef itk::GDCMImageIO                    ImageIOType;

    /**
       Set the direcory to scan in full path
       
       if you set the Recursive flag to ON,
       be careful that set the write directory before scanning
    */
    void SetOutputDirectory (const char* path)
    { this->OutputDirectory = path; }
    /* accessor */
    const char* GetOutputDirectory() const
    { return this->OutputDirectory.c_str(); }

    void SetFilePrefix (const char* prefix)
    { this->Prefix = prefix; }
    /* accessor */
    const char* GetFilePrefix() const
    { return this->Prefix.c_str(); }
    
    /**
       Save all volumes in a given directory
    */
    void WriteAll();
    
    /**
       Add an image to the exam
    */
    void AddVolume (ImageType::Pointer image);

    /**
      Access to the output reconstructed images
    */    
    std::vector<ImageType::Pointer> GetInputVolumes() const
    { return this->InputVolumeList; }
    

    
  protected:
    
    GDCMExporter();
    ~GDCMExporter();

    
  private:
    GDCMExporter(const Self&);
    void operator=(const Self&);

    std::string OutputDirectory;
    std::string Prefix;
    std::vector<ImageType::Pointer> InputVolumeList;

    bool ReverseOrder;
    

    
  };
  
} // end of namespace itk








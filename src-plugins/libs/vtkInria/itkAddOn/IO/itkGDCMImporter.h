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
#include <itkImageSeriesReader.h>
#include <itkGDCMSeriesFileNames.h>
#include <itkFlipImageFilter.h>
#include "itkDicomTagManager.h"

#include <string>
#include <vector>

#include <gdcmFile.h>
#include <gdcmSerieHelper.h>

namespace itk
{

  /**
     
     \class DICOMVolume
     \brief Internal class that represent a 3D volume image made from 2D DICOM files.
     \author Nicolas Toussaint
     
     Don't use this class independently. The user may use GDCMImporter instead.
     
     \see GDCMImporter GDCMSeriesFileNames ImageSeriesReader
     \ingroup   ProcessObject
     
  */
 class ITK_EXPORT DICOMVolume : public ProcessObject
  {
    
  public:
    typedef DICOMVolume Self;
    typedef ProcessObject            Superclass;
    typedef SmartPointer<Self>       Pointer;
    typedef SmartPointer<const Self> ConstPointer;

/*     typedef vtkMetaImageData::ImageType   ImageType; */
/*     typedef vtkMetaImageData::Image4DType Image4DType; */

    typedef unsigned short ImageComponentType;
    
    typedef itk::Image<ImageComponentType, 3> ImageType;
    typedef itk::Image<ImageComponentType, 4> Image4DType;

    typedef itk::ImageFileReader<ImageType> ReaderType;
    typedef itk::ImageSeriesReader<ImageType> SeriesReaderType;
    typedef itk::FlipImageFilter<ImageType> FlipFilterType;
    
    itkNewMacro  (Self);
    itkTypeMacro (DICOMVolume, ProcessObject);
    
    void SetImage(ImageType::Pointer image)
    { this->Image = image; }
    
    ImageType::Pointer GetImage() const
    { return this->Image; }

    void SetFileList(std::vector<std::string> list)
    { this->FileList = list; }

    std::vector<std::string> GetFileList() const
    { return this->FileList; }

    void SetDescription(std::string description)
    { this->Description = description; }

    std::string GetDescription() const
    { return this->Description; }

    void SetSerieHelper(gdcm::SerieHelper* helper)
    { this->SerieHelper = helper; }

    gdcm::SerieHelper* GetSerieHelper() const
    { return this->SerieHelper; }

    void SetgdcmFileList(std::vector<gdcm::File*> *list)
    { this->gdcmFileList = list; }

    std::vector<gdcm::File*> *GetgdcmFileList() const
    { return this->gdcmFileList; }

    unsigned int* GetDimensions();

    ImageType::Pointer TemporaryBuild();
    
    void Build();
    
    void Save (const char*);

    const itk::DicomTagManager::Pointer GetTagManager()
    {
      return this->TagManager;
    }

    void FillDicomTagManager();
    
    void SetSequenceFlag (bool val)
    { this->SequenceFlag = val; }
    bool IsSequence()
    { return this->SequenceFlag; }

    /**
       Use this method to release memory after volume parsing
    */
    void ReleaseMemory();

  protected:

    DICOMVolume()
    {
      Description  = "";
      Image        = 0;
      SerieHelper  = 0;
      gdcmFileList = 0;
      this->TagManager = itk::DicomTagManager::New();
      SequenceFlag = false;
    }
    ~DICOMVolume(){}
  
    
  private :
    DICOMVolume(const Self&);
    void operator=(const Self&);
    
    std::string              Description;
    ImageType::Pointer  Image;
    std::vector<std::string> FileList;
    gdcm::SerieHelper*       SerieHelper;
    std::vector<gdcm::File*> *gdcmFileList;

    itk::DicomTagManager::Pointer TagManager;

    bool SequenceFlag;
    
    
    
  };
  
  
  /**
     
     \class GDCMImporter
     \brief itkProcessObject to manage DICOM files, reordering, building volumes
     \author Nicolas Toussaint
     
     This class is a powerfull factory for DICOM file management.
     It uses gdcm library from ITK to parse DICOM files and order them with respect to the GDCMSeriesFileNames strategy.

     The user can simply set the input directory with SetInputDirectory(). Then the directory will be scaned by calling Scan().
     If you enconter some issues for reconstructing volumes (especially for interlaced files), you may use SplitVolumeByPositionConsistency() to reorder correctly the problematic volumes.
     When finished, you should call BuildAllVolumes() and recover all itkImages through the method GetDICOMVolumeList().

     If for some reason you want to split a volume with respect to a certain DICOM tag, you can use SplitVolumeByTag().

     This class only export 3D volume images, but they can represent 2D+t sequences for example
     \see GDCMImporter GDCMSeriesFileNames ImageSeriesReader
     \ingroup   ProcessObject
     
  */
 class ITK_EXPORT GDCMImporter : public ProcessObject
  {

  public:

    
    /** generic typedefs */
    typedef GDCMImporter Self;
    typedef ProcessObject       Superclass;
    typedef SmartPointer<Self>       Pointer;
    typedef SmartPointer<const Self> ConstPointer;
    
    /** generic constructors */
    itkNewMacro (Self);
    itkTypeMacro (GDCMImporter, ProcessObject);

    /** GDCMImporter's typedef, use GDCMImporter::ImageType as outputimage be match the templates */
    typedef DICOMVolume::ImageComponentType ImageComponentType;
    typedef DICOMVolume::ImageType ImageType;
    typedef DICOMVolume::Image4DType Image4DType;

    typedef itk::ImageFileReader<ImageType> ReaderType;
    typedef itk::ImageSeriesReader<ImageType> SeriesReaderType;

    typedef DICOMVolume::FlipFilterType FlipFilterType;

/*     DicomDictionary* GetDictionary() */
/*     { return this->Dictionary; } */
    

    /**
       DICOM tags : they are used to let the user sorting the DICOMs respecting a specific tag.
       More are to be added...
    */
    enum DICOMTag
    {
      D_SERIESUID,
      D_STUDYDESCRIPTION,
      D_SERIESDESCRIPTION,
      D_ACQUISITIONTIME,
      D_MODALITY
    };
    

    
    /** Set the recursive mode to ON if you want to scan DICOMs recursively in the InputDirectory */
    itkBooleanMacro(Recursive);
    itkGetMacro (Recursive, bool);
    itkSetMacro (Recursive, bool);

    

    /**
       Set the direcory to scan in full path
       if you set the Recursive flag to ON,
       be careful that set the write directory before scanning
    */
    void SetInputDirectory (const char* path)
    { this->InputDirectory = path; }
    /* accessor */
    const char* GetInputDirectory() const
    { return this->InputDirectory.c_str(); }    

    /**
      check if a file can be read by itk::GDCMImageIO
    */
    bool CanReadFile(const char* filename);

    /**
       Reset all buid volumes and filelists.
       dosen't reset the InputDirectory, so that the scanning would be faster afterward.
    */
    void Reset();
    /**
       Scan the directory respecting the GDCMSeriesFileNames strategy, plus some rules the user may have added.
       it will create a list of DICOMVolume that can be access by GetDICOMVolumeList(). 
    */
    void Scan();

    /**
       Read a single file, attemp to add content to the volume list
    */
    void ReadFile (const char* filename);
    
    /**
       Use this method for splitting volume with respect to a specific DICOM tag
    */
    
    void SplitVolumeByTag (DICOMVolume::Pointer inputvolume, unsigned long tag);
    /**
       For most cases, this method is sufficient to correctly separate interlaced volumes
    */
    void SplitVolumeByPositionConsistency (DICOMVolume::Pointer inputvolume);

    /** internal use */
    static uint16_t GetDICOMTagGroup(unsigned long tag);
    /** internal use */
    static uint16_t GetDICOMTagElement(unsigned long tag);

    /**
       Use this method to save a specific volume without keeping it in memory
    */
    void Save(int, const char*);
    /**
       Save all volumes in a givem directory
    */
    void SaveAll(const char*);
    
    /** internal use */
    void AddVolume (std::string description, std::vector<std::string> filelist, std::vector<gdcm::File*> *gdcmfilelist, ImageType::Pointer itkimage = 0);


    bool IsFileListSequence (std::vector<gdcm::File*> *gdcmfilelist);
    

    /**
       Use this method to release memory after volume parsing
    */
    void ReleaseMemory();
    
    /**
      Access to the output reconstructed images
    */    
    std::vector<ImageType::Pointer> GetOutputVolumes() const
    { return this->OutputVolumeList; }

    /**
      Pointers to DICOM volume objects
      Use with care
    */    
    std::vector<DICOMVolume::Pointer> GetDICOMVolumeList() const
    { return this->DICOMVolumeList; }
    /**
       Access a dicom volume by its id in list
    */
    DICOMVolume::Pointer GetDICOMVolume(unsigned int index) const
    {
      if (index >= this->DICOMVolumeList.size())
	return NULL;
      
      return this->DICOMVolumeList[index]; }

    /**
       Finds a dicom volume by its name
    */
    DICOMVolume::Pointer GetDICOMVolume(const char*) const;
    

    /**
       Permanently remove a volume from the list
    */
    void RemoveDICOMVolume (DICOMVolume::Pointer dcmvolume);

    /**
      Build 3D volumic images from 2D DICOM files
    */    
    void BuildAllVolumes();
    
    /** DEPRECATED */
    enum SplitModeIds
    {
      NOSPLIT,
      SPLITFOLLOWED,
      SPLITINTERLACED
    };


    /** internal use */
    gdcm::XCoherentFileSetmap SplitOnPosition(gdcm::FileList *fileSet);

/*     std::string GetVolumeTagValue (DICOMVolume::Pointer dcmvolume, DicomDictionary::DicomTag dicomtag); */
    

    
  protected:
    
    GDCMImporter();
    ~GDCMImporter();
    
    
  private:
    GDCMImporter(const Self&);
    void operator=(const Self&);

    std::string                           InputDirectory;
    bool                                  m_Recursive;
    
    itk::GDCMSeriesFileNames::Pointer    DICOMScanner;
    std::vector<ImageType::Pointer> OutputVolumeList;
    std::vector<DICOMVolume::Pointer>    DICOMVolumeList;

/*     DicomDictionary* Dictionary; */
    
  };
  
} // end of namespace itk





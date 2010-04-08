



/*=========================================================================

Program:   vtkINRIA3D
Module:    $Id: itkGDCMImporter3.h 522 2007-12-05 15:06:51Z ntoussaint $
Language:  C++
Author:    $Author: ntoussaint $
Date:      $Date: 2007-12-05 16:06:51 +0100 (Wed, 05 Dec 2007) $
Version:   $Revision: 522 $

Copyright (c) 2007 INRIA - Asclepios Project. All rights reserved.
See Copyright.txt for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef _itk_GDCMImporter3_h_
#define _itk_GDCMImporter3_h_

#include <itkImageSource.h>
#include <itkImage.h>
#include <itkImageFileReader.h>
#include <itkImageFileWriter.h>
#include <itkImageSeriesReader.h>
#include <itksys/SystemTools.hxx>

#include "gdcmScanner.h"

#include <sstream>
#include <vector>

namespace itk
{


  /**

     \class GDCMVolume
     \brief Class that represents an image made from DICOM files, inherits from itk::Image.
     \author Nicolas Toussaint

     \see GDCMImporter3 ImageSeriesReader Image
     \ingroup   ImageObjects
  */
  template <class TPixelType=unsigned short>
    class ITK_EXPORT GDCMVolume : public Image<TPixelType, 4>
    {
    public:
    /// ITK typedefs
    typedef GDCMVolume Self;
    /// ITK typedefs
    typedef GDCMVolume ImageType;
    /// ITK typedefs
    typedef Image<TPixelType, 3> SubImageType;
    /// ITK typedefs
    typedef Image<TPixelType, 4> Superclass;
    
    /// ITK typedefs
    typedef SmartPointer<Self> Pointer;
    /// ITK typedefs
    typedef SmartPointer<const Self> ConstPointer;
    /// gdcm::File typedef (used to retrieve some dicom information)    
    typedef std::vector<std::string> FileList;
    typedef std::map<std::string, FileList> FileListMapType;
    
    /// ITK typedefs
    itkNewMacro  (Self);
    /// ITK typedefs
    itkTypeMacro (GDCMVolume, Superclass);
    // superclass typedefs
    typedef typename Superclass::SpacingType SpacingType;
    /**/// serie reader typedef, reading a serie of dicom file to reconstruct a volume image
    typedef ImageSeriesReader<SubImageType> SeriesReaderType;
    /** Reader typedef, used to read a single file such as DICOMDIR file */
    typedef typename itk::ImageFileReader<SubImageType> ReaderType;
    /** Writer typedef, not used */
    typedef typename itk::ImageFileWriter<ImageType> WriterType;    
    
    /*
       Get/Set the FileList coming from the GDCM library.
       This list will then be used by Build() for reconstructing
       the volume image.
    */
    void SetFileListMap(FileListMapType map)
    {
      this->m_FileListMap = map;
      this->Modified();
      m_IsBuilt = 0;
    }
    /**
       Get/Set the FileList coming from the GDCM library.
       This list will then be used by Build() for reconstructing
       the volume image.
    */
    FileListMapType GetFileListMap (void) const
    { return this->m_FileListMap; }
    /**
       Set/Get the name of the image. This attribute can
       then be used for display purposes
    */
    itkGetStringMacro (Name);
    itkSetStringMacro (Name);
    /**
       Build() will actually fill the volume image considering
       the FileList (SetFileList())
    */
    void Build(void);
    /**
    */
    void Write(std::string filename);
    /** Use this method to release memory after volume parsing (e.g. to a filter) */
    void Initialize (void)
    {
      this->Superclass::Initialize();
      this->m_IsBuilt = 0;
    }

  protected:

    /**
       default GDCMVolume constructor,
       empty filelist and image name.
    */
    GDCMVolume()
    {
      this->SetName ("image");
      m_IsBuilt = 0;
    }
    /**
       default GDCMVolume destructor,
       free memory.
    */
    ~GDCMVolume()
    {
      this->Initialize();
    }
    /**
       Print the image information, including the name and
       the DICOM dictionnary (displaying the dictionnary of the first
       file contained in GetFileList()).
    */
    void PrintSelf(std::ostream& os, Indent indent) const;

  private :
    GDCMVolume(const Self&);
    void operator=(const Self&);
    std::string m_Name;
    FileListMapType m_FileListMap;
    bool m_IsBuilt;
  };





  

  /**

     \class GDCMImporter3
     \brief itkImageSource to manage DICOM files, reordering, building volumes
     \author Nicolas Toussaint

     \see GDCMImporter3 GDCMSeriesFileNames ImageSource GDCMSeriesFileNames
     \ingroup   DataSources

  */
  template <class TPixelType=unsigned short>
 class ITK_EXPORT GDCMImporter3 : public ImageSource < GDCMVolume <TPixelType> >
    {
    public:

    /** generic typedefs */
    typedef GDCMImporter3 Self;
    /** generic typedefs */
    typedef GDCMVolume <TPixelType> ImageType;
    /** generic typedefs */
    typedef ImageSource < ImageType > Superclass;
    /** generic typedefs */
    typedef SmartPointer<Self>       Pointer;
    /** generic typedefs */
    typedef SmartPointer<const Self> ConstPointer;
    
    /** generic constructors */
    itkNewMacro (Self);
    /** generic typedefs */
    itkTypeMacro (GDCMImporter3, Superclass);
    /** ImageDimension typedef, dimension of the output Image(s) */
    itkStaticConstMacro (ImageDimension, unsigned int, ImageType::ImageDimension);
    /** Pixel typedef */
    typedef typename ImageType::PixelType PixelType;
    /** GDCMVolume typedef, corresponding to the output Image type */
    typedef typename ImageType::Pointer ImagePointerType;
    /** stl vector of DataObject, used for a fast access to outputs array  */
    typedef itk::ProcessObject::DataObjectPointerArray DataObjectPointerArray;
    typedef typename ImageType::RegionType OutputImageRegionType;

    /** Get/Set the InputDirectory, root directory where a DICOM exam can be found*/
    itkGetStringMacro (InputDirectory);
    void SetInputDirectory (std::string d)
    {
      m_InputDirectory = d;
      m_IsScanned = 0;
      this->Modified();
    }
    

    typedef typename ImageType::FileList FileList;
    typedef typename ImageType::FileListMapType FileListMapType;
    typedef std::map<std::string, FileListMapType> FileListMapofMapType;
    /**
       Use this method to save a specific volume in a file
    */
    void SaveOutputInFile(unsigned int, const char*);
    /**
       Save all output images in a given directory
    */
    void SaveOutputsInDirectory(const char*);
    /**
       Scan the directory (or the DICOMDIR file) respecting the strategies explained above,
       The scan might be slow, it is linearly dependant on the number of
       files present in the SetInputDirectory() (or the size of DICOMDIR file).
    */
    void Scan (void);
    /**
       Restoring/Setting output states according to the helper.
    */
    void Reset (void);

  protected:

    virtual int SplitRequestedRegion(int i, int num, OutputImageRegionType& splitRegion);
    void ThreadedGenerateData(const OutputImageRegionType& outputRegionForThread, int threadId );

    /** default constructor */
    GDCMImporter3();
    /** default destructor, release memory */
    ~GDCMImporter3(){}
    void PrintSelf(std::ostream& os, Indent indent) const;

  private:
    GDCMImporter3(const Self&);
    void operator=(const Self&);

    std::string m_InputDirectory;
    FileListMapofMapType m_FileListMapofMap;

    gdcm::Scanner m_FirstScanner;
    gdcm::Scanner m_SecondScanner;
    gdcm::Scanner m_ThirdScanner;

    bool m_IsScanned;
    
    FileListMapType PrimarySort (FileList list);
    FileListMapType SecondarySort (FileList list);
    FileListMapType TertiarySort (FileList list);
    std::string GenerateUniqueName (FileListMapType map);
  };

} // end of namespace itk



#ifndef ITK_MANUAL_INSTANTIATION
#include "itkGDCMImporter3.txx"
#endif


#endif


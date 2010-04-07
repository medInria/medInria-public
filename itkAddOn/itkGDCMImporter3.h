



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
#include <itkDataObject.h>
#include <itkMetaDataObject.h>
#include <itkGDCMImageIOFactory.h>
#include <itkGDCMImageIO.h>
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
  template <class TPixelType=unsigned short, unsigned int NDimensions=3>
    class ITK_EXPORT GDCMVolume : public Image<TPixelType, NDimensions>
    {
    public:
    /// ITK typedefs
    typedef GDCMVolume Self;
    /// ITK typedefs
    typedef GDCMVolume ImageType;
    /// ITK typedefs
    typedef Image<TPixelType, NDimensions> Superclass;
    /// ITK typedefs
    typedef SmartPointer<Self> Pointer;
    /// ITK typedefs
    typedef SmartPointer<const Self> ConstPointer;
    /// gdcm::File typedef (used to retrieve some dicom information)    
    typedef std::vector<std::string> FileList;
    typedef std::map<std::string, FileList> FileListMapType;
    
    /// serie reader typedef, reading a serie of dicom file to reconstruct a volume image
    typedef typename itk::ImageSeriesReader<ImageType> SeriesReaderType;
    /// ITK typedefs
    itkNewMacro  (Self);
    /// ITK typedefs
    itkTypeMacro (GDCMVolume, Superclass);
    // superclass typedefs
    typedef typename Superclass::SpacingType SpacingType;

    /**
       Get/Set the FileList coming from the GDCM library.
       This list will then be used by Build() for reconstructing
       the volume image.
    */
    virtual void SetFileListMap(FileListMapType map)
    {
      if (map == this->m_FileListMap)
	return;

      this->m_FileListMap = map;
      m_IsBuilt = 0;
    }
    /**
       Get/Set the FileList coming from the GDCM library.
       This list will then be used by Build() for reconstructing
       the volume image.
    */
    virtual FileListMapType GetFileListMap (void) const
    { return this->m_FileListMap; }
    /**
       Set/Get the name of the image. This attribute can
       then be used for display purposes
    */
    itkGetStringMacro (Name);
    /**
       Set/Get the name of the image. This attribute can
       then be used for display purposes
    */
    itkSetStringMacro (Name);
    /**
       Build() will actually fill the volume image considering
       the FileList (SetFileList())
    */
    virtual void Build(void);
    /**
       Use this method to release memory after volume parsing (e.g. to a filter)
    */
    virtual void Initialize (void)
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
  template <typename TImage=Image<unsigned short, 3> >
 class ITK_EXPORT GDCMImporter3 : public ImageSource < TImage >
    {
    public:

    /** generic typedefs */
    typedef GDCMImporter3 Self;
    /** generic typedefs */
    typedef ImageSource < TImage >   Superclass;
    /** generic typedefs */
    typedef SmartPointer<Self>       Pointer;
    /** generic typedefs */
    typedef SmartPointer<const Self> ConstPointer;
    /** generic typedefs */
    typedef TImage ImageType;

    /** generic constructors */
    itkNewMacro (Self);
    /** generic typedefs */
    itkTypeMacro (GDCMImporter3, Superclass);

    /** ImageDimension typedef, dimension of the output Image(s) */
    itkStaticConstMacro (ImageDimension, unsigned int, ImageType::ImageDimension);
    itkStaticConstMacro (ImageDimension2, unsigned int, ImageType::ImageDimension+1);
    /** Pixel typedef */
    typedef typename ImageType::PixelType PixelType;
    /** GDCMVolume typedef, corresponding to the output Image type */
    typedef GDCMVolume<PixelType, ImageDimension> GDCMVolumeType;
    /** GDCMVolume typedef, corresponding to the output Image type */
    typedef typename GDCMVolumeType::Pointer GDCMVolumePointerType;
    /** Reader typedef, used to read a single file such as DICOMDIR file */
    typedef typename itk::ImageFileReader<GDCMVolumeType> ReaderType;
    /** Writer typedef, not used */
    typedef typename itk::ImageFileWriter<GDCMVolumeType> WriterType;
    typedef typename itk::ImageFileWriter<Image<PixelType, ImageDimension+1> > Writer4DType;
    
    /** stl vector of DataObject, used for a fast access to outputs array  */
    typedef itk::ProcessObject::DataObjectPointerArray DataObjectPointerArray;
    /** Get/Set the InputDirectory, root directory where a DICOM exam can be found*/
    itkGetStringMacro (InputDirectory);
    /** Get/Set the InputDirectory, root directory where a DICOM exam can be found*/
    itkSetStringMacro (InputDirectory);

    typedef typename GDCMVolumeType::FileList FileList;
    typedef std::map<std::string, FileList> FileListMapType;
    typedef std::map<std::string, FileListMapType> FileListMapofMapType;
    
    /// serie reader typedef, reading a serie of dicom file to reconstruct a volume image
    typedef typename itk::ImageSeriesReader<ImageType> SeriesReaderType;
    /**
      check if a file can be read by itk::GDCMImageIO
    */
    virtual bool CanReadFile(const char* filename);
    /**
       Read a single file, attempt to add content to the volume list
    */
    virtual void ReadFile (const char* filename);
    /**
       Use this method to save a specific volume in a file
    */
    virtual void SaveOutputInFile(unsigned int, const char*);
    /**
       Save all output images in a given directory
    */
    virtual void SaveOutputsInDirectory(const char*);
    /**
       Save all output images in a given directory. Root filename is provided
    */
    virtual void SaveOutputsInDirectory(const char*, const char*);
    /**
       Remove (release memory) of all output images.
       dosen't reset the InputDirectory, so that the scanning might be faster afterward.
    */
    virtual void Initialize(void)
    {
      this->PrepareOutputs();
    }
    /**
       Scan the directory (or the DICOMDIR file) respecting the strategies explained above,
       The scan might be slow, it is linearly dependant on the number of
       files present in the SetInputDirectory() (or the size of DICOMDIR file).
    */
    virtual void Scan (void);
    /**
       Explicitally removes the given output from the output list. It will release memory
       involved by this output if any.
    */
    virtual void RemoveGDCMVolume (GDCMVolumePointerType dicomimage);

    FileListMapofMapType GetGlobalDicomFileListMap (void);
    
    
  protected:

    /** Overload of the ImageSource process */
    virtual void GenerateData (void);

    
    void BuildAndWriteMap (FileListMapType map, const char* filename);
    
    /**
       Restoring/Setting output states according to the helper.
    */
    virtual void InitializeOutputs (void);
    /**
       INTERNAL:

       Generate an unused name using the UID series description from the
       first item in argument filelist.
       Returns a name.
    */
    virtual std::string GenerateName (FileList filelist);
    /** default constructor */
    GDCMImporter3();
    /** default destructor, release memory */
    ~GDCMImporter3()
    {
      // Release the memory
      this->Initialize();
    }
    /**
       Prints information concerning the Importer,
       does not print all outputs inner informations.
    */
    void PrintSelf(std::ostream& os, Indent indent) const;

    void Scan (const char* directory);
    void IPPSort (void);
    FileListMapType SortwrtIPP (FileList list, const char* rootid);

    
  private:
    GDCMImporter3(const Self&);
    void operator=(const Self&);

    std::string m_InputDirectory;
    FileListMapofMapType FileListMapofMap;

    gdcm::Scanner FirstScanner;
    gdcm::Scanner SecondScanner;
    gdcm::Scanner ThirdScanner;

    FileListMapType PrimarySort (FileList list);
    FileListMapType SecondarySort (FileList list);
    FileListMapType TertiarySort (FileList list);
    
    
  };

} // end of namespace itk



#ifndef ITK_MANUAL_INSTANTIATION
#include "itkGDCMImporter3.txx"
#endif


#endif


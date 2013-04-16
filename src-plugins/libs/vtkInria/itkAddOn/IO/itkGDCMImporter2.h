/*=========================================================================

 MedInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <itkImageSource.h>
#include <itkImage.h>
#include <itkImageFileReader.h>
#include <itkImageFileWriter.h>
#include <itkImageSeriesReader.h>
#include <itkGDCMSeriesFileNames.h>
#include <itkDataObject.h>
#include <itkMetaDataObject.h>
#include <itkGDCMImageIOFactory.h>
#include <itkGDCMImageIO.h>
#include <itksys/SystemTools.hxx>

#include <gdcmDicomDirPatient.h>
#include <gdcmDicomDirStudy.h>
#include <gdcmDicomDirSerie.h>
#include <gdcmDicomDirImage.h>
#include <gdcmDicomDir.h>
#include <gdcmDicomDirMeta.h>
#include <gdcmFile.h>
#include <gdcmSerieHelper.h>
#include <gdcmValEntry.h>
#include <gdcmDirList.h>

#include <sstream>
#include <vector>

namespace itk
{


  /**

     \class DICOMImage
     \brief Class that represents an image made from DICOM files, inherits from itk::Image.
     \author Nicolas Toussaint

     Don't use this class independently. Prefer using GDCMImporter(2) instead.

     This class is an itk::Image that has 2 enhancements:

     1. A name (GetName() and SetName())

     2. A FileList coming from gdcm library (SetFileList() and GetFileList()).
     This filelist is then used to build a volume image from a set of DICOM 2D files.
     Use Build() to actually build the inner volume image.

     NB: To be used with NDimensions == 3, otherwise there is no point using it.

     \see GDCMImporter2 ImageSeriesReader Image
     \ingroup   ImageObjects
  */
  template <class TPixelType=unsigned short, unsigned int NDimensions=3>
    class ITK_EXPORT DICOMImage : public Image<TPixelType, NDimensions>
    {
    public:
    /// ITK typedefs
    typedef DICOMImage Self;
    /// ITK typedefs
    typedef DICOMImage ImageType;
    /// ITK typedefs
    typedef Image<TPixelType, NDimensions> Superclass;
    /// ITK typedefs
    typedef SmartPointer<Self>       Pointer;
    /// ITK typedefs
    typedef SmartPointer<const Self> ConstPointer;
    /// gdcm::File typedef (used to retrieve some dicom information)
    typedef gdcm::File DICOMFile;
    /// gdcm::FileList typedef (a stl vector of DICOMFile)
    typedef gdcm::FileList DICOMFileList;
    /// serie reader typedef, reading a serie of dicom file to reconstruct a volume image
    typedef typename itk::ImageSeriesReader<ImageType> SeriesReaderType;
    /// ITK typedefs
    itkNewMacro  (Self);
    /// ITK typedefs
    itkTypeMacro (DICOMImage, Superclass);
    // superclass typedefs
    typedef typename Superclass::SpacingType SpacingType;

    /**
       Get/Set the FileList coming from the GDCM library.
       This list will then be used by Build() for reconstructing
       the volume image.
    */
    virtual void SetFileList(DICOMFileList* list)
    {
      if (list == this->m_FileList)
	return;
      if (this->m_FileList)
	delete this->m_FileList;
      this->m_FileList = list;
      m_IsBuilt = 0;
    }
    /**
       Get/Set the FileList coming from the GDCM library.
       This list will then be used by Build() for reconstructing
       the volume image.
    */
    virtual DICOMFileList* GetFileList() const
    { return this->m_FileList; }
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
    virtual void Build();
    /**
       Use this method to release memory after volume parsing (e.g. to a filter)
    */
    virtual void Initialize()
    {
      this->Superclass::Initialize();
      this->m_IsBuilt = 0;
    }

  protected:

    /**
       default DICOMImage constructor,
       empty filelist and image name.
    */
    DICOMImage()
    {
      this->SetName ("image");
      m_FileList = 0;
      m_IsBuilt = 0;
    }
    /**
       default DICOMImage destructor,
       free memory.
    */
    ~DICOMImage()
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
    DICOMImage(const Self&);
    void operator=(const Self&);
    std::string m_Name;
    DICOMFileList* m_FileList;
    bool m_IsBuilt;
  };


  /**

     \class GDCMImporter2
     \brief itkImageSource to manage DICOM files, reordering, building volumes
     \author Nicolas Toussaint

     This class is a powerfull factory for DICOM (-single frame) exam management.
     It uses gdcm library from ITK to parse DICOM files and order them with respect to an enhanced strategy
     inherited from GDCMHelperType.

     Output of this filter is a set of itk::Image that can be used as so.

     The user can simply set the input directory with SetInputDirectory(), preferrably containing a DICOMDIR file.
     Then the directory (or the DICOMDIR file) will be scaned by calling Scan().

     IMPORTANT NOTE: calling Scan() does not build the output images. It is an equivalent to a call of
     GenerateOutputInformation(). The user may use Scan() forwarded by Update() in order to
     fully build the volume images.

     The reason why the process is divided in two steps (Scan() and Update()) is because the default strategy
     used for reconstruction might be not sufficient for specific DICOM exam acquisition (e.g. Diffusion Weighted MRI).
     In those particular cases, the user may first call Scan(), followed by a wise re-ordering of the outputs. This re-ordering
     consists on split-merge strategies:

     AutoOrganization() splitting rule is based on image orientation and image size, while the method intends to
     merge UIDs that appear to be sufficiently similar (happens in 3D+time multi-shot MRI).

     AutoOrganizationPositionBased() splitting rule is more restrictive than the one used in AutoOrganization() in the sense
     that is also uses the Image Patient Position flags to distinguish volumes. The user may use this method for
     Diffusion Weighted MRI acquisitions.

     Update() will actually build the volume images with respect to the strategies explained above.
     The user may use GetOutput() and GetNumberOfOutputs() to use the reconstructed images.

     Downcasting an output into itk::DICOMImage will allow you to recover DICOM-meta-information

     \see GDCMImporter2 GDCMSeriesFileNames ImageSource GDCMSeriesFileNames
     \ingroup   DataSources

  */
  template <typename TImage=Image<unsigned short, 3> >
 class ITK_EXPORT GDCMImporter2 : public ImageSource < TImage >
    {
    public:

    /** generic typedefs */
    typedef GDCMImporter2 Self;
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
    itkTypeMacro (GDCMImporter2, Superclass);

    /** ImageDimension typedef, dimension of the output Image(s) */
    itkStaticConstMacro (ImageDimension, unsigned int, ImageType::ImageDimension);
    /** Pixel typedef */
    typedef typename ImageType::PixelType PixelType;
    /** DICOMImage typedef, corresponding to the output Image type */
    typedef DICOMImage<PixelType, ImageDimension> DICOMImageType;
    /** DICOMImage typedef, corresponding to the output Image type */
    typedef typename DICOMImageType::Pointer DICOMImagePointerType;
    /** Reader typedef, used to read a single file such as DICOMDIR file */
    typedef typename itk::ImageFileReader<DICOMImageType> ReaderType;
    /** Writer typedef, not used */
    typedef typename itk::ImageFileWriter<DICOMImageType> WriterType;
    /**
       GDCMSeriesFileNames is used fir DICOM ordering strategies, itself
       using the GDCMHelperType strategy. GDCMSeriesFileNames was formally used to
       scan a directory for DICOM files, now the scan is done manually in LoadFromDirectory(),
       in order to have access to the progress.
    */
    typedef itk::GDCMSeriesFileNames GDCMScannerType;
    /** DICOMFile sub-typedef */
    typedef typename DICOMImageType::DICOMFile DICOMFile;
    /** DICOMFile sub-typedef */
    typedef typename DICOMImageType::DICOMFileList DICOMFileList;
    /** stl vector of DataObject, used for a fast access to outputs array  */
    typedef itk::ProcessObject::DataObjectPointerArray DataObjectPointerArray;
    /** GDCMHelperType is used for ordering strategies */
    typedef gdcm::SerieHelper GDCMHelperType;
    /** DicomDirType typedef, for reading directly DICOMDIR file instead of exhaustive directory scanning */
    typedef gdcm::DicomDir DicomDirType;
    /** stl map of < std::string - DICOMFileList* >, used in the re-ordering process */
    typedef gdcm::XCoherentFileSetmap DICOMFileListMap;
    /** deprecated */
    enum PriorityRuleIds
    {
      SPACE_DOMINANT,
      TIME_DOMINANT
    };
    /** Get/Set the RecursiveScan mode to OFF if you don't want to scan DICOMs recursively in the InputDirectory */
    itkGetMacro (RecursiveScan, bool);
    /** Get/Set the RecursiveScan mode to OFF if you don't want to scan DICOMs recursively in the InputDirectory */
    itkSetMacro (RecursiveScan, bool);
    /** Get/Set the RecursiveScan mode to OFF if you don't want to scan DICOMs recursively in the InputDirectory */
    itkBooleanMacro(RecursiveScan);
    /** Get/Set the UseDicomDirFile mode to OFF if you explicitely don't want to use any existing DICOMDIR file */
    itkGetMacro (UseDicomDirFile, bool);
    /** Get/Set the UseDicomDirFile mode to OFF if you explicitely don't want to use any existing DICOMDIR file */
    itkSetMacro (UseDicomDirFile, bool);
    /** Get/Set the UseDicomDirFile mode to OFF if you explicitely don't want to use any existing DICOMDIR file */
    itkBooleanMacro(UseDicomDirFile);
    /** deprecated */
    itkGetMacro (PriorityRule, unsigned int);
    /** deprecated */
    itkSetClampMacro (PriorityRule, unsigned int, SPACE_DOMINANT, TIME_DOMINANT);
    /** Get/Set the InputDirectory, root directory where a DICOM exam can be found*/
    itkGetStringMacro (InputDirectory);
    /** Get/Set the InputDirectory, root directory where a DICOM exam can be found*/
    itkSetStringMacro (InputDirectory);
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
    virtual void Initialize()
    {
      this->PrepareOutputs();
    }
    /**
       Scan the directory (or the DICOMDIR file) respecting the strategies explained above,
       The scan might be slow, it is linearly dependant on the number of
       files present in the SetInputDirectory() (or the size of DICOMDIR file).
    */
    virtual void Scan();
    /**
       Restoring/Setting output states according to the helper.
    */
    virtual void InitializeOutputs();
    /**
       This method can be used to re-organise the outputs automatically.
       The strategy follows the following rules:

       1. If two UIDs appear to have the same name, they might contain the
       same object, but acquired in different slice position, given UIDs are
       then merged, only if they have the same orientation and are not too far in space.

       2. NOT IMPLEMENTED : some more specific flags can be checked for this merging/correlation process,
       such as the time delay, the diffusion gradient, etc.

       The user might use this method in case of 4D cardiac MRI acquisition containing multi-shots
       of the same volume.

       NB: no effect if called before Scan().
    */
    ///\todo implement the second rule
    virtual void AutoOrganization();
    /**
       This method can be used to re-organise the outputs automatically in a more restrictive way.
       The strategy follows an additionnal rule in comparison to AutoOrganization():

       Two images containing the same Image Position Patient will be separated
       into different volumes.

       The user might use this method in case of Diffusion Weighted MRI acquisitions,
       or T2 - proton density for instance.

       NB: no effect if called before Scan().
    */
    virtual void AutoOrganizationPositionBased();
    /**
       Explicitally removes the given output from the output list. It will release memory
       involved by this output if any.
    */
    virtual void RemoveDICOMImage (DICOMImagePointerType dicomimage);
    /**
       Returns true if the argument is contained in the output list
    */
    virtual bool HasOutput (DICOMImagePointerType dicomimage);

  protected:

    /** Overload of the ImageSource process */
    virtual void GenerateData();
    /**
       INTERNAL:

       Reorganise a filelist, according to the following rules:

       1. separate different image orientation sets.

       2. separate different image size sets.
    */
    virtual DICOMFileListMap MakeCoherent (DICOMFileList* filelist);
    /**
       INTERNAL:

       separate different Patient-Position file sets.
    */
    virtual DICOMFileListMap MakePositionCoherent (DICOMFileList* filelist);
    /**
       INTERNAL:

       separate different image size file sets.
    */
    virtual DICOMFileListMap MakeSizeCoherent (DICOMFileList* filelist);
    /**
       INTERNAL:

       For convenience: transpose a file set map
    */
    virtual DICOMFileListMap TransposeMap (DICOMFileListMap map);
    /**
       INTERNAL:

       Returns a file set map containing all the outputs' file sets.
    */
    virtual DICOMFileListMap GetGlobalDicomFileListMap();
    /**
       INTERNAL:

       Attemps to merge 2 file sets into one, considering two rules:

       1. If two UIDs appear to have the same name, they might contain the
       same object, but acquired in different slice position, given UIDs are
       then merged, only if they have the same orientation and are not too far in space.

       2. NOT IMPLEMENTED : some more specific flags can be checked for this merging/correlation process,
       such as the time delay, the diffusion gradient, etc.
    */
    ///\todo implement the second rule
    virtual bool AttemptMerge (DICOMFileList* list1, DICOMFileList* list2);
    /**
       INTERNAL:

       Generate an unused name using the UID series description from the
       first item in argument filelist.
       Returns a name.
    */
    virtual std::string GenerateName (DICOMFileList* filelist);
    /**
       Finds a file named filename in a directory named directory). returns joined path
       of the two arguments.

       1. It is case insensitive.

       2. If not found, the last character of the filename is substracted and
       a second case insensitive search is done.
    */
    virtual std::string FindFile (std::string directory, std::string filename);
    /**
       INTERNAL:

       Returns the total amount of image files contained in a DICOM exam explained
       in the DicomDir argument.
    */
    virtual unsigned long EstimateSizeOfDicomDir (DicomDirType* dicomdir);
    /**
       INTERNAL: DICOMDIR has been found, load it.
    */
    virtual void LoadFromDicomDirFile (const char* DicomDirFile);
    /**
       INTERNAL: Exhaustive scan of the directory.
    */
    virtual void LoadFromDirectory (const char* directory);
    /** default constructor */
    GDCMImporter2();
    /** default destructor, release memory */
    ~GDCMImporter2()
    {
      // Release the memory
      this->Initialize();
      delete m_DicomDir;
    }
    /**
       Prints information concerning the Importer,
       does not print all outputs inner informations.
    */
    void PrintSelf(std::ostream& os, Indent indent) const;

  private:
    GDCMImporter2(const Self&);
    void operator=(const Self&);

    std::string m_InputDirectory;
    bool m_RecursiveScan;
    bool m_UseDicomDirFile;
    unsigned int m_PriorityRule;

    typename GDCMScannerType::Pointer m_GDCMScanner;
    GDCMHelperType* m_GDCMHelper;
    DicomDirType* m_DicomDir;

  };

} // end of namespace itk



#ifndef ITK_MANUAL_INSTANTIATION
#include "itkGDCMImporter2.txx"
#endif





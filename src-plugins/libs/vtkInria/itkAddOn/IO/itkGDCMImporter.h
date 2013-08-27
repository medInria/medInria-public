/*=========================================================================

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
#include <itkImageRegionSplitterBase.h>

#include <itksys/SystemTools.hxx>

#include "gdcmScanner.h"

#include <sstream>
#include <vector>
#include <string>

namespace itk
{


  /**

     \class GDCMVolume
     \brief Class that represents an image made from DICOM files, inherits from itk::Image.
     \author Nicolas Toussaint

     \see GDCMImporter ImageSeriesReader Image
     \ingroup   ImageObjects
  */
  template <class TPixelType=unsigned short>
    class ITK_EXPORT GDCMVolume : public Image<TPixelType, 4>
    {
    public:
    /// ITK typedefs
    typedef GDCMVolume Self;
    typedef GDCMVolume ImageType;
    typedef Image<TPixelType, 3> SubImageType;
    typedef typename SubImageType::Pointer SubImagePointerType;
    typedef Image<TPixelType, 4> Superclass;
    typedef SmartPointer<Self> Pointer;
    typedef SmartPointer<const Self> ConstPointer;
    typedef typename Superclass::SpacingType SpacingType;
    typedef typename Superclass::RegionType RegionType;
    typedef typename Superclass::PointType PointType;
    typedef typename Superclass::DirectionType DirectionType;
    
    typedef std::vector<std::string> FileList;
    typedef std::map<std::string, FileList> FileListMapType;
    typedef Vector<double, 3> GradientType;
    typedef std::vector< GradientType > GradientsContainer;
    typedef std::pair <std::string, std::string> DicomEntry;
    typedef std::vector<DicomEntry> DicomEntryList;
    
    itkStaticConstMacro (ImageDimension, unsigned int, ImageType::ImageDimension);

    itkNewMacro  (Self);
    itkTypeMacro (GDCMVolume, Superclass);

    /**/// serie reader typedef, reading a serie of dicom file to reconstruct a volume image
    typedef ImageSeriesReader<SubImageType> SeriesReaderType;
    /** Reader typedef */
    typedef typename itk::ImageFileReader<SubImageType> ReaderType;
    /** Writer typedef */
    typedef typename itk::ImageFileWriter<ImageType> WriterType;    
    /** Writer typedef */
    typedef typename itk::ImageFileWriter<SubImageType> SubWriterType;   
    
    /**
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
    FileListMapType GetFileListMap() const
    { return this->m_FileListMap; }
    /**
       Set/Get the name of the image. This attribute can
       then be used for display purposes
    */
    itkGetStringMacro (Name);
    itkSetStringMacro (Name);
    /**
       Build() will actually fill the volume image considering
       the FileListMap (SetFileListMap())
    */
    void Build();
    /**
       Write the volume (or 4D volume) in a file
       This only depends on the FileMap currently present in the volume.
       a SeriesReaderType is used to sequencially reads all DICOM files
       that have been included in the each filemap item.
       Each 3D image is concatenated in a 4D image with a 4th spacing of 1.
    */
    void Write(std::string filename);
    /**
       Re-Initialize the volume.
       Release memory. The Build() method has to be re-called to
       be able to get the volume back.
    */
    void Initialize()
    {
      this->Superclass::Initialize();
      this->m_IsBuilt = 0;
    }
    /**
       Ask the filemap files if there is any gradient present,
       and if they are coherent (one null gradient and > 1 non-null gradients
    */
    bool IsVolumeDWIs();
    /**
       Write the gradient orientations in a file
    */
    void WriteGradients (std::string filename);

    unsigned int* GetSize() const;

    DicomEntryList GetDicomEntryList() const;
    
  protected:

    FileList MapToFileList (FileListMapType map) const;
    double Estimate4thSpacing (FileListMapType map) const;
    void CopyMetaDataFromSubImage(ImageType::Pointer image,
				  SubImagePointerType t_image,
				  FileListMapType map) const;
    
    /**
       default GDCMVolume constructor,
       empty filelist and image name.
    */
    GDCMVolume()
    {
      this->SetName ("image");
      m_IsBuilt = 0;
      m_SkipMeanDiffusivity = 0;
      m_MeanDiffusivitySkipped = 0;

      // 0018 9089 Gradient Direction Information
      m_GradientScanner.AddTag( gdcm::Tag(0x18,0x9089) );
    }
    ~GDCMVolume()
    {
      this->Initialize();
    }
    /**
       Print the image information, including the name and
       the DICOM dictionnary (displaying the dictionnary of the first
       file contained in GetFileListMap()).
    */
    void PrintSelf(std::ostream& os, Indent indent) const;

  private :
    GDCMVolume(const Self&);
    void operator=(const Self&);
    std::string m_Name;
    FileListMapType m_FileListMap;
    bool m_IsBuilt;
    bool m_SkipMeanDiffusivity;
    bool m_MeanDiffusivitySkipped;
    GradientsContainer m_Gradients;
    gdcm::Scanner m_GradientScanner;

  };
  

  class ITK_EXPORT ImageRegionSplitterByOutputs
  :public ImageRegionSplitterBase
  {
  public:
    /** Standard class typedefs. */
    typedef ImageRegionSplitterByOutputs Self;
    typedef Object                       Superclass;
    typedef SmartPointer< Self >         Pointer;
    typedef SmartPointer< const Self >   ConstPointer;
    
    /** Method for creation through the object factory. */
    itkNewMacro(Self);
    
    /** Run-time type information (and related methods). */
    itkTypeMacro(ImageRegionSplitterByOutputs, Object);
    
    /** Get the number of output to split the threads
     * Defaults to 0.
     */
    itkGetConstMacro(NumberOfOutputs, unsigned int);
    itkSetMacro(NumberOfOutputs, unsigned int);    
    
  protected:
    
    ImageRegionSplitterByOutputs()
    {
      this->m_NumberOfOutputs = 0;
    }
    
    virtual unsigned int GetNumberOfSplitsInternal(unsigned int dim,
						   const IndexValueType regionIndex[],
						   const SizeValueType regionSize[],
						   unsigned int requestedNumber) const
    {
      // determine the actual number of pieces that will be generated
      SizeValueType range = this->GetNumberOfOutputs();
      int valuesPerThread = (int)::vcl_ceil(range/(double)requestedNumber);
      int maxThreadIdUsed = (int)::vcl_ceil(range/(double)valuesPerThread) - 1;
      
      return maxThreadIdUsed + 1;
    }
    
    virtual unsigned int GetSplitInternal(unsigned int dim,
					  unsigned int i,
					  unsigned int numberOfPieces,
					  IndexValueType regionIndex[],
					  SizeValueType regionSize[]) const
    {
      // determine the actual number of pieces that will be generated
      SizeValueType range = this->GetNumberOfOutputs();
      int valuesPerThread = (int)::vcl_ceil(range/(double)numberOfPieces);
      int maxThreadIdUsed = (int)::vcl_ceil(range/(double)valuesPerThread) - 1;
      
      regionIndex[0] = 0;
      regionSize[0] = this->GetNumberOfOutputs();
      
      // Split the region
      if (i < maxThreadIdUsed)
      {
	regionIndex[0] += i*valuesPerThread;
	regionSize[0] = valuesPerThread;
      }
      if (i == maxThreadIdUsed)
      {
	regionIndex[0] += i*valuesPerThread;
	// last thread needs to process the "rest" dimension being split
	regionSize[0] = regionSize[0] - i*valuesPerThread;
      }  
      
      return maxThreadIdUsed + 1;
    }
    
    
    void PrintSelf(std::ostream & os, Indent indent) const
    {
      Superclass::PrintSelf(os, indent);
      os << indent << "NumberOfOutputs: " << m_NumberOfOutputs << std::endl;
    }    
    
  private:
    ImageRegionSplitterByOutputs(const ImageRegionSplitterByOutputs &); //purposely not implemented
    void operator=(const ImageRegionSplitterByOutputs &);      //purposely not implemented
    
    unsigned int m_NumberOfOutputs;
  };

  

  /**

     \class GDCMImporter
     \brief itkImageSource to manage DICOM files, reordering, building volumes
     \author Nicolas Toussaint

     \see GDCMImporter ImageSource
     \ingroup   DataSources

  */
  template <class TPixelType=unsigned short>
 class ITK_EXPORT GDCMImporter : public ImageSource < GDCMVolume <TPixelType> >
    {
    public:

    /** generic typedefs */
    typedef GDCMImporter Self;
    /** generic typedefs */
    typedef GDCMVolume <TPixelType> ImageType;
    /** generic typedefs */
    typedef ImageSource < GDCMVolume <TPixelType> > Superclass;
    /** generic typedefs */
    typedef SmartPointer<Self>       Pointer;
    /** generic typedefs */
    typedef SmartPointer<const Self> ConstPointer;
    
    /** generic constructors */
    itkNewMacro (Self);
    /** generic typedefs */
    itkTypeMacro (GDCMImporter, Superclass);
    /** ImageDimension typedef, dimension of the output Image(s) */
    itkStaticConstMacro (ImageDimension, unsigned int, ImageType::ImageDimension);
    /** Pixel typedef */
    typedef typename ImageType::PixelType PixelType;
    /** GDCMVolume typedef, corresponding to the output Image type */
    typedef typename ImageType::Pointer ImagePointerType;
    /** stl vector of DataObject, used for a fast access to outputs array  */
    typedef itk::ProcessObject::DataObjectPointerArray DataObjectPointerArray;
    typedef typename ImageType::RegionType OutputImageRegionType;

    typedef typename ImageType::FileList FileList;
    typedef typename ImageType::FileListMapType FileListMapType;
    typedef std::map<std::string, FileListMapType> FileListMapofMapType;
    typedef std::map<double, FileList> SortedMapType;

    typedef GDCMVolume <TPixelType> GDCMVolumeType;
    typedef GDCMVolumeType OutputImageType;
    
    /**
       Get/Set the InputDirectory,
       root directory where a DICOM exam can be found
    */
    itkGetStringMacro (InputDirectory);
    void SetInputDirectory (std::string d)
    {
      m_InputDirectory = d;
      m_IsScanned = 0;
      this->Modified();
    }
    /**
       Use this method to save a specific volume in a file
    */
    void WriteOutputInFile(unsigned int, std::string);
    /**
       Save all output images in a given directory
    */
    void WriteOutputsInDirectory(std::string);
    /**
       Scan the directory (or the DICOMDIR file) respecting the strategies explained above,
       The scan might be slow, it is linearly dependant on the number of
       files present in the SetInputDirectory().
       A cache system is used by the internal Scanner (from GDCM library). Thus
       a second call of Scan() will be faster.
    */
    void Scan();

    itkGetMacro (UsePhilipsPrivateTagRestrictions, unsigned int);
    itkSetClampMacro (UsePhilipsPrivateTagRestrictions, unsigned int, 0, 1);
    itkBooleanMacro (UsePhilipsPrivateTagRestrictions);

    itkGetMacro (WriteDictionaries, unsigned int);
    itkSetClampMacro (WriteDictionaries, unsigned int, 0, 1);
    itkBooleanMacro (WriteDictionaries);
    
    itkSetStringMacro (PreferredExtension);
    itkGetStringMacro (PreferredExtension);

    bool IsFileDWI (const char* filename);
    
  protected:

    /** Internal methods for mmultithreading. */
    
    virtual const ImageRegionSplitterBase* GetImageRegionSplitter(void) const;

    void ThreadedGenerateData(const OutputImageRegionType& outputRegionForThread, ThreadIdType threadId );
    void GenerateOutputs();
    /** default constructor */
    GDCMImporter();
    /** default destructor, release memory */
    ~GDCMImporter(){}
    void PrintSelf(std::ostream& os, Indent indent) const;

  private:
    GDCMImporter(const Self&);
    void operator=(const Self&);

    ImageRegionSplitterByOutputs::Pointer m_ImageRegionSplitter;

    unsigned int m_UsePhilipsPrivateTagRestrictions;

    unsigned int m_WriteDictionaries;
    
    std::string m_PreferredExtension;
    
    std::string m_InputDirectory;
    FileListMapofMapType m_FileListMapofMap;

    gdcm::Scanner m_FirstScanner;
    gdcm::Scanner m_SecondScanner;
    gdcm::Scanner m_ThirdScanner;

    bool m_IsScanned;
    /**
       Sorting system. divinded into 3 layers of sort.
    */
    /**
       First layer of sorting.
       From a list of files, the output map should have
       a list of "volumes" which might be 2D, 3D or 4D.
       list of discriminant tags :
       0x0010, 0x0010
       0x0020, 0x000d
       0x0020, 0x0037
       000020, 0x0011
       0x0018, 0x0050
       0x0028, 0x0010
       0x0028, 0x0011
       0x0018, 0x1312
       0x0008, 0x0008
    */
    FileListMapType PrimarySort (FileList list);
    /**
       Second/Third layer of sorting.
       second and third are supposed to differenciate within a 4D volume,
       list of discriminant tags :
       0018 0024
       0018 9087
       0018 9089
       0018 1060
    */
    FileListMapType SecondarySort (FileList list);
    /**
       Second/Third layer of sorting.
       second and third are supposed to differenciate within a 4D volume,
       list of discriminant tags :
       0020 0032
       0020 0037
    */
    FileListMapType TertiarySort (FileList list);
    FileListMapType TimeSort (FileListMapType map);
    FileListMapType InstanceNumberSort (FileList map);
    
    FileListMapType SimpleSort (FileList list);
    FileList Transpose (FileList list, unsigned int repetition);
    
    std::string GenerateUniqueName (FileListMapType map);
  };

} // end of namespace itk



#ifndef ITK_MANUAL_INSTANTIATION
#include "itkGDCMImporter.txx"
#endif





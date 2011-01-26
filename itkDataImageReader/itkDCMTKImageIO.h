#ifndef _itk_DCMTKImageIO_h_
#define _itk_DCMTKImageIO_h_

#ifdef _MSC_VER
#pragma warning ( disable : 4786 )
#endif

#include "itkMultiThreadedImageIOBase.h"
#include <itkMetaDataObject.h>

#include "itkDCMTKIOExport.h"

#include <map>
#include <vector>
#include <set>


class DcmElement;

namespace itk
{

  struct Vec3 
  {
      Vec3(double a, double b, double c)
      {
          x[0] = a;
          x[1] = b;
          x[2] = c;
      }

      double x[3];
  };


  class ITKDCMTKIO_EXPORT DCMTKImageIO : public MultiThreadedImageIOBase
  {

  public:
    typedef DCMTKImageIO             Self;
    typedef MultiThreadedImageIOBase Superclass;
    typedef SmartPointer<Self>       Pointer;
    typedef SmartPointer<const Self> ConstPointer;

    itkNewMacro (Self);
    itkTypeMacro(DCMTKImageIO, MultiThreadedImageIOBase);

    // some container typedefs
    typedef std::vector<std::string>                     StringVectorType;
    typedef std::pair<std::string,std::string>           StringPairType;
    typedef std::map<std::string, int>                   NameToIndexMapType;
    typedef std::map<int, std::string>                   IndexToNameMapType;
    typedef MetaDataObject <std::string>                 MetaDataStringType;
    typedef MetaDataObject <std::vector<std::string> >   MetaDataVectorStringType;

    typedef MultiThreadedImageIOBase::RegionType         RegionType;


    /**
    * CanReadFile - check if the file is DICOM and supported
    * @params: const char * - path to file
    * @return   bool - true if able to read
    */
    bool CanReadFile(const char*);
    
    
    /**
    * ReadImageInformation - parse for meta-information 
    */
    void ReadImageInformation();

    /**
    * CalculateImageProperties - calculate and set important image properties like dimension, pixel-spacing
    * @return   void
    */
    void CalculateImageProperties();

    /**
    * CanWriteFile - check if its possible to create new DICOM (not implemented)
    * @params: const char *
    * @return   bool - return true if able to write
    */
    bool CanWriteFile(const char*){return false;};

    /**
    * WriteImageInformation - write metadata (not implemented)
    */
    void WriteImageInformation(){};

    
    /**
    * Write - write the buffer (not implemented)
    * @params: const void * buffer
    */
    void Write(const void* buffer){};


    /**
    * GetAttribute - parse for 
    * @params: const std::string attribute
    * @return   itk::DCMTKImageIO::StringPairType
    */
    StringPairType GetAttribute(const std::string attribute) const;

    /**
    * GetOrderedFileNames - return list of pre-ordered filenames
    * @return   const StringVectorType& - list of files
    */
    const StringVectorType& GetOrderedFileNames (void) const;

   
    /**
    * GetMetaDataValueString - get the DICOM metadata if included in the file
    * Use the following format: ("group,elem") where group and elem are hex values of DICOM attributes.
    * @params: const char * key - key to look for
    * @params: int index - file index in case of multiple files, 0 in single file case
    * @return   std::string return String with result value
    */
    inline std::string GetMetaDataValueString (const char* key, int index) const;

   
    /**
    * GetMetaDataValueVectorString -get the DICOM metadata if included in the file
    * Use the following format: ("group,elem") where group and elem are hex values of DICOM attributes.
    * @params: const char * key - key to look for
    * @return   const StringVectorType& - return Stringlist with result value
    */
    inline const StringVectorType& GetMetaDataValueVectorString (const char* key) const;


    static double                    MAXIMUM_GAP;

    /** Override itk::MultiThreadedImageIOBase */
    virtual int SplitRequestedRegion (int id, int total, RegionType& region);

    /** Override itk::ImageIOBase */
    virtual ImageIORegion GenerateStreamableReadRegionFromRequestedRegion( const ImageIORegion & requested ) const;

    /** see super class for documentation
    *
    *   overridden to return true */
    virtual bool CanStreamRead( void );

  protected:
    
    DCMTKImageIO();
    ~DCMTKImageIO();

    void PrintSelf(std::ostream& os, Indent indent) const;
    void ThreadedRead (void* buffer, RegionType region, int threadId);
    void InternalRead (void* buffer, int slice, unsigned long pixelCount);

    std::vector<std::string>  SortSlices(const std::vector<std::string>& unorderdListOfFiles, std::multimap<double,std::string> *distanceMap );

    void DetermineNumberOfPixelComponents (void); //single
    void DeterminePixelType (void); //single
    void DetermineSpacing (void); // single and multi
    void DetermineDimensions (void); // single
    void DetermineOrigin (void); //multi only 
    void DetermineOrientation (void); // single
    void DetermineOrdering(void);
    
    // read the "DICOM-header"
    void ReadHeader( const std::string& name, const int& fileIndex, const int& fileCount );
    
    // get value from attribute and fill it into own dictionary
    inline void ReadDicomElement(DcmElement* element, const int &fileIndex, const int &fileCount );

    //helper
    inline StringPairType GetPairedMetadata( const char* metaName, const char* key) const;

    // return the number of slices
    int GetNumberOfSlices();

    // check if the image normal between to slices is the same
    bool SlicesAreParallel(int sliceIndex1, int sliceIndex2);

    /** Get the IPP vector converted to Vec3 */
    Vec3 GetPatientPositionPatientFromSlice(int index);

    /** Get the image normal computed from IOP */
    Vec3 GetImageNormalFromSlice(int index);

    /** Get the size of each pixel in bytes */
    unsigned int GetDcmComponentSize() const;

    /** Offset the given buffer pointer to point to the start of the slice given.*/
    void* GetDataPointerForSlice( void * buffer, int slice ) const;

    /** Get size of a single slice in bytes.*/
    size_t GetSliceSizeInBytes() const;
  private:

    DCMTKImageIO(const Self&);
    void operator=(const Self&);

    StringVectorType                    m_OrderedFileNames;
    std::string                         m_Directory;
    NameToIndexMapType                  m_FilenameToIndexMap;
    IndexToNameMapType                  m_IndexToFilenameMap;
    StringVectorType                    m_EmptyVector;

    bool m_DirectOrdering;


    //SliceLocationSetType             m_LocationSet;
    //SliceLocationToNamesMultiMapType m_LocationToFilenamesMap;
  };
  
  
} // end of namespace


#endif

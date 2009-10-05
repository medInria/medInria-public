#ifndef _itk_DCMTKImageIO_h_
#define _itk_DCMTKImageIO_h_

#ifdef _MSC_VER
#pragma warning ( disable : 4786 )
#endif

#include <map>
#include <vector>
#include <set>

#include "itkMultiThreadedImageIOBase.h"
#include <itkMetaDataObject.h>

#include "itkDataImageReaderPluginExport.h"

class DcmElement;


namespace itk
{

  class ITKDATAIMAGEREADERPLUGIN_EXPORT DCMTKImageIO : public MultiThreadedImageIOBase
  {

  public:
    typedef DCMTKImageIO             Self;
    typedef MultiThreadedImageIOBase Superclass;
    typedef SmartPointer<Self>       Pointer;
    typedef SmartPointer<const Self> ConstPointer;

    itkNewMacro (Self);
    itkTypeMacro(DCMTKImageIO, MultiThreadedImageIOBase);


    typedef std::map< std::string, std::vector< std::string > > StringMap;
    typedef std::map< std::string, std::vector< float > >       FloatMap;
    typedef std::map< std::string, std::vector< double > >      DoubleMap;
    typedef std::map< std::string, std::vector< int32_t > >     Int32Map;
    typedef std::map< std::string, std::vector< int16_t > >     Int16Map;
    typedef std::map< std::string, std::vector< uint32_t > >    UInt32Map;
    typedef std::map< std::string, std::vector< uint16_t > >    UInt16Map;

    typedef MetaDataObject <std::string>                 MetaDataStringType;
    typedef MetaDataObject < std::vector<std::string> >  MetaDataVectorStringType;
    typedef MetaDataObject <double>                      MetaDataDoubleType;
    typedef MetaDataObject < std::vector<double> >       MetaDataVectorDoubleType;
    typedef MetaDataObject <float>                       MetaDataFloatType;
    typedef MetaDataObject < std::vector<float> >        MetaDataVectorFloatType;
    typedef MetaDataObject <int32_t>                     MetaDataInt32Type;
    typedef MetaDataObject < std::vector<int32_t> >      MetaDataVectorInt32Type;
    typedef MetaDataObject <int16_t>                     MetaDataInt16Type;
    typedef MetaDataObject < std::vector<int16_t> >      MetaDataVectorInt16Type;
    typedef MetaDataObject <uint32_t>                    MetaDataUInt32Type;
    typedef MetaDataObject < std::vector<uint32_t> >     MetaDataVectorUInt32Type;
    typedef MetaDataObject <uint16_t>                    MetaDataUInt16Type;
    typedef MetaDataObject < std::vector<uint16_t> >     MetaDataVectorUInt16Type;

    typedef MultiThreadedImageIOBase::RegionType         RegionType;
    
    
    bool CanReadFile(const char*);
    
    
    void ReadImageInformation();
    

    bool CanWriteFile(const char*);
    

    void WriteImageInformation();

    
    void Write(const void* buffer);    


    // DICOM related stuff
    std::string GetPatientName() const;
    std::string GetPatientID() const;
    std::string GetPatientSex() const;
    std::string GetPatientAge() const;
    std::string GetStudyID() const;
    std::string GetPatientDOB() const;
    std::string GetStudyDescription() const;
    std::string GetSeriesDescription() const;
    std::string GetBodyPart() const;
    std::string GetNumberOfSeriesInStudy() const;
    std::string GetNumberOfStudyRelatedSeries() const;
    std::string GetStudyDate() const;
    std::string GetModality() const;
    std::string GetManufacturer() const;
    std::string GetInstitution() const;
    std::string GetModel() const;
    std::string GetScanOptions() const;
    

  protected:
    
    DCMTKImageIO();
    ~DCMTKImageIO();
    void PrintSelf(std::ostream& os, Indent indent) const {};

    void ThreadedRead (void* buffer, RegionType region, int threadId);    
    void InternalRead (void* buffer, int slice, unsigned long pixelCount, bool isJpeg=0);


    void SwapBytesIfNecessary(void* buffer, unsigned long numberOfPixels);


    void PopulateDictionary (StringMap& stringMap,
			     FloatMap& floatMap,
			     DoubleMap& doubleMap,
			     Int32Map& int32Map,
			     Int16Map& int16Map,
			     UInt32Map& uint32Map,
			     UInt16Map& uint16Map);

    void DeterminePixelType (void);
    void DetermineSpacing (void);
    void DetermineDimensions (void);
    void DetermineOrigin (void);
    void DetermineOrientation (void);
    

    void readCoreHeader( const std::string& name,
                         StringMap& stringMap,
                         FloatMap& floatMap,
                         DoubleMap& doubleMap,
                         Int32Map& int32Map,
                         Int16Map& int16Map,
                         UInt32Map& uint32Map,
                         UInt16Map& uint16Map,
                         int32_t fileIndex,
                         int32_t fileCount ) const;
    

    void readElement( ::DcmElement* element,
                      StringMap& stringMap,
                      FloatMap& floatMap,
                      DoubleMap& doubleMap,
                      Int32Map& int32Map,
                      Int16Map& int16Map,
                      UInt32Map& uint32Map,
                      UInt16Map& uint16Map,
                      int32_t fileIndex,
                      int32_t fileCount ) const;


    bool toScalar (const std::string& msg, float& value);
    bool toScalar (const std::string& msg, double& value);
    bool toScalar( const std::string& msg, int32_t& value, int32_t base = 10 );
    bool toScalar (const std::string& msg, int64_t& value, int32_t base = 10 );
    bool isSpace  (char);
    bool isOkInBase (char, int32_t);


    std::string GetMetaDataValueString (const char* key) const;
    std::vector<std::string> GetMetaDataValueVectorString (const char* key) const;
    
    
  private:
    DCMTKImageIO(const Self&);
    void operator=(const Self&);
    
    std::vector< std::string > m_OrderedFileNames;
    std::string                m_Directory;
  };
  
  
} // end of namespace


#endif

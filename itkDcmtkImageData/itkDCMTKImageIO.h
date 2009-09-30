#ifndef _itk_DCMTKImageIO_h_
#define _itk_DCMTKImageIO_h_

#ifdef _MSC_VER
#pragma warning ( disable : 4786 )
#endif

#include <map>
#include <vector>

#define HAVE_CONFIG_H
#include <dcmtk/dcmdata/dctk.h>
#include <dcmtk/dcmdata/dcistrmf.h>
#include <dcmtk/ofstd/ofstdinc.h>


#include <itkImageIOBase.h>
#include <itkMetaDataObject.h>
#include <itkMultiThreader.h>
#include <itkImageRegion.h>

#include <set>

namespace itk
{

  class ITK_EXPORT DCMTKImageIO : public ImageIOBase
  {

  public:
    typedef DCMTKImageIO       Self;
    typedef ImageIOBase        Superclass;
    typedef SmartPointer<Self> Pointer;
    typedef SmartPointer<const Self> ConstPointer;

    itkNewMacro (Self);
    itkTypeMacro(DCMTKImageIO, ImageIOBase);



    typedef std::map< std::string, std::vector< std::string > > StringMap;
    typedef std::map< std::string, std::vector< float > > FloatMap;
    typedef std::map< std::string, std::vector< double > > DoubleMap;
    typedef std::map< std::string, std::vector< int32_t > > Int32Map;
    typedef std::map< std::string, std::vector< int16_t > > Int16Map;
    typedef std::map< std::string, std::vector< uint32_t > > UInt32Map;
    typedef std::map< std::string, std::vector< uint16_t > > UInt16Map;

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

    typedef ImageRegion<1> RegionType;
    

    /**
       Overcome the SetFileName method to specify multiple dicom files.
     */
    virtual void SetFileNames (std::vector<std::string> filenames)
    { m_FileNames = filenames; }

    virtual std::vector<std::string> GetFileNames (void) const
    { return m_FileNames; }

    /*-------- This part of the interfaces deals with reading data. ----- */
    
    /** Determine the file type. Returns true if this ImageIO can read the
     * file specified. */
    virtual bool CanReadFile(const char*);
    
    /** Set the spacing and dimension information for the set filename. */
    virtual void ReadImageInformation();
    
    /** Reads the data from disk into the memory buffer provided. */
    virtual void Read(void* buffer);
    
    /*-------- This part of the interfaces deals with writing data. ----- */
    
    /** Determine the file type. Returns true if this ImageIO can write the
     * file specified. */
    virtual bool CanWriteFile(const char*);
    
    /** Set the spacing and dimension information for the set filename. */
    virtual void WriteImageInformation();
    
    /** Writes the data to disk from the memory buffer provided. Make sure
     * that the IORegions has been set properly. */
    virtual void Write(const void* buffer);    



    itkGetObjectMacro (MultiThreader, MultiThreader);

    itkSetMacro (NumberOfThreads, int);
    itkGetMacro (NumberOfThreads, int);
    
  protected:
    
    DCMTKImageIO();
    ~DCMTKImageIO();
    void PrintSelf(std::ostream& os, Indent indent) const {};

    void ThreadedRead (void* buffer, RegionType region, int threadId);
    
    void InternalRead (void* buffer, int slice, unsigned long pixelCount, bool isJpeg=0);


    static ITK_THREAD_RETURN_TYPE ThreaderCallback( void *arg );
    
    struct ThreadStruct
    {
      Pointer Reader;
      void*   Buffer;
    };

    int SplitRequestedRegion (int id, int total, RegionType& region);


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


    
    
  private:
    DCMTKImageIO(const Self&);
    void operator=(const Self&);
    

    std::vector< std::string > m_FileNames;
    std::vector< std::string > m_OrderedFileNames;
    std::string                m_Directory;


    MultiThreader::Pointer m_MultiThreader;
    int                    m_NumberOfThreads;
  };



  itkEventMacro (SliceReadEvent, AnyEvent);
  
  
} // end of namespace


#endif

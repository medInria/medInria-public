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
//#define INCLUDE_CSTDLIB
//#define INCLUDE_CSTRING
#include <dcmtk/ofstd/ofstdinc.h>


#include <itkImageIOBase.h>
#include <itkMetaDataObject.h>

#include <set>

namespace itk
{

  class ITK_EXPORT DCMTKImageIO : public ImageIOBase
  {

  public:
    typedef DCMTKImageIO         Self;
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
    
    DCMTKImageIO();
    ~DCMTKImageIO();
    void PrintSelf(std::ostream& os, Indent indent) const {};
    
  private:
    DCMTKImageIO(const Self&);
    void operator=(const Self&);
    
    void SwapBytesIfNecessary(void* buffer, unsigned long numberOfPixels);


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


    std::vector< std::string > m_FileNames;
    std::vector< std::string > m_OrderedFileNames;
    
  };



  itkEventMacro (SliceReadEvent, AnyEvent);
  
  
} // end of namespace


#endif

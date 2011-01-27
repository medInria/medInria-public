//#define __STDC_LIMIT_MACROS
//#define __STDC_CONSTANT_MACROS

#include "itkDCMTKImageIO.h"

#ifndef WIN32
#define HAVE_CONFIG_H
#endif

#include <dcmtk/config/osconfig.h>

#include <dcmtk/dcmdata/dctk.h>
#include <dcmtk/dcmdata/dcistrmf.h>
#include <dcmtk/ofstd/ofstdinc.h>
#include <dcmtk/dcmimgle/dcmimage.h>

/* The path separator for this platform.  */
#if defined(_WIN32) && !defined(__CYGWIN__)
# define ITK_FORWARD_PATH_SLASH '\\'
#else
# define ITK_FORWARD_PATH_SLASH '/'
#endif

#include <itksys/SystemTools.hxx>
#include <itksys/Directory.hxx>
#include <itkByteSwapper.h>
#include <itkMetaDataDictionary.h>
#include <itkSemaphore.h>
#include <itkMutexLockHolder.h>

#include <dcmtk/dcmdata/dcrledrg.h>
#include <dcmtk/dcmjpeg/djdecode.h>
#include <dcmtk/dcmdata/dcstack.h>

#include <vnl/vnl_vector.h>
#include <vnl/vnl_cross.h>


#include <float.h>

#include <fstream>

// anonymous namespace to protect scope of statics.
namespace
{
    int                         DCMTKImageIO_instanceCount (0);
    itk::SimpleFastMutexLock    DCMTKImageIO_cleanupLock;
} // anonymous namespace

//---------------------------------------------------------------------------------------------

namespace itk
{

double DCMTKImageIO::MAXIMUM_GAP = 999999;

DCMTKImageIO::DCMTKImageIO()
{

    this->SetNumberOfDimensions(3);
    this->SetNumberOfComponents (1);
    this->SetPixelType (SCALAR);
    this->SetComponentType (CHAR);

    if (ByteSwapper<int>::SystemIsBigEndian())
    {
        m_ByteOrder = BigEndian;
    }
    else
    {
        m_ByteOrder = LittleEndian;
    }

    m_Directory = "";

    {  // Scope the mutex locker
        itk::MutexLockHolder <itk::SimpleFastMutexLock> cleanupLocker ( DCMTKImageIO_cleanupLock);
        if ( DCMTKImageIO_instanceCount == 0 ) {
            DcmRLEDecoderRegistration::registerCodecs();
            DJDecoderRegistration::registerCodecs();
        }
        ++DCMTKImageIO_instanceCount;    // Mutex protected increment
    }

    m_DirectOrdering = true;

}

//---------------------------------------------------------------------------------------------


DCMTKImageIO::~DCMTKImageIO()
{
    {  // Scope the mutex locker
        itk::MutexLockHolder <itk::SimpleFastMutexLock> cleanupLocker ( DCMTKImageIO_cleanupLock);
        --DCMTKImageIO_instanceCount;  // Mutex protected decrement.
        if ( DCMTKImageIO_instanceCount == 0 ) {
            DcmRLEDecoderRegistration::cleanup();
            DJDecoderRegistration::cleanup();
        }
    }
}

//---------------------------------------------------------------------------------------------

void DCMTKImageIO::PrintSelf (std::ostream& os, Indent indent) const
{
    Superclass::PrintSelf (os, indent);
}

//---------------------------------------------------------------------------------------------

bool DCMTKImageIO::CanReadFile(const char* filename)
{
    DcmFileFormat dicomFile;
    OFCondition condition = dicomFile.loadFile( filename );
    if ( !condition.good() )
    {
        return false;
    }

    E_TransferSyntax xfer = dicomFile.getDataset()->getOriginalXfer();

    if( xfer == EXS_JPEG2000LosslessOnly ||
        xfer == EXS_JPEG2000 ||
        xfer == EXS_JPEG2000MulticomponentLosslessOnly ||
        xfer == EXS_JPEG2000Multicomponent )
    {
        return false;
    }

    // search for mandatory tags
    DcmStack stack;
    DcmTagKey searchKey;
    unsigned int group = 0x0028; // samples per pixel
    unsigned int elem  = 0x0002; // samples per pixel

    searchKey.set(group, elem);    
    if (dicomFile.search(searchKey, stack, ESM_fromHere, OFTrue) != EC_Normal)
        return false;

    group = 0x0028; // pixel type
    elem  = 0x0100; // pixel type
    searchKey.set(group, elem);    
    if (dicomFile.search(searchKey, stack, ESM_fromHere, OFTrue) != EC_Normal)
        return false;

    return true;
}

//---------------------------------------------------------------------------------------------


void DCMTKImageIO::ReadImageInformation()
{

    // parse the DICOM header of each file and store all fields in the Dictionary
    FileNameVectorType fileNames = this->GetFileNames();
    int fileCount = (int)( fileNames.size() );
    int fileIndex =0;
    m_FilenameToIndexMap.clear();
    m_IndexToFilenameMap.clear();

    for (FileNameVectorType::const_iterator iter = fileNames.begin(); iter != fileNames.end(); iter++)
    {
        std::string filename;
        if( m_Directory != "" )
            filename = m_Directory + ITK_FORWARD_PATH_SLASH + (*iter);
        else
            filename = (*iter);
        try
        {
            this->ReadHeader( filename, fileIndex, fileCount );
            m_FilenameToIndexMap.insert(std::pair<std::string, int>(filename, fileIndex));
            m_IndexToFilenameMap.insert(std::pair<int, std::string>(fileIndex, filename));
            ++fileIndex;
        }
        catch (ExceptionObject &e)
        {
            std::cerr << e; // continue to be robust to odd files
        }
    }

    // calculate vital image properties (dimension, pixel-spacing etc)
    this->CalculateImageProperties();

}

//---------------------------------------------------------------------------------------------

void DCMTKImageIO::DetermineNumberOfPixelComponents()
{
    const StringVectorType &pixelComponentCountVec = this->GetMetaDataValueVectorString ("(0028,0002)");
    if ( !pixelComponentCountVec.size() )
    {
        itkWarningMacro(<< "Tag (0028,0002) (SamplesPerPixel) not found" );
    }

    std::istringstream s_stream ( pixelComponentCountVec[0].c_str() );
    int samplesPerPixel = 1;
    if ( !(s_stream >> samplesPerPixel) )
    {
        itkWarningMacro( << "Cannot convert string to int: " << pixelComponentCountVec[0].c_str()
            << "assuming 1 component per pixel." );
        this->SetNumberOfComponents (1);
    }


    this->SetNumberOfComponents ( samplesPerPixel );

    if( samplesPerPixel==1 )
    {
        this->SetPixelType ( SCALAR );
    }
    else
    {
        this->SetPixelType ( RGB );
    }

}

//---------------------------------------------------------------------------------------------

void DCMTKImageIO::DeterminePixelType()
{
    const StringVectorType &bitsAllocatedVec = this->GetMetaDataValueVectorString ("(0028,0100)");
    if ( !bitsAllocatedVec.size() )
    {
        itkExceptionMacro ( << "Tag (0028,0100) (Pixel Type) not found" );
    }

    std::istringstream s_stream ( bitsAllocatedVec[0].c_str() );
    int bitsAllocated = 0;
    if ( !(s_stream >> bitsAllocated) )
        itkExceptionMacro ( << "Cannot convert string to int: " << bitsAllocatedVec[0].c_str() );


    const StringVectorType &signBitsVec = this->GetMetaDataValueVectorString ("(0028,0103)");
    std::string sign = "0";
    if (signBitsVec.size())
    {
        sign = signBitsVec[0];
    }
    else
    {
        itkWarningMacro( << "Missing Pixel Representation (0028,0103), assuming unsigned" );
    }

    if ( sign == "0" )
    {
        sign = "U";
    }
    else
    {
        sign = "S";
    }

    if( bitsAllocated == 8 && sign=="U" )
    {
        this->SetComponentType ( UCHAR );
    }
    else if ( bitsAllocated == 8 && sign=="S" )
    {
        this->SetComponentType ( CHAR );
    }
    else if ( bitsAllocated == 16 && sign=="U")
    {
        this->SetComponentType ( USHORT );
    }
    else if ( bitsAllocated == 16 && sign=="S")
    {
        this->SetComponentType ( SHORT );
    }
    else if ( bitsAllocated == 32 && sign=="U")
    {
        this->SetComponentType ( UINT );
    }
    else if ( bitsAllocated == 32 && sign=="S")
    {
        this->SetComponentType ( INT );
    }
    else if (bitsAllocated == 64 )
    {
        this->SetComponentType(DOUBLE);
    }
    else
        this->SetComponentType (UNKNOWNCOMPONENTTYPE);

}

//---------------------------------------------------------------------------------------------

void DCMTKImageIO::DetermineSpacing()
{
    for (int i = 0; i< GetNumberOfDimensions(); i++)
    {
        SetSpacing(i,1.0);
    }

    const StringVectorType &pixSpacingVec = this->GetMetaDataValueVectorString ("(0028,0030)");
    if ( !pixSpacingVec.size() )
    {
        itkWarningMacro( << "Tag (0028,0030) (PixelSpacing) was not found, assuming 1.0/1.0" );
    }
    else
    {
        std::string pixSpacingStr = pixSpacingVec[0];
        std::istringstream is_stream( pixSpacingStr );
        if (!(is_stream >> m_Spacing[0]))
        {
            itkWarningMacro( << "Cannot convert string to double: " << pixSpacingStr.c_str() );
        }
        if (!(is_stream >> m_Spacing[1]))
        {
            itkWarningMacro( << "Cannot convert string to double: " << pixSpacingStr.c_str() );
        }
    }


    if(GetNumberOfSlices() > 1)
    {

        // take only the first two slices into account
        std::multimap<double,std::string> distanceMap;
        this->SortSlices(this->GetFileNames(), &distanceMap);
 
        // calculate zspacing
        double zSpacing = 0;
        std::multimap<double, std::string >::iterator it = distanceMap.begin();
        double dist1, dist2;
        try
        {
            dist1 = (*it).first;
            it++;
            dist2 = (*it).first;
            zSpacing = abs(dist2 - dist1);
        }
        catch(std::exception e)
        {
            itkExceptionMacro(<<"Exception computing zspacing" << e.what());
            zSpacing = 1.0;
        }


        // since we can only work with 3D images, we need to make sure that the spacing is valid
        if (zSpacing == 0)
            zSpacing = 1.0;

        // finally setting the z-spacing
        SetSpacing(2,zSpacing);
    }


}

//---------------------------------------------------------------------------------------------

void DCMTKImageIO::DetermineDimensions()
{
    const StringVectorType &dimXVec = this->GetMetaDataValueVectorString ("(0028,0011)");
    if (!dimXVec.size())
        itkExceptionMacro ( << "Tag (0028,0011) (dim X) not found" );

    std::string dimXStr = dimXVec[0];
    std::istringstream is_streamX (dimXStr.c_str());
    if (!(is_streamX>>m_Dimensions[0]))
        itkExceptionMacro ( << "Cannot convert string to int: " << dimXStr.c_str()  << "\n");


    const StringVectorType &dimYVec = this->GetMetaDataValueVectorString ("(0028,0010)");
    if (!dimYVec.size())
        itkExceptionMacro ( << "Tag (0028,0010) (dim Y) not found" );

    std::string dimYStr = dimYVec[0];
    std::istringstream is_streamY (dimYStr.c_str());
    if (!(is_streamY>>m_Dimensions[1]))
        itkExceptionMacro ( << "Cannot convert string to int: " << dimYStr.c_str() << "\n" );


    // not taking multiple volumes into account at the moment
    int numberOfSlices = this->GetNumberOfSlices();
    // we have to set it at least to 3 dimensions since the datatype assumes this
    this->SetNumberOfDimensions(3);
    SetDimensions(2,numberOfSlices);


}

//---------------------------------------------------------------------------------------------

void DCMTKImageIO::DetermineOrigin()
{
    for(int i = 0; i < GetNumberOfDimensions();i++)
    {
        SetOrigin(i,0);
    }

    if( this->GetNumberOfDimensions()==4 )
    {
        m_Origin [3] = 0.0;
    }

    std::string s_origin = this->GetMetaDataValueString("(0020,0032)", 0); // first slice of series

    if ( s_origin=="" )
    {
        itkWarningMacro( "Tag (0020,0032) (ImageOrigin) was not found, assuming 0.0/0.0/0.0" );
        return;
    }

    std::istringstream is_stream( s_origin.c_str() );
    if (!(is_stream >> m_Origin[0]))
    {
        itkWarningMacro( "Cannot convert string to double: " << s_origin.c_str() );
    }
    if (!(is_stream >> m_Origin[1]))
    {
        itkWarningMacro( "Cannot convert string to double: " << s_origin.c_str() );
    }
    if (GetNumberOfDimensions() > 2)
    {
        if (!(is_stream >> m_Origin[2]))
        {
            itkWarningMacro( "Cannot convert string to double: " << s_origin.c_str() );
        }
    }

}

//---------------------------------------------------------------------------------------------

void DCMTKImageIO::DetermineOrientation()
{
    double orientation[6]={1.0, 0.0, 0.0, 0.0, 1.0, 0.0};
    const StringVectorType &orientationVec = this->GetMetaDataValueVectorString("(0020,0037)");
    if (!orientationVec.size())
    {
        itkWarningMacro( << "Tag (0020,0037) (PatientOrientation) was not found, assuming identity" );
    }
    else
    {
        std::string orientationStr = orientationVec[0];
        std::istringstream is_stream( orientationStr.c_str() );
        for( int i=0; i<6; i++ )
        {
            if (!(is_stream >> orientation[i]) )
            {
                itkWarningMacro( << "Cannot convert string to double: " << orientationStr.c_str() );
            }
        }
    }

    vnl_vector<double> rowDirection(3), columnDirection(3);
    rowDirection[0] = orientation[0];
    rowDirection[1] = orientation[1];
    rowDirection[2] = orientation[2];

    columnDirection[0] = orientation[3];
    columnDirection[1] = orientation[4];
    columnDirection[2] = orientation[5];

    vnl_vector<double> sliceDirection = vnl_cross_3d(rowDirection, columnDirection);

    this->SetDirection (0, rowDirection);
    this->SetDirection (1, columnDirection);
    if ( GetNumberOfDimensions() > 2 )
        this->SetDirection (2, sliceDirection);

    if( this->GetNumberOfDimensions()==4 )
    {
        m_Direction[0][3] = 0.0;
        m_Direction[1][3] = 0.0;
        m_Direction[2][3] = 0.0;
        m_Direction[3][3] = 1.0;
        m_Direction[3][0] = 0.0;
        m_Direction[3][1] = 0.0;
        m_Direction[3][2] = 0.0;
    }
}

//---------------------------------------------------------------------------------------------

const DCMTKImageIO::StringVectorType& DCMTKImageIO::GetOrderedFileNames( void ) const
{
    return m_OrderedFileNames;
}

//---------------------------------------------------------------------------------------------

void DCMTKImageIO::ThreadedRead (void* buffer, RegionType region, int threadId)
{
    unsigned long pixelCount = m_Dimensions[0] * m_Dimensions[1];

    int start = region.GetIndex()[0];
    int length = region.GetSize()[0];

    for( int i=start; i<start+length; i++)
    {
        this->InternalRead (buffer, i, pixelCount);
        if( threadId==0 )
        {
            this->SetProgress( (double)(i-start+1)/(double)(length) );
            this->InvokeEvent ( ProgressEvent() );
        }
    }
}

//---------------------------------------------------------------------------------------------

void DCMTKImageIO::InternalRead (void* buffer, int slice, unsigned long pixelCount)
{
    std::string filename;
    if( m_Directory=="" )
        filename = m_OrderedFileNames[slice];
    else
        filename = m_Directory + ITK_FORWARD_PATH_SLASH + m_OrderedFileNames[slice];

    DcmFileFormat dicomFile;
    DcmStack      stack;


    OFCondition cond = dicomFile.loadFile(filename.c_str(), EXS_Unknown, EGL_noChange, DCM_MaxReadLength, ERM_autoDetect);
    if (! cond.good())
    {
        itkExceptionMacro (<< cond.text() );
    }

    E_TransferSyntax xfer = dicomFile.getDataset()->getOriginalXfer();

    if( xfer == EXS_JPEG2000LosslessOnly ||
        xfer == EXS_JPEG2000 ||
        xfer == EXS_JPEG2000MulticomponentLosslessOnly ||
        xfer == EXS_JPEG2000Multicomponent )
    {
        itkExceptionMacro("Jpeg2000 encoding not supported yet.");
    }


    const Uint8* copyBuffer = 0;
    dicomFile.getDataset()->findAndGetUint8Array (DCM_PixelData, copyBuffer);

    Uint8* destBuffer = (Uint8*)(GetDataPointerForSlice(buffer, slice));
    if (!copyBuffer || !destBuffer)
    {
        itkExceptionMacro ( << "Bad copy or dest buffer" );
    }

    const size_t sliceSizeInBytes = GetSliceSizeInBytes ();
    std::memcpy (destBuffer, copyBuffer, sliceSizeInBytes);

}

//---------------------------------------------------------------------------------------------

unsigned int DCMTKImageIO::GetDcmComponentSize() const
{
    switch( this->GetComponentType() )
    {
    case CHAR:
        return sizeof(char);

    case UCHAR:
        return sizeof(Uint8);

    case SHORT:
        return sizeof(Sint16);

    case USHORT:
        return sizeof(Uint16);

    case INT:
        return sizeof(Sint32);

    case UINT:
        return sizeof(Uint32);

    case DOUBLE:
        return sizeof(Float64);

    default:
        throw ExceptionObject (__FILE__,__LINE__,"Unsupported pixel data type in DICOM");
    }
    // Should never get here.
    return 0;
}

size_t DCMTKImageIO::GetSliceSizeInBytes() const
{
    const size_t pixelCount = m_Dimensions[0] * m_Dimensions[1];
    const size_t singleSliceSize = pixelCount
        * this->GetDcmComponentSize()
        * this->GetNumberOfComponents();
    return singleSliceSize;
}

void* DCMTKImageIO::GetDataPointerForSlice( void * buffer, int slice ) const
{
    // Refuse to offset a null pointer.
    if ( !buffer )
        return NULL;

    const size_t singleSliceSize = this->GetSliceSizeInBytes();

    const ImageIORegion &ioRegion (GetIORegion());

    // If we have more than 2 dimensions, then the slice origin for the buffer may be offset.
    int slice0;
    if ( ioRegion.GetImageDimension () > 2 ) {
        slice0 = ioRegion.GetIndex( 2 );
    } else {
        slice0 = 0;
    }
    const size_t sliceCountOffset = slice - slice0;

    // So the offset in bytes is...
    const size_t offset = singleSliceSize * sliceCountOffset;

    // We cannot portably offset a void pointer, must cast to a byte type [assume char == byte] , increment, and cast back.
    return (static_cast<char*>(buffer) + offset);
}

//---------------------------------------------------------------------------------------------

void DCMTKImageIO::ReadHeader(const std::string& name, const int& fileIndex, const int& fileCount )
{

    DcmFileFormat dicomFile;
    OFCondition condition = dicomFile.loadFile( name.c_str() );

    // checking that given file is available
    if ( !condition.good() )
    {
        itkExceptionMacro ( << condition.text() );
    }

    // reading meta info
    DcmMetaInfo* metaInfo = dicomFile.getMetaInfo();
    for ( unsigned long e = 0; e < metaInfo->card(); e++ )
    {
        DcmElement* element = metaInfo->getElement( e );
        DcmPixelData* pixelData = dynamic_cast<DcmPixelData*>(element);
        if (!pixelData) // don't want to read PixData right now
        {
            this->ReadDicomElement( element, fileIndex, fileCount );
        }
    }

    // reading data set
    DcmDataset* dataSet = dicomFile.getDataset();
    for ( unsigned long e = 0; e < dataSet->card(); e++ )
    {
        DcmElement* element = dataSet->getElement( e );

        DcmPixelData* pixelData = dynamic_cast<DcmPixelData*>(element);
        if (!pixelData) // don't want to read PixData right now
        {
            this->ReadDicomElement( element, fileIndex, fileCount );
        }
    }

}

//---------------------------------------------------------------------------------------------

inline void DCMTKImageIO::ReadDicomElement(DcmElement* element, const int &fileIndex, const int &fileCount )
{

    DcmTag &dicomTag = const_cast<DcmTag &>(element->getTag());

    // create a name using the group/element
    Uint16 tagGroup   = dicomTag.getGTag();
    Uint16 tagElement = dicomTag.getETag();
    std::ostringstream oss;
    oss << '(' << std::hex << std::setw( 4 ) << std::setfill( '0' )<< tagGroup << ','
        << std::hex << std::setw( 4 ) << std::setfill( '0' ) << tagElement << ")";
    std::string tagKey = oss.str();

    // get the tag as string
    OFString ofstring;
    OFCondition cond = element->getOFStringArray (ofstring, 0);
    if ( !cond.good() )
    {
        return;
    }

    // we are removing trailing slashes and replace them with whitespace (is this good?)
    std::string s_value = ofstring.c_str();
    std::replace(s_value.begin(), s_value.end(), '\\', ' ');

    MetaDataDictionary& dicomDictionary = this->GetMetaDataDictionary();
    MetaDataDictionary::Iterator it = dicomDictionary.Find (tagKey);
    if (it!=dicomDictionary.End())
    {
        MetaDataVectorStringType* vec = dynamic_cast<MetaDataVectorStringType*>( it->second.GetPointer() );
        StringVectorType& value = const_cast< StringVectorType& >(vec->GetMetaDataObjectValue());
        value[fileIndex] = s_value;
    }
    else
    {
        StringVectorType vec (fileCount, "");
        vec[fileIndex] = s_value;
        EncapsulateMetaData< StringVectorType >(dicomDictionary, tagKey, vec);
    }
}

//---------------------------------------------------------------------------------------------

DCMTKImageIO::StringPairType DCMTKImageIO::GetPairedMetadata( const char* metaName, const char* key) const
{
    StringPairType pairObj;
    pairObj.first  = std::string(metaName);
    pairObj.second = GetMetaDataValueString (key, 0);
    return pairObj;
}

//---------------------------------------------------------------------------------------------

std::string DCMTKImageIO::GetMetaDataValueString( const char* key, int index ) const
{
    std::string value = "";
    const MetaDataDictionary& dicomDictionary = this->GetMetaDataDictionary();
    MetaDataDictionary::ConstIterator it = dicomDictionary.Find ( key );
    if( it!=dicomDictionary.End() )
    {
        if( MetaDataVectorStringType* metaData = dynamic_cast<MetaDataVectorStringType*>( it->second.GetPointer() ) )
        {
            value = metaData->GetMetaDataObjectValue()[index];
        }
    }
    return value;
}

//---------------------------------------------------------------------------------------------

const DCMTKImageIO::StringVectorType& DCMTKImageIO::GetMetaDataValueVectorString( const char* key ) const
{
    const MetaDataDictionary& dicomDictionary = this->GetMetaDataDictionary();
    MetaDataDictionary::ConstIterator it = dicomDictionary.Find ( key );
    if( it!=dicomDictionary.End() )
    {
        if( MetaDataVectorStringType* metaData = dynamic_cast<MetaDataVectorStringType*>( it->second.GetPointer() ) )
        {
            return metaData->GetMetaDataObjectValue();
        }
        else
        {
            return m_EmptyVector;
        }
    }
    return m_EmptyVector;
}

//---------------------------------------------------------------------------------------------

DCMTKImageIO::StringPairType DCMTKImageIO::GetAttribute( const std::string attribute ) const
{
    return GetPairedMetadata(attribute.c_str(), attribute.c_str());
}

//---------------------------------------------------------------------------------------------

void DCMTKImageIO::CalculateImageProperties()
{
    this->DetermineNumberOfPixelComponents();
    this->DeterminePixelType();
    this->DetermineDimensions();
    this->DetermineOrigin();
    this->DetermineOrientation();
    this->DetermineOrdering();
    this->DetermineSpacing();
    this->DetermineOrdering();

}

//---------------------------------------------------------------------------------------------

int DCMTKImageIO::GetNumberOfSlices()
{
    return this->GetFileNames().size(); // this does not take multiple volumes at all
}

//---------------------------------------------------------------------------------------------

void DCMTKImageIO::DetermineOrdering( void )
{


    //std::vector<std::string> ordered  = this->GetFileNames(); // no ordering performed here
    std::multimap<double,std::string> distanceMap;
    std::vector<std::string> ordered = SortSlices(this->GetFileNames(), &distanceMap);
    m_OrderedFileNames.clear();
    for(int i = 0; i < ordered.size(); i++)
    {
        m_OrderedFileNames.push_back(ordered.at(i));
    }

}

//---------------------------------------------------------------------------------------------

std::vector<std::string> DCMTKImageIO::SortSlices(const std::vector<std::string>& unorderdListOfFiles, std::multimap<double,std::string> *distanceMap)
{

    std::vector<std::string> orderedList;
    distanceMap->clear();

    if (unorderdListOfFiles.size() == 0)
        return orderedList;

    if (unorderdListOfFiles.size() == 1)
    {
        orderedList.push_back(unorderdListOfFiles.at(0));
        return orderedList;
    }

    Vec3 normal(0,0,0);
    Vec3 ipp(0,0,0);
    double dist, min, max;

    for(int sliceIndex=0; sliceIndex < unorderdListOfFiles.size(); sliceIndex++)
    {

        if ( sliceIndex == 0) // first slice
        {
            normal = GetImageNormalFromSlice(0);

            ipp = GetPatientPositionPatientFromSlice(sliceIndex);

            dist = 0;
            for ( int i = 0; i < 3; ++i )
            {
                // dot product to get distance
                dist += normal.x[i]*ipp.x[i];
            }

            distanceMap->insert(std::pair<double,std::string>(dist, unorderdListOfFiles.at(sliceIndex)));
            max = min = dist;
        }
        else
        {
            ipp = GetPatientPositionPatientFromSlice(sliceIndex);

            dist = 0;
            for ( int i = 0; i < 3; ++i )
            {
                // dot product to get distance
                dist += normal.x[i]*ipp.x[i];
            }
            distanceMap->insert(std::pair<double,std::string>(dist, unorderdListOfFiles.at(sliceIndex)));
            min = (min < dist) ? min : dist;
            max = (max > dist) ? max : dist;
        }
    }

    // Find out if min/max are coherent
    if ( min == max )
    {
        itkWarningMacro( << "Looks like all images have the exact same image position."
                         << "No PositionPatientOrdering sort performed" );
    }

    if(m_DirectOrdering)
    {
        for (std::multimap<double, std::string >::iterator it = distanceMap->begin(); it != distanceMap->end(); ++it)
        {
            orderedList.push_back( (*it).second );
        }
    }
    else
    {
        for (std::multimap<double, std::string >::reverse_iterator it = distanceMap->rbegin(); it != distanceMap->rend(); ++it)
        {
            orderedList.push_back( (*it).second );
        }
    }

    return orderedList;
}

//---------------------------------------------------------------------------------------------

bool DCMTKImageIO::SlicesAreParallel( int sliceIndex1, int sliceIndex2 )
{
    bool parallel = false;
    Vec3 normal1Vec = GetImageNormalFromSlice(sliceIndex1);
    Vec3 normal2Vec = GetImageNormalFromSlice(sliceIndex2);

    parallel = ((fabs(normal1Vec.x[0]-normal2Vec.x[0]) < 0.001)
             && (fabs(normal1Vec.x[1]-normal2Vec.x[1]) < 0.001)
             && (fabs(normal1Vec.x[2]-normal2Vec.x[2]) < 0.001));
    return parallel;
}

//---------------------------------------------------------------------------------------------

Vec3 DCMTKImageIO::GetImageNormalFromSlice( int index )
{
    double orientation[6]={1.0, 0.0, 0.0, 0.0, 1.0, 0.0};
    const StringVectorType &orientationVec = this->GetMetaDataValueVectorString("(0020,0037)");
    if (!orientationVec.size())
    {
        itkWarningMacro( << "Tag (0020,0037) (PatientOrientation) was not found, assuming identity" );
    }
    else
    {
        std::string orientationStr = orientationVec[index];
        std::istringstream is_stream( orientationStr.c_str() );
        for( int i=0; i<6; i++ )
        {
            if (!(is_stream >> orientation[i]) )
            {
                itkWarningMacro( << "Cannot convert string to double: " << orientationStr.c_str() );
            }
        }
    }

    // cross product to get normal
    double normal[3];
    normal[0] = orientation[1]*orientation[5] - orientation[2]*orientation[4];
    normal[1] = orientation[2]*orientation[3] - orientation[0]*orientation[5];
    normal[2] = orientation[0]*orientation[4] - orientation[1]*orientation[3];

    return Vec3(normal[0], normal[1],normal[2]);
}

//---------------------------------------------------------------------------------------------

Vec3 DCMTKImageIO::GetPatientPositionPatientFromSlice( int index )
{
    Vec3 IPP(0,0,0);
    std::string s_position = this->GetMetaDataValueString("(0020,0032)", index);
    std::istringstream is_stream( s_position.c_str() );
    is_stream >> IPP.x[0];
    is_stream >> IPP.x[1];
    is_stream >> IPP.x[2];

    return IPP;
}

//---------------------------------------------------------------------------------------------

int DCMTKImageIO::SplitRequestedRegion (int id, int total, RegionType& region)
{
    const ImageIORegion &ioRegion (GetIORegion());

    int fileCount (0);
    int firstFileIndex (0);
    if ( ioRegion.GetImageDimension() < 3 ) {
        fileCount = 1;
        firstFileIndex = 0;
    } else {
        fileCount = 1;
        firstFileIndex = ioRegion.GetIndex(2);
        for(int i(2); i<ioRegion.GetImageDimension(); ++i ) {
            fileCount *= ioRegion.GetSize(i);
        }
    }
    if (fileCount > (int)( this->GetFileNames().size() ) ) {
      itkExceptionMacro("Requested region exceeds size that can be read.");
    }

    int threadFileCount = (int)::ceil( fileCount/(double)total );

    RegionType::IndexType start;
    start[0] = id * threadFileCount;
    RegionType::SizeType length;
    length[0] = threadFileCount;

    int maxThreadInUse = (int)::ceil(fileCount/(double)threadFileCount) - 1;

    // in the case that there are more threads than slices, the start index may overflow.
    if( start[0] + length[0] > fileCount ) {

        if ( start [0] < fileCount) {
            length[0] = fileCount - start[0];
        } else {
            start[0] = fileCount -1;
            length[0] = 0;
        }
    }
    start[0] += firstFileIndex;

    region.SetIndex (start);
    region.SetSize (length);

    return maxThreadInUse+1;
}

//---------------------------------------------------------------------------------------------

bool DCMTKImageIO::CanStreamRead( void )
{
    return true;
}

//---------------------------------------------------------------------------------------------

ImageIORegion DCMTKImageIO::GenerateStreamableReadRegionFromRequestedRegion( const ImageIORegion & requestedRegion ) const
{
    // This implementation returns the requestedRegion if
    // "UseStreamedReading" is enabled

    ImageIORegion streamableRegion(this->m_NumberOfDimensions);
    if( this->GetUseStreamedReading () )
    {
        for( unsigned int i=0; i < this->m_NumberOfDimensions; i++ )
        {
            if ( i < 2 ) {
                streamableRegion.SetSize( i, this->m_Dimensions[i] );
                streamableRegion.SetIndex( i, 0 );
            } else {
                streamableRegion.SetSize( i, requestedRegion.GetSize (i) );
                streamableRegion.SetIndex( i, requestedRegion.GetIndex (i) );
            }
        }
        //    streamableRegion = requestedRegion;
    }
    else
    {
        for( unsigned int i=0; i < this->m_NumberOfDimensions; i++ )
        {
            streamableRegion.SetSize( i, this->m_Dimensions[i] );
            streamableRegion.SetIndex( i, 0 );
        }
    }

    return streamableRegion;
}

//---------------------------------------------------------------------------------------------


} // itk namespace

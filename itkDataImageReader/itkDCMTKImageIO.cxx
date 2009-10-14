//#define __STDC_LIMIT_MACROS
//#define __STDC_CONSTANT_MACROS

#include "itkDCMTKImageIO.h"

#define HAVE_CONFIG_H
#include <dcmtk/config/osconfig.h>

#include <dcmtk/dcmdata/dctk.h>
#include <dcmtk/dcmdata/dcistrmf.h>
#include <dcmtk/ofstd/ofstdinc.h>


#include <itksys/SystemTools.hxx>
#include <itksys/Directory.hxx>
#include <itkByteSwapper.h>
#include <itkMetaDataDictionary.h>

#include <dcmtk/dcmdata/dcrledrg.h>
#include <dcmtk/dcmjpeg/djdecode.h>
#include <dcmtk/dcmdata/dcstack.h>

#include <vnl/vnl_vector.h>
#include <vnl/vnl_cross.h>


#include <float.h>
#include <inttypes.h>
#include <stdint.h>

#include <fstream>



namespace itk
{
  
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

    DcmRLEDecoderRegistration::registerCodecs();
    DJDecoderRegistration::registerCodecs();
    
  }
  

  DCMTKImageIO::~DCMTKImageIO()
  {
    DcmRLEDecoderRegistration::cleanup();
    DJDecoderRegistration::cleanup();
  }


  
  bool DCMTKImageIO::CanReadFile(const char* filename)
  {
    ::DcmFileFormat dicomFile;
    ::OFCondition condition = dicomFile.loadFile( filename );
    if ( !condition.good() )
    {
      return false;
    }

    return true;
  }


  


  void DCMTKImageIO::DeterminePixelType()
  {
    
    const std::vector<std::string> &bitsAllocatedVec = this->GetMetaDataValueVectorString ("(0028,0100)");
    if ( !bitsAllocatedVec.size() )
    {
      itkExceptionMacro ( << "Tag (0028,0100) (Pixel Type) not found" );
    }
    
    std::istringstream s_stream ( bitsAllocatedVec[0].c_str() );
    int bitsAllocated = 0;
    if ( !(s_stream >> bitsAllocated) )
      itkExceptionMacro ( << "Cannot convert string to int: " << bitsAllocatedVec[0].c_str() );
    
    
    const std::vector<std::string> &signBitsVec = this->GetMetaDataValueVectorString ("(0028,0103)");
    std::string sign = "0";
    if (signBitsVec.size())
    {
      sign = signBitsVec[0];
    }
    else
    {
      itkWarningMacro (<< "Missing Pixel Representation (0028,0103), assuming unsigned");
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
  
  
  
  
  void DCMTKImageIO::DetermineSpacing()
  {
    m_Spacing[0] = 1.0;
    m_Spacing[1] = 1.0;
    const std::vector<std::string> &pixSpacingVec = this->GetMetaDataValueVectorString ("(0028,0030)");
    if ( !pixSpacingVec.size() )
    {
      itkWarningMacro ( << "Tag (0028,0030) (PixelSpacing) was not found, assuming 1.0" );
    }
    else
    {
      std::string pixSpacingStr = pixSpacingVec[0];
      std::istringstream is_stream( pixSpacingStr );
      if (!(is_stream >> m_Spacing[0]))
      {
	itkWarningMacro ( << "Cannot convert string to double: " << pixSpacingStr.c_str() );
      }
      if (!(is_stream >> m_Spacing[1]))
      {
	itkWarningMacro ( << "Cannot convert string to double: " << pixSpacingStr.c_str() );
      }
    }
    

    
    double sliceThickness = 1.0;
    const std::vector<std::string> &sliceThicknessVec = this->GetMetaDataValueVectorString ("(0018,0050)");
    if( sliceThicknessVec.size() )
    {
      std::string sliceThicknessStr = sliceThicknessVec[0];
      std::istringstream is_stream( sliceThicknessStr.c_str() );
      if (!(is_stream>>sliceThickness))
      {
	itkWarningMacro ( << "Cannot convert string to double: " << sliceThicknessStr.c_str() );
      }
    }
    
    double spacingBetweenSlices = 1.0;
    const std::vector<std::string> &spacingBetweenSlicesVec = this->GetMetaDataValueVectorString ("(0018,0088)");
    if( spacingBetweenSlicesVec.size() )
    {
      std::string spacingBetweenSlicesStr = spacingBetweenSlicesVec[0];
      std::istringstream is_stream( spacingBetweenSlicesStr.c_str() );
      if (!(is_stream>>spacingBetweenSlices))
      {
	itkWarningMacro ( << "Cannot convert string to double: " << spacingBetweenSlicesStr.c_str() );
      }
    }
    
    if (spacingBetweenSlices > 0.0)
    {
      m_Spacing[2] = spacingBetweenSlices;
    }
    else if (sliceThickness > 0.0)
    {
      m_Spacing[2] = sliceThickness;
    }
    if (this->GetNumberOfDimensions()==4)
      m_Spacing[3] = 1.0; 
  }
  
  
  
  void DCMTKImageIO::DetermineDimensions()
  {
    const std::vector<std::string> &dimXVec = this->GetMetaDataValueVectorString ("(0028,0011)");
    if (!dimXVec.size())
      itkExceptionMacro ( << "Tag (0028,0011) (dim X) not found" );
    
    std::string dimXStr = dimXVec[0];
    std::istringstream is_streamX (dimXStr.c_str());
    if (!(is_streamX>>m_Dimensions[0]))
      itkExceptionMacro ( << "Cannot convert string to int: " << dimXStr.c_str() );
    
    
    const std::vector<std::string> &dimYVec = this->GetMetaDataValueVectorString ("(0028,0010)");
    if (!dimYVec.size())
      itkExceptionMacro ( << "Tag (0028,0010) (dim Y) not found" );
    
    std::string dimYStr = dimYVec[0];
    std::istringstream is_streamY (dimYStr.c_str());
    if (!(is_streamY>>m_Dimensions[1]))
      itkExceptionMacro ( << "Cannot convert string to int: " << dimYStr.c_str() );
  }



  void DCMTKImageIO::DetermineOrigin()
  {
    m_Origin [0] = 0.0;
    m_Origin [1] = 0.0;
    m_Origin [2] = 0.0;
    if( this->GetNumberOfDimensions()==4 )
      m_Origin [3] = 0.0;
    
    const std::vector<std::string> &originVec = this->GetMetaDataValueVectorString("(0020,0032)");
    if (!originVec.size())
    {
      itkWarningMacro ( << "Tag (0020,0032) (ImageOrigin) was not found, assuming 0.0/0.0/0.0");
      return;
    }

    std::string originStr = originVec[0]; // should take the first slice in the pile
    std::istringstream is_stream( originStr.c_str() );
    if (!(is_stream >> m_Origin[0]))
    {
      itkWarningMacro ( << "Cannot convert string to double: " << originStr.c_str() );
    }
    if (!(is_stream >> m_Origin[1]))
    {
      itkWarningMacro ( << "Cannot convert string to double: " << originStr.c_str() );
    }
    if (!(is_stream >> m_Origin[2]))
    {
      itkWarningMacro ( << "Cannot convert string to double: " << originStr.c_str() );
    }
  }
  
  
  
  
  void DCMTKImageIO::DetermineOrientation()
  {

    double orientation[6]={1.0, 0.0, 0.0, 0.0, 1.0, 0.0};
    const std::vector<std::string> &orientationVec = this->GetMetaDataValueVectorString("(0020,0037)");
    if (!orientationVec.size())
    {
      itkWarningMacro ( << "Tag (0020,0037) (PatientOrientation) was not found, assuming identity");
    }
    else
    {
      std::string orientationStr = orientationVec[0];
      std::istringstream is_stream( orientationStr.c_str() );
      for( int i=0; i<6; i++ )
      {
	if (!(is_stream >> orientation[i]) )
	{
	  itkWarningMacro ( << "Cannot convert string to double: " << orientationStr.c_str() );
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
    
    m_Direction[0][0] = orientation[0];
    m_Direction[0][1] = orientation[1];
    m_Direction[0][2] = orientation[2];
    m_Direction[1][0] = orientation[3];
    m_Direction[1][1] = orientation[4];
    m_Direction[1][2] = orientation[5];
    m_Direction[2][0] = sliceDirection[0];
    m_Direction[2][1] = sliceDirection[1];
    m_Direction[2][2] = sliceDirection[2];
    
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

  
  
  void DCMTKImageIO::ReadImageInformation()
  {

    try
    {

      std::set< std::string > fileNames;     

      for( unsigned int i=0; i<this->GetFileNames().size(); i++ )
	fileNames.insert ( this->GetFileNames()[i] );


      int fileCount = (int)( fileNames.size() );
      if( fileCount == 0 )
      {
	itkExceptionMacro (<<"Cannot find any dicom in directory or dicom is not valid");	
      }

      std::set< std::string >::const_iterator f = fileNames.begin(), fe = fileNames.end();
      
      std::set< double >                   locations;
      std::multimap< double, std::string > locationToNameLut;
      std::map< std::string, int >         nameToIndexLut;


      int fileIndex = 0;
      double floatSliceLocation = 0;

      while ( f != fe )
      {
	std::string filename;
	if( m_Directory != "" )
	  filename = m_Directory + "/" + *f;
	else
	  filename = *f;

	try
	{
	  this->ReadHeader( filename, fileIndex, fileCount );
	  
	  floatSliceLocation = 0;

	  const std::vector< std::string > &vecSlice = this->GetMetaDataValueVectorString ("(0020,1041)");
	  if( vecSlice.size() )
	  {
	    std::istringstream is_stream ( vecSlice[fileIndex].c_str() );
	    is_stream >> floatSliceLocation;
	  }
	  else
	  {
	    const std::vector< std::string > &vecSlice2 = this->GetMetaDataValueVectorString ("(0020,0050)");
	    if( vecSlice2.size() )
	    {
	      std::istringstream is_stream ( vecSlice2[fileIndex].c_str() );
	      is_stream >> floatSliceLocation;
	    }
	    //else // Hopeless
	  }

	  locations.insert( floatSliceLocation );
	  locationToNameLut.insert( std::pair< double, std::string >(floatSliceLocation, *f ) );
	  nameToIndexLut[ *f ] = fileIndex;
	  ++ fileIndex;
	}
	catch (ExceptionObject &e)
	{
	  std::cerr << e; // continue
	}
	++ f;
      }

      

      
      // ordering location slices according to instance number
      std::set< double >::const_iterator l = locations.begin(), le = locations.end();
      while ( l != le )
      {
	std::multimap< double, std::string >::iterator n = locationToNameLut.lower_bound( *l ), ne = locationToNameLut.upper_bound( *l );
	
	// using that intermediate lut for instance number ordering
	std::map< int, std::list< std::string > > instanceNumberToNameLut;
	
	while ( n != ne )
	{	  
	  std::string instanceNumberString = this->GetMetaDataValueString ("(0020,0013)", nameToIndexLut[ n->second ]);	  
	  int instanceNumber = 0;
	  std::istringstream is_stream (instanceNumberString.c_str());
	  is_stream >> instanceNumber;
	  
	  instanceNumberToNameLut[ instanceNumber ].push_back( n->second );
	  ++ n;	  
	}
	
	// erasing name list corresponding to given location
	locationToNameLut.erase( *l );
	
	// inserting ordered file names
	std::map< int, std::list< std::string > >::const_iterator in = instanceNumberToNameLut.begin(), ine = instanceNumberToNameLut.end();
	while ( in != ine )
	{
	  std::list< std::string >::const_iterator fn = in->second.begin(), fne = in->second.end();
	  while ( fn != fne )
	  {
	    locationToNameLut.insert( std::pair< double, std::string >( *l, *fn ) );
	    ++ fn;
	  }
	  ++ in;
	}
	++ l;
      }


      // collecting slice count and rank count while doing sanity checks
      int sizeZ, sizeT;
      
      sizeZ = ( int )locations.size();
      sizeT = ( int )locationToNameLut.count( *locations.begin() );

      if( sizeT > 1 ) {
	this->SetNumberOfDimensions (4);
	m_Dimensions[3] =  sizeT;
      }
      else {
	this->SetNumberOfDimensions (3);
      }
      m_Dimensions[2] =  sizeZ;

      
      
      this->DeterminePixelType();
      this->DetermineDimensions();
      this->DetermineSpacing();
      this->DetermineOrigin();
      this->DetermineOrientation();
      

      m_OrderedFileNames = std::vector< std::string > ( sizeZ * sizeT );
      

      int32_t location = 0, rank = 0;
      l = locations.begin();
      while ( l != le )
      {
	if ( ( int )locationToNameLut.count( *l ) != sizeT )
	{
	  itkExceptionMacro( << "Inconsistent sizeT between locations: very probably, several series are present (this ImageIO can only proceed one serie at a time)" );	  
	}
	std::multimap< double, std::string >::const_iterator
	  n = locationToNameLut.lower_bound( *l ),
	  ne = locationToNameLut.upper_bound( *l );
	
	rank = 0;
	while ( n != ne )
	{
	  // we need to use sizeZ - 1 - location because z = 0 correspond to
	  // top of the brain
	  m_OrderedFileNames[ rank * sizeZ + ( sizeZ - 1 - location ) ] = n->second;
	  
	  ++ rank;
	  ++ n; 
	}	
	++ location;
	++ l;
      }

      this->SetFileNames ( m_OrderedFileNames );

    }
    catch(itk::ExceptionObject &e)
    {
      std::cerr << e;
      itkExceptionMacro ( << "Error in DCMTKImageIO::ReadImageInformation()");
    }

  }


  void DCMTKImageIO::ThreadedRead (void* buffer, RegionType region, int threadId)
  {
    unsigned long pixelCount = m_Dimensions[0] * m_Dimensions[1];
    
    int start = region.GetIndex()[0];
    int length = region.GetSize()[0];

    for( int i=start; i<start+length; i++)
    {
      this->InternalRead (buffer, i, pixelCount);
      if( threadId==0 )
	this->InvokeEvent ( SliceReadEvent() );
    }
  }
  


  void DCMTKImageIO::InternalRead (void* buffer, int slice, unsigned long pixelCount)
  {
    std::string filename;
    if( m_Directory=="" )
      filename = m_OrderedFileNames[slice];
    else
      filename = m_Directory + "/" + m_OrderedFileNames[slice];

    DcmFileFormat dicomFile;
    DcmStack      stack;
    
#ifdef UID_RawDataStorage
    DcmInputFileStream dicomStream( filename.c_str(), 0U );
#else
    DcmFileStream dicomStream( filename.c_str(), DCM_ReadMode );
#endif
    
    dicomFile.transferInit();
    dicomFile.read( dicomStream, EXS_Unknown, EGL_noChange );
    dicomFile.transferEnd();
    
    if ( dicomFile.search( DCM_PixelData, stack ) != EC_Normal )
    {
      itkExceptionMacro( << "PixelData tag was not found" );
    }
    DcmPixelData* pixelData = static_cast< ::DcmPixelData* >( stack.top() );
    
    
    unsigned char* copyBuffer = 0;
    Uint8* ucharBuffer = 0;
    //Sint16* shortBuffer = 0; // apparently, getSInt16Array is not implemented in DCMTK
    Uint16* ushortBuffer = 0;
    //Sint32* intBuffer = 0; // apparently, getSInt32Array is not implemented in DCMTK
    Uint32* uintBuffer = 0;
    Float64* doubleBuffer = 0;

    size_t length = 0;

    switch( this->GetComponentType() ){
	case CHAR:
	  throw ExceptionObject (__FILE__,__LINE__,"According to dcmtk, int8 pixel type is not a dicom supported format");
	  break;
	  
	case UCHAR:
	  pixelData->getUint8Array( ucharBuffer );
	  copyBuffer = (unsigned char*)( ucharBuffer );
	  length = (size_t)(pixelCount * sizeof( unsigned char ) );
	  break;
	  
	case SHORT:
	  pixelData->getUint16Array( ushortBuffer );
	  copyBuffer = (unsigned char*)( ushortBuffer );
	  length = (size_t)(pixelCount * sizeof( short ) );
	  break;
	  
	case USHORT:
	  pixelData->getUint16Array( ushortBuffer );
	  copyBuffer = (unsigned char*)( ushortBuffer );	  
	  length = (size_t)(pixelCount * sizeof( unsigned short ) );
	  break;
	  
	case INT:
	  pixelData->getUint32Array( uintBuffer );
	  copyBuffer = (unsigned char*)( uintBuffer );
	  length = (size_t)(pixelCount * sizeof( Sint32 ) );
	  break;
	  
	case UINT:
	  pixelData->getUint32Array( uintBuffer );
	  copyBuffer = (unsigned char*)( uintBuffer );
	  length = (size_t)(pixelCount * sizeof( Uint32 ) );
	  break;
	  
	case DOUBLE:
	  pixelData->getFloat64Array( doubleBuffer );
	  copyBuffer = (unsigned char*)( doubleBuffer );
	  length = (size_t)(pixelCount * sizeof( Float64 ) );
	  break;
	  
	default:
	  throw ExceptionObject (__FILE__,__LINE__,"Unsupported pixel data type in DICOM");
    }

    if (!copyBuffer)
    {
      itkExceptionMacro ( << "Bad copy buffer" );
    }

    unsigned char* destBuffer = (unsigned char*)(buffer);
    memcpy (destBuffer + slice*length, copyBuffer, length);
  }

  

  

  bool DCMTKImageIO::CanWriteFile( const char* filename)
  {
    return false;
  }


  void DCMTKImageIO::WriteImageInformation()
  {
  }

  

  void DCMTKImageIO::Write(const void* buffer)
  {
  }


  std::string DCMTKImageIO::GetPatientName() const
  {
    std::string name = this->GetMetaDataValueString ( "(0010,0010)", 0 );
    return name;
  }

  std::string DCMTKImageIO::GetPatientID() const
  {
    std::string name = this->GetMetaDataValueString ( "(0010,0020)", 0 );
    return name;
  }
  
  std::string DCMTKImageIO::GetPatientSex() const
  {
    std::string name = this->GetMetaDataValueString ( "(0010,0040)", 0 );
    return name;
  }
  
  std::string DCMTKImageIO::GetPatientAge() const
  {
    std::string name = this->GetMetaDataValueString ( "(0010,1010)", 0 );
    return name;
  }
  
  std::string DCMTKImageIO::GetStudyID() const
  {
    std::string name = this->GetMetaDataValueString ( "(0020,0010)", 0 );
    return name;
  }
  
  std::string DCMTKImageIO::GetPatientDOB() const
  {
    std::string name = this->GetMetaDataValueString ( "(0010,0030)", 0 );
    return name;
  }
  
  std::string DCMTKImageIO::GetStudyDescription() const
  {
    std::string name = this->GetMetaDataValueString ( "(0008,1030)", 0 );
    return name;
  }

  std::string DCMTKImageIO::GetSeriesDescription() const
  {
    std::string name = this->GetMetaDataValueString ( "(0008,103e)", 0 );
    return name;
  }
  
  std::string DCMTKImageIO::GetBodyPart() const
  {
    std::string name = this->GetMetaDataValueString ( "(0018,0015)", 0 );
    return name;
  }
  
  std::string DCMTKImageIO::GetNumberOfSeriesInStudy() const
  {
    std::string name = this->GetMetaDataValueString ( "(0020,1000)", 0 );
    return name;
  }
  
  std::string DCMTKImageIO::GetNumberOfStudyRelatedSeries() const
  {
    std::string name = this->GetMetaDataValueString ( "(0020,1206)", 0 );
    return name;
  }
  
  std::string DCMTKImageIO::GetStudyDate() const
  {
    std::string name = this->GetMetaDataValueString ( "(0008,0020)", 0 );
    return name;
  }
  
  std::string DCMTKImageIO::GetModality() const
  {
    std::string name = this->GetMetaDataValueString ( "(0008,0060)", 0 );
    return name;
  }
  
  std::string DCMTKImageIO::GetManufacturer() const
  {
    std::string name = this->GetMetaDataValueString ( "(0008,0070)", 0 );
    return name;
  }
  
  std::string DCMTKImageIO::GetInstitution() const
  {
    std::string name = this->GetMetaDataValueString ( "(0008,0080)", 0 );
    return name;
  }
  
  std::string DCMTKImageIO::GetModel() const
  {
    std::string name = this->GetMetaDataValueString ( "(0008,1090)", 0 );
    return name;
  }
  
  std::string DCMTKImageIO::GetScanOptions() const
  {
    std::string name = this->GetMetaDataValueString ( "(0018,0022)", 0 );
    return name;
  }
  
  
  void 
  DCMTKImageIO
  ::SwapBytesIfNecessary( void* buffer, unsigned long numberOfPixels )
  {
  }




  void DCMTKImageIO::ReadHeader(const std::string& name, const int& fileIndex, const int& fileCount )
  {

    DcmFileFormat dicomFile;
    OFCondition condition = dicomFile.loadFile( name.c_str() );
    
    // checking that given file is available
    if ( !condition.good() )
    {
      itkExceptionMacro ( << "Error in " << __func__ << ": " << condition.text() );
    }
    
    
    // manual call to load data into memory
    dicomFile.loadAllDataIntoMemory();
    
    
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


  inline void DCMTKImageIO::ReadDicomElement(DcmElement* element, const int &fileIndex, const int &fileCount )
  {

    DcmTag &dicomTag = const_cast<DcmTag &>(element->getTag());
    
    std::string tagName   = dicomTag.getTagName();
    std::string tagVRName = dicomTag.getVRName();
    
    Uint16 tagGroup   = dicomTag.getGTag();
    Uint16 tagElement = dicomTag.getETag();
    
    std::ostringstream oss;
    oss << '(' << hex << setw( 4 ) << setfill( '0' )<< tagGroup << ','
	<< hex << setw( 4 ) << setfill( '0' ) << tagElement << ")";
    std::string tagKey = oss.str();
    
    
    MetaDataDictionary& dicomDictionary = this->GetMetaDataDictionary();
    

    OFString ofstring;
    OFCondition cond = element->getOFStringArray (ofstring, 0);
    if ( !cond.good() )
    {
      //itkWarningMacro ( << "Cannot convert element to string.");
      //element->print (std::cout);
      //getchar();
      return;
    }
    
    std::string s_value = ofstring.c_str();
    std::replace(s_value.begin(), s_value.end(), '\\', ' ');
    
    MetaDataDictionary::Iterator it = dicomDictionary.Find (tagKey);
    if (it!=dicomDictionary.End())
    {
      MetaDataVectorStringType* vec = dynamic_cast<MetaDataVectorStringType*>( it->second.GetPointer() );
      std::vector<std::string>& value = const_cast< std::vector<std::string>& >(vec->GetMetaDataObjectValue());	
      value[fileIndex] = s_value;
    }
    else
    {
      std::vector< std::string > vec (fileCount, "");
      vec[fileIndex] = s_value;
      EncapsulateMetaData< std::vector<std::string> >(dicomDictionary, tagKey, vec);
    }
  }




  inline std::string DCMTKImageIO::GetMetaDataValueString (const char* key, int index) const
  {
    std::string value = "Not Found";
    const MetaDataDictionary& dicomDictionary = this->GetMetaDataDictionary();
    MetaDataDictionary::ConstIterator it = dicomDictionary.Find ( key );
    if( it!=dicomDictionary.End() ) {
      if( MetaDataVectorStringType* metaData = dynamic_cast<MetaDataVectorStringType*>( it->second.GetPointer() ) )
	value = metaData->GetMetaDataObjectValue()[index];
    }
    return value;
  }

  
  inline const std::vector<std::string>& DCMTKImageIO::GetMetaDataValueVectorString (const char* key) const
  {
    //std::vector<std::string> &values = m_EmptyVector;
    const MetaDataDictionary& dicomDictionary = this->GetMetaDataDictionary();
    MetaDataDictionary::ConstIterator it = dicomDictionary.Find ( key );
    if( it!=dicomDictionary.End() ) {
      if( MetaDataVectorStringType* metaData = dynamic_cast<MetaDataVectorStringType*>( it->second.GetPointer() ) ) {
	return metaData->GetMetaDataObjectValue();
      }
      else
	return m_EmptyVector;
    }
    return m_EmptyVector;
  }
  
}

#define __STDC_LIMIT_MACROS
#define __STDC_CONSTANT_MACROS

#include "itkDCMTKImageIO.h"

#include <itksys/SystemTools.hxx>
#include <itksys/Directory.hxx>
#include <itkByteSwapper.h>
#include <itkMetaDataDictionary.h>

#include <dcmtk/dcmjpeg/djdecode.h>
#include <dcmtk/dcmdata/dcstack.h>

#include <float.h>
#include <inttypes.h>
#include <stdint.h>

#include <fstream>

namespace std
{
  template < class T >
  bool isUnique( const vector< T >& thing, T& commonValue )
  {
    
    commonValue = thing[ 0 ];
    
    typename vector< T >::const_iterator i = thing.begin(), ie = thing.end();
    while ( i != ie )
    {
      
      if ( *i != commonValue )
      {
	
	return false;
	
      }
      ++ i;
      
    }
    return true;
    
  }
}



static std::string filter( const std::string& name )
{
  std::string result = name;

  std::string::iterator i = result.begin(),
                        ie = result.end();
  while ( i != ie )
  {

    if ( *i == '#' )
    {

      *i = '_';

    }
    ++ i;

  }
  return result;
}



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
  }
  

  DCMTKImageIO::~DCMTKImageIO()
  {}


  bool DCMTKImageIO::CanReadFile(const char* filename)
  {
    return true;
  }


  

  void DCMTKImageIO::ReadImageInformation()
  {

    try
    {

      //std::string directory = itksys::SystemTools::GetFilenamePath ( this->GetFileName() );
      
      // browsing and checking directory coherence
      /*
      ptk::Directory directory( name );
     
      if ( !directory.isValid() )
      {      
      throw ptk::FileNotFoundException( name );       
      }

      if ( directory.getCount() == 0 )
      {
      throw std::runtime_error( std::string( "no file in '" ) +
                                directory.getPath() + "' directory" );
				}
      */
      
      // checking item type compatibility
      /*
	if ( object.getHeader().hasAttribute( "item_type" ) )
	{
	std::string itemType;
	object.getHeader().getAttribute( "item_type", itemType );
	if ( ! ptk::TypeOf< T >::isSameType( itemType ) ) 
	{
	
        throw ptk::ItemTypeException(
	std::string( "object item type given as argument is different from " )
	+ ptk::TypeOf< T >::getName() );
	}
	}
      */


      
      // looping over file, checking inter-location header consistency
      // and collecting slice information

      std::set< std::string > fileNames;
      std::string s_directory = "";
      int32_t fileCount = 0;
      
      if( m_FileNames.size()>0 )
      {
	for(unsigned int i=0; i<m_FileNames.size(); i++ )
	  fileNames.insert ( itksys::SystemTools::GetFilenameName (m_FileNames[i]) );
	fileCount = (int32_t)( m_FileNames.size() );
	s_directory = itksys::SystemTools::GetFilenamePath ( m_FileNames[0] );
      }
      else
      {
	itksys::Directory directory;
	directory.Load( itksys::SystemTools::GetFilenamePath ( this->GetFileName() ).c_str() );
	
	for( unsigned long i=0; i<directory.GetNumberOfFiles(); i++ )
	{
	  fileNames.insert ( directory.GetFile ( i ) );
	}
	//fileNames.insert ( itksys::SystemTools::GetFilenameName (this->GetFileName()).c_str() );
	s_directory = directory.GetPath();
	fileCount = ( int32_t )fileNames.size()-2; // - ./ and ../
      }
      
	
      std::set< std::string >::const_iterator f = fileNames.begin(),
	fe = fileNames.end();
      
      StringMap stringMap;
      FloatMap  floatMap;
      DoubleMap doubleMap;
      Int32Map  int32Map;
      Int16Map  int16Map;
      UInt32Map uint32Map;
      UInt16Map uint16Map;
      
      std::set< float > locations;
      std::multimap< float, std::string > locationToNameLut;
      std::map< std::string, int32_t > nameToIndexLut;
      
      
      int32_t fileIndex = 0;
      float floatSliceLocation = 0;
      
      while ( f != fe )
      {

	std::string filename;
	if( s_directory != "" )
	  filename = s_directory + "/" + *f;
	else
	  filename = *f;
	
	std::ifstream is( filename.c_str() );
	
	try
	{
	  readCoreHeader( filename,
			  stringMap,
			  floatMap,
			  doubleMap,
			  int32Map,
			  int16Map,
			  uint32Map,
			  uint16Map,
			  fileIndex,
			  fileCount );
	  
	  floatSliceLocation = 0;
	  this->toScalar( stringMap[ "(0020,1041)" ][ fileIndex ], floatSliceLocation );

	  std::cout << stringMap[ "(0010,0010)" ][ fileIndex ] << " / "
		    << stringMap[ "(0008,103e)" ][ fileIndex ] << " / "
		    << stringMap[ "(0008,1030)" ][ fileIndex ] << std::endl;
	  
	  locations.insert( floatSliceLocation );
	  locationToNameLut.insert( std::pair< float, std::string >(floatSliceLocation, *f ) );
	  nameToIndexLut[ *f ] = fileIndex;
	  ++ fileIndex;
	}
	catch (ExceptionObject &e)
	{
	  std::cerr << e;
	  // continue
	}
	++ f;
      }
      

      
      // ordering location slices according to instance number
      std::set< float >::const_iterator l = locations.begin(), le = locations.end();
      while ( l != le )
      {
	
	std::multimap< float, std::string >::iterator
	  n = locationToNameLut.lower_bound( *l ),
	  ne = locationToNameLut.upper_bound( *l );
	
	// using that intermediate lut for instance number ordering
	std::map< int, std::list< std::string > > instanceNumberToNameLut;
	
	while ( n != ne )
	{
	  
	  std::string instanceNumberString = 
	    stringMap[ "(0020,0013)" ][ nameToIndexLut[ n->second ] ];
	  while ( instanceNumberString[ 0 ] == ' ' )
	  {
	    instanceNumberString = instanceNumberString.substr( 1U, instanceNumberString.length() - 1U ); 
	  }
	  
	  int32_t instanceNumber = 0;
	  this->toScalar( instanceNumberString, instanceNumber );
	  instanceNumberToNameLut[ instanceNumber ].push_back( n->second );
	  ++ n;
	  
	}
	
	// erasing name list corresponding to given location
	locationToNameLut.erase( *l );
	
	// inserting ordered file names
	std::map< int32_t, std::list< std::string > >::const_iterator
	  in = instanceNumberToNameLut.begin(),
	  ine = instanceNumberToNameLut.end();
	while ( in != ine )
	{
	  std::list< std::string >::const_iterator
	    fn = in->second.begin(),
	    fne = in->second.end();
	  
	  while ( fn != fne )
	  {
	    locationToNameLut.insert( std::pair< float, std::string >( *l,
								       *fn ) );
	    ++ fn;
	    
	  }
	  ++ in;
	  
	}
	
	++ l;
	
      }

      
      // checking series instance UID
      //
      // in fact, I noticed that in some cases, for instance perfusion EPI
      // imaging, series instance UID is changing according to time, so I prefered
      // not performing this test that may make the reader fail whereas there is
      // not specific error!
      //
      // std::string tmp;
      // if ( !std::isUnique( stringMap[ "(0020,000e)" ], tmp ) )
      // {
      
      //  throw std::runtime_error( "inconsistent series instance UID" );
      
      // }
      
      // creating dicom dictionary
      MetaDataDictionary& dicomDictionary = this->GetMetaDataDictionary();
      
      std::map< std::string, std::vector< std::string > >::iterator
	sm = stringMap.begin(), sme = stringMap.end();
      std::string commonString = "";
      while ( sm != sme )
      {

	if ( std::isUnique( sm->second, commonString ) )
	{
	  EncapsulateMetaData<std::string>(dicomDictionary, sm->first, commonString); 
	}
	else
	{
	  EncapsulateMetaData< std::vector<std::string> >(dicomDictionary, sm->first, sm->second); 
	}
	++ sm;
      }

      std::map< std::string, std::vector< float > >::iterator
	fm = floatMap.begin(), fme = floatMap.end();
      float commonFloat = 0;
      while ( fm != fme )
      {
	
	if ( std::isUnique( fm->second, commonFloat ) )
	{
	  EncapsulateMetaData<float>(dicomDictionary, fm->first, commonFloat); 
	}
	else
	{
	  EncapsulateMetaData< std::vector<float> >(dicomDictionary, fm->first, fm->second);
	  
	}
	++ fm;
	
      }
      
      std::map< std::string, std::vector< double > >::iterator
	dm = doubleMap.begin(), dme = doubleMap.end();
      double commonDouble = 0;
      while ( dm != dme )
      {
	
	if ( std::isUnique( dm->second, commonDouble ) )
	{
	  EncapsulateMetaData<double>( dicomDictionary, dm->first, commonDouble);
	}
	else
	{
	  EncapsulateMetaData< std::vector<double> >( dicomDictionary, dm->first, dm->second);
	}
	++ dm;
      }

      std::map< std::string, std::vector< int32_t > >::iterator
	i32m = int32Map.begin(), i32me = int32Map.end();
      int32_t commonInt32 = 0;
      while ( i32m != i32me )
      {
	
	if ( std::isUnique( i32m->second, commonInt32 ) )
	{
	  EncapsulateMetaData<int32_t>( dicomDictionary, i32m->first, commonInt32);
	}
	else
	{
	  EncapsulateMetaData< std::vector<int32_t> >( dicomDictionary, i32m->first, i32m->second);
	}
	++ i32m;
      }
      
      std::map< std::string, std::vector< int16_t > >::iterator
	i16m = int16Map.begin(), i16me = int16Map.end();
      int16_t commonInt16 = 0;
      while ( i16m != i16me )
      {
	
	if ( std::isUnique( i16m->second, commonInt16 ) )
	{
	  EncapsulateMetaData<int16_t>( dicomDictionary, i16m->first, commonInt16);
	}
	else
	{
	  EncapsulateMetaData< std::vector<int16_t> >( dicomDictionary, i16m->first, i16m->second);
	}
	++ i16m;
	
      }
      
      std::map< std::string, std::vector< uint32_t > >::iterator
	ui32m = uint32Map.begin(), ui32me = uint32Map.end();
      uint32_t commonUInt32 = 0;
      while ( ui32m != ui32me )
      {
	
	if ( std::isUnique( ui32m->second, commonUInt32 ) )
	{
	  EncapsulateMetaData<uint32_t>( dicomDictionary, ui32m->first, commonUInt32);
	}
	else
	{ 
	  EncapsulateMetaData< std::vector<uint32_t> >( dicomDictionary, ui32m->first, ui32m->second);
	}
	++ ui32m;
      }
      
      std::map< std::string, std::vector< uint16_t > >::iterator
	ui16m = uint16Map.begin(), ui16me = uint16Map.end();
      uint16_t commonUInt16 = 0;
      while ( ui16m != ui16me )
      {
	
	if ( std::isUnique( ui16m->second, commonUInt16 ) )
	{
	  EncapsulateMetaData<uint16_t>( dicomDictionary, ui16m->first, commonUInt16); 
	}
	else
	{
	  EncapsulateMetaData< std::vector<uint16_t> >( dicomDictionary, ui16m->first, ui16m->second); 
	}
	++ ui16m;
	
      }
      
      
      // checking item type compatibility with header
      MetaDataDictionary::ConstIterator
	bitsAllocatedIterator = dicomDictionary.Find( "(0028,0100)" );
      if ( bitsAllocatedIterator != dicomDictionary.End() )
      {

	uint16_t bitsAllocated = 0;
	
	if( MetaDataUInt16Type* metaDataBitsAllocated = dynamic_cast<MetaDataUInt16Type*>( bitsAllocatedIterator->second.GetPointer() ) ) {
	  bitsAllocated = metaDataBitsAllocated->GetMetaDataObjectValue();
	}
	else if ( MetaDataVectorUInt16Type* metaDataBitsAllocated = dynamic_cast<MetaDataVectorUInt16Type*>( bitsAllocatedIterator->second.GetPointer() ) ) {
	  bitsAllocated = metaDataBitsAllocated->GetMetaDataObjectValue()[0];
	}
	else {
	  throw std::runtime_error ("unsupported component type");
	}
	
	
	if( bitsAllocated == 8 ){
	  this->SetComponentType ( CHAR );
	}
	else if ( bitsAllocated == 16 ) {
	  this->SetComponentType ( SHORT );
	}
	else if ( bitsAllocated == 32 ) {
	  this->SetComponentType ( INT );
	}
      }
      else
      {
	throw std::runtime_error( "cannot find BitsAllocated tag (0028,0100)" );
      }
      
      
      
      // collecting slice count and rank count while doing sanity checks
      int32_t sizeZ, sizeT;
      
      sizeZ = ( int32_t )locations.size();
      sizeT = ( int32_t )locationToNameLut.count( *locations.begin() );


      if( sizeT > 1 ) {
	this->SetNumberOfDimensions (4);
      }
      else {
	this->SetNumberOfDimensions (3);
      }
      
      
      m_OrderedFileNames = std::vector< std::string > ( sizeZ * sizeT );
      
      int32_t location = 0, rank = 0;
      l = locations.begin();
      while ( l != le )
      {
	if ( ( int32_t )locationToNameLut.count( *l ) != sizeT )
	{
	  std::cout <<  ( int32_t )locationToNameLut.count( *l ) << " " << sizeT << std::endl;
	  throw std::runtime_error( "inconsistent sizeT between locations" );
	  
	}
	std::multimap< float, std::string >::const_iterator
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


      for(unsigned int i=0; i<m_OrderedFileNames.size(); i++ )
	std::cout << m_OrderedFileNames[i] << std::endl;
      
      
      
      
      // adding ordered file names
      /*
      object.addSemantic( "__dicom__",
			  "ordered_file_name",
			  ptk::Semantic(
					ptk::TypeOf< std::vector< std::string > >::getName()
					) );
      object.getHeader().addAttribute( "ordered_file_names", 
				       orderedFileNames );
      */
      // adding directory path
      /*
      object.addSemantic( "__dicom__",
			  "directory_path",
			  ptk::Semantic( ptk::TypeOf< std::string >::getName()) );
      object.getHeader().addAttribute( "directory_path", 
				       directory.getPath() );
      */
      
      // adding DICOM dictionary to object header
      //object.getHeader().addAttribute( "dicom", dicomDictionary );
      
      
      // adding size(s) and resolution(s)


      if( MetaDataUInt16Type* metaDataUint16 = dynamic_cast<MetaDataUInt16Type*>( dicomDictionary["(0028,0011)"].GetPointer() ) )
      {
	m_Dimensions[0] = ( int32_t )metaDataUint16->GetMetaDataObjectValue();	  
      }
      else
      {
	MetaDataVectorUInt16Type* metaDataVectorUint16 = dynamic_cast<MetaDataVectorUInt16Type*>( dicomDictionary["(0028,0011)"].GetPointer() );
	m_Dimensions[0] = ( int32_t )metaDataVectorUint16->GetMetaDataObjectValue()[0];
      }

      if( MetaDataUInt16Type* metaDataUint16 = dynamic_cast<MetaDataUInt16Type*>( dicomDictionary["(0028,0010)"].GetPointer() ) )
      {
	m_Dimensions[1] = ( int32_t )metaDataUint16->GetMetaDataObjectValue();
      }
      else
      {
	MetaDataVectorUInt16Type* metaDataVectorUint16 = dynamic_cast<MetaDataVectorUInt16Type*>( dicomDictionary["(0028,0010)"].GetPointer() );
	m_Dimensions[1] = ( int32_t )metaDataVectorUint16->GetMetaDataObjectValue()[0];
      }      
      m_Dimensions[2] =  sizeZ;
      m_Dimensions[3] =  sizeT;

      
      /*
      try
      {
	
	object.getHeader()[ "sizeX" ] = ( int32_t )dicomDictionary["(0028,0011)" ]->template getValue< uint16_t >();
	
      }
      catch ( std::exception& )
      {
	
	std::vector< uint16_t > values;
	values = dicomDictionary[ "(0028,0011)" ]->template getValue<
	std::vector < uint16_t > >();
	object.getHeader()[ "sizeX" ] = ( int32_t )values[ 0 ];
	
      }
      try
      {
	object.getHeader()[ "sizeY" ] = ( int32_t )dicomDictionary["(0028,0010)" ]->template getValue< uint16_t >();
      }
      catch ( std::exception& )
      {
	
	std::vector< uint16_t > values;
	values = dicomDictionary[ "(0028,0010)" ]->template getValue<
	std::vector < uint16_t > >();
	object.getHeader()[ "sizeY" ] = ( int32_t )values[ 0 ];
	
      }
      object.getHeader()[ "sizeZ" ] = sizeZ;
      object.getHeader()[ "sizeT" ] = sizeT;
      */

      
      // collecting pixel spacing
      std::string PixelSpacing;
      if( MetaDataStringType* metaDataString = dynamic_cast<MetaDataStringType*>( dicomDictionary["(0028,0030)"].GetPointer() ) )
      {
	PixelSpacing = metaDataString->GetMetaDataObjectValue();
      }
      else
      {
	MetaDataVectorStringType* metaDataVectorString = dynamic_cast<MetaDataVectorStringType*>( dicomDictionary["(0028,0030)"].GetPointer() );
	PixelSpacing = metaDataVectorString->GetMetaDataObjectValue()[0];
      }
      PixelSpacing[ PixelSpacing.find( "\\" ) ] = ' ';
      double pixSizeX;
      double pixSizeY;
      std::istringstream iss( PixelSpacing );
      iss >> pixSizeX >> pixSizeY;


      
      // collecting pixel spacing    
      std::string SliceThicknessString;
      if( MetaDataStringType* metaDataString = dynamic_cast<MetaDataStringType*>( dicomDictionary["(0018,0050)"].GetPointer() ) )
      {	
	SliceThicknessString = metaDataString->GetMetaDataObjectValue();
      }
      else
      {
	MetaDataVectorStringType* metaDataVectorString = dynamic_cast<MetaDataVectorStringType*>( dicomDictionary["(0018,0050)"].GetPointer() );
	SliceThicknessString = metaDataVectorString->GetMetaDataObjectValue()[0];
      }
      double SliceThickness = 0.0;
      if ( !this->toScalar( SliceThicknessString, 
			    SliceThickness ) )
      {
	
	throw std::runtime_error( "failed to convert SliceThicknessString to"
				  " string to double" );
	
      }


      
      // collecting space between slices
      double SpacingBetweenSlices = 0.0;
      if ( dicomDictionary.Find( "(0018,0088)" ) != dicomDictionary.End() )
      {
	
	std::string SpacingBetweenSlicesString;
	if( MetaDataStringType* metaDataString = dynamic_cast<MetaDataStringType*>( dicomDictionary["(0018,0088)"].GetPointer() ) )
	{
	  SpacingBetweenSlicesString = metaDataString->GetMetaDataObjectValue(); 
	}
	else
	{
	  MetaDataVectorStringType* metaDataVectorString = dynamic_cast<MetaDataVectorStringType*>( dicomDictionary["(0018,0088)"].GetPointer() );
	  SpacingBetweenSlicesString = metaDataVectorString->GetMetaDataObjectValue()[0];
	}
	if ( !this->toScalar( SpacingBetweenSlicesString, 
			      SpacingBetweenSlices ) )
	{
	  throw std::runtime_error( "failed to convert SpacingBetweenSlices to"
				    " string to double" ); 
	}
      }


      m_Spacing[0] = pixSizeX;
      m_Spacing[1] = pixSizeY;
      if ( SpacingBetweenSlices > 0.0 )
      {
	m_Spacing[2] = SpacingBetweenSlices;
      }
      else if ( SliceThickness > 0.0 )
      {
	m_Spacing[2] = SliceThickness;
      }
      m_Spacing[3] = 1.0;
      
      /*
      object.addSemantic( "__dicom__", "resolutionX",
			  ptk::Semantic( ptk::TypeOf< double >::getName() ) );
      object.addSemantic( "__dicom__", "resolutionY",
			  ptk::Semantic( ptk::TypeOf< double >::getName() ) );
      object.addSemantic( "__dicom__", "resolutionZ",
			  ptk::Semantic( ptk::TypeOf< double >::getName() ) );
      object.addSemantic( "__dicom__", "resolutionT",
			  ptk::Semantic( ptk::TypeOf< double >::getName() ) );
      object.getHeader()[ "resolutionX" ] = pixSizeX;
      object.getHeader()[ "resolutionY" ] = pixSizeY;
      if ( SpacingBetweenSlices > 0.0 )
      {
	
	object.getHeader()[ "resolutionZ" ] = SpacingBetweenSlices;
	
      }
      else if ( SliceThickness > 0.0 )
      {
	
	object.getHeader()[ "resolutionZ" ] = SliceThickness;
	
      }
      object.getHeader()[ "resolutionT" ] = 1.0;
      
      // notifying observers
      object.getHeader().setChanged();
      object.getHeader().notifyObservers();
      */
    }
    catch(std::exception& e)
    {
      std::cerr << e.what();
      throw ExceptionObject (__FILE__,__LINE__, "template < class T > "
			     "void ptk::DicomDiskFormat< T >::readHeader( "
			     "const std::string& name, "
			     "ptk::HeaderedObject& object ) const" );
    }
    
  }


  


  void DCMTKImageIO::Read(void* buffer)
  {

    if( m_OrderedFileNames.size()==0 )
    {
      throw ExceptionObject (__FILE__,__LINE__,"ordered file names is empty");
    }
    std::string directory = itksys::SystemTools::GetFilenamePath ( this->GetFileName() );

    bool isJpeg = 0;
    size_t toCopy = (size_t)(m_Dimensions[0] * m_Dimensions[1] * sizeof(unsigned short) );

    unsigned char* dest = (unsigned char*)(buffer);
    
    for( unsigned int i=0; i<m_OrderedFileNames.size(); i++)
    {
      std::string name = directory + "/" + m_OrderedFileNames[i];

      ::DcmFileFormat dicomFile;
      ::DcmStack stack;

      
#ifdef UID_RawDataStorage
      ::DcmInputFileStream dicomStream( name.c_str(), 0U );
#else
      ::DcmFileStream dicomStream( name.c_str(), DCM_ReadMode );
#endif
      if ( isJpeg )
      {
	::DJDecoderRegistration::registerCodecs();
      }
      dicomFile.transferInit();
      dicomFile.read( dicomStream, EXS_Unknown, EGL_noChange );
      dicomFile.transferEnd();

      if ( isJpeg )
      {
	::DcmDataset* dataSet = dicomFile.getDataset();
	dataSet->chooseRepresentation( EXS_LittleEndianExplicit, NULL );
      }
      
      if ( dicomFile.search( DCM_PixelData, stack ) != EC_Normal )
      {
	throw std::runtime_error( "pixel data tag not found" );
      }
      ::DcmPixelData* pixelData = static_cast< ::DcmPixelData* >( stack.top() );
      
      uint16_t* unsignedShortPointer = 0;
      pixelData->getUint16Array( unsignedShortPointer );

      unsigned char* copy = (unsigned char*)(unsignedShortPointer);
      if ( isJpeg )
      {
	::DJDecoderRegistration::cleanup();
      }

      std::memcpy (dest + i*toCopy, copy, toCopy);
      this->InvokeEvent ( SliceReadEvent() );
    }
    
    
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



  
  void 
  DCMTKImageIO
  ::SwapBytesIfNecessary( void* buffer, unsigned long numberOfPixels )
  {
  }




  void DCMTKImageIO::readCoreHeader(const std::string& name,
				    StringMap& stringMap,
				    FloatMap& floatMap,
				    DoubleMap& doubleMap,
				    Int32Map& int32Map,
				    Int16Map& int16Map,
				    UInt32Map& uint32Map,
				    UInt16Map& uint16Map,
				    int32_t fileIndex,
				    int32_t fileCount ) const
  {

    
    try
    {
      
      ::DcmFileFormat dicomFile;
      ::OFCondition condition = dicomFile.loadFile( name.c_str() );
      
      // checking that given file is available
      if ( !condition.good() )
      {
	throw std::runtime_error( name + ": " + condition.text() );
      }
      
      
      // manual call to load data into memory
      dicomFile.loadAllDataIntoMemory();

      
      // dump data to standard output
      //dicomFile.print( COUT, DCMTypes::PF_shortenLongTagValues );

      
      // reading meta info
      ::DcmMetaInfo* metaInfo = dicomFile.getMetaInfo();

      
      uint64_t e;
      for ( e = 0; e < metaInfo->card(); e++ )
      {
	
	::DcmElement* element = metaInfo->getElement( e );
	readElement( element,
		     stringMap,
		     floatMap,
		     doubleMap,
		     int32Map,
		     int16Map,
		     uint32Map,
		     uint16Map,
		     fileIndex,
		     fileCount );
	
      }
      // reading data set
      ::DcmDataset* dataSet = dicomFile.getDataset();
      
      for ( e = 0; e < dataSet->card(); e++ )
      {
	
	::DcmElement* element = dataSet->getElement( e );
	readElement( element,
		     stringMap,
		     floatMap,
		     doubleMap,
		     int32Map,
		     int16Map,
		     uint32Map,
		     uint16Map,
		     fileIndex,
		     fileCount );
	
      }
      
    }
    catch(std::exception &e)
    {
      std::cerr << e.what() << std::endl;
      
      throw ExceptionObject (__FILE__,__LINE__, "template < class T > "
			     "void ptk::DicomDiskFormat< T >::readCoreHeader( "
			     "const std::string& name, "
			     "typename ptk::DicomDiskFormat< T >::StringMap& stringMap, "
			     "typename ptk::DicomDiskFormat< T >::FloatMap& floatMap, "
			     "typename ptk::DicomDiskFormat< T >::DoubleMap& doubleMap, "
			     "typename ptk::DicomDiskFormat< T >::Int32Map& int32Map, "
			     "typename ptk::DicomDiskFormat< T >::Int16Map& int16Map, "
			     "typename ptk::DicomDiskFormat< T >::UInt32Map& uint32Map, "
			     "typename ptk::DicomDiskFormat< T >::UInt16Map& uint16Map, "
			     "int32_t fileIndex, "
			     "int32_t fileCount ) "
			     "const" );
    }
  }





  void DCMTKImageIO::readElement(::DcmElement* element,
				 StringMap& stringMap,
				 FloatMap& floatMap,
				 DoubleMap& doubleMap,
				 Int32Map& int32Map,
				 Int16Map& int16Map,
				 UInt32Map& uint32Map,
				 UInt16Map& uint16Map,
				 int32_t fileIndex,
				 int32_t fileCount ) const
  {
    
    try
    {
      
      ::DcmTag* dicomTag = ( ::DcmTag* )( &element->getTag() );
      ::DcmEVR eValueRepresentation = dicomTag->getEVR();
      
      std::string tagName = dicomTag->getTagName();
      std::string tagVRName = dicomTag->getVRName();
      uint16_t tagGroup = dicomTag->getGTag();
      uint16_t tagElement = dicomTag->getETag();
      
#ifdef ITK_DCMTK_DEBUG
      
      std::cout << '('
		<< setbase( 16 ) << setw( 4 ) << setfill( '0' )
		<< tagGroup << ','
		<< setbase( 16 ) << setw( 4 ) << setfill( '0' )
		<< tagElement
		<< ") -> " << tagName
		<< "(" << tagVRName << ")"
		<< setbase( 10 ) << " : " ;
      
#endif
      
      std::ostringstream oss;
      oss << '('
	  << setbase( 16 ) << setw( 4 ) << setfill( '0' )
	  << tagGroup << ','
	  << setbase( 16 ) << setw( 4 ) << setfill( '0' )
	  << tagElement
	  << setbase( 10 ) << ")";
      std::string tagGroupElement = oss.str();
      
      
      switch ( eValueRepresentation )
      {
	
	// application entity
	  case ::EVR_AE:
	    
	    {
	      if ( stringMap.find( tagGroupElement ) == stringMap.end() )
	      {
		stringMap[ tagGroupElement ] = std::vector< std::string >( fileCount, "" );
		
	      }
	      ::DcmApplicationEntity*
		  ae = static_cast< ::DcmApplicationEntity* >( element );
	      if ( !ae )
	      {
		throw std::runtime_error( "bad application entity" );
	      }
	      char* applicationEntityChPointer = 0;
	      ae->getString( applicationEntityChPointer );
	      if ( applicationEntityChPointer )
	      {
		std::string
		  applicationEntity = std::string( applicationEntityChPointer );
		stringMap[ tagGroupElement ][ fileIndex ]= filter(
								  applicationEntity );
	      } 
	    }
	    break;
	    
	    // age string
	  case ::EVR_AS:
	    
	    {
	      
	      if ( stringMap.find( tagGroupElement ) == stringMap.end() )
	      {
		
		stringMap[ tagGroupElement ] = std::vector< std::string >(
									  fileCount, "" );
		
	      }
	      ::DcmAgeString* as = static_cast< ::DcmAgeString* >( element );
	      if ( !as )
	      {
		
		throw std::runtime_error( "bad age string" );
		
	      }
	      char* ageStringChPointer = 0;
	      as->getString( ageStringChPointer );
	      if ( ageStringChPointer )
	      {
		
		std::string ageString = std::string( ageStringChPointer );
		
#ifdef ITK_DCMTK_DEBUG

            std::cout << ageString;

#endif

            stringMap[ tagGroupElement ][ fileIndex ] = filter( ageString );
	    
	      }
	      
	    }
	    break;
	    
	    // attribute tag
	  case ::EVR_AT:
	    
	    
	    break;
	    
	    // code string
	  case ::EVR_CS:
	    
	    {
	      
	      if ( stringMap.find( tagGroupElement ) == stringMap.end() )
	      {
		
		stringMap[ tagGroupElement ] = std::vector< std::string >(
									  fileCount, "" );
		
	      }
	      ::DcmCodeString* cs = static_cast< ::DcmCodeString* >( element );
	      if ( !cs )
	      {
		
		throw std::runtime_error( "bad code string" );
		
	      }
	      char* codeStringChPointer = 0;
	      cs->getString( codeStringChPointer );
	      if ( codeStringChPointer )
	      {
		
		std::string codeString = std::string( codeStringChPointer );
		
#ifdef ITK_DCMTK_DEBUG
		
		std::cout << codeString;

#endif
		
		stringMap[ tagGroupElement ][ fileIndex ] = filter( codeString );
		
	      }
	      
	    }
	    break;
	    
	    // date
	  case ::EVR_DA:
	    
	    {
	      
	      if ( stringMap.find( tagGroupElement ) == stringMap.end() )
	      {
		
		stringMap[ tagGroupElement ] = std::vector< std::string >(
									  fileCount, "" );
		
	      }
	      ::DcmDate* d = static_cast< ::DcmDate* >( element );
	      if ( !d )
	      {
		
		throw std::runtime_error( "bad date" );
		
	      }
	      char* dateChPointer = 0;
	      d->getString( dateChPointer );
	      if ( dateChPointer )
	      {
		
		std::string dateString = std::string( dateChPointer );
		
#ifdef ITK_DCMTK_DEBUG
		
		std::cout << dateString;
		
#endif
		
		stringMap[ tagGroupElement ][ fileIndex ] =  filter( dateString );
		
	      }
	      
	    }
	    break;
	    
	    // decimal string
	  case ::EVR_DS:
	    
	    {
	      
	      if ( stringMap.find( tagGroupElement ) == stringMap.end() )
	      {
		
		stringMap[ tagGroupElement ] = std::vector< std::string >(
									  fileCount, "" );
		
	      }
	      ::DcmDecimalString*
		  ds = static_cast< ::DcmDecimalString* >( element );
	      if ( !ds )
	      {
		
		throw std::runtime_error( "bad decimal string" );
		
	      }
	      char* decimalStringChPointer = 0;
	      ds->getString( decimalStringChPointer );
	      if ( decimalStringChPointer )
	      {
		
		std::string decimalString = std::string( decimalStringChPointer );
		
#ifdef ITK_DCMTK_DEBUG
		
		std::cout << decimalString;
		
#endif
		
		stringMap[ tagGroupElement ][ fileIndex ] =  filter( decimalString);
		
	      }
	      
	    }
	    break;
	    
	    // date time
	  case ::EVR_DT:
	    
	    {
	      
	      if ( stringMap.find( tagGroupElement ) == stringMap.end() )
	      {
		
		stringMap[ tagGroupElement ] = std::vector< std::string >(
									  fileCount, "" );
		
	      }
	      ::DcmDateTime* dt = static_cast< ::DcmDateTime* >( element );
	      if ( !dt )
	      {
		
		throw std::runtime_error( "bad date time" );
		
	      }
	      char* dateTimeChPointer = 0;
	      dt->getString( dateTimeChPointer );
	      if ( dateTimeChPointer )
	      {
		
		std::string dateTime = std::string( dateTimeChPointer );
		
#ifdef ITK_DCMTK_DEBUG
		
		std::cout << dateTime;
		
#endif
		
		stringMap[ tagGroupElement ][ fileIndex ] = filter( dateTime );
		
	      }
	      
	    }
	    break;
	    
	    // float
	  case ::EVR_FL:
	    
	    {
	      
	      if ( floatMap.find( tagGroupElement ) == floatMap.end() )
	      {
		
		floatMap[ tagGroupElement ] = std::vector< float >(
								   fileCount, 0.0f );
		
	      }
	      ::DcmFloatingPointSingle*
		  fl = static_cast< ::DcmFloatingPointSingle* >( element );
	      if ( !fl )
	      {
		
		throw std::runtime_error( "bad float" );
		
	      }
	      float floatValue = 0;
	      fl->getFloat32( floatValue );
	      
#ifdef ITK_DCMTK_DEBUG
	      
	      std::cout << floatValue;
	      
#endif
	      
	      floatMap[ tagGroupElement ][ fileIndex ] = floatValue;
	      
	    }
	    break;
	    
	    // double
	  case ::EVR_FD:
	    
	    {
	      
	      if ( doubleMap.find( tagGroupElement ) == doubleMap.end() )
	      {
		
		doubleMap[ tagGroupElement ] = std::vector< double >(
								     fileCount, 0.0 );
		
	      }
	      ::DcmFloatingPointDouble*
		  fd = static_cast< ::DcmFloatingPointDouble* >( element );
	      if ( !fd )
	      {
		
		throw std::runtime_error( "bad double" );
		
	      }
	      double doubleValue = 0;
	      fd->getFloat64( doubleValue );
	      
#ifdef ITK_DCMTK_DEBUG

          std::cout << doubleValue;

#endif

          doubleMap[ tagGroupElement ][ fileIndex ] = doubleValue;

        }
        break;

      // integer string
      case ::EVR_IS:

        {

          if ( stringMap.find( tagGroupElement ) == stringMap.end() )
          {

            stringMap[ tagGroupElement ] = std::vector< std::string >(
                                             fileCount, "" );

          }
          ::DcmIntegerString*
            is = static_cast< ::DcmIntegerString* >( element );
          if ( !is )
          {

            throw std::runtime_error( "bad integer string" );

          }
          char* integerStringChPointer = 0;
          is->getString( integerStringChPointer );
          if ( integerStringChPointer )
          {

            std::string integerString = std::string( integerStringChPointer );

#ifdef ITK_DCMTK_DEBUG

            std::cout << integerString;

#endif

            stringMap[ tagGroupElement ][ fileIndex ] = filter( integerString );

          }

        }        
        break;

      // long string
      case ::EVR_LO:

        {

          if ( stringMap.find( tagGroupElement ) == stringMap.end() )
          {

            stringMap[ tagGroupElement ] = std::vector< std::string >(
                                             fileCount, "" );

          }
          ::DcmLongString*
            ls = static_cast< ::DcmLongString* >( element );
          if ( !ls )
          {

            throw std::runtime_error( "bad long string" );

          }
          char* longStringChPointer = 0;
          ls->getString( longStringChPointer );
          if ( longStringChPointer )
          {

            std::string longString = std::string( longStringChPointer );

#ifdef ITK_DCMTK_DEBUG

            std::cout << longString;

#endif

            stringMap[ tagGroupElement ][ fileIndex ] = filter( longString );

          }

        }        
        break;

      // long text
      case ::EVR_LT:

        {

          if ( stringMap.find( tagGroupElement ) == stringMap.end() )
          {

            stringMap[ tagGroupElement ] = std::vector< std::string >(
                                             fileCount, "" );

          }
          ::DcmLongText*
            lt = static_cast< ::DcmLongText* >( element );
          if ( !lt )
          {

            throw std::runtime_error( "bad long text" );

          }
          char* longTextChPointer = 0;
          lt->getString( longTextChPointer );
          if ( longTextChPointer )
          {

            std::string longText = std::string( longTextChPointer );

#ifdef ITK_DCMTK_DEBUG

            std::cout << longText;

#endif

            stringMap[ tagGroupElement ][ fileIndex ] = filter( longText );

          }

        }        
        break;

      // other byte string
      case ::EVR_OB:

        {

          if ( stringMap.find( tagGroupElement ) == stringMap.end() )
          {

            stringMap[ tagGroupElement ] = std::vector< std::string >(
                                             fileCount, "" );

          }
          ::DcmOtherByteOtherWord*
            ob = static_cast< ::DcmOtherByteOtherWord* >( element );
          if ( !ob )
          {

            throw std::runtime_error( "bad other byte string" );

          }
          uint8_t* otherByteStringPointer = 0;
          ob->getUint8Array( otherByteStringPointer );

          if ( otherByteStringPointer )
          {

            std::ostringstream oss;
            size_t i;
            size_t length = ob->getLength() / sizeof( uint8_t );
            for ( i = 0; i < length; i++ )
            {

              oss << setw( 2 ) << setbase( 16 ) << setfill( '0' )
                  << ( int32_t )otherByteStringPointer[ i ] << " ";

            }
            oss << setbase( 10 ) << std::flush;

#ifdef ITK_DCMTK_DEBUG

            std::cout << oss.str();

#endif

            stringMap[ tagGroupElement ][ fileIndex ] = oss.str();

          }

        }        
        break;

      // other float string
      case ::EVR_OF:

        {

          ::DcmOtherFloat*
            of = static_cast< ::DcmOtherFloat* >( element );
          if ( !of )
          {

            throw std::runtime_error( "bad other float string" );

          }
          float* otherFloatStringPointer = 0;
          of->getFloat32Array( otherFloatStringPointer );

#ifdef ITK_DCMTK_DEBUG

          if ( otherFloatStringPointer )
          {

            size_t i;
            size_t length = of->getLength() / sizeof( float );
            for ( i = 0; i < length; i++ )
              std::cout << otherFloatStringPointer[ i ] << " ";

          }

#endif

        }        
        break;

      // other word string
      case ::EVR_OW:

        if ( ( tagGroup == 0x7fe0 ) && ( tagElement == 0x0010 ) )
        {

#ifdef ITK_DCMTK_DEBUG

           std::cout << "<pixel data>";

#endif

        }
        else
        {

          ::DcmOtherByteOtherWord*
            ow = static_cast< ::DcmOtherByteOtherWord* >( element );
          if ( !ow )
          {

            throw std::runtime_error( "bad other word string" );

          }
          uint16_t* otherWordStringPointer = 0;
          ow->getUint16Array( otherWordStringPointer );

#ifdef ITK_DCMTK_DEBUG

          if ( otherWordStringPointer )
          {

            size_t i;
            size_t length = ow->getLength() / sizeof( uint16_t );
            for ( i = 0; i < length; i++ )
              std::cout << setw( 4 ) << setbase( 16 ) << setfill( '0' )
                        << otherWordStringPointer[ i ] << " ";
            std::cout << setbase( 10 ) << std::flush;

          }

#endif

        }        
        break;

      // person name
      case ::EVR_PN:

        {

          if ( stringMap.find( tagGroupElement ) == stringMap.end() )
          {

            stringMap[ tagGroupElement ] = std::vector< std::string >(
                                             fileCount, "" );

          }
          ::DcmPersonName*
            pn = static_cast< ::DcmPersonName* >( element );
          if ( !pn )
          {

            throw std::runtime_error( "bad person name" );

          }
          char* personNameChPointer = 0;
          pn->getString( personNameChPointer );
          if ( personNameChPointer )
          {

            std::string personName = std::string( personNameChPointer );

#ifdef ITK_DCMTK_DEBUG

            std::cout << personName;

#endif

	    /*
	      if ( ptk::ConfigurationInfo::getInstance().hasAnonymousName() )
	      {
	      
              personName = "anonymous";
	      
	      }
	    */

            stringMap[ tagGroupElement ][ fileIndex ] = filter( personName );

          }

        }        
        break;

      // short string
      case ::EVR_SH:

        {

          if ( stringMap.find( tagGroupElement ) == stringMap.end() )
          {

            stringMap[ tagGroupElement ] = std::vector< std::string >(
                                             fileCount, "" );

          }
          ::DcmShortString*
            sh = static_cast< ::DcmShortString* >( element );
          if ( !sh )
          {

            throw std::runtime_error( "bad short string" );

          }
          char* shortStringChPointer = 0;
          sh->getString( shortStringChPointer );
          if ( shortStringChPointer )
          {

            std::string shortString = std::string( shortStringChPointer );

#ifdef ITK_DCMTK_DEBUG

            std::cout << shortString;

#endif

            stringMap[ tagGroupElement ][ fileIndex ] = filter( shortString );

          }

        }        
        break;

      // signed long
      // NOTE: a Dicom long is encoded on 32 bits
      case ::EVR_SL:
       
        {

          if ( int32Map.find( tagGroupElement ) == int32Map.end() )
          {

            int32Map[ tagGroupElement ] = std::vector< int32_t >(
                                            fileCount, 0 );

          }
          ::DcmSignedLong*
            sl = static_cast< ::DcmSignedLong* >( element );
          if ( !sl )
          {

            throw std::runtime_error( "bad signed long" );

          }
          Sint32 signedLongValue = 0;
          sl->getSint32( signedLongValue );

#ifdef ITK_DCMTK_DEBUG

          std::cout << signedLongValue;

#endif

          int32Map[ tagGroupElement ][ fileIndex ] = ( int32_t )signedLongValue;

        }
        break;

      // sequence of items
      case ::EVR_SQ:

        
        break;

      // signed short
      case ::EVR_SS:

        {

          if ( int16Map.find( tagGroupElement ) == int16Map.end() )
          {

            int16Map[ tagGroupElement ] = std::vector< int16_t >(
                                            fileCount, 0 );

          }
          ::DcmSignedShort*
            ss = static_cast< ::DcmSignedShort* >( element );
          if ( !ss )
          {

            throw std::runtime_error( "bad signed short" );

          }
          int16_t signedShortValue = 0;
          ss->getSint16( signedShortValue );

#ifdef ITK_DCMTK_DEBUG

          std::cout << signedShortValue;

#endif

          int16Map[ tagGroupElement ][ fileIndex ] = signedShortValue;

        }
        break;

      // short text
      case ::EVR_ST:

        {

          if ( stringMap.find( tagGroupElement ) == stringMap.end() )
          {

            stringMap[ tagGroupElement ] = std::vector< std::string >(
                                             fileCount, "" );

          }
          ::DcmShortText*
            st = static_cast< ::DcmShortText* >( element );
          if ( !st )
          {

            throw std::runtime_error( "bad short text" );

          }
          char* shortTextChPointer = 0;
          st->getString( shortTextChPointer );
          if ( shortTextChPointer )
          {

            std::string shortText = std::string( shortTextChPointer );

#ifdef ITK_DCMTK_DEBUG

            std::cout << shortText;

#endif

            stringMap[ tagGroupElement ][ fileIndex ] = filter( shortText );

          }

        }        
        break;

      // time
      case ::EVR_TM:

        {

          if ( stringMap.find( tagGroupElement ) == stringMap.end() )
          {

            stringMap[ tagGroupElement ] = std::vector< std::string >(
                                             fileCount, "" );

          }
          ::DcmTime*
            tm = static_cast< ::DcmTime* >( element );
          if ( !tm )
          {

            throw std::runtime_error( "bad time" );

          }
          char* timeChPointer = 0;
          tm->getString( timeChPointer );
          if ( timeChPointer )
          {

            std::string timeString = std::string( timeChPointer );

#ifdef ITK_DCMTK_DEBUG

            std::cout << timeString;

#endif

            stringMap[ tagGroupElement ][ fileIndex ] = filter( timeString );

          }

        }        
        break;

      // unique identifier
      case ::EVR_UI:

        {

          if ( stringMap.find( tagGroupElement ) == stringMap.end() )
          {

            stringMap[ tagGroupElement ] = std::vector< std::string >(
                                             fileCount, "" );

          }
          ::DcmUniqueIdentifier*
            ui = static_cast< ::DcmUniqueIdentifier* >( element );
          if ( !ui )
          {

            throw std::runtime_error( "bad time" );

          }
          char* uniqueIdentifierChPointer = 0;
          ui->getString( uniqueIdentifierChPointer );
          if ( uniqueIdentifierChPointer )
          {

            std::string 
              uniqueIdentifier = std::string( uniqueIdentifierChPointer );

#ifdef ITK_DCMTK_DEBUG

            std::cout << uniqueIdentifier;

#endif

            stringMap[ tagGroupElement ][ fileIndex ] = filter(
                                                             uniqueIdentifier );

          }

        }        
        break;

      // unsigned long
      // NOTE: a Dicom unsigned long is encoded on 32 bits
      case ::EVR_UL:

        {

          if ( uint32Map.find( tagGroupElement ) == uint32Map.end() )
          {

            uint32Map[ tagGroupElement ] = std::vector< uint32_t >(
                                            fileCount, 0U );

          }
          ::DcmUnsignedLong*
            ul = static_cast< ::DcmUnsignedLong* >( element );
          if ( !ul )
          {

            throw std::runtime_error( "bad unsigned long" );

          }
          Uint32 unsignedLongValue = 0;
          ul->getUint32( unsignedLongValue );

#ifdef ITK_DCMTK_DEBUG

          std::cout << unsignedLongValue;

#endif

          uint32Map[ tagGroupElement ][ fileIndex ] =
                                                  ( uint32_t )unsignedLongValue;

        }
        break;

      // unsigned short
      case ::EVR_US:

        {

          if ( uint16Map.find( tagGroupElement ) == uint16Map.end() )
          {

            uint16Map[ tagGroupElement ] = std::vector< uint16_t >(
                                             fileCount, 0U );

          }
          ::DcmUnsignedShort*
            us = static_cast< ::DcmUnsignedShort* >( element );
          if ( !us )
          {

            throw std::runtime_error( "bad unsigned short" );

          }
          uint16_t unsignedShortValue = 0;
          us->getUint16( unsignedShortValue );

#ifdef ITK_DCMTK_DEBUG

          std::cout << unsignedShortValue;

#endif

          uint16Map[ tagGroupElement ][ fileIndex ] = unsignedShortValue;

        }
        break;

      // unlimited text
      case ::EVR_UT:

        {

          if ( stringMap.find( tagGroupElement ) == stringMap.end() )
          {

            stringMap[ tagGroupElement ] = std::vector< std::string >(
                                             fileCount, "" );

          }
          ::DcmUnlimitedText*
            ut = static_cast< ::DcmUnlimitedText* >( element );
          if ( !ut )
          {

            throw std::runtime_error( "bad unlimited text" );

          }
          char* unlimitedTextChPointer = 0;
          ut->getString( unlimitedTextChPointer );
          if ( unlimitedTextChPointer )
          {

            std::string unlimitedText = std::string( unlimitedTextChPointer );

#ifdef ITK_DCMTK_DEBUG

            std::cout << unlimitedText;

#endif

            stringMap[ tagGroupElement ][ fileIndex ] = filter( unlimitedText );

          }

        }        
        break;

      default:

        std::cerr << tagVRName
                  << " DICOM value representation for tag "
                  << tagName
                  << "(" <<  tagGroup << ","
                  << tagElement << ") not managed"
                  << std::endl;

    }

#ifdef ITK_DCMTK_DEBUG

    std::cout << std::endl;

#endif

    }
    catch (std::exception& e)
    {
      std::cerr << e.what();
      throw ExceptionObject (__FILE__,__LINE__,"template < class T > "
             "void ptk::DicomDiskFormat< T >::readElement( "
             "::DcmElement* element, "
             "typename ptk::DicomDiskFormat< T >::StringMap& stringMap, "
             "typename ptk::DicomDiskFormat< T >::FloatMap& floatMap, "
             "typename ptk::DicomDiskFormat< T >::DoubleMap& doubleMap, "
             "typename ptk::DicomDiskFormat< T >::Int32Map& int32Map, "
             "typename ptk::DicomDiskFormat< T >::Int16Map& int16Map, "
             "typename ptk::DicomDiskFormat< T >::UInt32Map& uint32Map, "
             "typename ptk::DicomDiskFormat< T >::UInt16Map& uint16Map, "
             " int32_t fileIndex, "
             " int32_t fileCount "
             ") const" );
    }
    

  }


   bool DCMTKImageIO::toScalar( const std::string& msg,
				float& value )
   {
     
     double v = 0;
     bool ok = toScalar( msg, v );
     if ( ok && ( v < -FLT_MAX || v > FLT_MAX ) )
     {
       ok = false;
       v = 0.0;
     }
     value = ( float )v;
     return ok;
   }


   bool DCMTKImageIO::toScalar( const std::string& msg,
				double& value )
   {
     
     bool ok = true;
     
     if ( msg.empty() )
     {
       
       ok = false;
       value = 0;
       
     }
     else
     {
       
       char* end;
       value = strtod( msg.c_str(), &end );
       ok = msg[ 0 ] && ( end == 0 || *end == '\0' );
       
     }
     
     return ok;
     
   }


  bool DCMTKImageIO::toScalar( const std::string& msg,
			       int32_t& value, int32_t base )
  {
    
    int64_t v = 0;
    bool ok = toScalar( msg, v, base );
    if ( ok && ( v < INT32_MIN || v > INT32_MAX ) )
    {
      
      ok = false;
      v = 0;
      
    }
    value = ( int32_t )v;
    return ok;
    
  }


  bool DCMTKImageIO::toScalar( const std::string& msg,
			       int64_t& value, int32_t base )
  {
    
    const char* p = msg.c_str();
    int64_t val = INT64_C( 0 );
    int32_t l = msg.length();
    const int64_t maxMult = INT64_MAX / base;
    bool isOk = false;
    int32_t negative = 0;
    
    if ( !p )
      goto bye;
    while ( l && isSpace( *p ) )   // skip leading space
    {
      
      l --;
      p ++;
      
    }
    if ( l && *p == '-' ) 
    {
      
      l --;
      p ++;
      negative = 1;
      
    }
    else if ( *p == '+' )
    {
      
      l --;
      p ++;
      
    }
    
    if ( !l || !isOkInBase( *p, base ) )
      goto bye;
    while ( l && isOkInBase( *p, base ) )
    {
      
      l--;
      int32_t dv;
      if ( isdigit( *p ) )
      {
	
	dv = ( int32_t )( *p - '0' );
	
      }
      else
      {
	
	if ( *p >= 'a' && *p <= 'z' )
	  dv = *p - 'a' + 10;
	else
	  dv = *p - 'A' + 10;
	
      }
      if ( val > maxMult ||
	   ( val == maxMult && dv > ( INT64_MAX % base ) + negative ) )
	goto bye;
      val = base * val + dv;
      p ++;
      
    }
    if ( negative )
      val = -val;
    while ( l && isSpace( *p ) )  // skip trailing space
    {
      
      l --;
      p ++;
      
    }
    if ( !l )
      isOk = true;
    
  bye:
    
    if ( isOk )
    {
      
      value = val;
      return true;
      
    }
    else
    {
      
      value = 0L;
      return false;
      
    }
    
  }


  bool DCMTKImageIO::isSpace( char c )
  {
    
    return c >= 9 && c <= 13;
    
  }
  
  
  bool DCMTKImageIO::isOkInBase( char c, int32_t base )
  {
    
    if ( base <= 10 )
      return isdigit( c ) && ( int32_t )( c - '0' ) < base;
    else
      return isdigit( c ) || ( c >= 'a' && c < ( char )( 'a' + base - 10 ) )
	|| ( c >= 'A' && c < ( char )( 'A' + base - 10 ) );
    
  }
  
}

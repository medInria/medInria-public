/**
   Initial work: Cyril Poupon
 */

#define __STDC_LIMIT_MACROS
#define __STDC_CONSTANT_MACROS

#include "itkDCMTKImageIO.h"


#define HAVE_CONFIG_H
#include <dcmtk/dcmdata/dctk.h>
#include <dcmtk/dcmdata/dcistrmf.h>
#include <dcmtk/ofstd/ofstdinc.h>


#include <itksys/SystemTools.hxx>
#include <itksys/Directory.hxx>
#include <itkByteSwapper.h>
#include <itkMetaDataDictionary.h>

#include <dcmtk/dcmjpeg/djdecode.h>
#include <dcmtk/dcmdata/dcstack.h>

#include <vnl/vnl_vector.h>
#include <vnl/vnl_cross.h>


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
  std::string::iterator i = result.begin(), ie = result.end();
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

    m_Directory = "";
    
  }
  

  DCMTKImageIO::~DCMTKImageIO()
  {}


  
  bool DCMTKImageIO::CanReadFile(const char* filename)
  {
    ::DcmFileFormat dicomFile;
    ::OFCondition condition = dicomFile.loadFile( filename );
    if ( !condition.good() )
    {
      return false;
    }

    return true;
    
    /*
    if( this->GetFileNames().size()>0 )
    {
      for( unsigned int i=0; i<this->GetFileNames().size(); i++) {
	::DcmFileFormat dicomFile;
	::OFCondition condition = dicomFile.loadFile( this->GetFileNames()[i].c_str() );
	if ( !condition.good() )
	{
	  return false;
	}
      }
    }
    else
    {
      if( itksys::SystemTools::FileIsDirectory ( this->GetFileName() ) )
      {
	itksys::Directory directory;
	directory.Load( this->GetFileName() );
	for( unsigned long i=0; i<directory.GetNumberOfFiles(); i++ )
	{
	  std::string name = directory.GetPath();
	  name += directory.GetFile (i);
	  
	  ::DcmFileFormat dicomFile;
	  ::OFCondition condition = dicomFile.loadFile( name.c_str() );
	  if ( !condition.good() )
	  {
	    return false;
	  }
	}
      }
      else
      {
	::DcmFileFormat dicomFile;
	::OFCondition condition = dicomFile.loadFile( this->GetFileName() );
	if ( !condition.good() )
	{
	  return false;
	}
      }
    }  

    return true;
    */
  }


  void DCMTKImageIO::PopulateDictionary(StringMap& stringMap,
					FloatMap& floatMap,
					DoubleMap& doubleMap,
					Int32Map& int32Map,
					Int16Map& int16Map,
					UInt32Map& uint32Map,
					UInt16Map& uint16Map)
  {

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
  }


  


  void DCMTKImageIO::DeterminePixelType()
  {
    MetaDataDictionary& dicomDictionary = this->GetMetaDataDictionary();
    
    MetaDataDictionary::ConstIterator bitsAllocatedIterator = dicomDictionary.Find( "(0028,0100)" );
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
      
      
      MetaDataDictionary::ConstIterator signBits = dicomDictionary.Find ( "(0028,0103)" );
      std::string sign = "0";
      if( signBits == dicomDictionary.End() ) {
	itkWarningMacro (<< "Missing Pixel Representation (0028,0103), assuming unsigned");
      }
      else {
	
	if( MetaDataStringType* metaDataSign = dynamic_cast<MetaDataStringType*>( signBits->second.GetPointer() ) ){
	  sign = metaDataSign->GetMetaDataObjectValue();
	}
	else if ( MetaDataVectorStringType* metaDataSign = dynamic_cast<MetaDataVectorStringType*>( signBits->second.GetPointer() ) ){
	  sign = metaDataSign->GetMetaDataObjectValue()[0];
	}
	//else
	//itkWarningMacro (<< "Cannot cast sign bit");
      }
      
      if ( sign == "0" ) {
	sign = "U";
      }
      else {
	sign = "S";
      }
      
      if( bitsAllocated == 8 && sign=="U" ){
	this->SetComponentType ( UCHAR );
      }
      else if ( bitsAllocated == 8 && sign=="S" ){
	this->SetComponentType ( CHAR );
      }
      else if ( bitsAllocated == 16 && sign=="U") {
	this->SetComponentType ( USHORT );
      }
      else if ( bitsAllocated == 16 && sign=="S") {
	this->SetComponentType ( SHORT );
      }
      else if ( bitsAllocated == 32 && sign=="U") {
	this->SetComponentType ( UINT );
      }
      else if ( bitsAllocated == 32 && sign=="S") {
	this->SetComponentType ( INT );
      }
      else if (bitsAllocated == 64 ) {
	this->SetComponentType(DOUBLE);
      }
      else
	this->SetComponentType (UNKNOWNCOMPONENTTYPE);
    }
    else
    {
      throw std::runtime_error( "cannot find BitsAllocated tag (0028,0100)" );
    }
  }
  



  void DCMTKImageIO::DetermineSpacing()
  {
    MetaDataDictionary& dicomDictionary = this->GetMetaDataDictionary();

    std::string PixelSpacing;
    double pixSizeX = 1.0;
    double pixSizeY = 1.0;
    if( MetaDataStringType* metaDataString = dynamic_cast<MetaDataStringType*>( dicomDictionary["(0028,0030)"].GetPointer() ) )
    {
      PixelSpacing = metaDataString->GetMetaDataObjectValue();
    }
    else
    {
      if( MetaDataVectorStringType* metaDataVectorString = dynamic_cast<MetaDataVectorStringType*>( dicomDictionary["(0028,0030)"].GetPointer() ) ) {
	PixelSpacing = metaDataVectorString->GetMetaDataObjectValue()[0];
      }
      else
	itkExceptionMacro (<< "THIS IS NOT HAPPENING!!" );
    }
    PixelSpacing[ PixelSpacing.find( "\\" ) ] = ' ';
    std::istringstream iss( PixelSpacing );
    iss >> pixSizeX >> pixSizeY;
    

    std::string SliceThicknessString;
    double SliceThickness = 0.0;
    
    if( MetaDataStringType* metaDataString = dynamic_cast<MetaDataStringType*>( dicomDictionary["(0018,0050)"].GetPointer() ) )
    {	
      SliceThicknessString = metaDataString->GetMetaDataObjectValue();
    }
    else
    {
      if( MetaDataVectorStringType* metaDataVectorString = dynamic_cast<MetaDataVectorStringType*>( dicomDictionary["(0018,0050)"].GetPointer() ) ) {
	SliceThicknessString = metaDataVectorString->GetMetaDataObjectValue()[0];
	if ( !this->toScalar( SliceThicknessString, SliceThickness ) ) {
	  itkWarningMacro (<< "failed to convert SliceThicknessString from string to double: " << SliceThicknessString << ", assuming 1.0");
	  SliceThickness = 1.0;
	}
      }
    }
    
    
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
	if( MetaDataVectorStringType* metaDataVectorString = dynamic_cast<MetaDataVectorStringType*>( dicomDictionary["(0018,0088)"].GetPointer() ) ) {
	  SpacingBetweenSlicesString = metaDataVectorString->GetMetaDataObjectValue()[0];
	  if ( !this->toScalar( SpacingBetweenSlicesString, SpacingBetweenSlices ) ) {
	    itkWarningMacro (<< "failed to convert SpacingBetweenSlices from string to double: " << SpacingBetweenSlicesString  << ", assuming 1.0" );
	    SpacingBetweenSlices = 1.0;
	  }
	}
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
    if( this->GetNumberOfDimensions()==4 )
      m_Spacing[3] = 1.0;
    
  }



  void DCMTKImageIO::DetermineDimensions()
  {
    MetaDataDictionary& dicomDictionary = this->GetMetaDataDictionary();
    
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
  }



  void DCMTKImageIO::DetermineOrigin()
  {
    MetaDataDictionary& dicomDictionary = this->GetMetaDataDictionary();
    
    double originX = 0.0;
    double originY = 0.0;
    double originZ = 0.0;
    
    std::string s_origin = "0.0\0.0\0.0";
    if( MetaDataStringType* metaDataString = dynamic_cast<MetaDataStringType*>( dicomDictionary["(0020,0032)"].GetPointer() ) )
    {
      s_origin = metaDataString->GetMetaDataObjectValue();
    }
    else
    {
      MetaDataVectorStringType* metaDataVectorString = dynamic_cast<MetaDataVectorStringType*>( dicomDictionary["(0020,0032)"].GetPointer() );
      s_origin = metaDataVectorString->GetMetaDataObjectValue()[0];
    }
    
    std::replace ( s_origin.begin(), s_origin.end(), '\\', ' ');
    std::istringstream iss( s_origin );
    iss >> originX >> originY >> originZ;
    
    m_Origin [0] = originX;
    m_Origin [1] = originY;
    m_Origin [2] = originZ;
    if( this->GetNumberOfDimensions()==4 )
      m_Origin [3] = 0.0;
  }
  
  
  
  
  void DCMTKImageIO::DetermineOrientation()
  {
    MetaDataDictionary& dicomDictionary = this->GetMetaDataDictionary();

    std::string s_orientation = "1.0\0.0\0.0\0.0\1.0\0.0";
    if( MetaDataStringType* metaDataString = dynamic_cast<MetaDataStringType*>( dicomDictionary["(0020,0037)"].GetPointer() ) )
    {
      s_orientation = metaDataString->GetMetaDataObjectValue();
    }
    else
    {
      MetaDataVectorStringType* metaDataVectorString = dynamic_cast<MetaDataVectorStringType*>( dicomDictionary["(0020,0037)"].GetPointer() );
      s_orientation = metaDataVectorString->GetMetaDataObjectValue()[0];
    }
    
    double orientation[6];
    std::replace ( s_orientation.begin(), s_orientation.end(), '\\', ' ');
    std::istringstream iss( s_orientation );
    for( int i=0; i<6; i++ )
      iss >> orientation[i];

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

    if( this->GetNumberOfDimensions()==4 ){
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

      /*
	Three possibilities:
	
	- The user has specified a series of filenames using the SetFileNames() method.
	We treat this case in priority. All dicoms are read to form a 3D or 4D volume/

	- The user has specified a unique filename using SetFileName(). Image is read
	to form a 3D volume (one slice = 3D volume)

	- The user has specified a directory: we list all files in the directory and go
	back to the first case.
	
       */

      std::set< std::string > fileNames;     
      int32_t                 fileCount = 0;

      for( unsigned int i=0; i<this->GetFileNames().size(); i++ )
	fileNames.insert ( this->GetFileNames()[i] );

      /*
      if( this->GetFileNames().size()>0 )
      {
	for(unsigned int i=0; i<this->GetFileNames().size(); i++ )
	{
	  fileNames.insert ( itksys::SystemTools::GetFilenameName ( this->GetFileNames()[i] ) );
	}
	m_Directory = itksys::SystemTools::GetFilenamePath ( this->GetFileNames()[0] );
      }
      else
      {
	if( itksys::SystemTools::FileIsDirectory ( this->GetFileName() ) )
	{
	  itksys::Directory directory;
	  directory.Load( this->GetFileName() );
	  for( unsigned long i=0; i<directory.GetNumberOfFiles(); i++ )
	  {
	    std::string name = directory.GetPath();
	    name += directory.GetFile (i);
	    
	    ::DcmFileFormat dicomFile;
	    ::OFCondition condition = dicomFile.loadFile( name.c_str() );
	    if ( condition.good() )
	    {
	      fileNames.insert ( directory.GetFile ( i ) );
	    }
	  }
	  m_Directory = directory.GetPath();
	}
	else
	{
	  fileNames.insert ( itksys::SystemTools::GetFilenameName ( this->GetFileName() ) );
	  m_Directory = itksys::SystemTools::GetFilenamePath ( this->GetFileName() );
	}
      }
      */
      fileCount = (int32_t)( fileNames.size() );

      if( fileCount == 0 )
      {
	itkExceptionMacro (<<"Cannot find any dicom in directory or dicom is not valid");	
      }

      std::set< std::string >::const_iterator f = fileNames.begin(), fe = fileNames.end();
      
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
	if( m_Directory != "" )
	  filename = m_Directory + "/" + *f;
	else
	  filename = *f;
	
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

	  std::vector< std::string > vecSlice = stringMap["(0020,1041)"];
	  if( vecSlice.size()>0 ) {
	    this->toScalar( vecSlice[fileIndex], floatSliceLocation );
	  }
	  else {
	    vecSlice = stringMap["(0020,0050)"];
	    if( vecSlice.size()>0 ) {
	      this->toScalar( vecSlice[fileIndex], floatSliceLocation );
	    }
	    //else // Hopeless
	  }

	  locations.insert( floatSliceLocation );
	  locationToNameLut.insert( std::pair< float, std::string >(floatSliceLocation, *f ) );
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
	    locationToNameLut.insert( std::pair< float, std::string >( *l, *fn ) );
	    ++ fn;
	  }
	  ++ in;
	}
	++ l;
      }


      // collecting slice count and rank count while doing sanity checks
      int32_t sizeZ, sizeT;
      
      sizeZ = ( int32_t )locations.size();
      sizeT = ( int32_t )locationToNameLut.count( *locations.begin() );

      if( sizeT > 1 ) {
	this->SetNumberOfDimensions (4);
	m_Dimensions[3] =  sizeT;
      }
      else {
	this->SetNumberOfDimensions (3);
      }
      m_Dimensions[2] =  sizeZ;

      
      
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


      //std::cout << "TransferUID: " << stringMap[ "(0002,0010)" ][0] << std::endl;
      
      
      // creating dicom dictionary
      this->PopulateDictionary (stringMap,
				floatMap,
				doubleMap,
				int32Map,
				int16Map,
				uint32Map,
				uint16Map);
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
	if ( ( int32_t )locationToNameLut.count( *l ) != sizeT )
	{
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

      this->SetFileNames ( m_OrderedFileNames );
      
      for(unsigned int i=0; i<m_OrderedFileNames.size(); i++ )
	std::cout << this->GetFileNames()[i] << std::endl;
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


  void DCMTKImageIO::ThreadedRead (void* buffer, RegionType region, int threadId)
  {
    bool isJpeg = 0;
    unsigned long pixelCount = m_Dimensions[0] * m_Dimensions[1];
    
    int start = region.GetIndex()[0];
    int length = region.GetSize()[0];
    
    for( int i=start; i<start+length; i++)
    {
      this->InternalRead (buffer, i, pixelCount, isJpeg);
      if( threadId==0 )
	this->InvokeEvent ( SliceReadEvent() );
    }
  }
  


  void DCMTKImageIO::InternalRead (void* buffer, int slice, unsigned long pixelCount, bool isJpeg)
  {
    std::string filename;
    if( m_Directory=="" )
      filename = m_OrderedFileNames[slice];
    else
      filename = m_Directory + "/" + m_OrderedFileNames[slice];

    ::DcmFileFormat dicomFile;
    ::DcmStack stack;
    
#ifdef UID_RawDataStorage
    ::DcmInputFileStream dicomStream( filename.c_str(), 0U );
#else
    ::DcmFileStream dicomStream( filename.c_str(), DCM_ReadMode );
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
    
    
    unsigned char* copyBuffer = 0;
    Uint8* ucharBuffer = 0;
    Sint16* shortBuffer = 0;
    Uint16* ushortBuffer = 0;
    Sint32* intBuffer = 0;
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
	  pixelData->getSint16Array( shortBuffer );
	  copyBuffer = (unsigned char*)( shortBuffer );
	  length = (size_t)(pixelCount * sizeof( short ) );
	  break;
	  
	case USHORT:
	  pixelData->getUint16Array( ushortBuffer );
	  copyBuffer = (unsigned char*)( ushortBuffer );
	  length = (size_t)(pixelCount * sizeof( unsigned short ) );
	  break;
	  
	case INT:
	  pixelData->getSint32Array( intBuffer );
	  copyBuffer = (unsigned char*)( intBuffer );
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
    if ( isJpeg )
    {
      ::DJDecoderRegistration::cleanup();
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
    std::string name = this->GetMetaDataValueString ( "(0010,0010)" );
    return name;
  }

  std::string DCMTKImageIO::GetPatientID() const
  {
    std::string name = this->GetMetaDataValueString ( "(0010,0020)" );
    return name;
  }
  
  std::string DCMTKImageIO::GetPatientSex() const
  {
    std::string name = this->GetMetaDataValueString ( "(0010,0040)" );
    return name;
  }
  
  std::string DCMTKImageIO::GetPatientAge() const
  {
    std::string name = this->GetMetaDataValueString ( "(0010,1010)" );
    return name;
  }
  
  std::string DCMTKImageIO::GetStudyID() const
  {
    std::string name = this->GetMetaDataValueString ( "(0020,0010)" );
    return name;
  }
  
  std::string DCMTKImageIO::GetPatientDOB() const
  {
    std::string name = this->GetMetaDataValueString ( "(0010,0030)" );
    return name;
  }
  
  std::string DCMTKImageIO::GetStudyDescription() const
  {
    std::string name = this->GetMetaDataValueString ( "(0008,1030)" );
    return name;
  }

  std::string DCMTKImageIO::GetSeriesDescription() const
  {
    std::string name = this->GetMetaDataValueString ( "(0008,103e)" );
    return name;
  }
  
  std::string DCMTKImageIO::GetBodyPart() const
  {
    std::string name = this->GetMetaDataValueString ( "(0018,0015)" );
    return name;
  }
  
  std::string DCMTKImageIO::GetNumberOfSeriesInStudy() const
  {
    std::string name = this->GetMetaDataValueString ( "(0020,1000)" );
    return name;
  }
  
  std::string DCMTKImageIO::GetNumberOfStudyRelatedSeries() const
  {
    std::string name = this->GetMetaDataValueString ( "(0020,1206)" );
    return name;
  }
  
  std::string DCMTKImageIO::GetStudyDate() const
  {
    std::string name = this->GetMetaDataValueString ( "(0008,0020)" );
    return name;
  }
  
  std::string DCMTKImageIO::GetModality() const
  {
    std::string name = this->GetMetaDataValueString ( "(0008,0060)" );
    return name;
  }
  
  std::string DCMTKImageIO::GetManufacturer() const
  {
    std::string name = this->GetMetaDataValueString ( "(0008,0070)" );
    return name;
  }
  
  std::string DCMTKImageIO::GetInstitution() const
  {
    std::string name = this->GetMetaDataValueString ( "(0008,0080)" );
    return name;
  }
  
  std::string DCMTKImageIO::GetModel() const
  {
    std::string name = this->GetMetaDataValueString ( "(0008,1090)" );
    return name;
  }
  
  std::string DCMTKImageIO::GetScanOptions() const
  {
    std::string name = this->GetMetaDataValueString ( "(0018,0022)" );
    return name;
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


      uint64_t e;

      // reading meta info
      ::DcmMetaInfo* metaInfo = dicomFile.getMetaInfo();      
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
      
      std::string tagName   = dicomTag->getTagName();
      std::string tagVRName = dicomTag->getVRName();
      uint16_t tagGroup     = dicomTag->getGTag();
      uint16_t tagElement   = dicomTag->getETag();
      
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
		stringMap[ tagGroupElement ][ fileIndex ]= filter( applicationEntity );
	      }
	    }
	    break;
	    
	    
	    // age string
	  case ::EVR_AS:	    
	    {	      
	      if ( stringMap.find( tagGroupElement ) == stringMap.end() )
	      {
		stringMap[ tagGroupElement ] = std::vector< std::string >( fileCount, "" );
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
		stringMap[ tagGroupElement ] = std::vector< std::string >( fileCount, "" );		
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
		stringMap[ tagGroupElement ] = std::vector< std::string >( fileCount, "" );
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
		stringMap[ tagGroupElement ] = std::vector< std::string >( fileCount, "" );
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
		stringMap[ tagGroupElement ] = std::vector< std::string >( fileCount, "" );
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
		floatMap[ tagGroupElement ] = std::vector< float >( fileCount, 0.0f );
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
		doubleMap[ tagGroupElement ] = std::vector< double >( fileCount, 0.0 );
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
		stringMap[ tagGroupElement ] = std::vector< std::string >( fileCount, "" );
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
		stringMap[ tagGroupElement ] = std::vector< std::string >( fileCount, "" );
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

    if ( msg.empty() )
     {
       value = 0;
       return false;
     }
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


  std::string DCMTKImageIO::GetMetaDataValueString (const char* key) const
  {
    std::string value = "Not Found";
    const MetaDataDictionary& dicomDictionary = this->GetMetaDataDictionary();
    MetaDataDictionary::ConstIterator it = dicomDictionary.Find ( key );
    if( it!=dicomDictionary.End() ) {
      if( MetaDataStringType* metaData = dynamic_cast<MetaDataStringType*>( it->second.GetPointer() ) )
	value = metaData->GetMetaDataObjectValue();
    }
    return value;
  }


  std::vector<std::string> DCMTKImageIO::GetMetaDataValueVectorString (const char* key) const
  {
    std::vector<std::string> values;
    const MetaDataDictionary& dicomDictionary = this->GetMetaDataDictionary();
    MetaDataDictionary::ConstIterator it = dicomDictionary.Find ( key );
    if( it!=dicomDictionary.End() ) {
      if( MetaDataVectorStringType* metaData = dynamic_cast<MetaDataVectorStringType*>( it->second.GetPointer() ) ) {
	values = metaData->GetMetaDataObjectValue();
      }
    }
    return values;
  }
  
}

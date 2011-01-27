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

#include <dcmtk/dcmdata/dcrledrg.h>
#include <dcmtk/dcmjpeg/djdecode.h>
#include <dcmtk/dcmdata/dcstack.h>

#include <vnl/vnl_vector.h>
#include <vnl/vnl_cross.h>


#include <float.h>

#include <fstream>



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

    DcmRLEDecoderRegistration::registerCodecs();
    DJDecoderRegistration::registerCodecs();
    
  }
  

  DCMTKImageIO::~DCMTKImageIO()
  {
    DcmRLEDecoderRegistration::cleanup();
    DJDecoderRegistration::cleanup();
  }


  void DCMTKImageIO::PrintSelf (std::ostream& os, Indent indent) const
  {
    Superclass::PrintSelf (os, indent);
  }
  
  
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



  void DCMTKImageIO::DetermineNumberOfPixelComponents()
  {

    const StringVectorType &pixelComponentCountVec = this->GetMetaDataValueVectorString ("(0028,0002)");
    if ( !pixelComponentCountVec.size() )
    {
      itkExceptionMacro ( << "Tag (0028,0002) (SamplesPerPixel) not found" );
    }

    std::istringstream s_stream ( pixelComponentCountVec[0].c_str() );
    int samplesPerPixel = 1;
    if ( !(s_stream >> samplesPerPixel) )
    {
      itkWarningMacro ( << "Cannot convert string to int: " << pixelComponentCountVec[0].c_str()
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
  


  void DCMTKImageIO::DeterminePixelType()
  {
    /*
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
      itkWarningMacro (<< "Missing Pixel Representation (0028,0103), assuming unsigned" << std::endl);
    }

    if ( sign == "0" )
    {
      sign = "U";
    }
    else
    {
      sign = "S";
    }

    const StringVectorType &rescaleInterceptVec = this->GetMetaDataValueVectorString ("(0028,1052)");
    const StringVectorType &rescaleSlopeVec     = this->GetMetaDataValueVectorString ("(0028,1053)");

    double rescaleIntercept = 0.0;
    if (rescaleInterceptVec.size())
    {
      std::istringstream intercept_stream ( rescaleInterceptVec[0].c_str() );
      if ( !(intercept_stream >> rescaleIntercept) )
	itkExceptionMacro ( << "Cannot convert string to double: " << rescaleInterceptVec[0].c_str() );
    }
    else
      itkWarningMacro (<< "Missing Rescale Intercept (0028,1052)" << std::endl);

    double rescaleSlope = 1.0;
    if (rescaleSlopeVec.size())
    {
      std::istringstream slope_stream ( rescaleSlopeVec[0].c_str() );
      if ( !(slope_stream >> rescaleSlope) )
	itkExceptionMacro ( << "Cannot convert string to double: " << rescaleSlopeVec[0].c_str() );
    }
    else
      itkWarningMacro (<< "Missing Rescale Slope (0028,1053)" << std::endl);

    if (rescaleSlope==0.0)
    {
      itkWarningMacro (<< "Rescale Slope is null, setting it to 1.0" << std::endl);
      rescaleSlope = 1.0;
    }

    if (rescaleIntercept<0.0) // very probably signed representation
      sign = "S";
    
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
    */
    
    DicomImage *image = new DicomImage(m_FileName.c_str(), CIF_UseAbsolutePixelRange);
    if (image != NULL)
    {
      if (image->getStatus() == EIS_Normal)
      {
	const DiPixel *dmp = image->getInterData();
	
	switch( dmp->getRepresentation() )
	{
	    case EPR_Uint8:
	      this->SetComponentType ( UCHAR );
	      break;
	      
	    case EPR_Sint8:
	      this->SetComponentType ( CHAR );
	      break;
	      
	    case EPR_Uint16:
	      this->SetComponentType ( USHORT );
	      break;
		
	    case EPR_Sint16:
	      this->SetComponentType ( SHORT );
	      break;

	    case EPR_Uint32:
	      this->SetComponentType ( UINT );
	      break;

	    case EPR_Sint32:
	      this->SetComponentType ( INT );
	      break;

	    default:
	       this->SetComponentType (UNKNOWNCOMPONENTTYPE);
	}
      }
      else
	this->SetComponentType (UNKNOWNCOMPONENTTYPE);

      delete image;
    }
  }
  
  
  
  
  void DCMTKImageIO::DetermineSpacing()
  {
    m_Spacing[0] = 1.0;
    m_Spacing[1] = 1.0;
    const StringVectorType &pixSpacingVec = this->GetMetaDataValueVectorString ("(0028,0030)");
    if ( !pixSpacingVec.size() )
    {
      itkWarningMacro ( << "Tag (0028,0030) (PixelSpacing) was not found, assuming 1.0" << std::endl);
    }
    else
    {
      std::string pixSpacingStr = pixSpacingVec[0];
      std::istringstream is_stream( pixSpacingStr );
      if (!(is_stream >> m_Spacing[0]))
      {
	itkWarningMacro ( << "Cannot convert string to double: " << pixSpacingStr.c_str() << std::endl);
      }
      if (!(is_stream >> m_Spacing[1]))
      {
	itkWarningMacro ( << "Cannot convert string to double: " << pixSpacingStr.c_str() << std::endl);
      }
    }
    


    m_Spacing[2] = 1.0;

    const StringVectorType &imagePositions = this->GetMetaDataValueVectorString("(0020,0032)");

    /**
       Z-spacing is determined by the ImagePositionPatient flag. Compute the distance between
       2 consecutive slices and project it onto the acquisition axis (normal of the ImageOrientation).
       Average it over all slices, use a 10% margin to distinguish between volumes in case of 4D images.
     */
    if( imagePositions.size()>1 )
    {
      std::vector<double> gaps (imagePositions.size()-1, 0.0);

      vnl_vector<double> normal (3);
      normal[0] = m_Direction[2][0];
      normal[1] = m_Direction[2][1];
      normal[2] = m_Direction[2][2];

      double ref_gap = MAXIMUM_GAP; // choose ref_gap as minimum gap between slices
      for (unsigned int i=1; i<imagePositions.size(); i++)
      {
	std::istringstream is_stream1( imagePositions[i-1].c_str() );
	vnl_vector<double> pos1 (3);
	is_stream1 >> pos1[0];
	is_stream1 >> pos1[1];
	is_stream1 >> pos1[2];
	
	std::istringstream is_stream2( imagePositions[i].c_str() );
	vnl_vector<double> pos2 (3);
	is_stream2 >> pos2[0];
	is_stream2 >> pos2[1];
	is_stream2 >> pos2[2];
	
	vnl_vector<double> v21 = pos2-pos1;
	
	gaps[i-1] = fabs ( dot_product (normal, v21) );
	if (gaps[i-1]<ref_gap && gaps[i-1]>0.0)
	  ref_gap = gaps[i-1];
      }
      double total_gap = ref_gap;
      double min_gap = 0.9*ref_gap;
      double max_gap = 1.1*ref_gap;
      int gapCount = 1;
      for(unsigned int i=1; i<gaps.size(); i++)
      {
	if (gaps[i]>=min_gap && gaps[i]<=max_gap)
	{
	  total_gap += gaps[i];
	  ++gapCount;
	}
	else
	{
	  ; //itkWarningMacro (<< "Inconsistency in slice spacing: " << ref_gap << " " << gaps[i]);
	}
      }
      if (total_gap==MAXIMUM_GAP)
	m_Spacing[2] = 1.0;
      else
	m_Spacing[2] = total_gap/(double)(gapCount);
    }
    else // rely on the SpacingBetweenSlices tag
    {
      // never use sliceThickness as it has nothing to do with the z spacing
      /*
	double sliceThickness = 1.0;
	const StringVectorType &sliceThicknessVec = this->GetMetaDataValueVectorString ("(0018,0050)");
	if( sliceThicknessVec.size() )
	{
	std::string sliceThicknessStr = sliceThicknessVec[0];
	std::istringstream is_stream( sliceThicknessStr.c_str() );
	if (!(is_stream>>sliceThickness))
	{
	itkWarningMacro ( << "Cannot convert string to double: " << sliceThicknessStr.c_str() << std::endl );
	}
	else
	{
	m_Spacing[2] = sliceThickness;
	}
	}
      */
      
      double spacingBetweenSlices = 1.0;
      const StringVectorType &spacingBetweenSlicesVec = this->GetMetaDataValueVectorString ("(0018,0088)");
      if( spacingBetweenSlicesVec.size() )
      {
	std::string spacingBetweenSlicesStr = spacingBetweenSlicesVec[0];
	std::istringstream is_stream( spacingBetweenSlicesStr.c_str() );
	if (!(is_stream>>spacingBetweenSlices))
	{
	  itkWarningMacro ( << "Cannot convert string to double: " << spacingBetweenSlicesStr.c_str() << std::endl );
	}
	else
	{
	  m_Spacing[2] = spacingBetweenSlices;
	}
      }
    }
    
    if (this->GetNumberOfDimensions()==4)
      m_Spacing[3] = 1.0; 
  }
  
  
  
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
  }



  void DCMTKImageIO::DetermineOrigin()
  {
    m_Origin [0] = 0.0;
    m_Origin [1] = 0.0;
    m_Origin [2] = 0.0;
    if( this->GetNumberOfDimensions()==4 )
    {
      m_Origin [3] = 0.0;
    }


    int startIndex = m_FilenameToIndexMap[ m_LocationToFilenamesMap.lower_bound ( *m_LocationSet.begin() )->second ];
	
    std::string s_origin = this->GetMetaDataValueString("(0020,0032)", startIndex);
    if ( s_origin=="" )
    {
      itkWarningMacro ( << "Tag (0020,0032) (ImageOrigin) was not found, assuming 0.0/0.0/0.0" << std::endl);
      return;
    }
    
    std::istringstream is_stream( s_origin.c_str() );
    if (!(is_stream >> m_Origin[0]))
    {
      itkWarningMacro ( << "Cannot convert string to double: " << s_origin.c_str() << std::endl );
    }
    if (!(is_stream >> m_Origin[1]))
    {
      itkWarningMacro ( << "Cannot convert string to double: " << s_origin.c_str() << std::endl );
    }
    if (!(is_stream >> m_Origin[2]))
    {
      itkWarningMacro ( << "Cannot convert string to double: " << s_origin.c_str() << std::endl );
    }
  }
  
  
  
  
  void DCMTKImageIO::DetermineOrientation()
  {

    double orientation[6]={1.0, 0.0, 0.0, 0.0, 1.0, 0.0};
    const StringVectorType &orientationVec = this->GetMetaDataValueVectorString("(0020,0037)");
    if (!orientationVec.size())
    {
      itkWarningMacro ( << "Tag (0020,0037) (PatientOrientation) was not found, assuming identity" << std::endl);
    }
    else
    {
      std::string orientationStr = orientationVec[0];
      std::istringstream is_stream( orientationStr.c_str() );
      for( int i=0; i<6; i++ )
      {
	if (!(is_stream >> orientation[i]) )
	{
	  itkWarningMacro ( << "Cannot convert string to double: " << orientationStr.c_str() << std::endl );
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


  double DCMTKImageIO::GetZPositionForImage (int index)
  {
    std::string s_position = this->GetMetaDataValueString("(0020,0032)", index);
    double zpos = 0.0;
    double junk;
    std::istringstream is_stream( s_position.c_str() );
    if (!(is_stream >> junk) )
    {
      itkWarningMacro ( << "Cannot convert string to double: " << s_position.c_str() << std::endl );
    }
    if (!(is_stream >> junk) )
    {
      itkWarningMacro ( << "Cannot convert string to double: " << s_position.c_str() << std::endl );
    }
    if (!(is_stream >> zpos))
    {
      itkWarningMacro ( << "Cannot convert string to double: " << s_position.c_str() << std::endl );
    }

    return zpos;
  }


  
  void DCMTKImageIO::ReadImageInformation()
  {

    /**
       Using a set, we remove any duplicate filename - should we do this?
    */
    NameSetType fileNames;
    
    for( unsigned int i=0; i<this->GetFileNames().size(); i++ )
    {
      fileNames.insert ( this->GetFileNames()[i] );
    }
    
    int fileCount = (int)( fileNames.size() );
    if( fileCount == 0 )
    {
      itkExceptionMacro (<<"Cannot find any dicom in directory or dicom is not valid");	
    }
    
    
    m_LocationSet.clear();
    m_FilenameToIndexMap.clear();
    m_LocationToFilenamesMap.clear();
    
    
    int    fileIndex = 0;
    double sliceLocation = 0;
    
    
    
    /**
       The purpose of the next loop is to parse the DICOM header of each file, store all
       fields in the Dictionary, and order filenames depending on their sliceLocation,
       assuming that the sliceLocation field gives the order dicoms are obtained.
    */
    NameSetType::const_iterator f = fileNames.begin(), fe = fileNames.end();
    
    while ( f != fe )
    {
      std::string filename;
      if( m_Directory != "" )
	filename = m_Directory + ITK_FORWARD_PATH_SLASH + *f;
      else
	filename = *f;
      
      try
      {
	this->ReadHeader( filename, fileIndex, fileCount );
	
	sliceLocation = 0;
	std::string vecSlice = this->GetMetaDataValueString ("(0020,1041)", fileIndex );
	if( vecSlice!="" )
	{
	  std::istringstream is_stream ( vecSlice.c_str() );
	  is_stream >> sliceLocation;
	}
	else
	{
	  std::string vecSlice2 = this->GetMetaDataValueString ("(0020,0050)", fileIndex);
	  if( vecSlice2!="" )
	  {
	    std::istringstream is_stream ( vecSlice2.c_str() );
	    is_stream >> sliceLocation;
	  }
	else  // instance number
	{
	  std::string vecSlice3 = this->GetMetaDataValueString("(0020,0013)", fileIndex);
	  if (vecSlice3!="")
	  {
	    std::istringstream is_stream ( vecSlice3.c_str() );
	    is_stream >> sliceLocation;
	  }
	  else // cannot find the sliceLocation information, then we rely on the order files were inputed.
	  {
	    sliceLocation = (double)fileIndex;
	  }
	}
	}

	m_LocationSet.insert( sliceLocation );
	m_LocationToFilenamesMap.insert( std::pair< double, std::string >(sliceLocation, *f ) );
	m_FilenameToIndexMap[ *f ] = fileIndex;
	++fileIndex;
      }
      catch (ExceptionObject &e)
      {
	std::cerr << e; // continue to be robust to odd files
      }
      ++f;
    }
    

    
    
    /**
       In the next loop, slices are ordered according to their instance number, in case multiple
       volumes are found.
     */
    SliceLocationSetType::const_iterator l = m_LocationSet.begin(), le = m_LocationSet.end();
    while ( l!=le )
    {
      SliceLocationToNamesMultiMapType::iterator n = m_LocationToFilenamesMap.lower_bound( *l ),
	ne = m_LocationToFilenamesMap.upper_bound( *l );
      
      // using that intermediate lut for instance number ordering
      IndexToNamesMapType instanceNumberToNameMap;
      
      while ( n!=ne )
      {
	int instanceNumber = 0;
	std::string instanceNumberString = this->GetMetaDataValueString ("(0020,0013)", m_FilenameToIndexMap[ n->second ]);
	if( instanceNumberString!="" )
	{
	  std::istringstream is_stream ( instanceNumberString.c_str() );
	  is_stream >> instanceNumber;
	}
	// else, we assume all files have the same instance number (0), i.e.: the serie has only one volume
	
	instanceNumberToNameMap[instanceNumber].push_back( n->second );
	++n;
      }
	

      // We erase the filename list corresponding to the given location to fill it with the ordered filenames
      m_LocationToFilenamesMap.erase( *l );

      
      IndexToNamesMapType::const_iterator in = instanceNumberToNameMap.begin(), ine = instanceNumberToNameMap.end();
      while ( in!=ine )
      {
	std::list< std::string >::const_iterator fn = in->second.begin(), fne = in->second.end();
	while ( fn!=fne )
	{
	  m_LocationToFilenamesMap.insert( std::pair< double, std::string >( *l, *fn ) );
	  ++fn;
	}
	++in;
      }
      ++l;
    }

    
    
    // collecting slice count and rank count while doing sanity checks
    unsigned int sizeZ = m_LocationSet.size();
    unsigned int sizeT = m_LocationToFilenamesMap.count( *m_LocationSet.begin() );

    // check consistency
    SliceLocationSetType::const_iterator it = m_LocationSet.begin();
    while (it!=m_LocationSet.end())
    {
      if ( m_LocationToFilenamesMap.count(*it)!=sizeT )
      {
	itkExceptionMacro (<< "Inconsistency in dicom volumes: " << m_LocationToFilenamesMap.count(*it) << " vs. " << sizeT);
      }
      ++it;
    }
    
    if( sizeT > 1 )
    {
      this->SetNumberOfDimensions (4);
      m_Dimensions[3] =  sizeT;
    }
    else
    {
      this->SetNumberOfDimensions (3);
    }
    m_Dimensions[2] =  sizeZ;

    

    /**
       Now that m_FilenameToIndexMap and m_LocationToFilenamesMap are up-to-date, we may determine
       the pixel type, spacing, origin and so on.
     */
    this->DetermineNumberOfPixelComponents();
    this->DeterminePixelType();
    this->DetermineDimensions();
    this->DetermineOrigin();
    this->DetermineOrientation();
    this->DetermineSpacing(); // always called after DetermineOrientation    



    
    /**
       Determine the slice ordering. Depending on the sliceLocation and the imagePatientPosition,
       we may determine if the acquistion was made from feet-to-head or head-to-feet.
     */
    
    l = m_LocationSet.begin();  
    SliceLocationSetType::const_reverse_iterator lle = m_LocationSet.rbegin();
    
    double startLocation = *l;
    double endLocation   = *lle;
    int locSign = endLocation>startLocation?1.0:-1.0;
    
    // just check first volume
    int startIndex = m_FilenameToIndexMap[ m_LocationToFilenamesMap.lower_bound ( *l )->second ];
    int endIndex   = m_FilenameToIndexMap[ m_LocationToFilenamesMap.lower_bound ( *lle )->second ];
    
    double startSlice = this->GetZPositionForImage ( startIndex );
    double endSlice   = this->GetZPositionForImage ( endIndex );
    
    int sliceDirection = endSlice>startSlice?locSign:-locSign;




    
    /**
       Now order filenames such that we can read them sequentially and build the 3D/4D volume.
     */
    m_OrderedFileNames = StringVectorType ( sizeZ * sizeT );
    
    int location = 0;
    int rank     = 0;
    
    while ( l!=le )
    {
      SliceLocationToNamesMultiMapType::const_iterator n = m_LocationToFilenamesMap.lower_bound( *l ),
	ne = m_LocationToFilenamesMap.upper_bound( *l );
      
      rank = 0;
      while ( n!=ne )
      {
	if( sliceDirection>0 )
	  m_OrderedFileNames[ rank * sizeZ + location ] = n->second;
	else
	  m_OrderedFileNames[ rank * sizeZ + ( sizeZ - 1 - location ) ] = n->second;
	
	++rank;
	++n; 
      }	
      ++location;
      ++l;
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
      {
	this->SetProgress( (double)(i-start+1)/(double)(length) );
	this->InvokeEvent ( ProgressEvent() );
      }
    }
  }
  


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
    
    
    size_t length = pixelCount;
    int bitsPerSample = 8;
    switch( this->GetComponentType() )
    {  
	case CHAR:
	  length *= sizeof(char);
	  bitsPerSample = 8;
	  break;
	  
	case UCHAR:
	  length *= sizeof(Uint8);
	  bitsPerSample = 8;
	  break;
	  
	case SHORT:
	  length *= sizeof(Sint16);
	  bitsPerSample = 16;
	  break;
	    
	case USHORT:
	  length *= sizeof(Uint16);
	  bitsPerSample = 16;
	  break;
	  
	case INT:
	  length *= sizeof(Sint32);
	  bitsPerSample = 32;
	  break;
	  
	case UINT:
	  length *= sizeof(Uint32);
	  bitsPerSample = 32;
	  break;
	  
	case DOUBLE:
	  length *= sizeof(Float64);
	  bitsPerSample = 64;
	  break;
	  
	default:
	  throw ExceptionObject (__FILE__,__LINE__,"Unsupported pixel data type in DICOM");
    }

    length *= this->GetNumberOfComponents();
    

    const Uint8* copyBuffer = 0;


    // dicomFile.getDataset()->findAndGetUint8Array (DCM_PixelData, copyBuffer);


    // We use DicomImage as it rescales the raw values properly for visualization
    DicomImage *image = new DicomImage(filename.c_str(), CIF_UseAbsolutePixelRange);
    if (image != NULL)
    {
      if (image->getStatus() == EIS_Normal)
      {
	DcmFileFormat *dcm = new DcmFileFormat;
	dcm->loadFile (filename.c_str());
	DcmDataset *dset = dcm->getDataset();

	OFString ofstr;
	if (dset->findAndGetOFString(DCM_Modality, ofstr)==EC_Normal)
	{
	  if (ofstr=="CT")
	  {
	    double minValue, maxValue;
	    image->getMinMaxValues (minValue, maxValue);

	    if (minValue<-1000) // probably wrong pixelRepresentation
	    {
	      dset->putAndInsertUint16 (DCM_PixelRepresentation, 0);
	      
	      delete image;
	      image = new DicomImage (dcm, dset->getOriginalXfer(), CIF_UseAbsolutePixelRange);
	    }
	  }
	}
	
	const DiPixel *dmp = image->getInterData();
	if (!dmp)
	  itkExceptionMacro ( << "DiPixel object is null" );
	
	// copyBuffer = (Uint8 *)(image->getOutputData(bitsPerSample));
	copyBuffer = (Uint8 *)dmp->getData();
	if (!copyBuffer)
	{
	  itkExceptionMacro ( << "Bad copy buffer" );
	}

	delete dcm;
      }
    }
    
    Uint8* destBuffer = static_cast<Uint8*>(buffer);    
    if (!copyBuffer || !destBuffer)
    {
      itkExceptionMacro ( << "Bad copy or dest buffer" );
    }
    
    std::memcpy (destBuffer + slice*length, copyBuffer, length);

    delete image;
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
    std::string name = this->GetMetaDataValueString ( "(0020,000d)", 0 );
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

  std::string DCMTKImageIO::GetSeriesID() const
  {
    std::string name = this->GetMetaDataValueString ( "(0020,000e)", 0 );
    return name;
  }
  
  std::string DCMTKImageIO::GetOrientation() const
  {
    std::string name = this->GetMetaDataValueString ( "(0020,0037)", 0 );
    return name;
  }

  std::string DCMTKImageIO::GetSeriesNumber() const
  {
    std::string name = this->GetMetaDataValueString ( "(0020,0011)", 0 );
    return name;
  }

  std::string DCMTKImageIO::GetSequenceName() const
  {
    std::string name = this->GetMetaDataValueString ( "(0018,0024)", 0 );
    return name;
  }

  std::string DCMTKImageIO::GetSliceThickness() const
  {
    std::string name = this->GetMetaDataValueString ( "(0018,0050)", 0 );
    return name;
  }

  std::string DCMTKImageIO::GetRows() const
  {
    std::string name = this->GetMetaDataValueString ( "(0028,0010)", 0 );
    return name;
  }

  std::string DCMTKImageIO::GetColumns() const
  {
    std::string name = this->GetMetaDataValueString ( "(0028,0011)", 0 );
    return name;
  }

  std::string DCMTKImageIO::GetAcquisitionDate() const
  {
    return this->GetMetaDataValueString ( "(0008,0022)", 0 );
  }

  std::string DCMTKImageIO::GetReferringPhysicianName() const
  {
    return this->GetMetaDataValueString ( "(0008,0090)", 0 );
  }

  std::string DCMTKImageIO::GetPerformingPhysicianName() const
  {
    return this->GetMetaDataValueString ( "(0008,1050)", 0 );
  }

  std::string DCMTKImageIO::GetProtocolName() const
  {
    return this->GetMetaDataValueString ( "(0018,1030)", 0 );
  }

  std::string DCMTKImageIO::GetAcquisitionComments() const
  {
    return this->GetMetaDataValueString ( "(0018,4000)", 0 );
  }

  std::string DCMTKImageIO::GetPatientStatus() const
  {
    return this->GetMetaDataValueString ( "(0011,1010)", 0 );
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
      itkExceptionMacro ( << condition.text() );
    }
    
    // manual call to load data into memory
    //dicomFile.loadAllDataIntoMemory();
    
    
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
    oss << '(' << std::hex << std::setw( 4 ) << std::setfill( '0' )<< tagGroup << ','
	<< std::hex << std::setw( 4 ) << std::setfill( '0' ) << tagElement << ")";
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


  /*

  inline std::string DCMTKImageIO::GetMetaDataValueString (const char* key, int index) const
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

  */
  /*
  inline const DCMTKImageIO::StringVectorType& DCMTKImageIO::GetMetaDataValueVectorString (const char* key) const
  {
    const MetaDataDictionary& dicomDictionary = this->GetMetaDataDictionary();
    MetaDataDictionary::ConstIterator it = dicomDictionary.Find ( key );
    if( it!=dicomDictionary.End() )
    {
      if( MetaDataVectorStringType* metaData = dynamic_cast<MetaDataVectorStringType*>( it->second.GetPointer() ) )
      {
	return metaData->GetMetaDataObjectValue();
      } else
      {
	return m_EmptyVector;
      }
    }
    return m_EmptyVector;
  }
  */  
}

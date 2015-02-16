/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medItkGisImageIO.h>

#include <itksys/SystemTools.hxx>
#include <itkByteSwapper.h>

#include <fstream>

namespace itk
{

  GISImageIO::GISImageIO()
  {
    m_IsBinary = true;
    this->SetNumberOfDimensions(3);
    m_PixelType         = SCALAR;
    m_ComponentType     = UCHAR;
    
    if (ByteSwapper<int>::SystemIsBigEndian())
    {
      m_ByteOrder = BigEndian;
    }
    else
    {
      m_ByteOrder = LittleEndian;
    }
  }
  

  GISImageIO::~GISImageIO()
  {}


  bool GISImageIO::CanReadFile(const char* filename)
  {

      std::string extension = itksys::SystemTools::GetFilenameLastExtension(filename);

      if( extension!=".ima" && extension!=".dim" )
      {
          return false;
      }

      // look if the header file exists
      std::string header_file = filename;
      if( extension==".ima")
      {
          std::string path = itksys::SystemTools::GetFilenamePath(filename);
          if (path=="")
          {
             path = ".";
          }
          header_file = path + "/" + itksys::SystemTools::GetFilenameWithoutLastExtension(filename) + ".dim";
      }

//     if ( !itksys::SystemTools::FileExists(header_file.c_str(), true) )
      if ( !itksys::SystemTools::FileExists(header_file.c_str()) )
      {
          std::cerr << "Error: Header file does not exist." << std::endl;
          return false;
      }

      return true;
}


  

  void GISImageIO::ReadImageInformation()
  {

    std::string extension = itksys::SystemTools::GetFilenameLastExtension(m_FileName);

    if( extension!=".ima" && extension!=".dim" )
      {
	throw itk::ExceptionObject(__FILE__,__LINE__,"Error in GISImageIO::ReadImageInformation(): Extension is not recognized.");
      }

    // look if the header file exists
    std::string header_file = m_FileName;
    if( extension!=".dim")
    {
        std::string path = itksys::SystemTools::GetFilenamePath(m_FileName);
        if (path=="")
        {
           path = ".";
        }
        header_file = path + "/" + itksys::SystemTools::GetFilenameWithoutLastExtension(m_FileName) + ".dim";
    }

//     if ( !itksys::SystemTools::FileExists(header_file.c_str(), true) )
    if ( !itksys::SystemTools::FileExists(header_file.c_str()) )
      {
	std::cerr << "Error: Header file does not exist." << std::endl;
      }

    std::ifstream ifs (header_file.c_str());
    
    if( ifs.fail() )
      {
	throw itk::ExceptionObject(__FILE__,__LINE__,"Error: Cannot read header file.");
      }

    int dims[4];
    for( int i=0; i<3; i++)
    {
      ifs >> dims[i];
    }

    // the last (4th) dimension is sometimes not set: the keyword type appears then
    std::string junk;
    ifs >> junk;
    if( junk!="-type" )
    {
      dims[3] = atoi ( junk.c_str() );
      
      if ( dims[3]>1 )
      {
	// this->SetNumberOfDimensions(3);
	this->SetNumberOfDimensions(4);
	//this->SetNumberOfComponents( dims[3] );
	this->SetNumberOfComponents( 1 );
	//this->SetPixelType (VECTOR);
	this->SetPixelType (SCALAR);
      }
      else
      {
	this->SetNumberOfDimensions(3);
	this->SetNumberOfComponents(1);
	this->SetPixelType (SCALAR);
      }

      ifs >> junk;
      
    }
    else
    {
      dims[3] = 1;
      this->SetNumberOfDimensions(3);
      this->SetNumberOfComponents(1);
      this->SetPixelType (SCALAR);
    }

    for( unsigned int i=0; i<this->GetNumberOfDimensions(); i++ )
    {
      m_Dimensions[i] = dims[i];
    }

    
    if (junk != "-type")
    {
      throw itk::ExceptionObject(__FILE__,__LINE__,"Error: Cannot find keyword \"-type\".");
    }
    
    std::string s_type;
    ifs >> s_type;
    
    // parse the type
    if( s_type== "S8" )
    {
      m_ComponentType = CHAR;
    }
    else if( s_type== "U8" )
    {
      m_ComponentType = UCHAR;
    }
    else if( s_type== "S16" )
    {
      m_ComponentType = SHORT;
    }
    else if( s_type== "U16" )
    {
      m_ComponentType = USHORT;
    }
    else if( s_type== "S32" )
    {
      m_ComponentType = INT;
    }
    else if( s_type== "U32" )
    {
      m_ComponentType = UINT;
    }
    else if( s_type== "F32" || s_type=="FLOAT")
    {
      m_ComponentType = FLOAT;
    }
    else if( s_type== "F64" || s_type=="DOUBLE")
    {
      m_ComponentType = DOUBLE;
    }
    else
    {
      char message[512];
      sprintf( message, "Error: Unsupported pixel type: %s.", s_type.c_str());
      throw itk::ExceptionObject(__FILE__,__LINE__,message);
    }
    
    
    // voxel size
    double spacing[4];
    
    ifs >> junk;    
    if (junk != "-dx")
    {
      throw itk::ExceptionObject(__FILE__,__LINE__,"Error: Cannot find keyword \"-dx\".");
    }
    ifs >> spacing[0];
    
    
    ifs >> junk; 
    if (junk != "-dy")
    {
      throw itk::ExceptionObject(__FILE__,__LINE__,"Error: Cannot find keyword \"-dy\".");
    }
    ifs >> spacing[1];
    
    
    ifs >> junk; 
    if (junk != "-dz")
    {
      throw itk::ExceptionObject(__FILE__,__LINE__,"Error: Cannot find keyword \"-dz\".");
    }
    ifs >> spacing[2];
    

    ifs >> junk; 
    if ( this->GetNumberOfDimensions()==4 && junk != "-dt")
    {
      throw itk::ExceptionObject(__FILE__,__LINE__,"Error: Cannot find keyword \"-dt\".");
    }
    else
    {
      ifs >> spacing[3];
      ifs >> junk;
    }
    
    for( unsigned int i=0;i<this->GetNumberOfDimensions(); i++)
    {
      m_Spacing[i] = spacing[i];
    }
    
    
    // byte order
    if( junk != "-bo")
    {
      // assuming it's LittleEndian
      //throw itk::ExceptionObject(__FILE__,__LINE__,"Error: Cannot find keyword \"-bo\".");
      m_ByteOrder = LittleEndian;
    }
    else
    {
      std::string s_byteorder;
      ifs >> s_byteorder;
      if( s_byteorder == "DCBA" )
      {
	m_ByteOrder = LittleEndian;
      }
      else
      {
	m_ByteOrder = BigEndian;
      }
      ifs >> junk;
    }
    
    
    // binary/ascii buffer    
    if( junk != "-om" )
    {
      //throw itk::ExceptionObject(__FILE__,__LINE__,"Error: Cannot find keyword \"-om\".");
      // assuming it's binary
      m_IsBinary = true;
    }
    else
    {
      std::string s_om;
      ifs >> s_om;
      if( s_om=="binar" )
      {
	m_IsBinary = true;
      }
      else
      {
	m_IsBinary = false;
      }
    }

    
  }


  


  void GISImageIO::Read(void* buffer)
  {
    
    std::string extension = itksys::SystemTools::GetFilenameLastExtension(m_FileName);
    std::string s_filename = m_FileName;
    
    if( extension != ".ima" )
    {
      std::string path = itksys::SystemTools::GetFilenamePath(m_FileName);
      if (path==""){
	path = ".";
      }
      s_filename = path+"/"+itksys::SystemTools::GetFilenameWithoutLastExtension(m_FileName)+".ima";
    }
    
    std::ifstream ifs;

    if( m_IsBinary )
    {
      ifs.open(s_filename.c_str(), std::ios::in | std::ios::binary);
    }
    else
    {
      ifs.open(s_filename.c_str(), std::ios::in);
    }
    
    if( ifs.fail() )
    {
      throw itk::ExceptionObject(__FILE__,__LINE__,"Error: Cannot open buffer in read mode.");
    }
    
    
    /*
    std::cout << "filename " << s_filename << " " << m_FileName << std::endl;
    std::cout << "Image size in bytes: " << (Superclass::SizeType)(this->GetImageSizeInBytes()) << std::endl;
    std::cout << "Type: " << this->GetComponentTypeAsString( this->GetComponentType() ) << std::endl;
    std::cout << "Size of buffer: " << sizeof( buffer)  << std::endl;
    std::cout << "Number of dimensions: " << this->GetNumberOfDimensions() << std::endl;
    std::cout << "Number of pixels: " << this->GetImageSizeInPixels() << std::endl;
    */


    if( m_IsBinary )
    {
      if( !this->ReadBufferAsBinary( ifs, buffer, this->GetImageSizeInBytes() ) )
      {
	throw itk::ExceptionObject(__FILE__,__LINE__,"Error while reading buffer as binary.");
      }
      this->SwapBytesIfNecessary( buffer, this->GetImageSizeInComponents());
    }
    else
    {
      this->ReadBufferAsASCII( ifs, buffer, m_ComponentType, this->GetImageSizeInBytes() );
    }
  }


  bool GISImageIO::CanWriteFile( const char* filename)
  {
    
    std::string extension = itksys::SystemTools::GetFilenameLastExtension(filename);

    if( extension!=".dim" && extension!=".ima" )
      {
	itkDebugMacro(<<"The filename extension is not recognized");
	return false;
      }

    /*
    if( this->GetNumberOfDimensions()<3 || this->GetNumberOfDimensions()>4 )
      {
	itkDebugMacro(<<"GIS image format only supports image of dimensions 3 or 4.");
	return false;
      }    

    // check the type as well
    //if( m_ComponentType != 
    */
    return true;
  }


  void GISImageIO::WriteImageInformation()
  {
    
    std::string header_file = m_FileName;
    std::string extension = itksys::SystemTools::GetFilenameLastExtension(m_FileName);   
    if( extension!=".dim")
    {
        std::string path = itksys::SystemTools::GetFilenamePath(m_FileName);
        if (path=="")
        {
           path = ".";
        }
        header_file = path + "/" + itksys::SystemTools::GetFilenameWithoutLastExtension(m_FileName) + ".dim";
    }
    
    
    std::ofstream ofs (header_file.c_str());
    if( ofs.fail() )
    {
      throw ExceptionObject(__FILE__,__LINE__,"Error: Cannot open header file for writing.");
    }
    

    if( this->GetNumberOfDimensions()==3 && this->GetPixelType()==SCALAR)
    {
      for( int i=0; i<3; i++ )
      {
	ofs << m_Dimensions[i] << " ";
      }
      ofs << "1\n";
    }
    else if (this->GetNumberOfDimensions()==3 && this->GetPixelType()==VECTOR )
    {
      for( int i=0; i<3; i++ )
      {
	ofs << m_Dimensions[i] << " ";
      }
      ofs << this->GetNumberOfComponents() << std::endl;
    }    

    else if ( this->GetNumberOfDimensions()==4 )
    {
      for( int i=0; i<4; i++ )
      {
	ofs << m_Dimensions[i] << " ";
      }
      ofs << std::endl;  
    }
    else 
    {
      throw ExceptionObject(__FILE__,__LINE__,"Error: Number of dimensions of pixel type not supported.");
    }
    
    
    // write the type
    ofs << "-type ";
    switch (m_ComponentType )
    {
	case CHAR:
	  ofs << "S8\n";
	  break;
	case UCHAR:
	  ofs << "U8\n";
	  break;
	case SHORT:
	  ofs << "S16\n";
	  break;
	case USHORT:
	  ofs << "U16\n";
	  break;
	case INT:
	  ofs << "S32\n";
	  break;
	case UINT:
	  ofs << "U32\n";
	  break;
	case FLOAT:
	  ofs << "FLOAT\n";
	  break;
	case DOUBLE:
	  ofs << "DOUBLE\n";
	  break;
	default:
	  throw ExceptionObject(__FILE__,__LINE__,"Error: Pixel type not supported by GIS format.");
    }
    
    // spacing
    
    ofs << "-dx " << m_Spacing[0] << " -dy " << m_Spacing[1] << " -dz " << m_Spacing[2];
    
    if( this->GetNumberOfDimensions()==3 )
    {
      ofs << " -dt 1\n";
    }
    else if ( this->GetNumberOfDimensions()==4 )
    {
      ofs << " -dt " << m_Spacing[3] << std::endl;
    }
    else
    {} // never here
    
    
    // byte order
    if( m_ByteOrder == LittleEndian )
    {
      ofs << "-bo DCBA\n";
    }
    else
    {
      ofs << "-bo ABCD\n";
    }
    
    // buffer style: only write binary files for now
    m_IsBinary = true;
    ofs << "-om binar\n";
    
  }

  

  void GISImageIO::Write(const void* buffer)
  {

    // check if image is supported
    if( this->GetNumberOfDimensions()<3 || this->GetNumberOfDimensions()>4 )
    {
      throw ExceptionObject (__FILE__,__LINE__,"GIS image format only supports image of dimensions 3 or 4.");
    }


    if ( m_PixelType != SCALAR && m_PixelType != VECTOR)
    {
      throw ExceptionObject (__FILE__,__LINE__,"Error: GIS only supports SCALAR and VECTOR images.");
    }

    if (m_ComponentType != CHAR &&
	m_ComponentType != UCHAR &&
	m_ComponentType != SHORT &&
	m_ComponentType != USHORT &&
	m_ComponentType != INT &&
	m_ComponentType != UINT &&
	m_ComponentType != FLOAT &&
	m_ComponentType != DOUBLE)
    {
      throw ExceptionObject (__FILE__,__LINE__,"Error: Pixel type is not supported by GIS format yet.");
    }
    
    

    
    this->WriteImageInformation();
    
    std::string extension = itksys::SystemTools::GetFilenameLastExtension(m_FileName);
    std::string s_filename = m_FileName;
    
    if( extension != ".ima" )
    {
        std::string path = itksys::SystemTools::GetFilenamePath(m_FileName);
        if (path=="")
        {
           path = ".";
        }
        s_filename = path + "/" + itksys::SystemTools::GetFilenameWithoutLastExtension(m_FileName) + ".ima";
    }
    
    std::ofstream ofs(s_filename.c_str());
    if( ofs.fail() )
    {
      throw itk::ExceptionObject(__FILE__,__LINE__,"Error: Cannot open buffer in write mode.");
    }


    const unsigned long numberOfBytes      = static_cast<unsigned long>( this->GetImageSizeInBytes() );
    const unsigned long numberOfComponents = static_cast<unsigned long>( this->GetImageSizeInComponents() );

    // Swap bytes if necessary
    if ( m_ByteOrder == LittleEndian )
    {
      char * tempBuffer = new char[ numberOfBytes ];
      memcpy( tempBuffer, buffer , numberOfBytes );
      this->SwapBytesIfNecessary(tempBuffer, numberOfComponents );
      ofs.write( tempBuffer, numberOfBytes );
      delete [] tempBuffer;
    }
    else //if ( m_ByteOrder == BigEndian )
    {
      char * tempBuffer = new char[ numberOfBytes ];
      memcpy( tempBuffer, buffer , numberOfBytes );
      this->SwapBytesIfNecessary(tempBuffer, numberOfComponents );
      ofs.write( tempBuffer, numberOfBytes );
      delete [] tempBuffer;
    }
   
  }



  
  void 
  GISImageIO
  ::SwapBytesIfNecessary( void* buffer, unsigned long numberOfPixels )
  {
    switch(m_ComponentType)
      {
      case CHAR:
	{
	  if ( m_ByteOrder == LittleEndian )
	    {
	      ByteSwapper<char>::SwapRangeFromSystemToLittleEndian((char*)buffer, numberOfPixels );
	    }
	  else if ( m_ByteOrder == BigEndian )
	    {
	      ByteSwapper<char>::SwapRangeFromSystemToBigEndian((char *)buffer, numberOfPixels );
	    }
	  break;
	}
      case UCHAR:
	{
	  if ( m_ByteOrder == LittleEndian )
	    {
	      ByteSwapper<unsigned char>::SwapRangeFromSystemToLittleEndian(
									    (unsigned char*)buffer, numberOfPixels );
	    }
	  else if ( m_ByteOrder == BigEndian )
	    {
	      ByteSwapper<unsigned char>::SwapRangeFromSystemToBigEndian(
									 (unsigned char *)buffer, numberOfPixels );
	    }
	  break;
	}
      case SHORT:
	{
	  if ( m_ByteOrder == LittleEndian )
	    {
	      ByteSwapper<short>::SwapRangeFromSystemToLittleEndian(
								    (short*)buffer, numberOfPixels );
	    }
	  else if ( m_ByteOrder == BigEndian )
	    {
	      ByteSwapper<short>::SwapRangeFromSystemToBigEndian(
								 (short *)buffer, numberOfPixels );
	    }
	  break;
	}
      case USHORT:
	{
	  if ( m_ByteOrder == LittleEndian )
	    {
	      ByteSwapper<unsigned short>::SwapRangeFromSystemToLittleEndian(
									     (unsigned short*)buffer, numberOfPixels );
	    }
	  else if ( m_ByteOrder == BigEndian )
	    {
	      ByteSwapper<unsigned short>::SwapRangeFromSystemToBigEndian(
									  (unsigned short *)buffer, numberOfPixels );
	    }
	  break; 
	}
      case INT:
	{
	  if ( m_ByteOrder == LittleEndian )
	    {
	      ByteSwapper<int>::SwapRangeFromSystemToLittleEndian(
								  (int*)buffer, numberOfPixels );
	    }
	  else if ( m_ByteOrder == BigEndian )
	    {
	      ByteSwapper<int>::SwapRangeFromSystemToBigEndian(
							       (int*)buffer, numberOfPixels );
	    }
	  break;
	}
      case UINT:
	{
	  if ( m_ByteOrder == LittleEndian )
	    {
	      ByteSwapper<unsigned int>::SwapRangeFromSystemToLittleEndian(
									   (unsigned int*)buffer, numberOfPixels );
	    }
	  else if ( m_ByteOrder == BigEndian )
	    {
	      ByteSwapper<unsigned int>::SwapRangeFromSystemToBigEndian(
									(unsigned int *)buffer, numberOfPixels );
	    }
	  break; 
	}
	
      case FLOAT:
	{
	  if ( m_ByteOrder == LittleEndian )
	    {
	      ByteSwapper<float>::SwapRangeFromSystemToLittleEndian(
								    (float*)buffer, numberOfPixels );
	    }
	  else if ( m_ByteOrder == BigEndian )
	    {
	      ByteSwapper<float>::SwapRangeFromSystemToBigEndian(
								 (float *)buffer, numberOfPixels );
	    }
	  break; 
	}
      case DOUBLE:
	{
	  if ( m_ByteOrder == LittleEndian )
	    {
	      ByteSwapper<double>::SwapRangeFromSystemToLittleEndian(
								     (double*)buffer, numberOfPixels );
	    }
	  else if ( m_ByteOrder == BigEndian )
	    {
	      ByteSwapper<double>::SwapRangeFromSystemToBigEndian(
								  (double *)buffer, numberOfPixels );
	    }
	  break; 
	}
      default:
	ExceptionObject exception(__FILE__, __LINE__);
	exception.SetDescription("Pixel Type Unknown");
	throw exception;
      }
  }
  
}

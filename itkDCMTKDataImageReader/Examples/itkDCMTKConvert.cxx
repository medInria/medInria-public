/* itkDcmtkReaderTest.cxx --- 
 * 
 * Author: Pierre Fillard
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Fri Oct  2 21:46:08 2009 (+0200)
 * Version: $Id$
 * Last-Updated: Fri Oct  2 21:47:00 2009 (+0200)
 *           By: Julien Wintz
 *     Update #: 3
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#include <iostream>

#include <itkDCMTKImageIO.h>
#include <itkImageFileReader.h>
#include <itkImageFileWriter.h>



int main (int narg, char* arg[])
{

    if( narg<3 )
    {
        std::cerr << "Usage: " << arg[0] << " <dicom1 dicom2 ... dicomN> <output>\n";
        return -1;
    }
  
    std::vector<std::string> fileNames;
    for( int i=1; i<narg-1; i++){
        std::cout << "Inserting: " << arg[i] << std::endl;
        fileNames.push_back ( arg[i] );
    }

    const char* output = arg[narg-1];
    
  
    itk::DCMTKImageIO::Pointer io = itk::DCMTKImageIO::New();
    io->SetFileNames ( fileNames );

    try
    {
      io->ReadImageInformation();
    }
    catch (itk::ExceptionObject &e)
    {
      std::cerr << e;
      return -1;
    }


    if (io->GetNumberOfDimensions()==3 )
    {
      
      switch (io->GetComponentType())
      {
	  case itk::ImageIOBase::CHAR:
	    {
	      typedef itk::Image<char, 3> ImageType;
	      itk::ImageFileReader<ImageType>::Pointer reader = itk::ImageFileReader<ImageType>::New();
	      reader->SetImageIO ( io );
	      reader->SetFileName ( arg[1] );
	      try
	      {
		reader->Update();
	      }
	      catch (itk::ExceptionObject &e)
	      {
		std::cerr <<e;
		return -1;
	      }
	      ImageType::Pointer image = reader->GetOutput();
	      itk::ImageFileWriter<ImageType>::Pointer writer = itk::ImageFileWriter<ImageType>::New();
	      writer->SetFileName ( output );
	      writer->SetInput ( image );
	      try
	      {
		writer->Update();
	      }
	      catch (itk::ExceptionObject &e)
	      {
		std::cerr << e;
		return -1;
	      }
	      break;
	    }
	    
	  case itk::ImageIOBase::UCHAR:
	    {
	      typedef itk::Image<unsigned char, 3> ImageType;
	      itk::ImageFileReader<ImageType>::Pointer reader = itk::ImageFileReader<ImageType>::New();
	      reader->SetImageIO ( io );
	      reader->SetFileName ( arg[1] );
	      try
	      {
		reader->Update();
	      }
	      catch (itk::ExceptionObject &e)
	      {
		std::cerr <<e;
		return -1;
	      }
	      ImageType::Pointer image = reader->GetOutput();
	      itk::ImageFileWriter<ImageType>::Pointer writer = itk::ImageFileWriter<ImageType>::New();
	      writer->SetFileName ( output );
	      writer->SetInput ( image );
	      try
	      {
		writer->Update();
	      }
	      catch (itk::ExceptionObject &e)
	      {
		std::cerr << e;
		return -1;
	      }
	      break;
	    }
	    
	  case itk::ImageIOBase::SHORT:
	    {
	      typedef itk::Image<short, 3> ImageType;
	      itk::ImageFileReader<ImageType>::Pointer reader = itk::ImageFileReader<ImageType>::New();
	      reader->SetImageIO ( io );
	      reader->SetFileName ( arg[1] );
	      try
	      {
		reader->Update();
	      }
	      catch (itk::ExceptionObject &e)
	      {
		std::cerr <<e;
		return -1;
	      }
	      ImageType::Pointer image = reader->GetOutput();
	      itk::ImageFileWriter<ImageType>::Pointer writer = itk::ImageFileWriter<ImageType>::New();
	      writer->SetFileName ( output );
	      writer->SetInput ( image );
	      try
	      {
		writer->Update();
	      }
	      catch (itk::ExceptionObject &e)
	      {
		std::cerr << e;
		return -1;
	      }
	      break;
	    }
	    
	  case itk::ImageIOBase::USHORT:
	    {
	      typedef itk::Image<unsigned short, 3> ImageType;
	      itk::ImageFileReader<ImageType>::Pointer reader = itk::ImageFileReader<ImageType>::New();
	      reader->SetImageIO ( io );
	      reader->SetFileName ( arg[1] );
	      try
	      {
		reader->Update();
	      }
	      catch (itk::ExceptionObject &e)
	      {
		std::cerr <<e;
		return -1;
	      }
	      ImageType::Pointer image = reader->GetOutput();
	      itk::ImageFileWriter<ImageType>::Pointer writer = itk::ImageFileWriter<ImageType>::New();
	      writer->SetFileName ( output );
	      writer->SetInput ( image );
	      try
	      {
		writer->Update();
	      }
	      catch (itk::ExceptionObject &e)
	      {
		std::cerr << e;
		return -1;
	      }
	      break;
	    }
	    
	    
	  default:
	  {
	    std::cout << "Unsupported type: " << io->GetComponentTypeAsString ( io->GetComponentType()) << std::endl;
	    return -1;
	  }
	  
      }
    }
    else if (io->GetNumberOfDimensions()==4)
    {

      switch (io->GetComponentType())
      {
	  case itk::ImageIOBase::CHAR:
	    {
	      typedef itk::Image<char, 4> ImageType;
	      itk::ImageFileReader<ImageType>::Pointer reader = itk::ImageFileReader<ImageType>::New();
	      reader->SetImageIO ( io );
	      reader->SetFileName ( arg[1] );
	      try
	      {
		reader->Update();
	      }
	      catch (itk::ExceptionObject &e)
	      {
		std::cerr <<e;
		return -1;
	      }
	      ImageType::Pointer image = reader->GetOutput();
	      itk::ImageFileWriter<ImageType>::Pointer writer = itk::ImageFileWriter<ImageType>::New();
	      writer->SetFileName ( output );
	      writer->SetInput ( image );
	      try
	      {
		writer->Update();
	      }
	      catch (itk::ExceptionObject &e)
	      {
		std::cerr << e;
		return -1;
	      }
	      break;
	    }
	    
	  case itk::ImageIOBase::UCHAR:
	    {
	      typedef itk::Image<unsigned char, 4> ImageType;
	      itk::ImageFileReader<ImageType>::Pointer reader = itk::ImageFileReader<ImageType>::New();
	      reader->SetImageIO ( io );
	      reader->SetFileName ( arg[1] );
	      try
	      {
		reader->Update();
	      }
	      catch (itk::ExceptionObject &e)
	      {
		std::cerr <<e;
		return -1;
	      }
	      ImageType::Pointer image = reader->GetOutput();
	      itk::ImageFileWriter<ImageType>::Pointer writer = itk::ImageFileWriter<ImageType>::New();
	      writer->SetFileName ( output );
	      writer->SetInput ( image );
	      try
	      {
		writer->Update();
	      }
	      catch (itk::ExceptionObject &e)
	      {
		std::cerr << e;
		return -1;
	      }
	      break;
	    }
	    
	  case itk::ImageIOBase::SHORT:
	    {
	      typedef itk::Image<short, 4> ImageType;
	      itk::ImageFileReader<ImageType>::Pointer reader = itk::ImageFileReader<ImageType>::New();
	      reader->SetImageIO ( io );
	      reader->SetFileName ( arg[1] );
	      try
	      {
		reader->Update();
	      }
	      catch (itk::ExceptionObject &e)
	      {
		std::cerr <<e;
		return -1;
	      }
	      ImageType::Pointer image = reader->GetOutput();
	      itk::ImageFileWriter<ImageType>::Pointer writer = itk::ImageFileWriter<ImageType>::New();
	      writer->SetFileName ( output );
	      writer->SetInput ( image );
	      try
	      {
		writer->Update();
	      }
	      catch (itk::ExceptionObject &e)
	      {
		std::cerr << e;
		return -1;
	      }
	      break;
	    }
	    
	  case itk::ImageIOBase::USHORT:
	    {
	      typedef itk::Image<unsigned short, 4> ImageType;
	      itk::ImageFileReader<ImageType>::Pointer reader = itk::ImageFileReader<ImageType>::New();
	      reader->SetImageIO ( io );
	      reader->SetFileName ( arg[1] );
	      try
	      {
		reader->Update();
	      }
	      catch (itk::ExceptionObject &e)
	      {
		std::cerr <<e;
		return -1;
	      }
	      ImageType::Pointer image = reader->GetOutput();
	      itk::ImageFileWriter<ImageType>::Pointer writer = itk::ImageFileWriter<ImageType>::New();
	      writer->SetFileName ( output );
	      writer->SetInput ( image );
	      try
	      {
		writer->Update();
	      }
	      catch (itk::ExceptionObject &e)
	      {
		std::cerr << e;
		return -1;
	      }
	      break;
	    }
	    
	    
	  default:
	  {
	    std::cout << "Unsupported type: " << io->GetComponentTypeAsString ( io->GetComponentType()) << std::endl;
	    return -1;
	  }
	  
      }
      
    }
    
  
    return 0;  
}

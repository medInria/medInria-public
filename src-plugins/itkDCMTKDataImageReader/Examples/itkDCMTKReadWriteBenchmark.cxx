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

#include <itkCommand.h>
#include <itkDCMTKImageIO.h>
#include <itkImageFileReader.h>
#include <itkImageFileWriter.h>

#include <time.h>




int main (int narg, char* arg[])
{

    if( narg<2 )
    {
        std::cerr << "Usage: " << arg[0] << " dicom1 dicom2 ...\n";
        return -1;
    }
  
    std::vector<std::string> fileNames;
    for( int i=1; i<narg; i++) {
        fileNames.push_back (arg[i]);
    }
  
  
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

    int repetitions = 5;

    std::string exts[6]={".dcm",".mhd",".nii",".nii.gz",".nhdr",".nrrd"};
    std::vector<std::string> extensions;
    for (int i=0; i<6; i++)
      extensions.push_back ( exts[i] );
    
    std::string s_output = "output";


    switch (io->GetComponentType())
    {
	case itk::ImageIOBase::CHAR:
	  {
	    typedef itk::Image<char, 3> ImageType;
	    typedef itk::ImageFileReader<ImageType> ReaderType;
	    ReaderType::Pointer reader = ReaderType::New();
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

	    typedef itk::ImageFileWriter<ImageType> WriterType;
	    for (unsigned int i=0; i<extensions.size(); i++)
	    {
	      double totalW = 0.0;
	      double totalR = 0.0;
	      for( int j=0; j<repetitions; j++)
	      {
		std::string s_outfilename = s_output + extensions[i];
		WriterType::Pointer writer = WriterType::New();
		writer->SetFileName ( s_outfilename.c_str() );
		writer->SetInput ( image );
		
		clock_t t1 = clock();
		try
		{
		  writer->Update();
		}
		catch (itk::ExceptionObject &e)
		{
		  std::cerr <<e;
		}
		clock_t t2 = clock();
		totalW += (double)(t2-t1);
		//std::cout << "For extension: " << extensions[i] << ", writing time is:" << (double)(t2-t1)/(double)(CLOCKS_PER_SEC) << std::endl;
		/*
		  }		  
		  for (unsigned int i=0; i<extensions.size(); i++)
		  {
		  std::string s_outfilename = s_output + extensions[i];
		*/
		ReaderType::Pointer reader = ReaderType::New();
		reader->SetFileName ( s_outfilename.c_str() );
		
		/*clock_t*/ t1 = clock();
		try
		{
		  reader->Update();
		}
		catch (itk::ExceptionObject &e)
		{
		  std::cerr <<e;
		}
		/*clock_t*/ t2 = clock();
		totalR += (double)(t2-t1);
		//std::cout << "For extension: " << extensions[i] << ", reading time is:" << (double)(t2-t1)/(double)(CLOCKS_PER_SEC) << std::endl;
	      }

	      std::cout << "Average Write/Read for extension " << extensions[i] << " is: " << totalW/((double)(repetitions*CLOCKS_PER_SEC))
			<< " / " << totalR/((double)(repetitions*CLOCKS_PER_SEC)) << std::endl;
	      
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
	    //image->DisconnectPipeline();
	    std::cout << image << std::endl;
	    break;
	  }

	case itk::ImageIOBase::SHORT:
	  {
	    typedef itk::Image<short, 3> ImageType;
	    typedef itk::ImageFileReader<ImageType> ReaderType;
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
	    //image->DisconnectPipeline();

	    typedef itk::ImageFileWriter<ImageType> WriterType;
	    for (unsigned int i=0; i<extensions.size(); i++)
	    {
	      double totalW = 0.0;
	      double totalR = 0.0;
	      std::string s_outfilename = s_output + extensions[i];

	      for( int j=0; j<repetitions; j++)
	      {
		WriterType::Pointer writer = WriterType::New();
		writer->SetFileName ( s_outfilename.c_str() );
		writer->SetInput ( image );
		
		clock_t t1 = clock();
		try
		{
		  writer->Update();
		}
		catch (itk::ExceptionObject &e)
		{
		  std::cerr <<e;
		}
		clock_t t2 = clock();
		totalW += (double)(t2-t1);
		//std::cout << "For extension: " << extensions[i] << ", writing time is:" << (double)(t2-t1)/(double)(CLOCKS_PER_SEC) << std::endl;
		/*}
		  for (unsigned int i=0; i<extensions.size(); i++)
		  {
		  std::string s_outfilename = s_output + extensions[i];
		*/
		ReaderType::Pointer reader = ReaderType::New();
		reader->SetFileName ( s_outfilename.c_str() );
		
		/*clock_t*/ t1 = clock();
		try
		{
		  reader->Update();
		}
		catch (itk::ExceptionObject &e)
		{
		  std::cerr <<e;
		}
		/*clock_t*/ t2 = clock();
		totalR += (double)(t2-t1);
		//std::cout << "For extension: " << extensions[i] << ", reading time is:" << (double)(t2-t1)/(double)(CLOCKS_PER_SEC) << std::endl;
	      }
	      std::cout << "Average Write/Read for extension " << extensions[i] << " is: " << totalW/((double)(repetitions*CLOCKS_PER_SEC))
			<< " / " << totalR/((double)(repetitions*CLOCKS_PER_SEC)) << std::endl;
	    }
	    break;
	  }

	  case itk::ImageIOBase::USHORT:
	  {
	    typedef itk::Image<unsigned short, 3> ImageType;
	    typedef itk::ImageFileReader<ImageType> ReaderType;
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
	    //image->DisconnectPipeline();

	    typedef itk::ImageFileWriter<ImageType> WriterType;
	    for (unsigned int i=0; i<extensions.size(); i++)
	    {
	      double totalW = 0.0;
	      double totalR = 0.0;
	      for( int j=0; j<repetitions; j++)
	      {
		std::string s_outfilename = s_output + extensions[i];
		WriterType::Pointer writer = WriterType::New();
		writer->SetFileName ( s_outfilename.c_str() );
		writer->SetInput ( image );
		
		clock_t t1 = clock();
		try
		{
		  writer->Update();
		}
		catch (itk::ExceptionObject &e)
		{
		  std::cerr <<e;
		}
		clock_t t2 = clock();
		totalW += (double)(t2-t1);
		//std::cout << "For extension: " << extensions[i] << ", writing time is:" << (double)(t2-t1)/(double)(CLOCKS_PER_SEC) << std::endl;
		/*}
		  for (unsigned int i=0; i<extensions.size(); i++)
		  {	   
		  std::string s_outfilename = s_output + extensions[i];
		*/
		ReaderType::Pointer reader = ReaderType::New();
		reader->SetFileName ( s_outfilename.c_str() );
		
		/*clock_t*/ t1 = clock();
		try
		{
		  reader->Update();
		}
		catch (itk::ExceptionObject &e)
		{
		  std::cerr <<e;
		}
		/*clock_t*/ t2 = clock();
		totalR += (double)(t2-t1);
		//std::cout << "For extension: " << extensions[i] << ", reading time is:" << (double)(t2-t1)/(double)(CLOCKS_PER_SEC) << std::endl;
	      }
	      std::cout << "Average Write/Read for extension " << extensions[i] << " is: " << totalW/((double)(repetitions*CLOCKS_PER_SEC))
			<< " / " << totalR/((double)(repetitions*CLOCKS_PER_SEC)) << std::endl;
	    }
	    break;
	  }
	  
	  
	default:
	  {
	    std::cout << "Unsupported type: " << io->GetComponentTypeAsString ( io->GetComponentType()) << std::endl;
	    return -1;
	  }
	  
	  
    }
    

    std::cout << "Threads: " << io->GetNumberOfThreads() /* << " Elapsed time:" << (double)(t2-t1)/(double)(CLOCKS_PER_SEC)*/ << std::endl;
  
    return 0;  
}

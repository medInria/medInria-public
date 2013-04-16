/*=========================================================================

 MedInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <iostream>

#include <itkDCMTKImageIO.h>
#include <itkImageFileReader.h>
#include <itkImageFileWriter.h>

#include <QtCore>
#include <QDir>
#include <QString>

int main (int narg, char* arg[])
{

    if( narg<3 )
    {
        std::cerr << "Usage: " << arg[0] << " <dicom1 dicom2 ... dicomN> <output>\n";
        return -1;
    }

    std::vector<std::string> fileNames;

    if (narg==3) { // special case of directory
        QFileInfo finfo(arg[1]);
        if (finfo.isDir())
        {
            QDir dir (arg[1]);
            dir.setFilter(QDir::Files | QDir::NoSymLinks | QDir::NoDotAndDotDot);
            foreach (QString file, dir.entryList()) {
                if (!file.isEmpty()) {
                QString fullname = finfo.filePath() + "/" + file;
                qDebug() << "inserting: " << fullname;
                fileNames.push_back(fullname.toAscii().constData());
                }
            }
        }
        else
            fileNames.push_back(arg[1]);
    }
    else {
    for( int i=1; i<narg-1; i++){
        std::cout << "Inserting: " << arg[i] << std::endl;
        fileNames.push_back ( arg[i] );
    }
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
          reader->SetFileName ( fileNames[0].c_str() );
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
	      reader->SetFileName ( fileNames[0].c_str() );
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
	      reader->SetFileName ( fileNames[0].c_str() );
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
	      reader->SetFileName ( fileNames[0].c_str() );
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
	      reader->SetFileName ( fileNames[0].c_str() );
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
	      reader->SetFileName ( fileNames[0].c_str() );
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
	      reader->SetFileName ( fileNames[0].c_str() );
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
	      reader->SetFileName ( fileNames[0].c_str() );
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

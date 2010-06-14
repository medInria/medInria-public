/*=========================================================================

Program:   vtkINRIA3D
Module:    $Id: itkGDCMExporter.cxx 522 2007-12-05 15:06:51Z ntoussaint $
Language:  C++
Author:    $Author: ntoussaint $
Date:      $Date: 2007-12-05 16:06:51 +0100 (Wed, 05 Dec 2007) $
Version:   $Revision: 522 $

Copyright (c) 2007 INRIA - Asclepios Project. All rights reserved.
See Copyright.txt for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "itkGDCMExporter.h"

#include <itkGDCMImageIOFactory.h>
#include <itkGDCMImageIO.h>

#include <itkImageFileWriter.h>
#include <itkCastImageFilter.h>


#include <sstream>


namespace itk
{
  
  GDCMExporter::GDCMExporter()
  {
    itk::GDCMImageIOFactory::RegisterOneFactory();    
    this->OutputDirectory = ".";
    this->Prefix = "IMG";
    this->ReverseOrder = false;
    
  }


  
  GDCMExporter::~GDCMExporter()
  {
    
  }

  void GDCMExporter::AddVolume (ImageType::Pointer image)
  {
    this->InputVolumeList.push_back(image);
  }
  

  


  void GDCMExporter::WriteAll(void)
  {

    unsigned int currentindex = 1;
    
    for (unsigned int volumeindex=0; volumeindex<this->InputVolumeList.size(); volumeindex++)
    {

    

      typedef itk::CastImageFilter< ImageType, ShortImageType> CastFilterType;
      CastFilterType::Pointer myCast = CastFilterType::New();
      myCast->SetInput (this->InputVolumeList[volumeindex]);
      myCast->Update();
      ShortImageType::Pointer volume = myCast->GetOutput();
      	
      typedef itk::ImageFileReader< ShortImageType > ReaderType;
      typedef itk::ExtractImageFilter< ShortImageType, ShortImageSliceType > ShortExtractType;
      typedef itk::ImageFileWriter< ShortImageSliceType > WriterType;

      unsigned int numberOfSlices = volume->GetLargestPossibleRegion().GetSize()[2];

      ImageIOType::Pointer gdcmIO = ImageIOType::New();
      DictionaryType dictionary;
  

      dictionary = this->InputVolumeList[volumeindex]->GetMetaDataDictionary();

      for (unsigned int i = 0; i < numberOfSlices; i++)
      {
    
    
	itksys_ios::ostringstream value;
	ShortImageType::PointType origin;
	ShortImageType::IndexType index;
	index.Fill(0);
	index[2] = i;
	volume->TransformIndexToPhysicalPoint(index, origin);

	// Set all required DICOM fields
	value.str("");
	value << origin[0] << "\\" << origin[1] << "\\" << origin[2];
	itk::EncapsulateMetaData<std::string>(dictionary,"0020|0032", value.str());

	value.str("");
	value << i + 1;
	itk::EncapsulateMetaData<std::string>(dictionary,"0020|0013", value.str());

	ShortImageType::RegionType extractRegion;
	ShortImageType::SizeType extractSize;
	ShortImageType::IndexType extractIndex;
	extractSize = volume->GetLargestPossibleRegion().GetSize();
	extractIndex.Fill(0);
	if (this->ReverseOrder)
        {
	  extractIndex[2] = numberOfSlices - i - 1;
        }
	else
        {
	  extractIndex[2] = i;
        }
	extractSize[2] = 0;
	extractRegion.SetSize(extractSize);
	extractRegion.SetIndex(extractIndex);

	ShortExtractType::Pointer extract = ShortExtractType::New();
	extract->SetInput(volume);
	extract->SetExtractionRegion(extractRegion);
	extract->GetOutput()->SetMetaDataDictionary(dictionary);

	WriterType::Pointer writer = WriterType::New();
	value.str("");
	value << this->OutputDirectory.c_str() << "/" << this->Prefix <<"_" << currentindex << ".dcm";
	currentindex++;
      
	writer->SetFileName(value.str().c_str());
	writer->SetInput(extract->GetOutput());
	try
        {
	  writer->SetImageIO(gdcmIO);
	  writer->Update();
        }
	catch( itk::ExceptionObject & excp )
        {
	  std::cerr << "Exception thrown while writing the file " << std::endl;
	  std::cerr << excp << std::endl;
        }
      }



      
    }
    

  

  
  }

}




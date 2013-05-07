/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include "itkGDCMImporter.h"

#include <itkGDCMImageIOFactory.h>
#include <itkGDCMImageIO.h>

#include <itkImageFileWriter.h>
#include <itkImageIOBase.h>
#include "itkDicomTagManager.h"
#include "gdcmFileHelper.h"

#include <itksys/SystemTools.hxx>
#include <sstream>


namespace itk
{
  
  GDCMImporter::GDCMImporter()
  {
    itk::GDCMImageIOFactory::RegisterOneFactory();
    
    this->DICOMScanner = itk::GDCMSeriesFileNames::New();
    this->DICOMScanner->SetUseSeriesDetails(true);
    
    this->InputDirectory = "";
    this->m_Recursive = true;

//     this->Dictionary = new DicomDictionary;
  }


  
  GDCMImporter::~GDCMImporter()
  {
//     delete this->Dictionary;
    
    // this->Reset(); // no need to reset when object is deleted
    
  }

  
  uint16_t GDCMImporter::GetDICOMTagGroup(unsigned long tag)
  {
    switch(tag)
    {
	case D_MODALITY :
	  return 0x0008;
	case D_SERIESUID :
	  return 0x0020;
	case D_STUDYDESCRIPTION :
	  return 0x0008;
	case D_SERIESDESCRIPTION :
	  return 0x0008;
	case D_ACQUISITIONTIME :
	  return 0x0008;
	default :
	  return 0x0020;
    }
  }

  
  uint16_t GDCMImporter::GetDICOMTagElement(unsigned long tag)
  {
    switch(tag)
    {
	case D_MODALITY :
	  return 0x0060;
	case D_SERIESUID :
	  return 0x000e;
	case D_STUDYDESCRIPTION :
	  return 0x1030;
	case D_SERIESDESCRIPTION :
	  return 0x103e;
	case D_ACQUISITIONTIME :
	  return 0x0032;
	default :
	  return 0x0020;
    }
  }


  itk::DICOMVolume::Pointer GDCMImporter::GetDICOMVolume(const char* name) const
  {
    for (unsigned int i=0; i<this->DICOMVolumeList.size(); i++)
    {
      itk::DICOMVolume::Pointer dcmvolume = this->GetDICOMVolume(i);
      
      if (strcmp(dcmvolume->GetDescription().c_str(), name) == 0)
	return dcmvolume;
    }
    return NULL;
    
  }
  

  bool GDCMImporter::CanReadFile (const char* filename)
  {
    ReaderType::Pointer reader = ReaderType::New();
    reader->SetFileName(filename);
    try
    {
      reader->Update();
      return true;
    }
    catch (itk::ExceptionObject & e)
    {
      std::cerr << e;
      return false;
    }
  }


  void GDCMImporter::Reset()
  {
    this->DICOMVolumeList.clear();
    this->OutputVolumeList.clear();
  }



  void GDCMImporter::ReadFile (const char* filename)
  {

    ReaderType::Pointer reader = ReaderType::New();
    reader->SetFileName(filename);
    try
    {
      reader->Update();
    }
    catch (itk::ExceptionObject & e)
    {
      std::cerr << e << std::endl;
      throw itk::ExceptionObject (__FILE__,__LINE__,"Error in GDCMImporter::ReadFile()");
    }

    std::string description = itksys::SystemTools::GetFilenameName(filename);
    
    std::vector<std::string> s_filenames;
    std::string s_filename = filename;
    s_filenames.push_back (s_filename);

    gdcm::FileList* gdcmfilelist = new gdcm::FileList;
    gdcm::File* gdcmfile = new gdcm::File;
    gdcmfilelist->push_back (gdcmfile);    
//     gdcmfile->Load (s_filename);    

    try
    {
      
      this->AddVolume (description, s_filenames, gdcmfilelist, reader->GetOutput() );
    }
    catch(itk::ExceptionObject & e)
    {
      std::cerr<<e<<std::endl;
      throw itk::ExceptionObject (__FILE__,__LINE__,"Error in GDCMImporter::ReadFile()");
    }


  }
  

  
  void GDCMImporter::Scan()
  {
    this->UpdateProgress(0.05);
    
    if (!this->InputDirectory.size())
    {
      itkWarningMacro(<<"Input directory not set !"<<std::endl<<"No scan performed."<<std::endl);
      return;
    }
    std::cout<<"reseting..."<<std::flush<<std::endl;

    this->Reset();
    
    std::cout<<"setting progress..."<<std::flush<<std::endl;
    this->UpdateProgress(0.1);
    
    std::cout<<"setting recursive..."<<std::flush<<std::endl;
    this->DICOMScanner->SetRecursive( this->GetRecursive() );	
    std::cout<<"setting directory..."<<std::flush<<std::endl;
    this->DICOMScanner->SetUseSeriesDetails(true);
    this->DICOMScanner->SetDirectory( this->InputDirectory );
    std::cout<<"setting directory: done..."<<std::flush<<std::endl;
    std::vector<std::string> UIDs = this->DICOMScanner->GetSeriesUIDs();
    std::cout<<"uids set"<<std::flush<<std::endl;
    this->UpdateProgress(0.2);
    std::vector<std::string> totalfilenames = this->DICOMScanner->GetInputFileNames();
    this->UpdateProgress(0.3);

    //unsigned int total_size = totalfilenames.size();
    //unsigned int total_iterator = 0;

    this->UpdateProgress(0.0);

    for (unsigned int i=0; i<UIDs.size(); i++)
    {

      std::string uid = UIDs[i];
      std::cout<<"scanning :"<<UIDs[i].c_str()<<std::endl;

      
      std::vector<std::string> filenamelist  = this->DICOMScanner->GetFileNames(uid);
      std::vector<gdcm::File*>* gdcmfilelist = this->DICOMScanner->GetSeriesHelper()->GetSingleSerieUIDFileSet(uid);

        
      std::ostringstream os;
      
      if( gdcmfilelist->size()>0 )
      {
	std::string date = (*gdcmfilelist)[0]->GetEntryValue(0x0008,0x0021);
	if (date == gdcm::GDCM_UNFOUND || date=="")
        {
          date = "nodate";
        }

        std::string sequenceName = (*gdcmfilelist)[0]->GetEntryValue(0x0018, 0x0024);
        if( sequenceName == gdcm::GDCM_UNFOUND || sequenceName == "")
        {
          char number[256];
          sprintf (number, "%d", i);
          sequenceName = number;
        }
        
        
//         os <<"#"<<i<<"_"<< date << "_" << (*gdcmfilelist)[0]->GetEntryValue(0x0008,0x0060) << "_" << sequenceName ;
	os <<itksys::SystemTools::MakeCindentifier ((*gdcmfilelist)[0]->GetEntryValue(0x0008, 0x1030).c_str()).c_str()<<sequenceName;
      }
      
      bool Is2DFiles = false;
      for (unsigned int file_it=0; file_it<gdcmfilelist->size(); file_it++)
      {
	int x = (*gdcmfilelist)[file_it]->GetXSize();
	int y = (*gdcmfilelist)[file_it]->GetYSize();
	int z = (*gdcmfilelist)[file_it]->GetZSize();

	Is2DFiles = (x == 1 || y == 1 || z == 1);
	if (!Is2DFiles)
	  break;
      }
      if (!Is2DFiles && gdcmfilelist->size())
      {
	// cannot construct from 3D files
	itkWarningMacro(<<std::endl<<"file : "<<(*gdcmfilelist)[0]->GetFileName().c_str()
			<<std::endl<<"cannot import 3D (or 2D+t) DICOM files !"
			<<std::endl<<"volume "<<os.str()<<" will not be imported"<<std::endl);

	continue;
      }


      
      std::string name = os.str().c_str();
      
      //bool validname=false;
      
      std::string::iterator last = name.end();
#ifndef __WXMSW__
      last = std::remove (name.begin(), last, ' ');
      name.resize ( (size_t)(last-name.begin()));
#endif
      std::replace (name.begin(), last, '/', '_');
      std::replace (name.begin(), last, '\\', '_');
      std::replace (name.begin(), last, ':', '_');
      
      
      if (name.length() == 0)
      {
	name = "NotAValidName";
      }


      try
      {
	this->AddVolume(name.c_str(), filenamelist, gdcmfilelist);
      }
      catch(itk::ExceptionObject & e)
      {
	std::cerr<<e<<std::endl;
	throw itk::ExceptionObject (__FILE__,__LINE__,"Error in GDCMImporter::AddVolume()");
      }
      this->UpdateProgress((double)i/(double)UIDs.size());
      
    }    
    
  }
  

  void GDCMImporter::SplitVolumeByTag (DICOMVolume::Pointer inputvolume, unsigned long tag)
  {
    
    uint16_t group   = GDCMImporter::GetDICOMTagGroup(tag);
    uint16_t element = GDCMImporter::GetDICOMTagElement(tag);

    gdcm::XCoherentFileSetmap filesetmap = this->DICOMScanner->GetSeriesHelper()->SplitOnTagValue(inputvolume->GetgdcmFileList(), group, element);
    gdcm::XCoherentFileSetmap::iterator it;

    std::vector<itk::DICOMVolume::Pointer> listToInsert;
    
    for (it = filesetmap.begin(); it != filesetmap.end(); it++)
    {
      std::string splittag = (*it).first;
      if (splittag == gdcm::GDCM_UNFOUND)
	splittag = "notag";
      
      gdcm::FileList* gdcmfilelist = (*it).second;

      if (!gdcmfilelist->size())
      {
        continue; // What is it supposed to do here?
	          // Sometime when the input volume doesn't have to be splitted,
	          // we end up with a empty secondary file list (size = 0),
	          // if so then we don't want insert an empty-file-list volume, do we ?
	          // this is what it is supposed to do
      }
      
      itk::DICOMVolume::Pointer dicomvolume = itk::DICOMVolume::New();
      std::string description = inputvolume->GetDescription()+"_"+splittag;
      dicomvolume->SetDescription (description);
      dicomvolume->SetgdcmFileList (gdcmfilelist);
      dicomvolume->SetSerieHelper(this->DICOMScanner->GetSeriesHelper());

      std::vector<std::string> filelist;
      gdcm::FileList::iterator it2;
      for(it2 = gdcmfilelist->begin(); it2 != gdcmfilelist->end(); it2++ )
      {
        filelist.push_back((*it2)->GetFileName());
      }
      	
      dicomvolume->SetFileList (filelist);
      dicomvolume->FillDicomTagManager();
      listToInsert.push_back (dicomvolume);
    }


    // now insert all elements of listToInsert into this->DicomVolume
    std::vector<itk::DICOMVolume::Pointer> testList (1);
    testList[0] = inputvolume;

    std::vector<itk::DICOMVolume::Pointer>::iterator toInsert = search (this->DICOMVolumeList.begin(), this->DICOMVolumeList.end(),
                                                                        testList.begin(), testList.end());
    if( toInsert == this->DICOMVolumeList.end() )
    {
      std::cerr << "Error: Cannot find dicom volume in input list." << std::endl;
      return;
    }
    
    this->DICOMVolumeList.insert (++toInsert, listToInsert.begin(), listToInsert.end());
    
    this->RemoveDICOMVolume(inputvolume);
  }



  

  void GDCMImporter::SplitVolumeByPositionConsistency (DICOMVolume::Pointer inputvolume)
  {
    gdcm::XCoherentFileSetmap filesetmap = this->SplitOnPosition(inputvolume->GetgdcmFileList());
    gdcm::XCoherentFileSetmap newfilesetmap;
    
    gdcm::XCoherentFileSetmap::iterator it = filesetmap.begin();
    while( it != filesetmap.end() )
    {
      gdcm::FileList* gdcmfilelist = (*it).second;
      
      if (!gdcmfilelist->size())
      {
        continue; // What is it supposed to do here?
	          // Same as above : 
	          // Sometime when the input volume doesn't have to be splitted,
	          // we end up with a empty secondary file list (size = 0),
	          // if so then we don't want insert an empty-file-list volume, do we ?
	          // this is what it is supposed to do
      }
      
      unsigned int fileid = 0, count = 0;
      
      gdcm::FileList::iterator it2 = gdcmfilelist->begin();
      unsigned int filelistsize = gdcmfilelist->size();
      
      while( it2 != gdcmfilelist->end() )
      {
        
	std::ostringstream strid;
	strid << inputvolume->GetDescription().c_str()<<"_item_";
	if (filelistsize > 10 && count < 10 )
	  strid<<"0";
	if (filelistsize > 100 && count < 100 )
	  strid<<"0";
	strid<<fileid;
        
	if (!newfilesetmap.count (strid.str()))
	{
	  newfilesetmap[strid.str()] = new gdcm::FileList;
	}
	
	newfilesetmap[strid.str()]->push_back( (*it2) );
	
	fileid++;

        ++it2;
	count++;
	
      }

      ++it;
    }


    
    std::vector<itk::DICOMVolume::Pointer> listToInsert;
    
    for (it = newfilesetmap.begin(); it != newfilesetmap.end(); it++)
    {
      gdcm::FileList* gdcmfilelist = (*it).second;
      
      if (!gdcmfilelist->size())
	continue;
      
      
      itk::DICOMVolume::Pointer dicomvolume = itk::DICOMVolume::New();
      std::string description = (*it).first;
      
      dicomvolume->SetDescription (description);
      dicomvolume->SetgdcmFileList (gdcmfilelist);
      dicomvolume->SetSerieHelper(this->DICOMScanner->GetSeriesHelper());

      gdcm::FileList::iterator it2;

      this->DICOMScanner->GetSeriesHelper()->OrderFileList( gdcmfilelist );
      dicomvolume->SetSequenceFlag (this->IsFileListSequence (gdcmfilelist));
//       bool valid = this->DICOMScanner->GetSeriesHelper()->ImagePositionPatientOrdering (gdcmfilelist);
//       if (!valid)
//       {
// 	valid = this->DICOMScanner->GetSeriesHelper()->ImageNumberOrdering (gdcmfilelist);

	
	
// 	if (valid)
// 	  dicomvolume->SetSequenceFlag (1);
//       }
//       if (!valid)
// 	valid = this->DICOMScanner->GetSeriesHelper()->FileNameOrdering (gdcmfilelist);
      
//       this->DICOMScanner->GetSeriesHelper()->OrderFileList( gdcmfilelist );

      std::vector<std::string> filelist;
      for(it2 = gdcmfilelist->begin(); it2 != gdcmfilelist->end(); it2++ )
      {
        filelist.push_back((*it2)->GetFileName());
      }
      
      dicomvolume->SetFileList (filelist);
      dicomvolume->FillDicomTagManager();

	  
      listToInsert.push_back (dicomvolume);
    }


    std::vector<itk::DICOMVolume::Pointer> testList (1);
    testList[0] = inputvolume;
    
    std::vector<itk::DICOMVolume::Pointer>::iterator toInsert = search (this->DICOMVolumeList.begin(), this->DICOMVolumeList.end(),
                                                                        testList.begin(), testList.end());

    if( toInsert == this->DICOMVolumeList.end() )
    {
      std::cerr << "Error: Cannot find dicom volume in input list." << std::endl;
      return;
    }
    
    this->DICOMVolumeList.insert (++toInsert, listToInsert.begin(), listToInsert.end());
    
    this->RemoveDICOMVolume(inputvolume);
  }




  void GDCMImporter::RemoveDICOMVolume (DICOMVolume::Pointer dcmvolume)
  {
    std::vector<itk::DICOMVolume::Pointer> testList (1);
    testList[0] = dcmvolume;
    
    std::vector<itk::DICOMVolume::Pointer>::iterator toRemove = search (this->DICOMVolumeList.begin(), this->DICOMVolumeList.end(),
                                                                        testList.begin(), testList.end());

    if( toRemove == this->DICOMVolumeList.end() )
    {
      std::cerr << "Error: Cannot find dicom volume to be removed from input list." << std::endl;
      return;
    }

    dcmvolume->ReleaseMemory();
    
    this->DICOMVolumeList.erase (toRemove);
  }


  

  void GDCMImporter::AddVolume (std::string description, std::vector<std::string> filelist, std::vector<gdcm::File*> *gdcmfilelist, ImageType::Pointer itkimage)
  {
    itk::DICOMVolume::Pointer dicomvolume = itk::DICOMVolume::New();
    std::ostringstream os;
    os <<description.c_str();

 //    bool valid = this->DICOMScanner->GetSeriesHelper()->ImagePositionPatientOrdering (gdcmfilelist);
//     if (!valid)
//     {
//       valid = this->DICOMScanner->GetSeriesHelper()->ImageNumberOrdering (gdcmfilelist);
//       if (valid)
// 	dicomvolume->SetSequenceFlag (1);
//     }
//     if (!valid)
//       valid = this->DICOMScanner->GetSeriesHelper()->FileNameOrdering (gdcmfilelist);

    this->DICOMScanner->GetSeriesHelper()->OrderFileList( gdcmfilelist );
    dicomvolume->SetSequenceFlag (this->IsFileListSequence (gdcmfilelist));
    
    dicomvolume->SetDescription (os.str());
    dicomvolume->SetFileList (filelist);
    dicomvolume->SetgdcmFileList (gdcmfilelist);
    dicomvolume->SetSerieHelper(this->DICOMScanner->GetSeriesHelper());
    dicomvolume->FillDicomTagManager();

    std::string found = (*dicomvolume->GetgdcmFileList())[0]->GetEntryValue (0x0018,0x0072);	
    if ((found != gdcm::GDCM_UNFOUND) && found.size())
    {
      dicomvolume->SetSequenceFlag (true);
    }


    
    if (!itkimage.IsNull())
      dicomvolume->SetImage(itkimage);
    
    try
    {
      this->DICOMVolumeList.push_back(dicomvolume);
    }
    catch (itk::ExceptionObject & e)
    {
      std::cerr << e;
      throw itk::ExceptionObject (__FILE__,__LINE__,"Error in GDCMImporter::AddVolume()");
    }    
  }

  bool GDCMImporter::IsFileListSequence (std::vector<gdcm::File*> *gdcmfilelist)
  {
    return false;
    
//     if (!gdcmfilelist->size())
//       return false;
//     float origin[3];
//     origin[0] = (*gdcmfilelist)[0]->GetXOrigin();
//     origin[1] = (*gdcmfilelist)[0]->GetYOrigin();
//     origin[2] = (*gdcmfilelist)[0]->GetZOrigin();

//     for (unsigned int i=0; i<gdcmfilelist->size(); i++)
//     {
//       double p[3], distance;
//       p[0] = (*gdcmfilelist)[i]->GetXOrigin();
//       p[1] = (*gdcmfilelist)[i]->GetYOrigin();
//       p[2] = (*gdcmfilelist)[i]->GetZOrigin();

//       distance = 0;
      
//       for (unsigned int j=0; j<3; j++)
// 	distance += (p[j] - origin[j]) * (p[j] - origin[j]);
//       distance = sqrt (distance);

//       if (distance > 0.0001)
// 	return false;
//     }

//     return true;
  }
  
  

  void GDCMImporter::BuildAllVolumes()
  {

    this->UpdateProgress(0.0);
    
    for (unsigned int i=0; i<this->DICOMVolumeList.size(); i++)
    {
      try
      {
      	this->DICOMVolumeList[i]->Build();
	this->UpdateProgress((double)i/(double)this->DICOMVolumeList.size());
      }
      catch (itk::ExceptionObject & e)
      {
	std::cerr << e <<std::endl;
        //throw itk::ExceptionObject (__FILE__,__LINE__,"Error in GDCMImporter::BuildAllVolumes()");
        // We don't throw exception here as we want to reconstruct as many volumes
        // as possible, i.e. even if one volume fails does not mean that all volumes
        // will fail.
      }
    }
  }
  
  
  DICOMVolume::ImageType::Pointer DICOMVolume::TemporaryBuild ()
  {

    if ( this->GetFileList().size() < 2)
    {
      return 0;
    }
    
    SeriesReaderType::Pointer dicomreader = SeriesReaderType::New();
    itk::GDCMImageIO::Pointer io = itk::GDCMImageIO::New();
    
    dicomreader->SetFileNames( this->GetFileList() );
    dicomreader->SetImageIO( io );
    
    try
    {
      dicomreader->Update();

      ImageType::Pointer image = dicomreader->GetOutput();

      // well try to get the dictionary :

      itk::MetaDataDictionary dictionary;
      itk::MetaDataDictionary::Iterator it;
      
      
      std::vector<itk::DicomTag> taglist = this->GetTagManager()->GetTagList();
      for (unsigned int i=0; i<taglist.size(); i++)
      {
	itk::EncapsulateMetaData<std::string>(dictionary,taglist[i].identifier,std::string(taglist[i].value));
      }      

      image->SetMetaDataDictionary(dictionary);
      
      if (this->IsSequence())
      {
	DICOMVolume::ImageType::SpacingType spacing;
	DICOMVolume::ImageType::SpacingType newspacing;
	spacing = image->GetSpacing();
	
	newspacing[0] = spacing[0];
	newspacing[1] = spacing[1];
	newspacing[2] = 0;
	image->SetSpacing (newspacing);
      }

//       ImageType::DirectionType matrix = image->GetDirection();
//       ImageType::DirectionType transposed = ImageType::DirectionType (matrix.GetTranspose());
//       typedef itk::Point<ImageType::DirectionType::ValueType, ImageType::ImageDimension> PointType;
//       PointType origin = image->GetOrigin();
//       PointType neworigin = transposed * origin;
//       image->SetOrigin (neworigin);
      /**
         The GDCM reader seems to flip in the A-P direction the image.
         We flip it back until this is resolved internally in ITK.
       */
      /*
      FlipFilterType::Pointer flipper = FlipFilterType::New();
      flipper->SetInput (dicomreader->GetOutput());
      FlipFilterType::FlipAxesArrayType axis;
      axis[0] = true;
      axis[1] = true;
      axis[2] = false;
      flipper->SetFlipAxes (axis);

      flipper->Update();

      image = flipper->GetOutput();
      */

      return image;
      
    }
    catch (itk::ExceptionObject & e)
    {
      std::cerr << e;
      throw itk::ExceptionObject (__FILE__,__LINE__,"Error in DICOMVolume::Build()");
    }
  }
  
  
  void DICOMVolume::Build()
  {

    /*
    if ( this->GetFileList().size() < 2)
    {
      return;
    }

//     if (this->gdcmFileList->size())
//     {
//       std::cout<<this->Description.c_str()<<", position : "
// 	       <<(*this->gdcmFileList)[0]->GetEntryValue(0x0018,0x5100)<<std::endl
// 	       <<(*this->gdcmFileList)[0]->GetEntryValue(0x0020,0x0020)<<std::endl
// 	       <<(*this->gdcmFileList)[0]->GetEntryValue(0x0020,0x0035)<<std::endl
// 	       <<(*this->gdcmFileList)[0]->GetEntryValue(0x0020,0x0037)<<std::endl
// 	       <<(*this->gdcmFileList)[0]->GetEntryValue(0x0054,0x0410)<<std::endl
//  	       <<(*this->gdcmFileList)[0]->GetEntryValue(0x0018,0x9302)<<std::endl
//  	       <<(*this->gdcmFileList)[0]->GetEntryValue(0x0018,0x1310)<<std::endl
//  	       <<(*this->gdcmFileList)[0]->GetEntryValue(0x0054,0x0410)<<std::endl
//  	       <<(*this->gdcmFileList)[0]->GetEntryValue(0x0018,0x1314)<<std::endl
//  	       <<(*this->gdcmFileList)[0]->GetEntryValue(0x0070,0x0041)<<std::endl<<std::endl;
//     }
    
    SeriesReaderType::Pointer dicomreader = SeriesReaderType::New();
    itk::GDCMImageIO::Pointer io = itk::GDCMImageIO::New();
    
    dicomreader->SetFileNames( this->GetFileList() );
    dicomreader->SetImageIO( io );
    
    try
    {
      dicomreader->Update();

      ImageType::Pointer image = dicomreader->GetOutput();

      
      //  The GDCM reader seems to flip in the A-P direction the image.
      //  We flip it back until this is resolved internally in ITK.       
      FlipFilterType::Pointer flipper = FlipFilterType::New();
      flipper->SetInput (dicomreader->GetOutput());
      FlipFilterType::FlipAxesArrayType axis;
      axis[0] = true;
      axis[1] = true;
      axis[2] = false;
      flipper->SetFlipAxes (axis);

      flipper->Update();

      image = flipper->GetOutput();
      
      this->SetImage (image);
    }
    catch (itk::ExceptionObject & e)
    {
      std::cerr << e;
      throw itk::ExceptionObject (__FILE__,__LINE__,"Error in DICOMVolume::Build()");
    }
    */

    if (!this->GetImage().IsNull())
    {
      //itkWarningMacro(<<"image is already built"<<endl);
      return;
    }
    
    try
    {
      this->SetImage ( this->TemporaryBuild() );
    }
    catch (itk::ExceptionObject & e)
    {
      std::cerr << e;
      throw itk::ExceptionObject (__FILE__,__LINE__,"Error in DICOMVolume::Build()");
    }
    
    
  }



  void DICOMVolume::Save (const char* filename)
  {

    if ( this->GetFileList().size() < 2)
    {
      return;
    }

    ImageType::Pointer image = this->GetImage();

    if( image.IsNull() )
    {
      
      SeriesReaderType::Pointer dicomreader = SeriesReaderType::New();
      itk::GDCMImageIO::Pointer io = itk::GDCMImageIO::New();
      
      dicomreader->SetFileNames( this->GetFileList() );
      dicomreader->SetImageIO( io );


      try
      {
        dicomreader->Update();
        image = dicomreader->GetOutput();
      }
      catch (itk::ExceptionObject & e)
      {
        std::cerr << e;
        throw itk::ExceptionObject (__FILE__,__LINE__,"Error in GDCMImporter::Save()");
      }

      if (this->IsSequence())
      {
	DICOMVolume::ImageType::SpacingType spacing;
	DICOMVolume::ImageType::SpacingType newspacing;
	spacing = image->GetSpacing();
	
	newspacing[0] = spacing[0];
	newspacing[1] = spacing[1];
	newspacing[2] = 0;
	image->SetSpacing (newspacing);
      }
      
//       ImageType::DirectionType matrix = image->GetDirection();
//       ImageType::DirectionType transposed = ImageType::DirectionType (matrix.GetTranspose());
//       typedef itk::Point<ImageType::DirectionType::ValueType, ImageType::ImageDimension> PointType;
//       PointType origin = image->GetOrigin();
//       PointType neworigin = transposed * origin;
//       image->SetOrigin (neworigin);
      
    }

    
//     itk::Matrix<double,3,3> cosines;
//     cosines[0][0]= 1;
//     cosines[0][1]= 0;
//     cosines[0][2]= 0;
//     cosines[1][0]= 0;
//     cosines[1][1]=-1;
//     cosines[1][2]= 0;
//     cosines[2][0]= 0;
//     cosines[2][1]= 0;
//     cosines[2][2]= 1;
    
//     image->SetDirection(cosines);
    
    
    itk::ImageFileWriter<ImageType>::Pointer myWriter = itk::ImageFileWriter<ImageType>::New();
    myWriter->SetInput( image );
    myWriter->SetFileName( filename );

    try
    {
      myWriter->Update();
    }
    catch (itk::ExceptionObject &e)
    {
      std::cerr << e;
      throw itk::ExceptionObject (__FILE__,__LINE__,"Error while writing the image.");
    }
    
  }
    

  

  void GDCMImporter::Save(int n, const char* filename)
  {
    // save a volume without keeping it in memory
    if( n<0 || n>=(int)this->DICOMVolumeList.size() )
    {
      throw itk::ExceptionObject(__FILE__,__LINE__, "Error in GDCMImporter::Save(): Index is out of range.");
    }


    try
    {
      this->DICOMVolumeList[n]->Save (filename);
    }
    catch (itk::ExceptionObject & e)
    {
      std::cerr << e;
      throw itk::ExceptionObject (__FILE__,__LINE__,"Error in GDCMImporter::Save()");
    }
  
  }
  
  
  void GDCMImporter::SaveAll(const char* directory)
  {
    for( unsigned int i=0; i<this->DICOMVolumeList.size(); i++)
    {
      try
      {
        std::string filename = directory;
        filename += "/" + this->DICOMVolumeList[i]->GetDescription() + ".mha";
        this->Save(i, filename.c_str());
      }
      catch(itk::ExceptionObject &e)
      {
        std::cerr << e;
      }
      this->UpdateProgress( double(i)/double(this->DICOMVolumeList.size()) );
    }
  }

  void GDCMImporter::ReleaseMemory ()
  {
    for( unsigned int i=0; i<this->DICOMVolumeList.size(); i++)
    {
      this->DICOMVolumeList[i]->ReleaseMemory();
    }
  }
  



  gdcm::XCoherentFileSetmap GDCMImporter::SplitOnPosition(gdcm::FileList *fileSet)
  {
    gdcm::XCoherentFileSetmap CoherentFileSet;
    
    size_t nb = fileSet->size();
    if (nb == 0 )
      return CoherentFileSet;
    float pos[3];
    std::string strImPos;  // read on disc
    itksys_ios::ostringstream ossPosition;
    std::string strPosition; // re computed
    gdcm::FileList::const_iterator it = fileSet->begin();
    for ( ; it != fileSet->end(); it++)
    {     
      // Information is in :      
      // 0020,0032 : Image Position Patient
      // 0020,0030 : Image Position (RET)
      
      strImPos = (*it)->GetEntryValue(0x0020,0x0032);
      if ( strImPos == gdcm::GDCM_UNFOUND)
      {
        std::cerr << "Unfound Image Position Patient (0020,0032)\n";
        strImPos = (*it)->GetEntryValue(0x0020,0x0030); // For ACR-NEMA images
        if ( strImPos == gdcm::GDCM_UNFOUND )
        {
          std::cerr << "Unfound Image Position (RET) (0020,0030)\n";
          // User wants to split on the 'Position'
          // No 'Position' info found.
          // We return an empty Htable !
          return CoherentFileSet;
        }  
      }
      
      if ( sscanf( strImPos.c_str(), "%f \\%f \\%f ", 
                   &pos[0], &pos[1], &pos[2]) != 3 )
      {
		  std::cerr << "Wrong number for Position : ["
                       << strImPos << "]\n";
             return CoherentFileSet;
      }

      // Let's build again the 'position' string, to be sure of it's format      

      ossPosition << pos[0];      
      for (int i = 1; i < 3; i++)
      {
        ossPosition << "\\";
        ossPosition << pos[i]; 
      }      
      strPosition = ossPosition.str();
      ossPosition.str("");

      if ( CoherentFileSet.count(strPosition) == 0 )
      {
        //std::cerr << " New Position :[" << strPosition << "]\n";
        // create a File set in 'position' position
        CoherentFileSet[strPosition] = new gdcm::FileList;
      }
      // Current Position and DICOM header match; add the file:
      CoherentFileSet[strPosition]->push_back( (*it) );
    }   
    return CoherentFileSet;
  }
  
  unsigned int* DICOMVolume::GetDimensions()
  {
    
    unsigned int* ret = new unsigned int[3];
    ret[0] = 0;
    ret[1] = 0;
    ret[2] = 0;
    
    
    if (this->gdcmFileList->size())
    {
      ret[0] = (*this->gdcmFileList)[0]->GetXSize();
      ret[1] = (*this->gdcmFileList)[0]->GetYSize();
    }
    ret[2] = this->gdcmFileList->size();
    
    return ret;
  }

  void DICOMVolume::ReleaseMemory()
  {
    this->SetImage (0);
  }
  


    
  void DICOMVolume::FillDicomTagManager()
  {

    if (!this->GetgdcmFileList()->size())
      return;

    this->GetTagManager()->ClearTags();
    
    for (unsigned int i=0; i<itk::DicomTagManager::GetDictionarySize(); i++)
    {
      itk::DicomTag dictionarytag = itk::DicomTagManager::GetDictionaryTag (i); 
      std::string found = (*this->GetgdcmFileList())[0]->GetEntryValue (dictionarytag.group,dictionarytag.element);	
      if (found == gdcm::GDCM_UNFOUND || !found.size())
	continue;
      
      itk::DicomTag tag = dictionarytag;
      tag.value = found.c_str();
      
      this->GetTagManager()->AddTag (tag);
    }
  }
  


//   std::string GDCMImporter::GetVolumeTagValue (DICOMVolume::Pointer dcmvolume, DicomDictionary::DicomTag dicomtag)
//   {
//     std::string output = "";
//     if (!dcmvolume->GetgdcmFileList()->size())
//       return output;

    
//     std::string found = (*dcmvolume->GetgdcmFileList())[0]->GetEntryValue (dicomtag.group,dicomtag.element);

//     if (found == gdcm::GDCM_UNFOUND || !found.size())
//       return output;

//     output = found;
//     return output;
	
//   }
  
  
  
}



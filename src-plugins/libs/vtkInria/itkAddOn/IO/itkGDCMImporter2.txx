/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include "itkGDCMImporter2.h"

namespace itk
{


//----------------------------------------------------------------------------
  template <class TPixelType, unsigned int NDimensions>
  void DICOMImage<TPixelType, NDimensions>::Build()
  {
    if (this->m_IsBuilt)
      return;

    if (!this->m_FileList->size())
      throw itk::ExceptionObject (__FILE__,__LINE__,"Error in DICOMImage::Build(): File list empty");

    typename SeriesReaderType::Pointer dicomreader = SeriesReaderType::New();
    typename itk::GDCMImageIO::Pointer io = itk::GDCMImageIO::New();
//     io->LoadPrivateTagsOn();
//     io->LoadSequencesOn();
    std::vector<std::string> filelist;
    for (unsigned int i=0; i<this->GetFileList()->size(); i++)
      filelist.push_back ((*this->GetFileList())[i]->GetFileName());
    dicomreader->SetFileNames( filelist );
    dicomreader->SetImageIO( io );

    try
    {
      dicomreader->Update();
    }
    catch (itk::ExceptionObject & e)
    {
      this->m_IsBuilt = 0;
      std::cerr << e;
      throw itk::ExceptionObject (__FILE__,__LINE__,"Error in DICOMImage::Build()");
    }

    this->Graft (dicomreader->GetOutput());

    if (NDimensions == 3)
    {
      bool ispositionhomogeneous = true;
      for (unsigned int i=1; i<this->GetFileList()->size(); i++)
      {
	DICOMFile* f1 = (*this->GetFileList())[i-1];
	DICOMFile* f2 = (*this->GetFileList())[i];
	if ( (f1->GetXOrigin() != f2->GetXOrigin()) ||
	     (f1->GetYOrigin() != f2->GetYOrigin()) ||
	     (f1->GetZOrigin() != f2->GetZOrigin()))
	{
	  ispositionhomogeneous = false;
	  break;
	}
      }

      if (ispositionhomogeneous)
      {
	SpacingType spacing = this->GetSpacing();
	spacing[2] = itk::NumericTraits<float>::min();
	this->SetSpacing (spacing);
      }
    }

    // This line actually copy the dicom information from the reader to the image
    // so that we can retrieve it after the import process.
    this->SetMetaDataDictionary (io->GetMetaDataDictionary());

    this->m_IsBuilt = 1;

  }

//----------------------------------------------------------------------------
  template <class TPixelType, unsigned int NDimensions>
  void DICOMImage<TPixelType, NDimensions>::PrintSelf(std::ostream& os, Indent indent) const
  {
    Superclass::PrintSelf( os, indent );
    unsigned long NumberOfFiles = 0;
    if (this->GetFileList())
      NumberOfFiles = this->GetFileList()->size();
    os << indent << "Number of Files: " << NumberOfFiles << std::endl;
    os << indent << "Name: " << this->GetName() << std::endl;
    os << indent << "Is image built: " << m_IsBuilt << std::endl;
    os << indent << "Dictionary : " << std::endl;

    std::string value;
    itk::MetaDataDictionary dict = this->GetMetaDataDictionary();
    //Smarter approach using real iterators
    itk::MetaDataDictionary::ConstIterator itr = dict.Begin();
    itk::MetaDataDictionary::ConstIterator end = dict.End();

    while(itr != end)
    {
      const std::string &key = itr->first;
      ExposeMetaData<std::string>(dict, key, value);
      os << indent<< indent << key.c_str() << " : " << value.c_str()<< std::endl;
      ++itr;
    }
  }




//----------------------------------------------------------------------------
  template <typename TImage>
  GDCMImporter2<TImage>::GDCMImporter2()
  {
    itk::GDCMImageIOFactory::RegisterOneFactory();
    m_GDCMScanner = itk::GDCMSeriesFileNames::New();
    m_GDCMScanner->SetUseSeriesDetails(true);
    m_GDCMScanner->LoadPrivateTagsOff();
    m_GDCMHelper = m_GDCMScanner->GetSeriesHelper();

    m_DicomDir = new DicomDirType();

    this->SetInputDirectory ("");
    this->SetRecursiveScan (true);
    this->SetUseDicomDirFile (true);
    this->SetPriorityRule (TIME_DOMINANT);

    this->SetNumberOfRequiredInputs (0);
    this->SetNumberOfRequiredOutputs (0);
    this->SetNumberOfOutputs (0);
  }


//----------------------------------------------------------------------------
  template <typename TImage>
  bool GDCMImporter2<TImage>::CanReadFile (const char* filename)
  {
    typename ReaderType::Pointer reader = ReaderType::New();
    reader->SetFileName(filename);
    try
    {
      reader->GenerateOutputInformation();
      return true;
    }
    catch (itk::ExceptionObject & e)
    {
      std::cerr << e;
      return false;
    }
  }


//----------------------------------------------------------------------------
  template <typename TImage>
  void GDCMImporter2<TImage>::ReadFile (const char* filename)
  {

    typename ReaderType::Pointer reader = ReaderType::New();
    typename itk::GDCMImageIO::Pointer io = itk::GDCMImageIO::New();
//     io->LoadPrivateTagsOn();
//     io->LoadSequencesOn();
    reader->SetFileName(filename);
    reader->SetImageIO (io);

    try
    {
      reader->Update();
    }
    catch (itk::ExceptionObject & e)
    {
      std::cerr << e << std::endl;
      throw itk::ExceptionObject (__FILE__,__LINE__,"Error in GDCMImporter2::ReadFile()");
    }
    DICOMFileList* filelist = new DICOMFileList;
    DICOMFile* file = new DICOMFile;
    file->SetFileName (filename);
    file->Load();
    filelist->push_back (file);
    std::string name = this->GenerateName (filelist);
    typename DICOMImageType::Pointer image = DICOMImageType::New();
    image->SetName (name.c_str());
    image->SetFileList (filelist);
    image->Graft (reader->GetOutput());
    image->SetMetaDataDictionary (io->GetMetaDataDictionary());
    this->AddOutput (image);
  }




//----------------------------------------------------------------------------
  template <typename TImage>
  void
  GDCMImporter2<TImage>::SaveOutputInFile (unsigned int N, const char* filename)
  {

    try
    {
      typename DICOMImageType::Pointer image = static_cast<DICOMImageType*>(this->GetOutput (N));
      if (image.IsNull())
	throw itk::ExceptionObject (__FILE__,__LINE__,"Error in GDCMImporter2::SaveOutputInFile(), cannot convert image");
      image->Build();

      typename WriterType::Pointer writer = WriterType::New();
      writer->SetFileName (filename);
      writer->SetInput (image);

      writer->Update();
    }
    catch (itk::ExceptionObject & e)
    {
      std::cerr << e << std::endl;
      throw itk::ExceptionObject (__FILE__,__LINE__,"Error in GDCMImporter2::SaveOutputInFile()");
    }

  }



//----------------------------------------------------------------------------
  template <typename TImage>
  void
  GDCMImporter2<TImage>::SaveOutputsInDirectory (const char* directory)
  {
    for( unsigned int i=0; i<this->GetNumberOfOutputs(); i++)
    {
      typename DICOMImageType::Pointer image = static_cast<DICOMImageType*>(this->GetOutput (i));

      try
      {
        if (image.IsNull())
        throw itk::ExceptionObject (__FILE__,__LINE__,"Error in GDCMImporter2::SaveOutputsInDirectory(), cannot convert image");
        std::string name = itksys::SystemTools::MakeCindentifier (image->GetName());
        std::string filename = directory;
#ifdef WIN32
        filename += "\\";
#else  // WIN32
        filename += "/";
#endif // WIN32
        // Use this ITK extension so that the image keeps
        // its orientation information coming from the DICOM flags
        filename += name + ".mha";
        this->SaveOutputInFile(i, filename.c_str());
        this->UpdateProgress( double(i)/double(this->GetNumberOfOutputs()) );
      }
      catch(itk::ExceptionObject &e)
      {
        std::cerr <<"cannot save output "<<i<<std::endl;
        std::cerr << e <<std::endl;
      }
    }

  }


  template <typename TImage>
  void
  GDCMImporter2<TImage>::SaveOutputsInDirectory (const char* directory, const char* rootfilename)
  {
    char buffer[32];

    for ( unsigned int i = 0; i < this->GetNumberOfOutputs(); i++ )
    {
      typename DICOMImageType::Pointer image = static_cast<DICOMImageType*>( this->GetOutput(i) );

      try
      {
        if ( image.IsNull() )
          throw itk::ExceptionObject (__FILE__,__LINE__,"Error in GDCMImporter2::SaveOutputsInDirectory(), cannot convert image");

        std::string name = itksys::SystemTools::MakeCindentifier (image->GetName());
        std::string filename = directory;
#ifdef WIN32
        filename += "\\";
#else  // WIN32
        filename += "/";
#endif // WIN32

        sprintf(buffer, "%03d", i );

        // Use this ITK extension so that the image keeps
        // its orientation information coming from the DICOM flags
        filename = filename + rootfilename + "_" + buffer + ".mha";
        this->SaveOutputInFile(i, filename.c_str());
        this->UpdateProgress( double(i)/double(this->GetNumberOfOutputs()) );
      }
      catch(itk::ExceptionObject &e)
      {
        std::cerr <<"cannot save output "<<i<<std::endl;
        std::cerr << e <<std::endl;
      }
    }

  }


//----------------------------------------------------------------------------
  template <typename TImage>
  bool GDCMImporter2<TImage>::AttemptMerge (DICOMFileList* list1, DICOMFileList* list2)
  {
    if (!list1->size() || !list2->size())
      return false;

    bool dowemerge = false;
    typename DICOMFileList::iterator it1;
    typename DICOMFileList::iterator it2;
    for (it1 = list1->begin(); it1 != list1->end(); it1++)
    {
      for (it2 = list2->begin(); it2 != list2->end(); it2++)
      {
	std::string name1 = (*it1)->GetEntryValue(0x0008, 0x103E);
	std::string name2 = (*it2)->GetEntryValue(0x0008, 0x103E);
	if (name1 == name2)
	{
	  dowemerge = true;
	  break;
	}
	///\todo : test more reliable entries
      }
      if (dowemerge)
	break;
    }
    if (dowemerge)
      list1->insert (list1->end(), list2->begin(), list2->end());
    return dowemerge;
  }


//----------------------------------------------------------------------------
  template <typename TImage>
  void GDCMImporter2<TImage>::AutoOrganization()
  {
    // Retrieve the entire list of files
    DICOMFileListMap buffermap = this->GetGlobalDicomFileListMap();
    if (!buffermap.size())
      throw itk::ExceptionObject (__FILE__,__LINE__,"Error in GDCMImporter2::AutoOrganization(), nothing to organize");
    DICOMFileListMap::iterator mapit = buffermap.begin();
    unsigned int count = 0;
    this->UpdateProgress(0.00);

    // First step:
    // Attempt to merge the series that correspond to the same object
    // For instance in case of 4D cardiac exam in multi-shots style.

    while (mapit != buffermap.end())
    {
      DICOMFileListMap::iterator mergingit = mapit;;
      mergingit++;
      while (mergingit != buffermap.end())
      {
	if (this->AttemptMerge ((*mapit).second, (*mergingit).second))
	{
	  DICOMFileListMap::iterator mergingit2 = mergingit;
	  mergingit2++;
	  buffermap.erase (mergingit);
	  mergingit = mergingit2;
	}
	else
	  mergingit++;
      }
      count++;
      this->UpdateProgress ((double)(count)/(double)(buffermap.size()));
      mapit++;
    }

    DICOMFileListMap::iterator it;

    this->UpdateProgress(0.00);

    // Second step:
    // For each of the buffer map items, make it coherent (check inconsistencies)
    // This implies to separate inconsistent files into other map items
    // then add the final result into the outputs

    try
    {
      this->SetNumberOfOutputs (0);
      this->Initialize();

      for (it = buffermap.begin(); it != buffermap.end(); it++)
      {
	DICOMFileListMap coherentmap = this->MakeCoherent ((*it).second);
	DICOMFileListMap::iterator it2;
	for (it2 = coherentmap.begin(); it2 != coherentmap.end(); it2++)
	{
	  DICOMFileList* filelist = (*it2).second;
	  if ( !filelist->size() )
	    continue;

	  std::string name = this->GenerateName (filelist);	  
	  typename DICOMImageType::Pointer image = DICOMImageType::New();
	  image->SetName (name.c_str());
	  image->SetFileList (filelist);
	  this->AddOutput (image);
	  this->UpdateProgress ((double)(this->GetNumberOfOutputs())/(double)(coherentmap.size()));
	}
      }
    }
    catch (itk::ExceptionObject & e)
    {
      this->UpdateProgress(1.0);
      this->SetProgress(0.0);
      std::cerr << e << std::endl;
      throw itk::ExceptionObject (__FILE__,__LINE__,"Error in GDCMImporter2::AutoOrganization(), during output adding");
    }

    this->UpdateProgress(1.0);
    this->SetProgress(0.0);
  }



//----------------------------------------------------------------------------
  template <typename TImage>
  void GDCMImporter2<TImage>::AutoOrganizationPositionBased()
  {

    DICOMFileListMap buffermap = this->GetGlobalDicomFileListMap();
    if (!buffermap.size())
      throw itk::ExceptionObject (__FILE__,__LINE__,"Error in GDCMImporter2::AutoOrganizationPositionBased(), nothing to organize");
    DICOMFileListMap::iterator mapit = buffermap.begin();
    this->UpdateProgress(0.00);

    DICOMFileListMap::iterator it;

    try
    {
      this->SetNumberOfOutputs (0);
      this->Initialize();

      for (it = buffermap.begin(); it != buffermap.end(); it++)
      {
	DICOMFileListMap coherentmap = this->MakePositionCoherent ((*it).second);
	DICOMFileListMap::iterator it2;
	for (it2 = coherentmap.begin(); it2 != coherentmap.end(); it2++)
	{
	  DICOMFileList* filelist = (*it2).second;
	  if ( !filelist->size() )
	    continue;
	  this->m_GDCMHelper->OrderFileList (filelist);
	  std::string name = this->GenerateName (filelist);
	  typename DICOMImageType::Pointer image = DICOMImageType::New();
	  image->SetName (name.c_str());
	  image->SetFileList (filelist);
	  this->AddOutput (image);
	  this->UpdateProgress ((double)(this->GetNumberOfOutputs())/(double)(coherentmap.size()));
	}
      }
    }
    catch (itk::ExceptionObject & e)
    {
      this->UpdateProgress(1.0);
      this->SetProgress(0.0);
      std::cerr << e << std::endl;
      throw itk::ExceptionObject (__FILE__,__LINE__,"Error in GDCMImporter2::AutoOrganization(), during output adding");
    }

    this->UpdateProgress(1.0);
    this->SetProgress(0.0);
  }



//----------------------------------------------------------------------------
  template <typename TImage>
  void GDCMImporter2<TImage>::Scan()
  {
    if (!this->GetInputDirectory() || !(*this->GetInputDirectory()))
    {
      itkWarningMacro(<<"Input directory not set !"<<std::endl<<"No scan performed."<<std::endl);
      throw itk::ExceptionObject (__FILE__,__LINE__,"Error in GDCMImporter2::Scan()");
    }

    this->SetNumberOfOutputs(0);
    this->Initialize();

    std::string DicomDirFile = this->FindFile (this->GetInputDirectory(), "dicomdir" );
    if (this->GetUseDicomDirFile() && itksys::SystemTools::FileExists (DicomDirFile.c_str()) )
    {
      try
      {
	std::cout<<"loading the dicom dir file... ";
	this->LoadFromDicomDirFile (DicomDirFile.c_str());
	std::cout<<"done."<<std::endl;
      }
      catch (itk::ExceptionObject & e)
      {
	std::cerr << e << std::endl;
	throw itk::ExceptionObject (__FILE__,__LINE__,"Error in GDCMImporter2::Scan()");
      }
    }
    else
    {
      // We did not want to use (or did not find any) DICOMDIR file,
      // we will scan from scratch all dicom file headers
      try
      {
	std::cout<<"scanning the directory for DICOM files... ";
	this->LoadFromDirectory (this->m_InputDirectory.c_str());
	std::cout<<"done. "<<std::endl;
      }
      catch (itk::ExceptionObject & e)
      {
	std::cerr << e << std::endl;
	throw itk::ExceptionObject (__FILE__,__LINE__,"Error in GDCMImporter2::Scan()");
      }
    }

    try
    {
      this->InitializeOutputs();
    }
    catch (itk::ExceptionObject & e)
    {
      std::cerr << e << std::endl;
      throw itk::ExceptionObject (__FILE__,__LINE__,"Error in GDCMImporter2::Scan()");
    }

  }



//----------------------------------------------------------------------------
  template <typename TImage>
  void GDCMImporter2<TImage>::InitializeOutputs()
  {

    std::vector<std::string> UIDs;
    UIDs = m_GDCMScanner->GetSeriesUIDs();

    this->SetNumberOfOutputs (0);
    this->Initialize();

    try
    {

      this->UpdateProgress(0.0);

      for (unsigned int uid=0; uid<UIDs.size(); uid++)
      {
	std::string uidname = UIDs[uid];
	DICOMFileList* t_filelist = m_GDCMHelper->GetSingleSerieUIDFileSet(uidname);
	this->m_GDCMHelper->OrderFileList (t_filelist);
	DICOMFileListMap coherentmap = this->MakeCoherent (t_filelist);

	if( !coherentmap.size() )
	  continue;

	DICOMFileListMap::iterator it;

	for (it = coherentmap.begin(); it != coherentmap.end(); it++)
	{
	  DICOMFileList* filelist = (*it).second;
	  if ( !filelist->size() )
	    continue;
	  std::string name = this->GenerateName (filelist);
	  typename DICOMImageType::Pointer image = DICOMImageType::New();
	  image->SetName (name.c_str());
	  image->SetFileList (filelist);
	  this->AddOutput (image);
	  this->UpdateProgress((double)uid/(double)UIDs.size());
	}
      }
    }
    catch (itk::ExceptionObject & e)
    {
      this->UpdateProgress(1.0);
      this->SetProgress(0.0);
      std::cerr << e << std::endl;
      throw itk::ExceptionObject (__FILE__,__LINE__,"Error in GDCMImporter2::InitializeOutputs(), during tree structuring");
    }
    this->UpdateProgress(1.0);
    this->SetProgress(0.0);
  }





//----------------------------------------------------------------------------
  template <typename TImage>
  void GDCMImporter2<TImage>::GenerateData()
  {
    this->UpdateProgress(0.0);
    try
    {
      for (unsigned int i=0; i<this->GetNumberOfOutputs(); i++)
      {
	typename DICOMImageType::Pointer image = static_cast<DICOMImageType*>(this->GetOutput (i));
	if (image.IsNull())
	  continue;
	image->Build();
	this->UpdateProgress((double)i/(double)this->GetNumberOfOutputs());
      }
    }
    catch (itk::ExceptionObject & e)
    {
      this->UpdateProgress(1.0);
      this->SetProgress(0.0);
      std::cerr << e << std::endl;
      throw itk::ExceptionObject (__FILE__,__LINE__,"Error in GDCMImporter2::GenerateData()");
    }
  }

//----------------------------------------------------------------------------
  template <typename TImage>
  typename GDCMImporter2<TImage>::DICOMFileListMap
  GDCMImporter2<TImage>::GetGlobalDicomFileListMap()
  {
    DICOMFileListMap retmap;
    for (unsigned int i=0; i<this->GetNumberOfOutputs(); i++)
    {
      typename DICOMImageType::Pointer image = static_cast<DICOMImageType*>(this->GetOutput (i));
      if (image.IsNull())
	continue;

      retmap[image->GetName()] = image->GetFileList();
    }
    return retmap;
  }


//----------------------------------------------------------------------------
  template <typename TImage>
  typename GDCMImporter2<TImage>::DICOMFileListMap
  GDCMImporter2<TImage>::TransposeMap (DICOMFileListMap map)
  {
    DICOMFileListMap retmap;
    DICOMFileListMap::iterator it;
    DICOMFileListMap::iterator firstit = map.begin();

    for (it = map.begin(); it != map.end(); it++)
    {
      if ((*it).second->size() != (*firstit).second->size())
      {
	std::cerr<<"size mismatch !"<<std::endl;
	continue;
      }
      DICOMFileList* filelist = (*it).second;
      typename DICOMFileList::iterator fileit;
      unsigned count = 0;
      for (fileit = filelist->begin(); fileit != filelist->end(); fileit++)
      {
	std::ostringstream differentiator;
	if (filelist->size() > 10 && count < 10 )
	  differentiator<<"0";
	if (filelist->size() > 100 && count < 100 )
	  differentiator<<"0";
	differentiator<<count;

	if (!retmap.count (differentiator.str()))
	{
	  retmap[differentiator.str()] = new DICOMFileList;
	}
	retmap[differentiator.str()]->push_back( (*fileit) );
	count++;
      }
    }
    return retmap;
  }

//----------------------------------------------------------------------------
  template <typename TImage>
  typename GDCMImporter2<TImage>::DICOMFileListMap
  GDCMImporter2<TImage>::MakePositionCoherent (DICOMFileList* filelist)
  {
    DICOMFileListMap positionsplitmap;
    DICOMFileListMap t_map;
    t_map = this->m_GDCMHelper->SplitOnPosition (filelist);
    positionsplitmap = this->TransposeMap (t_map);

    return positionsplitmap;
  }

//----------------------------------------------------------------------------
  template <typename TImage>
  typename GDCMImporter2<TImage>::DICOMFileListMap
  GDCMImporter2<TImage>::MakeSizeCoherent (DICOMFileList* filelist)
  {
    DICOMFileListMap retmap;

    unsigned int nb = filelist->size();
    if (nb == 0 )
      return retmap;

    typename DICOMFileList::const_iterator it;
    for ( it = filelist->begin(); it != filelist->end(); ++it)
    {
      unsigned int x = (*it)->GetXSize();
      unsigned int y = (*it)->GetYSize();

      std::ostringstream differentiator;
      differentiator<<x<<y;

      if ( !retmap.count(differentiator.str()) )
	retmap[differentiator.str()] = new DICOMFileList;
      retmap[differentiator.str()]->push_back( (*it) );
    }
    return retmap;
  }

//----------------------------------------------------------------------------
  template <typename TImage>
  typename GDCMImporter2<TImage>::DICOMFileListMap
  GDCMImporter2<TImage>::MakeCoherent (DICOMFileList* filelist)
  {
    DICOMFileListMap orientationsplitmap = this->m_GDCMHelper->SplitOnOrientation (filelist);
    DICOMFileListMap::iterator orientationit;
    DICOMFileListMap retmap;

    for (orientationit = orientationsplitmap.begin(); orientationit != orientationsplitmap.end(); orientationit++)
    {
      DICOMFileListMap sizesplitmap = this->MakeSizeCoherent ((*orientationit).second);
      DICOMFileListMap::iterator sizeit;
      for (sizeit = sizesplitmap.begin(); sizeit != sizesplitmap.end(); sizeit++)
      {
	std::string differentiator = (*orientationit).first+"."+(*sizeit).first;
	retmap[differentiator] = (*sizeit).second;
      }
    }
    return retmap;
  }


//----------------------------------------------------------------------------
  template <typename TImage>
  std::string
  GDCMImporter2<TImage>::GenerateName (DICOMFileList* filelist)
  {
    std::string description = (*filelist)[0]->GetEntryValue(0x0008, 0x103E);
    if (description == gdcm::GDCM_UNFOUND)
    {
      (*filelist)[0]->Load();
      description = (*filelist)[0]->GetEntryValue(0x0008, 0x103E);
    }

    bool taken = 0; unsigned int n=1;
    std::string name = description.c_str();

    for (unsigned int i=0; i<this->GetNumberOfOutputs(); i++)
    {
      typename DICOMImageType::Pointer image = static_cast<DICOMImageType*>(this->GetOutput (i));
      if (image.IsNull())
	continue;
      if (! strcmp ( image->GetName(), name.c_str() ) )
      {
	taken = 1;
	break;
      }
    }

    while(taken)
    {
      std::ostringstream os;
      os <<itksys::SystemTools::MakeCindentifier (description.c_str()).c_str();
      if (n < 10 )
	  os<<"0";
      os<<n;

      taken = 0;
      for (unsigned int i=0; i<this->GetNumberOfOutputs(); i++)
      {

	typename DICOMImageType::Pointer image = static_cast<DICOMImageType*>(this->GetOutput (i));
	if (image.IsNull())
	  continue;
	if (! strcmp ( image->GetName(), os.str().c_str() ) )
	{
	  taken = 1;
	  break;
	}
      }
      n++;
      name = os.str();
    }

    return name; //return name.c_str();
  }

//----------------------------------------------------------------------------
  template <typename TImage>
  bool
  GDCMImporter2<TImage>::HasOutput (DICOMImagePointerType dicomimage)
  {
    // find the output in the list of outputs
    DataObjectPointerArray::iterator it = std::find(this->GetOutputs().begin(), this->GetOutputs().end(), dicomimage);
    return (it != this->GetOutputs().end());
  }

  //----------------------------------------------------------------------------
  template <typename TImage>
  void
  GDCMImporter2<TImage>::RemoveDICOMImage (DICOMImagePointerType dicomimage)
  {
    DataObjectPointerArray::iterator it = std::find(this->GetOutputs().begin(), this->GetOutputs().end(), dicomimage);
    this->GetOutputs().erase(it);
  }


//----------------------------------------------------------------------------
  template <typename TImage>
  void
  GDCMImporter2<TImage>::PrintSelf(std::ostream& os, Indent indent) const
  {
    Superclass::PrintSelf( os, indent );
    os << indent << "Input Directory: " << m_InputDirectory.c_str() << std::endl;
    os << indent << "RecursiveScan: " << m_RecursiveScan << std::endl;
    os << indent << "UseDicomDirFile: " << m_UseDicomDirFile << std::endl;
    os << indent << "Number of Volumes: " << this->GetNumberOfOutputs() << std::endl;
  }



//----------------------------------------------------------------------------
  template <typename TImage>
  void GDCMImporter2<TImage>::LoadFromDicomDirFile (const char* DicomDirFile)
  {
    if (!itksys::SystemTools::FileExists (DicomDirFile, true))
      throw itk::ExceptionObject (__FILE__,__LINE__,"Error in GDCMImporter2::LoadFromDicomDirFile(): file not found");

    this->m_DicomDir->SetFileName (DicomDirFile);
    this->m_DicomDir->Load();

    unsigned long TotalImages = this->EstimateSizeOfDicomDir (this->m_DicomDir);
    std::string directory = itksys::SystemTools::GetParentDirectory (DicomDirFile);
    std::string filename = "";
    std::string bufferfilename = "";
    unsigned long counter_im = 0;
    gdcm::DicomDirPatient* patient = NULL;
    gdcm::DicomDirStudy* study = NULL;
    gdcm::DicomDirSerie* serie = NULL;
    gdcm::DicomDirImage* image = NULL;
    patient = m_DicomDir->GetFirstPatient();
    if (patient)
      study = patient->GetFirstStudy();
    if (study)
      serie = study->GetFirstSerie();
    if (serie)
      image = serie->GetFirstImage();

    this->UpdateProgress(0.00);
    this->m_GDCMHelper->Clear();

    while (patient)
    {
      study = patient->GetFirstStudy();
      while (study)
      {
	serie = study->GetFirstSerie();
	while (serie)
	{
	  image = serie->GetFirstImage();
	  while (image)
	  {
	    filename = static_cast<gdcm::ValEntry *>(image->GetDocEntry (0x0004, 0x1500))->GetValue();
	    bufferfilename = this->FindFile (directory, filename);
	    if (itksys::SystemTools::FileExists (bufferfilename.c_str()))
	      m_GDCMHelper->AddFileName (bufferfilename);
	    image = serie->GetNextImage();
	    this->UpdateProgress((double)counter_im/(double)TotalImages);
	    counter_im++;
	  }
	  serie = study->GetNextSerie();
	}
	study = patient->GetNextStudy();
      }
      patient = m_DicomDir->GetNextPatient();
    }

    this->UpdateProgress(1.0);
    this->SetProgress(0.0);
  }



//----------------------------------------------------------------------------
  template <typename TImage>
  void GDCMImporter2<TImage>::LoadFromDirectory (const char* directoryname)
  {
    if (!itksys::SystemTools::FileExists (directoryname))
      throw itk::ExceptionObject (__FILE__,__LINE__,"Error in GDCMImporter2::LoadFromDicomDirFile(): directory not found");

    this->UpdateProgress(0.00);
    this->m_GDCMHelper->Clear();

    unsigned long counter_im = 0;
    gdcm::DirList dirlist (directoryname, this->m_RecursiveScan);
    gdcm::DirListType filenames_list = dirlist.GetFilenames();
    unsigned long TotalImages = filenames_list.size();

    for( gdcm::DirListType::const_iterator it = filenames_list.begin();
        it != filenames_list.end(); ++it)
    {
      if (itksys::SystemTools::FileExists ((*it).c_str(), true))
	m_GDCMHelper->AddFileName ((*it));
      this->UpdateProgress((double)counter_im/(double)TotalImages);
      counter_im++;
    }
    this->UpdateProgress(1.0);
    this->SetProgress(0.0);
  }




//----------------------------------------------------------------------------
  template <typename TImage>
  std::string GDCMImporter2<TImage>::FindFile (std::string directory, std::string filename)
  {

#ifndef WIN32
    itksys::SystemTools::ConvertToUnixSlashes (directory);
    itksys::SystemTools::ConvertToUnixSlashes (filename);
#endif

    std::string bufferfilename;
    std::string filename_s = filename;
    std::vector<std::string> joinpaths;
#ifdef WIN32
    directory+="\\";
#else  // WIN32
    directory+="/";
#endif // WIN32
    joinpaths.push_back (directory);
    joinpaths.push_back (filename);

    bufferfilename = itksys::SystemTools::JoinPath (joinpaths);
    if (itksys::SystemTools::FileExists(bufferfilename.c_str()))
      return bufferfilename;

    filename_s.resize (filename_s.size()-1);
    joinpaths.pop_back();
    joinpaths.push_back (filename_s);
    bufferfilename = itksys::SystemTools::JoinPath (joinpaths);
    if (itksys::SystemTools::FileExists(bufferfilename.c_str()))
      return bufferfilename;

    filename = itksys::SystemTools::LowerCase (filename.c_str());
    joinpaths.pop_back();
    joinpaths.push_back (filename);
    bufferfilename = itksys::SystemTools::JoinPath (joinpaths);
    if (itksys::SystemTools::FileExists(bufferfilename.c_str()))
      return bufferfilename;

    filename = itksys::SystemTools::UpperCase (filename.c_str());
    joinpaths.pop_back();
    joinpaths.push_back (filename);
    bufferfilename = itksys::SystemTools::JoinPath (joinpaths);
    if (itksys::SystemTools::FileExists(bufferfilename.c_str()))
      return bufferfilename;

    filename_s = itksys::SystemTools::LowerCase (filename_s.c_str());
    joinpaths.pop_back();
    joinpaths.push_back (filename_s);
    bufferfilename = itksys::SystemTools::JoinPath (joinpaths);
    if (itksys::SystemTools::FileExists(bufferfilename.c_str()))
      return bufferfilename;

    filename_s = itksys::SystemTools::UpperCase (filename_s.c_str());
    joinpaths.pop_back();
    joinpaths.push_back (filename_s);
    bufferfilename = itksys::SystemTools::JoinPath (joinpaths);
    if (itksys::SystemTools::FileExists(bufferfilename.c_str()))
      return bufferfilename;

    std::cerr<<filename.c_str()<<": file not found"<<std::endl;
    return bufferfilename;
  }


//----------------------------------------------------------------------------
  template <typename TImage>
  unsigned long GDCMImporter2<TImage>::EstimateSizeOfDicomDir (DicomDirType* dicomdir)
  {
    unsigned long counter_im = 0;
    gdcm::DicomDirPatient* patient = NULL;
    gdcm::DicomDirStudy* study = NULL;
    gdcm::DicomDirSerie* serie = NULL;
    gdcm::DicomDirImage* image = NULL;
    patient = dicomdir->GetFirstPatient();
    if (patient)
      study = patient->GetFirstStudy();
    if (study)
      serie = study->GetFirstSerie();
    if (serie)
      image = serie->GetFirstImage();
    while (patient)
    {
      study = patient->GetFirstStudy();
      while (study)
      {
	serie = study->GetFirstSerie();
	while (serie)
	{
	  image = serie->GetFirstImage();
	  while (image)
	  {
	    counter_im++;
	    image = serie->GetNextImage();
	  }
	  serie = study->GetNextSerie();
	}
	study = patient->GetNextStudy();
      }
      patient = dicomdir->GetNextPatient();
    }
    return counter_im;
  }


} // end of namespace ITK


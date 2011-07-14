/*=========================================================================

Program:   vtkINRIA3D
Module:    $Id: itkGDCMImporter3.cxx 522 2007-12-05 15:06:51Z ntoussaint $
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
#include "itkGDCMImporter3.h"
#include <itkGDCMImageIO.h>

#include "gdcmReader.h"
#include "gdcmImageReader.h"
#include "gdcmDirectionCosines.h"
#include "gdcmStringFilter.h"

#include <ctype.h>

namespace itk
{


  
//----------------------------------------------------------------------------
  template <class TPixelType>
  void GDCMVolume<TPixelType>::Write (std::string filename)
  {
    typename WriterType::Pointer writer = WriterType::New();
    writer->SetFileName (filename);
    writer->SetInput (this);
    
    try
    {
      writer->Write();
    }
    catch (itk::ExceptionObject & e)
    {
      std::cerr<<e<<std::endl;
      itkExceptionMacro (<<"Cannot write "<<filename<<std::endl);
    }
  }
  

//----------------------------------------------------------------------------
  template <class TPixelType>
  void GDCMVolume<TPixelType>::WriteGradients (std::string filename)
  {
    // Recovering the acquistion plane orientation
    // A vector in the world coordinate system would have to
    // be rotated with the inverse orientation matrix to get its
    // its equivalent in the image coordinate system
    typename SubImageType::DirectionType directions;
    for (unsigned int i=0; i<ImageDimension - 1; i++)
      for (unsigned int j=0; j<ImageDimension - 1; j++)
	directions[i][j] = this->GetDirection()[i][j];
    typename SubImageType::DirectionType inversedirections = typename SubImageType::DirectionType (directions.GetInverse());

    std::ofstream file (filename.c_str());
    if(file.fail())
      itkExceptionMacro (<<"Unable to open file for writing: " << filename.c_str() << std::endl);

    file << m_Gradients.size() << std::endl;
    for (unsigned int i=0; i<m_Gradients.size(); i++)
    {
      GradientType g = inversedirections * m_Gradients [i];      
      if (NumericTraits<double>::IsPositive (g.GetNorm()))
	g.Normalize();
      file << g[0] << "\t" << g[1] << "\t" << g[2] << std::endl;
    }
    file.close();
  }
  


//----------------------------------------------------------------------------
  template <class TPixelType>
  bool GDCMVolume<TPixelType>::IsVolumeDWIs (void)
  {
    FileListMapType map = this->GetFileListMap();
    bool ret = 0;
    double cumulatednorm = 0.0;
    
    m_MeanDiffusivitySkipped = 0;
    m_Gradients.clear();

    bool engaged = 0;

    if (map.size())
    {
      typename FileListMapType::iterator it;
      typename FileListMapType::iterator last = map.end();
      last--;
      
      for (it = map.begin(); it != map.end(); ++it)
      {
	if (!(*it).second.size())
	{
	  ret = 0;
	  break;
	}
	m_GradientScanner.Scan ((*it).second);
	
	std::string file = (*it).second[0];
	const char *value = this->m_GradientScanner.GetValue(file.c_str(), gdcm::Tag(0x18,0x9089));
	if( !value )
        {
	  ret = 0;
	  break;
	}

	gdcm::Element<gdcm::VR::DS,gdcm::VM::VM3> gradient;
	std::stringstream ss;
	ss.str( value );
	gradient.Read(ss );
	ret = 1;	
	GradientType gr;
	for (unsigned int i=0; i<3; i++)
	  gr[i] = gradient[i];
	
	if (!engaged && NumericTraits<double>::IsPositive (gr.GetNorm()))
	  engaged = 1;
	
	cumulatednorm += gr.GetNorm();
	
	if (!m_SkipMeanDiffusivity || !engaged || NumericTraits<double>::IsPositive (gr.GetNorm()) )
	  m_Gradients.push_back (gr);
	else
	  m_MeanDiffusivitySkipped = 1;
      }
    }
    
    return ret && NumericTraits<double>::IsPositive (cumulatednorm);
  }
  

//----------------------------------------------------------------------------
  template <class TPixelType>
  void GDCMVolume<TPixelType>::Build(void)
  {

    FileListMapType map = this->GetFileListMap();

    typedef typename ImageType::RegionType RegionType;
    typedef typename ImageType::SpacingType SpacingType;
    typedef typename ImageType::PointType PointType;
    typedef typename ImageType::DirectionType DirectionType;
    typedef ImageRegionIterator<ImageType> IteratorType;
    typedef typename IteratorType::IndexType IndexType;    
    typename ImageType::Pointer image = ImageType::New();
    typename GDCMImageIO::Pointer io = GDCMImageIO::New();
    typename FileListMapType::iterator it;
    
    bool metadatacopied = 0;
    IteratorType itOut;

    if (this->IsVolumeDWIs() && this->m_SkipMeanDiffusivity && this->m_MeanDiffusivitySkipped)
    {
      while (map.size() > m_Gradients.size())
      {
	typename FileListMapType::iterator last = map.end();
	last--;
	map.erase (last);
      }
    }
      
    std::cout<<"building volume "<<this->GetName()<<" containing "<<map.size()<<" subvolumes..."<<std::flush;

    for (it = map.begin(); it != map.end(); ++it)
    {
      typename SeriesReaderType::Pointer seriesreader = SeriesReaderType::New();
      seriesreader->UseStreamingOn();
      
      typename ReaderType::Pointer singlereader = ReaderType::New();
      typename SubImageType::Pointer t_image = 0;
      
      if ((*it).second.size() > 1)
      {
	seriesreader->SetFileNames( (*it).second );
	seriesreader->SetImageIO( io );
	try
	{
	  seriesreader->Update();
	}
	catch (itk::ExceptionObject & e)
	{
	  std::cerr << e;
	  itkExceptionMacro ( <<"Cannot read file list beggining with "<<(*it).second[0]<<std::endl
			      <<", volume will not be built"<<std::endl);
	}
      
	t_image= seriesreader->GetOutput();
      }
      else
      {
	singlereader->SetFileName( (*it).second[0] );
	singlereader->SetImageIO( io );
	try
	{
	  singlereader->Update();
	}
	catch (itk::ExceptionObject & e)
	{
	  std::cerr << e;
	  itkExceptionMacro ( <<"Cannot read single file "<<(*it).second[0]<<std::endl
			      <<", volume will not be built"<<std::endl);
	}
      
	t_image= singlereader->GetOutput();
      }

      if (!metadatacopied)
      {
	RegionType region;
	region.SetSize (0, t_image->GetLargestPossibleRegion().GetSize()[0]);
	region.SetSize (1, t_image->GetLargestPossibleRegion().GetSize()[1]);
	region.SetSize (2, t_image->GetLargestPossibleRegion().GetSize()[2]);
	region.SetSize (3, map.size());
	image->SetRegions (region);
	image->Allocate();
	SpacingType spacing;
	spacing[0] = t_image->GetSpacing()[0];
	spacing[1] = t_image->GetSpacing()[1];
	spacing[2] = t_image->GetSpacing()[2];

	if (map.size() > 1)
	{
	  std::string firstfile = (*map.begin()).second[0];
	  gdcm::Scanner scanner; scanner.AddTag (gdcm::Tag(0x18,0x1088)); scanner.AddTag (gdcm::Tag(0x18,0x1060));
	  scanner.Scan (this->MapToFileList (map));
	  const char* value = scanner.GetValue (firstfile.c_str(), gdcm::Tag(0x0018,0x1088));
	  unsigned int heartrate = 60;
	  if (value && std::atoi (value) >= 1)
	  {
	    heartrate = std::atoi (value);
	  }
	  else
	  {
	    const gdcm::Scanner::ValuesType& values = scanner.GetValues(gdcm::Tag(0x18,0x1060));
	    if (values.size())
	    {
	      std::cout<<"size is : "<<values.size()<<"... "<<std::endl;
	      
	      gdcm::Scanner::ValuesType::iterator it;
	      std::vector<float> triggers;
	      for (it = values.begin(); it != values.end(); ++it)
	      {
	      	std::cout<<"found trigger : "<<(*it).c_str()<<std::endl;
	      	triggers.push_back (std::atof ((*it).c_str()));
	      }
	      std::sort (triggers.begin(), triggers.end());
	      for (unsigned int i=0; i<triggers.size(); i++)
		std::cout<<"tr: "<<triggers[i]<<std::endl;
	      
	      float meanspacing = (triggers[triggers.size() - 1] - triggers[0]) / (float)(values.size() - 1);
	      heartrate = std::floor (60.0 * 1000.0 / (triggers[triggers.size() - 1] + meanspacing));
	      std::cout<<"calculated heart-rate = "<<heartrate<<std::endl;
	    }
	    else
	      heartrate = 60;
	  }

	  if (heartrate <= 1)
	    heartrate = 60;
	  spacing[3] = 60.0 / ( (double)(heartrate) * (double)(map.size()) );
	}
	else
	  spacing[3] = 1;
	image->SetSpacing (spacing);
	PointType origin;
	origin[0] = t_image->GetOrigin()[0];
	origin[1] = t_image->GetOrigin()[1];
	origin[2] = t_image->GetOrigin()[2];
	origin[3] = 0;
	image->SetOrigin (origin);
	DirectionType direction;
	for (unsigned int i=0; i<4; i++)
	  for (unsigned int j=0; j<4; j++)
	  {
	    if ((i < 3) && (j < 3))
	      direction[i][j] = t_image->GetDirection()[i][j];
	    else
	      direction[i][j] = (i == j) ? 1 : 0;
	  }

	image->SetDirection(direction);
	itOut = IteratorType (image, region);

	this->SetMetaDataDictionary (io->GetMetaDataDictionary());
	
	metadatacopied = 1;
      }
      
      ImageRegionIterator<SubImageType> itIn(t_image, t_image->GetLargestPossibleRegion());
      while (!itIn.IsAtEnd())
      {
	itOut.Set(itIn.Get());
	++itIn;
	++itOut;
      }
    }
    std::cout<<"done"<<std::endl;

    this->Graft (image);
    this->SetMetaDataDictionary (io->GetMetaDataDictionary());
  }


//----------------------------------------------------------------------------
  template <class TPixelType>
  typename GDCMVolume<TPixelType>::FileList GDCMVolume<TPixelType>::MapToFileList (FileListMapType map) const
  {
    FileList list;
    
    typename FileListMapType::iterator it;
    for (it = map.begin(); it != map.end(); ++it)
    {
      typename FileList::iterator it2;
      for (it2 = (*it).second.begin(); it2 != (*it).second.end(); ++it2)
      {
	list.push_back (*it2);
      }
    }
    return list;
  }
  
//----------------------------------------------------------------------------
  template <class TPixelType>
  void GDCMVolume<TPixelType>::PrintSelf(std::ostream& os, Indent indent) const
  {
    //Superclass::PrintSelf( os, indent );
    unsigned long NumberOfFiles = 0;
    NumberOfFiles = this->GetFileListMap().size();
    os << indent << "Number of SubVolumes: " << NumberOfFiles << std::endl;
    os << indent << "Name: " << this->GetName() << std::endl;
    os << indent << "Is image built: " << m_IsBuilt << std::endl;
    os << indent << "Dictionary : " << std::endl;

    std::string value;
    MetaDataDictionary dict = this->GetMetaDataDictionary();
    //Smarter approach using real iterators
    MetaDataDictionary::ConstIterator itr = dict.Begin();
    MetaDataDictionary::ConstIterator end = dict.End();

    while(itr != end)
    {
      const std::string &key = itr->first;
      ExposeMetaData<std::string>(dict, key, value);
      os << indent<< indent << key.c_str() << " : " << value.c_str()<< std::endl;
      ++itr;
    }
  }

//----------------------------------------------------------------------------
  template <class TPixelType>
  unsigned int* GDCMVolume<TPixelType>::GetSize (void)
  {
    unsigned int* size = new unsigned int[4];
    size[0] = size[1] = size[2] = size[3] = 0;
    
    FileListMapType map = m_FileListMap;
    unsigned int fourthdimension = map.size();
    if (fourthdimension == 0)
      return size;
    unsigned int thirddimension = (*map.begin()).second.size();
    gdcm::ImageReader reader;
    reader.SetFileName( (*map.begin()).second[0].c_str() );
    if( !reader.Read() )
    {
      std::cerr<< "Cannot read requested file: "<<(*map.begin()).second[0].c_str()<<std::endl;
      return size;
    }
    const gdcm::Image &image = reader.GetImage();
    const unsigned int *dims = image.GetDimensions();
    size[0] = dims[0];
    size[1] = dims[1];
    size[2] = thirddimension;
    size[3] = fourthdimension;
    
    return size;
  }
  
  


//----------------------------------------------------------------------------
  template <class TPixelType>
  GDCMImporter3<TPixelType>::GDCMImporter3()
  {
    m_InputDirectory  = "";
    m_IsScanned = 0;
    
    this->SetNumberOfRequiredInputs (0);
    this->SetNumberOfRequiredOutputs (0);
    this->SetNumberOfOutputs (0);

    // Given a list of file names
    
    // A first sort will determine how many (4D) outputs the importer (IO)
    // will have.
    // Usually, one only discriminate patient name, study uid and series uid.
    // Additionally, we discriminate different "Series Number", "Slice Thickness",
    // "Rows", "Columns" and "Orientation".
    
    // Indeed, multiple volumes in a single output will be stored in a  4D volume.
    // Thus, one needs consistent slice thickness, orientation and slice XxY size.
    
    // A second sort will determine how many volumes each output
    // will contain.
    // We discriminate "Sequence name", "Trigger Time", "Diffusion B-factor",
    // and  "Gradient Orientation". We might have to read private "shadowed"
    // tags in order to get this last two values.
    
    // A third sort will order the slices of each volume according to  the data
    // "Image Position Patient", with respect to the "Image Orientation patient"
    // An exception should be thrown if several slices have the same position.
    // If this occurs, it means the sorting process either in the first or
    // second sort was not discriminant enough.
    
    
    // 0010 0010 Patient name
    // we want to reconstruct an image from a single
    // patient
    this->m_FirstScanner.AddTag( gdcm::Tag(0x10,0x10) ); // patient's name
    // 0020 000d Study uid
    // single study
    this->m_FirstScanner.AddTag( gdcm::Tag(0x20,0xd) );  // study uid
    // 0020 000e Series uid
    // single serie
    this->m_FirstScanner.AddTag( gdcm::Tag(0x20,0xe) );  // series uid
    // 0020 0037 Orientation
    // single orientation matrix
    this->m_FirstScanner.AddTag( gdcm::Tag(0x20,0x37) ); // orientation
    // 0020 0011 Series Number
    // A scout scan prior to a CT volume scan can share the same
    // SeriesUID, but they will sometimes have a different Series Number
    this->m_FirstScanner.AddTag( gdcm::Tag(0x0020, 0x0011) );
    // 0018 0050 Slice Thickness
    // On some CT systems, scout scans and subsequence volume scans will
    //   have the same SeriesUID and Series Number - YET the slice 
    //   thickness will differ from the scout slice and the volume slices.
    this->m_FirstScanner.AddTag( gdcm::Tag(0x0018, 0x0050) );
    // 0028 0010 Rows
    // If the 2D images in a sequence don't have the same number of rows,
    // then it is difficult to reconstruct them into a 3D volume.
    this->m_FirstScanner.AddTag( gdcm::Tag(0x0028, 0x0010));
    // 0028 0011 Columns
    // If the 2D images in a sequence don't have the same number of columns,
    // then it is difficult to reconstruct them into a 3D volume.
    this->m_FirstScanner.AddTag( gdcm::Tag(0x0028, 0x0011));

    // 0018 0024 Sequence Name
    // For T1-map and phase-contrast MRA, the different flip angles and
    //   directions are only distinguished by the Sequence Name
    this->m_SecondScanner.AddTag( gdcm::Tag(0x0018, 0x0024) );
    // 0018 9087 Diffusion B-Factor
    // If the 2D images in a sequence don't have the b-value,
    // then we separate the DWIs.
    this->m_SecondScanner.AddTag( gdcm::Tag(0x18,0x9087));
    // 0018 9089 Gradient Orientation
    // If the 2D images in a sequence don't have the same gradient orientation,
    // then we separate the DWIs.
    this->m_SecondScanner.AddTag( gdcm::Tag(0x18,0x9089));
    // 0018 1060 Trigger Time
    this->m_SecondScanner.AddTag( gdcm::Tag(0x18,0x1060) );
    
    // 0020 0032 Position Patient
    this->m_ThirdScanner.AddTag( gdcm::Tag(0x20,0x32) );  
    // 0020 0037 Orientation Patient
    this->m_ThirdScanner.AddTag( gdcm::Tag(0x20,0x37) );    
    
  }


//----------------------------------------------------------------------------
  template <class TPixelType>
  void GDCMImporter3<TPixelType>::Scan (void)
  {

    if (m_IsScanned)
      return;
    
    if (!itksys::SystemTools::FileExists (this->m_InputDirectory.c_str()))
      itkExceptionMacro (<<"Directory "<<this->m_InputDirectory.c_str()<<" not found."<<std::endl);

    this->UpdateProgress(0.00);

    this->m_FileListMapofMap.clear();
    
    gdcm::Directory directory;
    std::cout<<"loading"<<std::endl;
    unsigned long nfiles = directory.Load( this->m_InputDirectory, true);
    std::cout<<"done : "<<nfiles<< " files included."<<std::endl;

    if (!nfiles)
    {
      itkExceptionMacro (<<"The GDCM loader did not succeed loading directory "
			 <<this->m_InputDirectory<<" (or there is no file in directory)"<<std::endl);
    }    
    
    // First sort of the filenames.
    // it will distinguish patient name, series and instance uids, sequence name
    // as well as the image orientation and the nb of columns and rows 
    FileListMapType map;
    try
    {
      map = this->PrimarySort (directory.GetFilenames());
    }
    catch ( itk::ExceptionObject & e)
    {
      std::cerr << e << std::endl;
      itkExceptionMacro (<<"Scanning failed"<<std::endl);
    }
    
    std::cout<<"primary sort gives "<<map.size()<<" different volumes (outputs)"<<std::endl;
    
    typename FileListMapType::const_iterator it;
    for (it = map.begin(); it != map.end(); ++it)
    {
      
      // Second sort of the filenames.
      // it will distinguish the gradient orientation, the b-value, trigger delay
      // (and the T2/ proton density (TODO))
      FileListMapType mapi;
      try
      {
	mapi = this->SecondarySort ((*it).second);
      }
      catch ( itk::ExceptionObject & e)
      {
	std::cerr << e << std::endl;
	continue;
      }
    
      std::cout<<"  secondary division gives "<<mapi.size()<<" subvolumes"<<std::endl;    

      mapi = this->TimeSort (mapi);

      typename FileListMapType::iterator it2;
      for (it2 = mapi.begin(); it2 != mapi.end(); ++it2)
      {
	
	// Third sort of the filenames.
	// it will detect identical image positions
	// and separate distinct volumes.
	// If all image positions are identical, no sort is done.
	FileListMapType mapij;
	try
	{
	  mapij = this->TertiarySort ((*it2).second);
	}
	catch ( itk::ExceptionObject & e)
	{
	  std::cerr << e << std::endl;
	  continue;
	}

	if (mapij.size() >= 1)
	{

	  // Time sort.
	  // A hack to sort the images trigger-time-wise
	  // In case of DWIs, no sort is done since
	  // trigger times are similar
// 	  FileListMapType mapijtimed = this->TimeSort (mapij);
	  FileListMapType mapijtimed = mapij;

	  typename FileListMapType::iterator it3;
	  std::string rootid = (*it2).first;
	  mapi.erase (it2);
	  it2--;
	  for(it3 = mapijtimed.begin(); it3 != mapijtimed.end(); it3++)
	  {
	    std::ostringstream os;
	    os << rootid << (*it3).first;
	    
	    typename FileListMapType::value_type newpair(os.str(),(*it3).second );
	    it2 = mapi.insert (it2, newpair);
	  }
	}
      }
      // We add the map of volumes in the global "database".
      // If more than 1 volume is in the map, it will be saved as a 4D image.
      this->m_FileListMapofMap[(*it).first] = mapi;
    }

    std::cout<<"sorting finished "<<std::endl;
    m_IsScanned = 1;

    this->GenerateOutputs();
  }

  //----------------------------------------------------------------------------
  template <class TPixelType>
  typename GDCMImporter3<TPixelType>::FileListMapType
  GDCMImporter3<TPixelType>::PrimarySort (FileList list)
  {

    FileListMapType ret;
    
    std::cout<<"first scan"<<std::endl;    
    if (!this->m_FirstScanner.Scan (list))
    {
      itkExceptionMacro (<<"The first scanner did not succeed scanning directory "
			 <<this->m_InputDirectory<<std::endl);
    }
    std::cout<<"done"<<std::endl;
    
    gdcm::Directory::FilenamesType::const_iterator file;
    gdcm::Scanner::TagToValue::const_iterator it;
    
    for (file = list.begin(); file != list.end(); ++file)
    {
      if( this->m_FirstScanner.IsKey((*file).c_str()) )
      {
	const gdcm::Scanner::TagToValue& mapping = this->m_FirstScanner.GetMapping((*file).c_str());
	
	if ( !this->m_FirstScanner.GetValue  ((*file).c_str(), gdcm::Tag(0x0028, 0x0010)) )
	  continue;
	
	std::ostringstream os;
	os<<"firstscan.";
	for(it = mapping.begin(); it != mapping.end(); ++it)
	  os <<(*it).second<<".";
	
	if (ret.find(os.str()) == ret.end())
	{
	  FileList newlist;
	  newlist.push_back ((*file).c_str());
	  ret[os.str()] = newlist;
	}
	else
	  ret[os.str()].push_back ((*file).c_str());
      }
      else
      {
	itkWarningMacro (<<"The file "<<(*file).c_str()
			 <<" does not appear in the scanner mappings, skipping. "<<std::endl);
      }
      
    }    

    return ret;
    
  }

  //----------------------------------------------------------------------------
  template <class TPixelType>
  typename GDCMImporter3<TPixelType>::FileListMapType
  GDCMImporter3<TPixelType>::SecondarySort (FileList list)
  {

    FileListMapType ret;
    
    if (!this->m_SecondScanner.Scan (list))
    {
      itkExceptionMacro (<<"The second scanner did not succeed scanning the list, skipping"
			 <<std::endl);
    }
      
    gdcm::Directory::FilenamesType::const_iterator file;
    gdcm::Scanner::TagToValue::const_iterator it;
    
    for (file = list.begin(); file != list.end(); ++file)
    {
      if( this->m_SecondScanner.IsKey((*file).c_str()) )
      {
	const gdcm::Scanner::TagToValue& mapping = this->m_SecondScanner.GetMapping((*file).c_str());
	
	std::ostringstream os;
	os<<"secondscan.";
	
	for(it = mapping.begin(); it != mapping.end(); ++it)
	  os <<(*it).second<<".";

	if (ret.find(os.str()) == ret.end())
	{
	  FileList newlist;
	  newlist.push_back ((*file).c_str());
	  ret[os.str()] = newlist;
	}
	else
	  ret[os.str()].push_back ((*file).c_str());
      }
      else
      {
	itkWarningMacro (<<"The file "<<(*file).c_str()
			 <<" does not appear in the scanner mappings, skipping. "<<std::endl);
      }
    }    

    return ret;
    
  }
  

//----------------------------------------------------------------------------
  template <class TPixelType>
  typename GDCMImporter3<TPixelType>::FileListMapType
  GDCMImporter3<TPixelType>::TertiarySort (FileList list)
  {

    FileListMapType ret;
    
    if (!this->m_ThirdScanner.Scan (list))
    {
      itkExceptionMacro (<<"The third scanner did not succeed scanning the list, skipping"
			 <<std::endl);
    }

    const char *reference = list[0].c_str();
    gdcm::Scanner::TagToValue const &t2v = this->m_ThirdScanner.GetMapping(reference);
    gdcm::Scanner::TagToValue::const_iterator firstit = t2v.find( gdcm::Tag(0x20,0x37) );
    if( (*firstit).first != gdcm::Tag(0x20,0x37) )
    {
      itkExceptionMacro (<<"No orientation information in first file "
			 <<reference<<std::endl);
    }

    const char *dircos = (*firstit).second;
    std::stringstream ss;
    ss.str( dircos );
    gdcm::Element<gdcm::VR::DS,gdcm::VM::VM6> cosines;
    cosines.Read( ss );
    
    // http://www.itk.org/pipermail/insight-users/2003-September/004762.html
    // Compute normal:
    // The steps I take when reconstructing a volume are these: First,
    // calculate the slice normal from IOP:
    double normal[3];
    normal[0] = cosines[1]*cosines[5] - cosines[2]*cosines[4];
    normal[1] = cosines[2]*cosines[3] - cosines[0]*cosines[5];
    normal[2] = cosines[0]*cosines[4] - cosines[1]*cosines[3];
    
    SortedMapType sorted;
    
    std::vector<std::string>::const_iterator it;

    for(it = list.begin(); it != list.end(); ++it)
    {
      const char *filename = (*it).c_str();
      bool iskey = this->m_ThirdScanner.IsKey(filename);
      if( iskey )
      {
	const char *value = this->m_ThirdScanner.GetValue(filename, gdcm::Tag(0x20,0x32));
	if( value )
        {
	  gdcm::Element<gdcm::VR::DS,gdcm::VM::VM3> ipp;
	  std::stringstream ss;
	  ss.str( value );
	  ipp.Read( ss );
	  double dist = 0;
	  for (int i = 0; i < 3; ++i)
	    dist += normal[i]*ipp[i];

	  bool found = 0;
	  typename SortedMapType::iterator finder;
	  for (finder = sorted.begin(); finder != sorted.end(); ++finder)
	    if ( std::abs ( (*finder).first - dist ) < NumericTraits<double>::min() )
	    {
	      found = 1;
	      break;
	    }
	  
	  if( !found )
	  {
	    FileList newlist;
	    newlist.push_back (filename);
	    typename SortedMapType::value_type newpair(dist,newlist);
	    sorted.insert( newpair );
	  }
	  else
	    (*finder).second.push_back (filename);
	}
      }
      else
      {
	itkWarningMacro (<<"The file "<<filename
			 <<" does not appear in the scanner mappings, skipping. "<<std::endl);
      }
    }

    if ((list.size() % sorted.size()) != 0)
    {
      itkExceptionMacro (<<"There appears to be inconsistent file list sizes "<<std::endl
			 <<"Scanner outputs "<<sorted.size()<<" different image positions "
			 <<"within a total list of "<<list.size()<<" files."<<std::endl
			 <<"no sorting will be performed"<<std::endl);
      ///\todo Here we could take into account the files that are consistent
      /// to each other and leave the rest, to build a truncated volume.
    }

    unsigned int nb_of_volumes = list.size() / sorted.size();

    std::cout<<"Tertiary sort : "<<nb_of_volumes<<std::endl;
    
    
    for (unsigned int i=0; i<nb_of_volumes; i++)
    {
      std::ostringstream os;
      os<<".";
      if (i < 100)
	os << "0";
      if (i < 10)
	os << "0";
      os<<i;
      
      FileList newfilelist;

      typename SortedMapType::const_iterator toinsert;
      
      for (toinsert = sorted.begin(); toinsert != sorted.end(); ++toinsert)
      {
	newfilelist.push_back ((*toinsert).second[i]);
      }
      
      typename FileListMapType::value_type newpair(os.str(),newfilelist);
      ret.insert (newpair);
    }
    
    return ret;
  }

 
//----------------------------------------------------------------------------
  template <class TPixelType>
  typename GDCMImporter3<TPixelType>::FileListMapType
  GDCMImporter3<TPixelType>::TimeSort  (FileListMapType map)
  {

    FileListMapType ret;
    
    if (map.size() <= 1)
      return map;
    
    SortedMapType sorted;
    typename FileListMapType::const_iterator it;
    
    for(it = map.begin(); it != map.end(); ++it)
    {
      const char *filename = (*it).second[0].c_str();
      bool iskey = this->m_SecondScanner.IsKey(filename);
      if( iskey )
      {
	const char *value = this->m_SecondScanner.GetValue(filename, gdcm::Tag(0x18,0x1060));
	if( value )
        {
	  typename SortedMapType::value_type newpair(std::atof (value), (*it).second);
	  sorted.insert (newpair);
	}
      }
      else
      {
	itkWarningMacro (<<"The file "<<filename
			 <<" does not appear in the scanner mappings, skipping. "<<std::endl);
      }
    }


    if (sorted.size() <= 1)
    {
      // It means all volumes have the same trigger time.
      // We do not want to then reduce the ordering.
      // We give back the input map
      return map;
    }
    
    typename SortedMapType::const_iterator it2;
    unsigned int iteration = 0;
    
    for(it2 = sorted.begin(); it2 != sorted.end(); ++it2)
    {
      std::ostringstream os;
      if (iteration < 100)
	os << "0";
      if (iteration < 10)
	os << "0";
      os<<iteration;
      
      typename FileListMapType::value_type newpair(os.str(), (*it2).second);
      ret.insert (newpair);
      iteration++;
    }

    return ret;
  }
  

//----------------------------------------------------------------------------
  template <class TPixelType>
  void GDCMImporter3<TPixelType>::GenerateOutputs  (void)
  {
    // release memory
    this->PrepareOutputs();
    this->SetNumberOfOutputs (0);

    typename FileListMapofMapType::iterator it;
    
    for (it = this->m_FileListMapofMap.begin(); it != this->m_FileListMapofMap.end(); it++)
    {
      if ( !(*it).second.size() ) continue;
      
      typename ImageType::Pointer image = ImageType::New();
      
      image->SetName (this->GenerateUniqueName((*it).second));
      image->SetFileListMap ((*it).second);
      this->AddOutput (image);
    }

    this->Modified();
  }

//----------------------------------------------------------------------------
  template <class TPixelType>
  void
  GDCMImporter3<TPixelType>::WriteOutputInFile (unsigned int i, std::string filename)
  {
    try
    {
      this->GetOutput (i)->Build();
      this->GetOutput (i)->Write (filename);
    }
    catch (itk::ExceptionObject & e)
    {
      std::cerr << e << std::endl;
      itkExceptionMacro (<<"Cannot write "<<filename<<std::endl);
    }
  }

//----------------------------------------------------------------------------
  template <class TPixelType>
  void
  GDCMImporter3<TPixelType>::WriteOutputsInDirectory (std::string directory)
  {
    for( unsigned int i=0; i<this->GetNumberOfOutputs(); i++)
    {
      std::string filename = itksys::SystemTools::AppendStrings
	( itksys::SystemTools::ConvertToOutputPath (directory.c_str()).c_str(),
	  this->GetOutput(i)->GetName(),
	  ".mha");
      
      try
      {
	std::cout<<"Writing "<<filename<<" ... "<<std::flush;
	this->GetOutput(i)->Write (filename);
	std::cout<<"done. "<<std::endl;

	if (this->GetOutput(i)->IsVolumeDWIs())
	{
	  std::string gradientsname = itksys::SystemTools::AppendStrings
	    ( itksys::SystemTools::ConvertToOutputPath (directory.c_str()).c_str(),
	      this->GetOutput(i)->GetName(),
	      ".txt");

	  std::cout<<"Writing "<<gradientsname<<" ... "<<std::flush;
	  this->GetOutput(i)->WriteGradients (gradientsname);
	  std::cout<<"done. "<<std::endl;
	}
      }
      catch (itk::ExceptionObject & e)
      {
	std::cerr << e << std::endl;
	continue;
      }
    }
    
  }

//----------------------------------------------------------------------------
  template <class TPixelType>
  std::string
  GDCMImporter3<TPixelType>::GenerateUniqueName (FileListMapType map)
  {
    std::string ret;

    // Grab the description given in the DICOM header
    // of the first file in the map.
    // --> We consider the description to be similar in
    // all files of the map
    std::string description = "unknown";
    if (map.size())
    {
      if ((*map.begin()).second.size())
      {
	std::string file = (*map.begin()).second[0];
	gdcm::Reader reader;
	reader.SetFileName (file.c_str());
	std::set<gdcm::Tag> set;
	//set.insert (gdcm::Tag(0x8, 0x103e));
	
	// if (reader.ReadSelectedTags(set))
	if (reader.ReadUpToTag(gdcm::Tag(0x8, 0x103e), set))
	{
	  gdcm::StringFilter filter;
	  filter.SetFile (reader.GetFile());
	  std::string toadd = filter.ToString (gdcm::Tag(0x8, 0x103e));
	  if (toadd.size() > 1)
	    description = toadd;
	}
      }
    }

    // Remove space and replace underscores
    // Clean sides of the string
    itksys::SystemTools::ReplaceString (description, " ", "-");
    itksys::SystemTools::ReplaceString (description, "_", "-");
    bool finished;
    finished = 0;
    while(!finished)
    {
      std::string::size_type dot_pos = description.rfind("-");
      if( description.size() && (dot_pos == (description.size()-1)) )
	description = description.substr(0, dot_pos);
      else
	finished = 1;
    }
    finished = 0;
    
    while(!finished)
    {
      std::string::size_type dot_pos = description.find("-");
      if( description.size() && (dot_pos == 0) )
	description = description.substr(dot_pos+1, description.size());
      else
	finished = 1;
    }
    if (!description.size())
      description = "unknown";
    
    ret = description;

    // Check if name is taken
    // if it is then add a number at the end of it
    bool taken = 0;
    unsigned int toadd = 0;
    for (unsigned int i=0; i<this->GetNumberOfOutputs(); i++)
      if (! strcmp ( this->GetOutput (i)->GetName(), ret.c_str() ) )
      {
	taken = 1;
	toadd++;
	break;
      }
    while(taken)
    {
      std::ostringstream replacement;      
      replacement << description << "-";
      if (toadd < 10) replacement << "0";
      replacement << toadd;
      ret = replacement.str();
      
      taken = 0;
      for (unsigned int i=0; i<this->GetNumberOfOutputs(); i++)
	if (! strcmp ( this->GetOutput (i)->GetName(), ret.c_str() ) )
	{
	  taken = 1;
	  toadd++;
	  break;
	}
    }
    return ret;
  }

//----------------------------------------------------------------------------
  template <class TPixelType>
  void 
  GDCMImporter3<TPixelType>::ThreadedGenerateData(const OutputImageRegionType& region, int id)
  {
    if (!m_IsScanned)
      itkExceptionMacro (<<"MUST call Scan() before Update()");

    unsigned int start = region.GetIndex()[0];
    unsigned int end = start + region.GetSize()[0];

    for (unsigned int i = start; i < end; i++)
    {
      try
      {
	this->GetOutput (i)->Build();
      }
      catch (itk::ExceptionObject & e)
      {
	std::cerr << e << std::endl;
	continue;
      }
    }
  }

//----------------------------------------------------------------------------
  template <class TPixelType>
  int 
  GDCMImporter3<TPixelType>::SplitRequestedRegion(int i, int num, OutputImageRegionType& splitRegion)
  {
    typename ImageType::IndexType splitIndex;
    splitIndex.Fill (0);
    
    typename ImageType::SizeType splitSize;
    splitSize.Fill (0);
    
    // determine the actual number of pieces that will be generated
    typename ImageType::SizeType::SizeValueType range = this->GetNumberOfOutputs();
    int valuesPerThread = (int)::vcl_ceil(range/(double)num);
    int maxThreadIdUsed = (int)::vcl_ceil(range/(double)valuesPerThread) - 1;

    splitIndex[0] = 0;
    splitSize[0] = this->GetNumberOfOutputs();
    
    // Split the region
    if (i < maxThreadIdUsed)
    {
      splitIndex[0] += i*valuesPerThread;
      splitSize[0] = valuesPerThread;
    }
    if (i == maxThreadIdUsed)
    {
      splitIndex[0] += i*valuesPerThread;
      // last thread needs to process the "rest" dimension being split
      splitSize[0] = splitSize[0] - i*valuesPerThread;
    }  
  
    // set the split region ivars
    splitRegion.SetIndex( splitIndex );
    splitRegion.SetSize( splitSize );

    return maxThreadIdUsed + 1;
  }

  
//----------------------------------------------------------------------------
  template <class TPixelType>
  void
  GDCMImporter3<TPixelType>::PrintSelf(std::ostream& os, Indent indent) const
  {
    Superclass::PrintSelf( os, indent );
    os << indent << "Input Directory: " << m_InputDirectory.c_str() << std::endl;
    os << indent << "Number of Volumes: " << this->GetNumberOfOutputs() << std::endl;
  }
} // end of namespace ITK


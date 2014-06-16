/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <vtkMetaDataSet.h>
#include <medVtkInriaExport.h>
#include <string>
#include <vector>

#include <vtkMetaImageData.h>
#include <itkExtractImageFilter.h>
#include <itkImageRegionIterator.h>
#include <itkImageRegion.h>
#include <itkImage.h>



/**
   \class vtkMetaDataSetSequence vtkMetaDataSetSequence.h "vtkMetaDataSetSequence.h"
   \brief usefull class to handle a sequence (serie) of vtkMetaDataSet
   \author Nicolas Toussaint
   
   This class is a powerfull vtk Addon class that helps handling a serie of vtkDataSets.
   
   vtkMetaDataSet has a flag of time, that allows this class to handle a sequence
   of different vtkMetaDataSet (of the same type), the output dataset (got from GetDataSet())
   can be updated to a specific time with UpdateToTime().

   It does not compute any time interpolation. 
   
   \see
   vtkMetaImageData vtkMetaSurfaceMesh vtkMetaVolumeMesh
   vtkMetaDataSet vtkDataManager
*/


class vtkDoubleArray;

class MEDVTKINRIA_EXPORT vtkMetaDataSetSequence: public vtkMetaDataSet
{
 public:

  static vtkMetaDataSetSequence* New();
  vtkTypeRevisionMacro(vtkMetaDataSetSequence,vtkMetaDataSet);
  virtual void PrintSelf(ostream& os, vtkIndent indent);

  /**
     Read and Write the Sequence into a directory
  */
  virtual void Read  (const char* dirname);
  virtual void Write (const char* dirname);
  /*   void ReadSingleFile (const char* filename); */
  /*   void WriteSingleFile (const char* filename); */
  
  virtual void ReadAndAddFile (const char* filename, double time = 0);
  
  /**
     read and assign some scalars to a frame of this sequence (should be point set). 
     Either vtkMetaSurfaceMesh or vtkMetaVolumeMesh. The scalars are casted in float type
     for memory purpose. the scalars are added to the PointData or CellData of the dataset
     according to a flag written in the file.
     head of the file should be : \n\n
     keyword (string that be the name of the scalar field)\n
     type (flag that says where the scalars should be assigned : 1 for vertices, 2 for cells) \n
     nb dim (integers. nb is the number of Tuples of the field; dim is the Tuple size)\n\n
  */
  virtual void ReadFrameData (const char* filename, unsigned int iter);

  virtual void ReadData (const char* filename);
  
  //BTX

  typedef itk::Image<short,3> ShortImageType;
  typedef ShortImageType::DirectionType ShortDirectionType;
  
  
  /**
     Writes a file of a given scalar component type.
     Use with care. Please prefer using Write() 
  */
  template <typename type> inline void WriteSingleFile (const char* filename)
  {
    
      
    vtkImageData* imagedata = vtkImageData::SafeDownCast (this->GetDataSet());
    if (!imagedata)
      throw vtkErrorCode::CannotOpenFileError;
    typedef typename itk::Image<type, 4> Image4DType;
    typedef typename itk::Image<type, 3> Image3DType;
    typedef typename Image4DType::RegionType Region4dType;
    typedef typename Image4DType::SpacingType Spacing4Dtype;
    typedef typename Image4DType::PointType Point4Dtype;
    typedef typename Image4DType::DirectionType Direction4Dtype;
    
    typedef typename itk::ImageRegionIterator<Image4DType> Iterator4DType;
    typedef typename Iterator4DType::IndexType Index4DType;
    
    typename Image4DType::Pointer image = Image4DType::New();
    Region4dType region;
    region.SetSize (0, imagedata->GetDimensions()[0]);
    region.SetSize (1, imagedata->GetDimensions()[1]);
    region.SetSize (2, imagedata->GetDimensions()[2]);
    region.SetSize (3, this->GetNumberOfMetaDataSets());
    
    image->SetRegions (region);
    image->Allocate();
    
    Spacing4Dtype spacing;
    spacing[0] = imagedata->GetSpacing()[0];
    spacing[1] = imagedata->GetSpacing()[1];
    spacing[2] = imagedata->GetSpacing()[2];
    spacing[3] = this->GetTimeResolution();
    image->SetSpacing (spacing);

    Point4Dtype origin;
    origin[0] = imagedata->GetOrigin()[0];
    origin[1] = imagedata->GetOrigin()[1];
    origin[2] = imagedata->GetOrigin()[2];
    origin[3] = 0;
    image->SetOrigin (origin);

    vtkMetaImageData* firstmetaimage = vtkMetaImageData::SafeDownCast(this->GetMetaDataSet (0));
    if (firstmetaimage)
    {
      typename Image3DType::Pointer firstimage;
      firstmetaimage->GetItkImage<type>(firstimage);
      
      if (firstimage)
      {
	Direction4Dtype direction;

	for (unsigned int i=0; i<4; i++)
	  for (unsigned int j=0; j<4; j++)
	  {
	    if ((i < 3) && (j < 3))
	      direction[i][j] = firstimage->GetDirection()[i][j];
	    else
	      direction[i][j] = (i == j) ? 1 : 0;
	  }

	image->SetDirection(direction);
	
      }
    }

    
    
    Iterator4DType itOut(image, region);
    Index4DType index;
    
    
    for (int t=0; t<this->GetNumberOfMetaDataSets(); t++)
    {
      index[3] = t;
      vtkMetaImageData* metaimage = vtkMetaImageData::SafeDownCast (this->GetMetaDataSet (t));
      
      typename Image3DType::Pointer t_image;
      metaimage->GetItkImage<type>(t_image);
      if (t_image.IsNull())
      {
	std::cerr<<"there is a problem here !"<<std::endl;
	std::cerr<<"cannot cast frame image into itk::image !"<<std::endl;
	throw vtkErrorCode::CannotOpenFileError;
      }
      
      typename itk::ImageRegionIterator<Image3DType> itIn(t_image, t_image->GetLargestPossibleRegion());
      typename itk::ImageRegionIterator<Image3DType>::IndexType t_index;
      
      
      for (int x=0; x<imagedata->GetDimensions()[0]; x++)
      {
	index[0] = x;
	t_index[0] = x;
	for (int y=0; y<imagedata->GetDimensions()[1]; y++)
	{
	  index[1] = y;
	  t_index[1] = y;
	  for (int z=0; z<imagedata->GetDimensions()[2]; z++)
	  {
	    index[2] = z;
	    t_index[2] = z;
	    
	    itOut.SetIndex (index);
	    itIn.SetIndex (t_index);
	    
	    itOut.Set (itIn.Get());
	  }
	}
      }
    }
    
    
    typename itk::ImageFileWriter<Image4DType>::Pointer writer 
      = itk::ImageFileWriter<Image4DType>::New();
    writer->SetFileName (filename);
    
    writer->SetInput (image);
    
    try
    {
      std::cout << "writing: " << filename << std::endl;
      writer->Write();
    }
    catch (itk::ExceptionObject & e)
    {
      std::cerr<<e<<std::endl;
      vtkErrorMacro(<<"unable to write file : "<<filename<<endl);
      throw vtkErrorCode::UserError;
    } 
  }




  template <typename type> inline void ReadSingleFile (const char* filename)
  {

  
    typedef typename itk::Image<type, 4> Image4DType;
    typedef typename itk::Image<type, 3> Image3DType;
  
    typedef typename Image3DType::DirectionType Direction3Dtype;
    typedef typename Image4DType::DirectionType Direction4Dtype;
    
    typename itk::ImageFileReader<Image4DType>::Pointer reader 
      = itk::ImageFileReader<Image4DType>::New();
    reader->SetFileName (filename);
  
    try
    {
      std::cout << "reading: " << filename << std::endl;
      reader->Update();
    }
    catch (itk::ExceptionObject &)
    {
      vtkErrorMacro(<<"unable to open file : "<<filename<<endl);
      throw vtkErrorCode::CannotOpenFileError;
    }
    
    if ( !strcmp (reader->GetImageIO()->GetNameOfClass(), "PhilipsRECImageIO"))
    {
      Direction3Dtype correct3ddirection = this->ExtractPARRECImageOrientation(filename);
      Direction4Dtype correct4ddirection;
      correct4ddirection.Fill (0);
      for (unsigned int i=0; i<3; i++)
	for (unsigned int j=0; j<3; j++)
	  correct4ddirection[i][j] = correct3ddirection[i][j];
      correct4ddirection[3][3] = 1;
      reader->GetOutput()->SetDirection (correct4ddirection);

      typename Image3DType::PointType correct3dorigin = this->ExtractPARRECImageOrigin (filename, correct3ddirection);
      typename Image4DType::PointType correct4dorigin;
      for (unsigned int i=0; i<3; i++)
	correct4dorigin[i] = correct3dorigin[i];
      correct4dorigin[3] = 0.0;
      reader->GetOutput()->SetOrigin (correct4dorigin);
    }  

    
    // here we look for the 4th dimension:
    SetITKDataSet<type>(reader->GetOutput(), filename);
  }
  
  template <typename type> inline void SetITKDataSet (typename itk::Image<type, 4>* dataset, const char* filename = "nofile")
  {
    
    typedef typename itk::Image<type, 4> Image4DType;
    typedef typename itk::Image<type, 3> Image3DType;
  
    typename Image4DType::Pointer image = dataset;

    typename Image4DType::SizeType size = image->GetLargestPossibleRegion().GetSize();
    unsigned int nVolumes = size[3];

    // split the 4D volume into 3D volumes
    typename Image4DType::RegionType regionToExtract = image->GetLargestPossibleRegion();
    typename Image4DType::IndexType index;
    index[0] = 0;
    index[1] = 0;
    index[2] = 0;
    index[3] = 0;
    size[3] = 0;
    regionToExtract.SetSize (size);
    regionToExtract.SetIndex (index);
    
    if( nVolumes==0 )
    {
      throw vtkErrorCode::CannotOpenFileError;
    }

    this->Type = vtkMetaDataSet::VTK_META_IMAGE_DATA;
  

    double res = image->GetSpacing ()[3];
  
    for( unsigned int n=0; n<nVolumes;  n++)
    {
      typedef typename itk::ExtractImageFilter<Image4DType, Image3DType> ExtractImageType;
      regionToExtract.SetIndex (3,n);
      typename ExtractImageType::Pointer myExtractor = ExtractImageType::New();
      myExtractor->SetExtractionRegion (regionToExtract);
        myExtractor->SetDirectionCollapseToGuess();
      myExtractor->SetInput (image);
      
      try
      {
	myExtractor->Update();
      }
      catch (itk::ExceptionObject &)
      {
	vtkErrorMacro(<<"no volume in file : "<<filename<<endl);
	throw vtkErrorCode::UserError;
      }

      vtkMetaImageData* imagedata = vtkMetaImageData::New();

      try
      {

	imagedata->SetItkImage<type>(myExtractor->GetOutput());
      
	double time = (double)(n)*res;
	std::string name = vtksys::SystemTools::GetFilenameWithoutExtension((vtksys::SystemTools::GetFilenameName (filename)).c_str()).c_str();
	std::ostringstream o_name;
	o_name<<name.c_str()<<"("<<n<<")";
      
	imagedata->SetName (o_name.str().c_str());
	imagedata->SetTime (time);
	this->AddMetaDataSet (imagedata );
      
      
      }
      catch (itk::ExceptionObject &)
      {
	vtkErrorMacro(<<"error when reading : skipping volume "<<n<<" of file "<<filename<<endl);
      }

      imagedata->Delete();
    
    }
    
  }
  

  ShortDirectionType ExtractPARRECImageOrientation (const char* filename);
  ShortImageType::PointType ExtractPARRECImageOrigin (const char* filename, ShortDirectionType direction);
  

  //ETX
  /**
     Insert a vtkMetaDataSet at the end of the sequence list
     As soon as the metadataset time flag is set, the list will be reordered
     with respect to the progress of time.
     The first vtkMetaDataSet added will determine the type of the sequence.
     adding new heterogeneous vtkMetaDataSet will fail.
  */
  virtual void AddMetaDataSet (vtkMetaDataSet* metadataset);
  /**
     Remove a given vtkMetaDataSet from the sequence.
     This will unregister the given vtkMetaDataSet from the sequence.
  */
  virtual void RemoveMetaDataSet (vtkMetaDataSet* metadataset);
  virtual void RemoveMetaDataSet (unsigned int i);
  /**
     Clear the sequence, empty the list of vtkMetaDataSet.
  */
  virtual void RemoveAllMetaDataSets ();
  /**
     Check if a given vtkMetaDataSet belong to this sequence.
  */
  virtual bool HasMetaDataSet (vtkMetaDataSet* metadataset);

  /**
     Find a metadataset in sequence by its name
  */
  virtual vtkMetaDataSet* FindMetaDataSet (const char* name);
  /**
     Find a metadataset in sequence by its time (returns the closest without interpolation
  */
  virtual vtkMetaDataSet* FindMetaDataSet (double time, unsigned int &id);
  /**
     This method has to be called if the user wants to see the attributes of the output vtkDataSet (GetDataSet())
     changing respecting the given time. No time interpolation is computed.
     Updating to an out of range time will set the output to the closest time available.
  */
  virtual void UpdateToTime (double time = 0);
  /**
     This method has the same effect as UpdateToTime() exept the argument is the index of the
     vtkMetaDataSet to use in the list. Out of range index will set the output to the closest index available.
  */
  virtual void UpdateToIndex (unsigned int id = 0);

  /**
     Access to one of the vtkMetaDataSet in the sequence list
  */
  virtual vtkMetaDataSet* GetMetaDataSet (unsigned int i);

  /**
     Access to the entire list of vtkMetaDataSet.
     Use with care.
  */
  //BTX
  std::vector<vtkMetaDataSet*> GetMetaDataSetList() const
  { return MetaDataSetList; }
  //ETX

  /**
     Set some variables : SequenceDuration
     Not very usefull as soon as the duration is computed from
     the different time flags of the list of vtkMetaDataSet
  */
  //vtkSetMacro (SequenceDuration, double);
  void SetSequenceDuration(double s)
  {
    this->SequenceDuration = s;
    this->ComputeTimesFromDuration();
  }
  /**
     Access to some variables : SequenceDuration
  */
  vtkGetMacro (SequenceDuration, double);

  virtual const char* GetDataSetType() const
  {
    return "Sequence";
  }
  /**
     Size of the vtkMetaDataSet list.
  */
  int GetNumberOfMetaDataSets() const
  { return MetaDataSetList.size(); }
  /**
     Access to the minimal time flag of the sequence.
  */
  double GetMinTime() const;
  /**
     Access to the maximal time flag of the sequence.
  */
  double GetMaxTime() const;
  /**
     Access to the resolution of the sequence, i.e. the time step used.
  */
  double GetTimeResolution();
  /**
     Internal use : Compute the time in case of loop.
  */
  virtual double GetRelativeTime (double time);
  /**
     Internal use : Compute the time duration of the sequence.
  */
  virtual void ComputeSequenceDuration();
  virtual void ComputeTimesFromDuration();
  /**
     Use this method to colorize the actors of this metadata by a given vtkDataArray
     The array must be contained by the member vtkDataSet of this metadataset.
     Use GetArray() to find a specific array
  */
  virtual void ColorByArray(vtkDataArray* array);
  /**
     Get/Set the scalar visibility, see parent class
  */
  virtual void SetScalarVisibility(bool val);
  /**
     Internal use : not used anymore...
  */
  virtual void BuildSequenceFromGeometry(const char* filename, unsigned int Niter);
  /**
     Get/Set method for the same geometry flag.
     If set to true, the topology of the output dataset will stay the same
     while the points coordinate and scalars will change among time.
     Set to true only if you are sure that the topology is unchanged between the
     different frames of the sequence.
  */
   bool GetSameGeometryFlag()
  { return this->SameGeometryFlag; }
  void SetSameGeometryFlag ( bool flag)
  { this->SameGeometryFlag = flag; }

  static unsigned int CanReadFile (const char* filename);

  /**
     Generate a array of size [number of frames] containing the value of an attribute
     (given with its name) at a certain position (given by the id of the point/cell to follow). 
     Return object is NULL when the attribute is not found in the frames, or when the point/cell id
     is not valid. 
     If succeeded this methods creates a new instance of vtkDoubleArray. Please make sure you
     delete the object after using it.
  */
  vtkDoubleArray* GenerateFollowerTimeTable(const char* arrayname, unsigned int idtofollow);
  /**
     Generate a array of size [number of frames] containing the value of an attribute
     contained in the MetaDataDictionary of the frames. The metadata is found by its key
     and has to be in double format. 
     Return object is NULL when the metadata is not found in the frames, or when
     the metadata is not a double expression. 
     If succeeded this methods creates a new instance of vtkDoubleArray. Please make sure you
     delete the object after using it.
  */
  vtkDoubleArray* GenerateMetaDataTimeTable(const char* metadatakey);

  vtkGetMacro(ParseAttributes, bool);
  vtkSetMacro(ParseAttributes, bool);

  /**
     Copy some informations from a given metadataset
     It corresponds basically to all metadataset characteristics unless the vtkDataSet
     name, time, metadata dictionary...
  */
  virtual void CopyInformation (vtkMetaDataSet* metadataset);

  virtual double* GetCurrentScalarRange();

  vtkGetMacro (CurrentId, int);
  
 protected:
  vtkMetaDataSetSequence();
  ~vtkMetaDataSetSequence();

  /**
     Internal use : Initialize sequence, not used yet.
  */
  virtual void Initialize();
  /**
     Internal use : Build the output from a given vtkMetaDataSet.
  */
  virtual void   BuildMetaDataSetFromMetaDataSet (vtkMetaDataSet* metadataset);

  virtual void ReadDataAdvanced (const char* filename);
  
  
  //BTX
  std::vector<vtkMetaDataSet*> MetaDataSetList;
  //ETX

  
 private:
  
  vtkMetaDataSetSequence(const vtkMetaDataSetSequence&);      // Not implemented.
  void operator=(const vtkMetaDataSetSequence&);              // Not implemented.

  int    CurrentId;
  double SequenceDuration;
  bool   SameGeometryFlag;
  bool  ParseAttributes;

  //BTX
  std::vector<itk::ProcessObject::Pointer> Converters;
  //ETX
};




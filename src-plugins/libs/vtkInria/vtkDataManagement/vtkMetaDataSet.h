/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <vtkDataObject.h>
#include <string>
#include <vector>

#include <vtkDataSet.h>

//#include <vtkActor.h>

#include <itkMetaDataObject.h>
#include <itkMetaDataDictionary.h>
/**
   \class vtkMetaDataSet vtkMetaDataSet.h "vtkMetaDataSet.h"
   \brief Abstract class for vtkDataset handling
   \author Nicolas Toussaint
   
   This class is a powerfull vtk Addon class that helps handling a vtkDataSet.
   see the lower level classes for details
   \see
   vtkMetaImageData vtkMetaSurfaceMesh vtkMetaVolumeMesh vtkDataManager
*/

class vtkDataArrayCollection;
class vtkDataArray;
class vtkActor;
class vtkActorCollection;
class vtkScalarsToColors;
class vtkPolyData;


class vtkMetaDataSet: public vtkDataObject
{
 public:

  static vtkMetaDataSet* New();
  vtkTypeRevisionMacro(vtkMetaDataSet,vtkDataObject);
  virtual void PrintSelf(ostream& os, vtkIndent indent);

  //BTX
  typedef itk::MetaDataDictionary DictionaryType;
  /**
     Access to the metadataset dictionary
     The dictionary can be used to hold some arbitrary types of flags
  */
  DictionaryType GetMetaDataDictionary()
  { return this->MetaDataDictionary; }
  /**
     Set the metadataset dictionary
  */
  void SetMetaDataDictionary (DictionaryType dictionary)
  { this->MetaDataDictionary = dictionary; }
  //ETX

  //BTX
  /**
     Fill the metadataset dictionary with this method
     Example :
       mymetadataset->SetMetaData< std::vector<int> >( "idlist", myidlist );
  */
  template <class type> inline void SetMetaData (std::string key, type value)
  {
    if (this->MetaDataDictionary.HasKey (key))
      itk::EncapsulateMetaData<type>(this->MetaDataDictionary, key, (type)value);
    else
      itk::EncapsulateMetaData<type>(this->MetaDataDictionary, key, (type)value);
    
  }
  
  /**
     Get a metadataset dictionary value with this method
     Example :
       bool isvalid = mymetadataset->GetMetaData< std::vector<int> >( "idlist", outputidlist );
  */
  template <class type> inline bool GetMetaData (std::string key, type &ret)
  {
    
    //type toret;//=vtkMetaDataSet::VTK_META_UNKNOWN; // to be implemanted with good template
    bool valid = itk::ExposeMetaData<type>(this->MetaDataDictionary, key, ret);
    //ret = toret;
    if(!valid)
      return false;
    return true;
  }


  std::vector<std::string> GetMetaDataKeyList()
  {
    std::vector<std::string> ret;
    ret = this->MetaDataDictionary.GetKeys();

    return ret;
  }
  

  
  //ETX
    
  //BTX
  enum DataSetTypeId
  {
    VTK_META_IMAGE_DATA      = 0,
    VTK_META_SURFACE_MESH    = 1,
    VTK_META_VOLUME_MESH     = 2,    
    VTK_META_DATATYPE_NUMBER = 3,
    VTK_META_UNKNOWN = 10000
  };
  //ETX

  /**
     Set the dataset associated with the metadataset
  */
  virtual void SetDataSet (vtkDataSet* dataset);
  /**
     Get the dataset associated with the metadataset
  */ 
  vtkGetObjectMacro (DataSet, vtkDataSet);
  /**
     Get the type of the metadataset :
      vtkMetaDataSet::VTK_META_IMAGE_DATA, vtkMetaDataSet::VTK_META_SURFACE_MESH,
      vtkMetaDataSet::VTK_META_VOLUME_MESH, or vtkMetaDataSet::VTK_META_UNKNOWN
  */ 
  vtkGetMacro (Type, unsigned int);
  vtkSetMacro (Type, unsigned int);
  

  /**
     Access to the visualization property of the metadataset
     downcast the object to the write type :
     vtkProperty for vtkMetaSurfaceMesh
     vtkVolumeProperty for vtkMetaImageData
  */
  vtkGetObjectMacro(Property, vtkObject);
  vtkSetObjectMacro(Property, vtkObject);

  /**
     Add an actor to the metadataset. Use this method
     to be able to handle several actors of the same metadataset at the same time :
     mymetadataset->SetVisibility(true);
  */
  void AddActor (vtkActor* actor);
  /**
     Removes an actor from the list
  */
  void RemoveActor (vtkActor* actor);
  /**
     Clear actor list
  */
  void RemoveAllActors ();
  /**
     Access to an actor of the list
  */
  vtkActor* GetActor (unsigned int i);
  /**
     returns the amount of actors handled by this metadataset
  */
  unsigned int GetNumberOfActors() const;
  /**
     returns true if the actor is handled by this metadataset
  */
  bool HasActor (vtkActor* actor);
  /**
     Get/Set the visibility of the metadataset : to be used with AddActor()
  */
  void SetVisibility (bool visible);
  /**
     Get/Set the visibility of the metadataset : to be used with AddActor()
  */
  int GetVisibility();
  /**
     Get/Set the visibility of the metadataset : to be used with AddActor()
  */
  void SetActorVisibility (unsigned int it, bool visible);
  /**
     Get/Set the visibility of the metadataset : to be used with AddActor()
  */
  int GetActorVisibility (unsigned int it);
  /**
     Internal use only
  */
  vtkGetMacro(PickedPointId, int);
  vtkSetMacro(PickedPointId, int);
  /**
     Internal use only
  */
  vtkGetMacro(PickedCellId, int);
  vtkSetMacro(PickedCellId, int);
  /**
     Get/Set methods fot the metadataset name
  */ 
  virtual const char* GetName() const;
  virtual void        SetName (const char* name);
  /**
     Get/Set methods fot the metadataset current file path
  */ 
  virtual const char* GetFilePath() const;
  virtual void        SetFilePath (const char* path);
  /**
     Get/Set methods for the metadataset tag
      You can associate several metadataset to a single tag to retrieve them easily
  */
  virtual const char* GetTag() const;
  virtual void        SetTag (const char* path);
  /**
     Overridden methods for read and write the dataset
  */ 
  virtual void Read  (const char* filename);
  virtual void Write (const char* filename);
  /**
     read and assign some scalars to the dataset (should be point set). 
     Either vtkMetaSurfaceMesh or vtkMetaVolumeMesh. The scalars are casted in float type
     for memory purpose. the scalars are added to the PointData or CellData of the dataset
     according to a flag written in the file.
     head of the file should be : \n\n
     keyword (string that be the name of the scalar field)\n
     type (flag that says where the scalars should be assigned : 1 for vertices, 2 for cells) \n
     nb dim (integers. nb is the number of Tuples of the field; dim is the Tuple size)\n\n
  */
  virtual void ReadData (const char* filename);
  /**
     read and assign some scalars to the dataset (should be point set). 
     Either vtkMetaSurfaceMesh or vtkMetaVolumeMesh. The scalars are casted in float type
     for memory purpose. the scalars are added to the PointData or CellData of the dataset
     according to a flag written in the file.
     head of the file should be : \n\n
     keyword (string that be the name of the scalar field)\n
     type (flag that says where the scalars should be assigned : 1 for vertices, 2 for cells) \n
     nb dim (integers. nb is the number of Tuples of the field; dim is the Tuple size)\n\n
  */
  virtual void WriteData (const char* filename, const char* dataname);
  /**
     Get the type of the metadataset as string
  */
  virtual const char* GetDataSetType() const
  {
    return "MetaDataSet";
  }
  /**
     Get/Set methods for a flag of time
  */
  void SetTime (double time)
  {
    this->Time = time;
    this->SetMetaData<double>("Time", time);
  }
  double GetTime()
  {
    double tmp = this->Time;
    bool isvalid = this->GetMetaData<double>("Time", tmp);
    if (isvalid)
      this->Time = tmp;
    
    return this->Time;
  }
  
  /**
     Copy some informations from a given metadataset
     It corresponds basically to all metadataset characteristics unless the vtkDataSet
     name, time, metadata dictionary...
  */
  virtual void CopyInformation (vtkMetaDataSet* metadataset);
  /**
     Access to the collection of vtkDataArray contained by the member vtkDataSet
     in points or cells for colorization mapping purpose
  */
  virtual void GetColorArrayCollection(vtkDataArrayCollection* collection);

  /**
     Access to the collection of vtkDataArray contained in this metadataset. 
     These arrays do not have any colorization purpose. They are stored for the user
     convenience. 
  */
  virtual vtkDataArrayCollection* GetArrayCollection()
  {
    return this->ArrayCollection;
  }
  /**
     Returns the vtkDataArray contained in the DataSet or in metadataset, named name
     returns NULL if not found
  */
  virtual vtkDataArray* GetArray (const char* name);

  /**
     Add an array to the MetaDataSet,
     does not add it to the pointdata or celldata
     Access to this dataarray by its name with GetArray()
  */
  virtual void AddArray (vtkDataArray* array);
  
  /**
     Use this method to colorize the actors of this metadata by a given vtkDataArray
     The array must be contained by the member vtkDataSet of this metadataset.
     Use GetArray() to find a specific array
  */
  virtual void ColorByArray(vtkDataArray* array);
  /**
     Get/Set the scalar visibility, operates on the actors
  */
  virtual void SetScalarVisibility(bool val);
  /**
     Get/Set the scalar visibility, operates on the actors
  */
  virtual bool GetScalarVisibility();
  
  /**
     Get/Set the scalar visibility, operates on the actors
  */
  virtual void ScalarVisibilityOn();
  /**
     Get/Set the scalar visibility, operates on the actors
  */
  virtual void ScalarVisibilityOff();
  /**
     Get the currently used scalar array for visualization
  */
  vtkGetObjectMacro (CurrentScalarArray, vtkDataArray);
  void SetCurrentActiveArray (vtkDataArray* array)
  {
    this->CurrentScalarArray = array;
  }

  virtual double* GetCurrentScalarRange();
  
  
  /**
     Description:
     Lock/Unlock flag
  */
  vtkGetMacro(Lock, int);
  vtkSetClampMacro(Lock, int, 0, 1);
  vtkBooleanMacro(Lock, int);
  

  virtual void CreateWirePolyData(){};
  
  vtkGetObjectMacro (WirePolyData, vtkPolyData);
  void SetWirePolyData(vtkPolyData* data);
  
 protected:
  vtkMetaDataSet();
  ~vtkMetaDataSet();

  /**
     Method called everytime the dataset changes for connexion updates
   */
  virtual void LinkFilters();
  /**
     Method called everytime the dataset changes for initialization
  */
  virtual void Initialize();

  /**
     Internal use : read and assign positions to the dataset (should be point set). 
     Either vtkMetaSurfaceMesh or vtkMetaVolumeMesh
  */
  virtual void ReadPosition(const char* filename);
  /**
     Internal use : read and assign some scalars to the dataset (should be point set). 
     Either vtkMetaSurfaceMesh or vtkMetaVolumeMesh. The scalars are casted in float type
     for memory purpose. 
  */
  virtual void ReadDataInternal(const char* filename);

  
  unsigned int Type;

  int PickedPointId;
  int PickedCellId;
  
  vtkDataSet* DataSet;
  vtkActorCollection* ActorList;
  double      Time;
  vtkObject*  Property;
  vtkDataArrayCollection* ArrayCollection;

  vtkDataArray* CurrentScalarArray;
  
 private:
  
  vtkMetaDataSet(const vtkMetaDataSet&);        // Not implemented.
  void operator=(const vtkMetaDataSet&);        // Not implemented.

  //BTX
  std::string Name;
  std::string FilePath;
  std::string Tag;
  //ETX

  int Lock;
  
  //BTX
  DictionaryType MetaDataDictionary;
  //ETX

  vtkPolyData* WirePolyData;
  
  
 
};




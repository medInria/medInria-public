#pragma once
/*=========================================================================

medInria

Copyright (c) INRIA 2013 - 2020. All rights reserved.
See LICENSE.txt for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.

=========================================================================*/

#include <vtkObject.h>

#include <string>
#include <vector>

#include <vtkDataMeshPluginExport.h>
#include <vtkCommand.h>

class vtkMetaDataSet;
class vtkMetaDataSetSequence;

/**
   \class vtkDataManager vtkDataManager.h "vtkDataManager.h"
   \brief usefull class to handle several datasets, and dataset sequences
   \author Nicolas Toussaint
   
   This class is a powerfull vtk tool to manage datasets.
   
   \see
   vtkMetaSurfaceMesh vtkMetaVolumeMesh
   vtkMetaDataSetSequence vtkMetaDataSet
*/


class VTKDATAMESHPLUGIN_EXPORT vtkDataManager: public vtkObject
{
 public:

  static vtkDataManager* New();
  vtkTypeMacro(vtkDataManager,vtkObject);
  virtual void PrintSelf(ostream& os, vtkIndent indent);

  /**
     Primary method to get a metadataset of the manager from its index.
     returns null object if out of range.
  */
  virtual vtkMetaDataSet* GetMetaDataSet    (unsigned int i) const;
  /**
     Secondary method to get a metadataset of the manager
     from the metadataset's name.
     returns null object if the name is not referenced in datamanager.
  */
  virtual vtkMetaDataSet* GetMetaDataSet    (const char* name) const;
  /**
     Secondary method to get a metadataset of the manager
     from the metadataset's tag.
     returns null object if the name is not referenced in datamanager.
  */
  virtual vtkMetaDataSet* GetMetaDataSetFromTag (const char* tag) const;

  //bool HasMetaDataSet (vtkMetaDataSet* metadataset, unsigned int &id);

  /**
     Primary method to add a metadataset to the manager.
     The metadataset is added at the end of the list.
  */
  virtual void AddMetaDataSet    ( vtkMetaDataSet* metadataset);
  /**
     Only method to remove a metadataset from the datamanager.
     This method simply unreferences the object by calling a delete.
  */
  virtual void RemoveMetaDataSet ( vtkMetaDataSet* metadataset);
  /**
     Only method to get a sequence of the manager from its index.
     returns null pointer if out of range.
  */
  virtual vtkMetaDataSet* ReadFile (const char* filename, const char* name = nullptr, bool forsequence = false);
  /**
     Call this method to scan an entire directory.
     This will add every dataset that is readable in the manager.
  */
  virtual void ScanDirectory (const char* dirname);
  /**
     Call this method to scan an entire directory where files belong to a sequence.
     This will add every dataset that is readable in a sequence, set it to the given duration
     and add the sequence to the manager.
  */
  virtual vtkMetaDataSetSequence* ScanDirectoryForSequence (const char* dirname, double duration = 2.0);
  /**
     returns true if the given metadataset is referenced in the manager.
     if yes, returns the its index in the list.
  */
  virtual bool IsInManager (vtkMetaDataSet* metadataset, unsigned int &id) const;
  /**
     returns true if the given name is used in the manager.
     if yes, returns the its index in the list.
  */
  virtual bool IsNameInManager (const char* name, unsigned int &id) const;
  /**
     returns true if the given tag is used in the manager.
     if yes, returns the its index in the list.
  */
  virtual bool IsTagInManager (const char* tag, unsigned int &id) const;
  /**
     returns the number of metadatasets referenced in the manager
  */
  virtual unsigned int GetNumberOfMetaDataSet() const;
  /**
     returns the number of metadatasets referenced in the manager for a specific type.
     This type can be : VTK_META_IMAGE_DATA, VTK_META_SURFACE_MESH or VTK_META_VOLUME_MESH.     
  */
  virtual unsigned int GetNumberOfTypedMetaDataSet (unsigned int type) const;
  /**
     Method to update the current time of all sequences referenced in the manager.
     NB : can be accessed directly from the vtkMetaDataSetSequence.
  */
  virtual void UpdateSequencesToTime (double);
  /**
     Method to update the current time of all sequences referenced in the manager.
     NB : can be accessed directly from the vtkMetaDataSetSequence.
  */
  virtual void UpdateSequencesToIndex (unsigned int);
  /**
     Method to update the current time of all sequences referenced in the manager.
     NB : can be accessed directly from the vtkMetaDataSetSequence.
  */
  virtual void UpdateSequencesMatchingTagToTime (const char* tag, double time);
  /**
     Get the minimum time flag from all sequences referenced in the manager.
  */
  virtual double GetSequencesRangeMin();
  /**
     Get the maximum time flag from all sequences referenced in the manager.
  */
  virtual double GetSequencesRangeMax();
  /**
     Get the maximum number of metadatasets from all sequences referenced in the manager.
  */
  virtual unsigned int GetSequencesMaxNumber();
  
  //BTX
  /**
     access to the list of metadatasets referenced in the manager.
     Use this method carefully.
  */
  std::vector<vtkMetaDataSet*> GetMetaDataSetList() const
  { return MetaDataSetList; }
  /**
     access to the list of metadatasets of a specific type referenced in the manager.
     Use this method carefully.
     The type can be : VTK_META_IMAGE_DATA, VTK_META_SURFACE_MESH or VTK_META_VOLUME_MESH.
  */
  std::vector<vtkMetaDataSet*> GetTypedMetaDataSetList (unsigned int type) const;
  /**
     access to the list of metadatasets of a specific tag referenced in the manager.
     Use this method carefully.
     Tag argument can be set to each metadataset by SetTag().
  */
  std::vector<vtkMetaDataSet*> GetMetaDataSetListFromTag (const char* tag) const;
  /**
     access to the list of sequences of a specific tag referenced in the manager.
     Use this method carefully.
     Tag argument can be set to each metadataset by SetTag().
  */
  std::vector<vtkMetaDataSetSequence*> GetSequenceListFromTag (const char* tag) const;
  //ETX
  
  /**
     returns a string of characters corresponding to the default name
     to be used when adding a metadataset. Use this method to avoid two
     metadatasets with the same name.
  */
  virtual const char* CreateDefaultName (unsigned int type, const char* filename = nullptr);

  /**
     Duplicate a metadataset from a metadataset,
     this is a static method returning a new object, to be deleted outside the method
  */
  static vtkMetaDataSet* DuplicateMetaDataSet(vtkMetaDataSet* input);
  
  //BTX
  /**
     enum for the events that can appear in one of the
     referenced metadatasets in the manager.
  */
  enum EventIds
  {
    MetaDataSetPickEvent=(vtkCommand::UserEvent+1),
    MetaDataChangedPickEvent
  };
  //ETX

 protected:
  vtkDataManager();
  ~vtkDataManager();
  
 private:
  
  vtkDataManager(const vtkDataManager&);      // Not implemented.
  void operator=(const vtkDataManager&);      // Not implemented.
  
  //BTX
  std::vector<vtkMetaDataSet*> MetaDataSetList;
  //ETX
};

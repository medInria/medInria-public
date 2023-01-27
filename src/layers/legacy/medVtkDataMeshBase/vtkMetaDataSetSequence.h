#pragma once
/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2020. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include "medVtkDataMeshBaseExport.h"

#include <vtkMetaDataSet.h>

#include <string>
#include <vector>

/**
   \class vtkMetaDataSetSequence vtkMetaDataSetSequence.h "vtkMetaDataSetSequence.h"
   \brief usefull class to handle a sequence (series) of vtkMetaDataSet
   \author Nicolas Toussaint
   
   This class is a powerfull vtk Addon class that helps handling a series of vtkDataSets.
   
   vtkMetaDataSet has a flag of time, that allows this class to handle a sequence
   of different vtkMetaDataSet (of the same type), the output dataset (got from GetDataSet())
   can be updated to a specific time with UpdateToTime().

   It does not compute any time interpolation. 
   
   \see
   vtkMetaSurfaceMesh vtkMetaVolumeMesh
   vtkMetaDataSet vtkDataManager
*/


class vtkDoubleArray;

class MEDVTKDATAMESHBASE_EXPORT vtkMetaDataSetSequence: public vtkMetaDataSet
{
 public:

  static vtkMetaDataSetSequence* New();
  vtkTypeMacro(vtkMetaDataSetSequence,vtkMetaDataSet);
  void PrintSelf(ostream& os, vtkIndent indent) override;

  virtual vtkMetaDataSetSequence* Clone() override;

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
  void SetSequenceDuration(double s)
  {
    this->SequenceDuration = s;
    this->ComputeTimesFromDuration();
  }
  /**
     Access to some variables : SequenceDuration
  */
  vtkGetMacro (SequenceDuration, double)

  const char* GetDataSetType() const override
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
  void ColorByArray(vtkDataArray* array) override;
  /**
     Get/Set the scalar visibility, see parent class
  */
  void SetScalarVisibility(bool val) override;

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

  /**
     Generate a array of size [number of frames] containing the value of an attribute
     (given with its name) at a certain position (given by the id of the point/cell to follow). 
     Return object is null when the attribute is not found in the frames, or when the point/cell id
     is not valid. 
     If succeeded this methods creates a new instance of vtkDoubleArray. Please make sure you
     delete the object after using it.
  */
  vtkDoubleArray* GenerateFollowerTimeTable(const char* arrayname, unsigned int idtofollow);
  /**
     Generate a array of size [number of frames] containing the value of an attribute
     contained in the MetaDataDictionary of the frames. The metadata is found by its key
     and has to be in double format. 
     Return object is null when the metadata is not found in the frames, or when
     the metadata is not a double expression. 
     If succeeded this methods creates a new instance of vtkDoubleArray. Please make sure you
     delete the object after using it.
  */
  vtkDoubleArray* GenerateMetaDataTimeTable(const char* metadatakey);

  vtkGetMacro(ParseAttributes, bool)
  vtkSetMacro(ParseAttributes, bool)

  /**
     Copy some informations from a given metadataset
     It corresponds basically to all metadataset characteristics unless the vtkDataSet
     name, time, metadata dictionary...
  */
  void CopyInformation (vtkMetaDataSet* metadataset) override;

  virtual void GetCurrentScalarRange(double range[2]);

  void GetScalarRange(double range[2], QString attributeName = QString("")) override;

  vtkGetMacro (CurrentId, int);
  
protected:
  vtkMetaDataSetSequence();
  vtkMetaDataSetSequence(const vtkMetaDataSetSequence& other);
  ~vtkMetaDataSetSequence();

  /**
     Internal use : Initialize sequence, not used yet.
  */
  void Initialize() override;
  /**
     Internal use : Build the output from a given vtkMetaDataSet.
  */
  virtual void   BuildMetaDataSetFromMetaDataSet (vtkMetaDataSet* metadataset);
  
  
  //BTX
  std::vector<vtkMetaDataSet*> MetaDataSetList;
  //ETX

 private:
  void operator=(const vtkMetaDataSetSequence&);              // Not implemented.

  int    CurrentId;
  double SequenceDuration;
  bool   SameGeometryFlag;
  bool  ParseAttributes;
};

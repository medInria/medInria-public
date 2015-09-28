/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <vtkMultiBlockDataSet.h>
#include <medVtkInriaExport.h>
#include <map>

class vtkPolyData;

/**
   This class carries a set of fibers as vtkPolyData and relative bundles
   as vtkPolyData as well.
 */

class MEDVTKINRIA_EXPORT vtkFiberDataSet : public vtkMultiBlockDataSet
{
 public:
  static vtkFiberDataSet *New();
  vtkTypeMacro(vtkFiberDataSet, vtkMultiBlockDataSet);

  struct vtkFiberBundleType
  {
    int          Id;
    vtkPolyData *Bundle;
    double       Red;
    double       Green;
    double       Blue;
    vtkFiberBundleType(): Id (-1), Bundle (0), Red (0.0), Green (0.0), Blue (0.0) {}
  };
  
  
  /**
     Store bundle as a map of string (name) whose value is BundleType struct.
   */
  typedef std::map< std::string, vtkFiberBundleType > vtkFiberBundleListType;

  void         SetFibers (vtkPolyData *fibers);
  vtkPolyData *GetFibers();
  
  void ChangeBundleName(const std::string &oldName, const std::string &newName);
  void AddBundle    (const std::string &name, vtkPolyData *bundle, double color[3]);
  void RemoveBundle (const std::string &name);

  /**
     Remove all bundles.
   */
  void Clear();

  int GetNumberOfBundles() const;
  
  vtkFiberBundleType     GetBundle (const std::string &name);
  vtkFiberBundleListType GetBundleList() const;
    
  void SetBundleColor (const std::string &name, double color[3]);
  void GetBundleColor (const std::string &name, double color[3]);

 protected:
  vtkFiberDataSet();
  ~vtkFiberDataSet();
  
 private:
  vtkFiberBundleListType Bundles;
  
  
};


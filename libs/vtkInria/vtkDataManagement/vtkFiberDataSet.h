/*=========================================================================

Program:   vtkINRIA3D
Module:    $Id: vtkFibersManager.h 1467 2010-02-17 23:31:08Z filus $
Language:  C++
Author:    $Author: filus $
Date:      $Date: 2010-02-18 00:31:08 +0100 (Thu, 18 Feb 2010) $
Version:   $Revision: 1467 $

Copyright (c) 2007 INRIA - Asclepios Project. All rights reserved.
See Copyright.txt for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef _vtk_FiberDataSet_
#define _vtk_FiberDataSet_

#include <vtkMultiBlockDataSet.h>
#include "vtkDataManagementExport.h"
#include <map>

class vtkPolyData;

/**
   This class carries a set of fibers as vtkPolyData and relative bundles
   as vtkPolyData as well.
 */

class VTK_DATAMANAGEMENT_EXPORT vtkFiberDataSet : public vtkMultiBlockDataSet
{
 public:
  static vtkFiberDataSet *New();
  vtkTypeRevisionMacro(vtkFiberDataSet, vtkMultiBlockDataSet);

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
  vtkPolyData *GetFibers (void);
  
  void AddBundle    (const std::string &name, vtkPolyData *bundle, double color[3]);
  void RemoveBundle (const std::string &name);

  /**
     Remove all bundles.
   */
  void Clear (void);

  int GetNumberOfBundles (void) const;
  
  vtkFiberBundleType     GetBundle (const std::string &name);
  vtkFiberBundleListType GetBundleList (void) const;
    
  void SetBundleColor (const std::string &name, double color[3]);
  void GetBundleColor (const std::string &name, double color[3]);

 protected:
  vtkFiberDataSet();
  ~vtkFiberDataSet();
  
 private:
  vtkFiberBundleListType Bundles;
  
  
};


#endif

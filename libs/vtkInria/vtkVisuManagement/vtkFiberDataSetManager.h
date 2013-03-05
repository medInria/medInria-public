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
#pragma once

#include "vtkVisuManagementExport.h"

#include "vtkFibersManager.h"


class vtkFiberDataSet;
class vtkActor;
class vtkRenderWindowInteractor;

class vtkFiberDataSetManagerPrivate;

class VTK_VISUMANAGEMENT_EXPORT vtkFiberDataSetManager : public vtkFibersManager
{
 public:
  static vtkFiberDataSetManager *New();
  vtkTypeRevisionMacro(vtkFiberDataSetManager, vtkFibersManager);

  virtual void SetInput (vtkFiberDataSet *input);
  vtkGetObjectMacro (FiberDataSet, vtkFiberDataSet);
 
  // derived from vtkFibersManager
  virtual void Enable();
  virtual void Disable();
  
  // virtual void SetRenderWindowInteractor (vtkRenderWindowInteractor *);
  // virtual void RemoveAllActors();
  virtual void SetRenderingModeToPolyLines();
  virtual void SetRenderingModeToTubes();
  virtual void SetRenderingModeToRibbons();
  virtual void ChangeMapperToUseHardwareShaders();
  virtual void ChangeMapperToDefault();

  //BTX
  virtual void SetBundleVisibility(const std::string &name, int visibility);
  virtual int  GetBundleVisibility(const std::string &name);
  virtual vtkActor *GetBundleActor(const std::string &name);
  //ETX
  
  virtual void ShowAllBundles();
  virtual void HideAllBundles();

  /*
    virtual void SetColorModeToLocalFiberOrientation();
    virtual void SetColorModelToGlobalFiberOrientation();
    virtual void SetColorModeToPointArray (const int& id);
  */

  // specific
  //BTX
  void Validate (const std::string &name, double color[3]);
  //ETX
  
 protected:
  vtkFiberDataSetManager();
  ~vtkFiberDataSetManager();

  void CreateBundleRenderingPipeline();
  //BTX
  inline void CreateRenderingPipelineForBundle (const std::string &name);
  //ETX
  
  void AddBundleActors();
  void RemoveBundleActors();
  
 private:

  vtkFiberDataSet               *FiberDataSet;
  vtkFiberDataSetManagerPrivate *d;
};



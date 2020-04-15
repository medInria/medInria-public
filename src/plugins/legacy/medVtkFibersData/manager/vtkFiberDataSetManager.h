/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2018. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medVtkFibersDataPluginExport.h>

#include "vtkFibersManager.h"


class vtkFiberDataSet;
class vtkActor;
class vtkRenderWindowInteractor;

class vtkFiberDataSetManagerPrivate;

class MEDVTKFIBERSDATAPLUGIN_EXPORT vtkFiberDataSetManager : public vtkFibersManager
{
 public:
  static vtkFiberDataSetManager *New();
  vtkTypeMacro(vtkFiberDataSetManager, vtkFibersManager);

    using vtkFibersManager::SetInput;
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
    void ChangeBundleName(const std::string &oldName, const std::string &newName);
    void RemoveBundle(const std::string &name);
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



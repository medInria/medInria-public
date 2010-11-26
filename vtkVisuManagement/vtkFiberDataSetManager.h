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
#ifndef _vtk_FiberDataSetManager_h_
#define _vtk_FiberDataSetManager_h_

#include "vtkINRIA3DConfigure.h"

#include "vtkFibersManager.h"

#include <map>

class vtkFiberDataSet;
class vtkActor;

class VTK_VISUMANAGEMENT_EXPORT vtkFiberDataSetManager : public vtkFibersManager
{
 public:
  static vtkFiberDataSetManager *New();
  vtkTypeRevisionMacro(vtkFiberDataSetManager, vtkFibersManager);

  typedef std::map<std::string, vtkActor*> vtkFiberBundleActorListType;
  
  virtual void SetInput (vtkFiberDataSet *input);
  vtkGetObjectMacro (FiberDataSet, vtkFiberDataSet);


  // derived from vtkFibersManager
  virtual void RemoveAllActors (void);
  virtual void SetRenderingModeToPolyLines (void);
  virtual void SetRenderingModeToTubes (void);
  virtual void SetRenderingModeToRibbons (void);
  virtual void ChangeMapperToUseHardwareShaders(void);
  virtual void ChangeMapperToDefault(void);
  /*
    virtual void SetColorModeToLocalFiberOrientation (void);
    virtual void SetColorModelToGlobalFiberOrientation (void);
    virtual void SetColorModeToPointArray (const int& id);
  */

  // specific

 protected:
  vtkFiberDataSetManager();
  ~vtkFiberDataSetManager();

  void CreateBundleRenderingPipeline (void);

 private:

  vtkFiberDataSet            *FiberDataSet;
  vtkFiberBundleActorListType FiberBundleActorList;
  
};

#endif

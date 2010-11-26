/*=========================================================================

Program:   vtkINRIA3D
Module:    $Id: vtkLookupTableManager.h 1198 2009-07-21 14:22:52Z filus $
Language:  C++
Author:    $Author: filus $
Date:      $Date: 2009-07-21 16:22:52 +0200 (Tue, 21 Jul 2009) $
Version:   $Revision: 1198 $

Copyright (c) 2007 INRIA - Asclepios Project. All rights reserved.
See Copyright.txt for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef _vtk_LookupTableManager_h_
#define _vtk_LookupTableManager_h_

#include "vtkINRIA3DConfigure.h"

#include <vector>

#include <vtkObject.h>
#include <vtkLookupTable.h>

class VTK_VISUMANAGEMENT_EXPORT vtkLookupTableManager: public vtkObject
{

 public:

  //BTX
  enum LookupTableIds
  {
    LUT_BW = 0,
    LUT_BWINV,
    LUT_SPECTTUM,
    LUT_HOTMETAL,
    LUT_HOTGREEN,
    LUT_HOTIRON,
    LUT_GECOLORS,
    LUT_FLOW,
    LUT_LONI,
    LUT_LONI2,
    LUT_ASYMETRY,
    LUT_PVALUE,
    LUT_ROI,
    LUT_VRMUSCLESBONES,
    LUT_VRBONES,
    LUT_VRREDVESSELS,
    LUT_BLACKBODY,
    LUT_CARDIAC,
    LUT_GRAYRAINBOW,
    LUT_STERN
  };
  //ETX

  static vtkLookupTableManager* New();
  vtkTypeRevisionMacro (vtkLookupTableManager, vtkObject);

  static vtkLookupTable* GetBWLookupTable (void);

  static vtkLookupTable* GetBWInverseLookupTable (void);

  static vtkLookupTable* GetSpectrumLookupTable (void);

  static vtkLookupTable* GetHotMetalLookupTable (void);

  static vtkLookupTable* GetGEColorLookupTable (void);

  static vtkLookupTable* GetFlowLookupTable (void);

  static vtkLookupTable* GetLONILookupTable (void);

  static vtkLookupTable* GetLONI2LookupTable (void);

  static vtkLookupTable* GetAsymmetryLookupTable (void);

  static vtkLookupTable* GetPValueLookupTable (void);

  static vtkLookupTable* GetRedBlackAlphaLookupTable(void);

  static vtkLookupTable* GetGreenBlackAlphaLookupTable(void);

  static vtkLookupTable* GetBlueBlackAlphaLookupTable(void);

  static vtkLookupTable* GetROILookupTable (void);

  static vtkLookupTable* GetVRMusclesBonesLookupTable (void);

  static vtkLookupTable* GetCardiacLookupTable (void);

  static vtkLookupTable* GetGrayRainbowLookupTable (void);
  
  static vtkLookupTable* GetHotGreenLookupTable (void);
  
  static vtkLookupTable* GetHotIronLookupTable (void);
  
  static vtkLookupTable* GetSternLookupTable (void);
  
  static vtkLookupTable* GetVRBonesLookupTable (void);
  
  static vtkLookupTable* GetVRRedVesselsLookupTable (void);
  
  static vtkLookupTable* GetBlackBodyLookupTable (void);
  

  static std::vector<std::string> GetAvailableLookupTables(void);
    
  static vtkLookupTable* GetLookupTable(const int &);
  static vtkLookupTable* GetLookupTable(const std::string &);

 protected:
  vtkLookupTableManager();
  ~vtkLookupTableManager();



 private:


};



#endif

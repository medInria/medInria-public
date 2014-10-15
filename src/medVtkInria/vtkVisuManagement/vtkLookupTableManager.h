/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medVtkInriaExport.h>

#include <vector>

#include <vtkObject.h>
#include <vtkLookupTable.h>

class MEDVTKINRIA_EXPORT vtkLookupTableManager: public vtkObject
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
    LUT_STERN,
    LUT_JET
  };
  //ETX

  static vtkLookupTableManager* New();
  vtkTypeRevisionMacro (vtkLookupTableManager, vtkObject);

  static vtkLookupTable* GetBWLookupTable();

  static vtkLookupTable* GetBWInverseLookupTable();

  static vtkLookupTable* GetSpectrumLookupTable();

  static vtkLookupTable* GetHotMetalLookupTable();

  static vtkLookupTable* GetGEColorLookupTable();

  static vtkLookupTable* GetFlowLookupTable();

  static vtkLookupTable* GetLONILookupTable();

  static vtkLookupTable *GetLONIInversedLookupTable();

  static vtkLookupTable* GetLONI2LookupTable();

  static vtkLookupTable* GetAsymmetryLookupTable();

  static vtkLookupTable* GetPValueLookupTable();

  static vtkLookupTable* GetRedBlackAlphaLookupTable();

  static vtkLookupTable* GetGreenBlackAlphaLookupTable();

  static vtkLookupTable* GetBlueBlackAlphaLookupTable();

  static vtkLookupTable* GetROILookupTable();

  static vtkLookupTable* GetVRMusclesBonesLookupTable();

  static vtkLookupTable* GetCardiacLookupTable();

  static vtkLookupTable* GetGrayRainbowLookupTable();
  
  static vtkLookupTable* GetHotGreenLookupTable();
  
  static vtkLookupTable* GetHotIronLookupTable();
  
  static vtkLookupTable* GetSternLookupTable();
  
  static vtkLookupTable* GetVRBonesLookupTable();
  
  static vtkLookupTable* GetVRRedVesselsLookupTable();
  
  static vtkLookupTable* GetBlackBodyLookupTable();
  
  static vtkLookupTable* GetJetLookupTable();
  
  //BTX
  static std::vector<std::string> GetAvailableLookupTables();
  //ETX
  
  static vtkLookupTable* GetLookupTable(const int &);
  //BTX
  static vtkLookupTable* GetLookupTable(const std::string &);
  //ETX
protected:
  vtkLookupTableManager();
  ~vtkLookupTableManager();



 private:


};





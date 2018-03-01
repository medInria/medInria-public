/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2018. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medVtkInriaExport.h>

#include <vector>

#include <vtkObject.h>

class vtkColorTransferFunction;
class vtkPiecewiseFunction;

class MEDVTKINRIA_EXPORT vtkTransferFunctionPresets: public vtkObject
{

 public:

  static vtkTransferFunctionPresets* New();
  //BTX
  static std::vector<std::string> GetAvailablePresets();
  static void GetTransferFunction( const std::string & name,
				   vtkColorTransferFunction * rgb,
				   vtkPiecewiseFunction * alpha );
  //ETX

protected:
  static void GetTransferFunctionFromTable( unsigned int size,
					    const int * table,
					    bool logAlpha,
					    vtkColorTransferFunction * rgb,
					    vtkPiecewiseFunction * alpha );
  static void GetTransferFunctionFromTable( unsigned int size,
					    const float table[][3],
					    bool logAlpha,
					    vtkColorTransferFunction * rgb,
					    vtkPiecewiseFunction * alpha );
  static void GetTransferFunctionFromInversedTable( unsigned int size,
                        const float table[][3],
                        bool logAlpha,
                        vtkColorTransferFunction * rgb,
                        vtkPiecewiseFunction * alpha );

  static void GetBWTransferFunction( vtkColorTransferFunction * rgb,
				     vtkPiecewiseFunction * alpha );
  static void GetBWInverseTransferFunction( vtkColorTransferFunction * rgb,
					    vtkPiecewiseFunction * alpha );
  static void GetSpectrumTransferFunction( vtkColorTransferFunction * rgb,
					   vtkPiecewiseFunction * alpha );
  static void GetHotMetalTransferFunction( vtkColorTransferFunction * rgb,
					   vtkPiecewiseFunction * alpha );
  static void GetHotGreenTransferFunction( vtkColorTransferFunction * rgb,
					   vtkPiecewiseFunction * alpha );
  static void GetHotIronTransferFunction( vtkColorTransferFunction * rgb,
					  vtkPiecewiseFunction * alpha );
  static void GetGEColorTransferFunction( vtkColorTransferFunction * rgb,
					  vtkPiecewiseFunction * alpha );
  static void GetFlowTransferFunction( vtkColorTransferFunction * rgb,
				       vtkPiecewiseFunction * alpha );
  static void GetLONITransferFunction( vtkColorTransferFunction * rgb,
				       vtkPiecewiseFunction * alpha );
  static void GetLONIInversedTransferFunction( vtkColorTransferFunction * rgb,
                       vtkPiecewiseFunction * alpha );
  static void GetLONI2TransferFunction( vtkColorTransferFunction * rgb,
					vtkPiecewiseFunction * alpha );
  static void GetAsymmetryTransferFunction( vtkColorTransferFunction * rgb,
					    vtkPiecewiseFunction * alpha );
  static void GetPValueTransferFunction( vtkColorTransferFunction * rgb,
					 vtkPiecewiseFunction * alpha );
  static void GetRedBlackAlphaTransferFunction( vtkColorTransferFunction * rgb,
						vtkPiecewiseFunction * alpha );
  static void GetGreenBlackAlphaTransferFunction(
    vtkColorTransferFunction * rgb, vtkPiecewiseFunction * alpha );
  static void GetBlueBlackAlphaTransferFunction( vtkColorTransferFunction * rgb,
						 vtkPiecewiseFunction * alpha );
  // static void GetROITransferFunction( vtkColorTransferFunction * rgb,
  // 				      vtkPiecewiseFunction * alpha );
  static void GetVRMusclesBonesTransferFunction( vtkColorTransferFunction * rgb,
						 vtkPiecewiseFunction * alpha );
  static void GetVRBonesTransferFunction( vtkColorTransferFunction * rgb,
					  vtkPiecewiseFunction * alpha );
  static void GetVRRedVesselsTransferFunction( vtkColorTransferFunction * rgb,
					       vtkPiecewiseFunction * alpha );
  static void GetCardiacTransferFunction( vtkColorTransferFunction * rgb,
					  vtkPiecewiseFunction * alpha );
  static void GetGrayRainbowTransferFunction( vtkColorTransferFunction * rgb,
					      vtkPiecewiseFunction * alpha );
  static void GetSternTransferFunction( vtkColorTransferFunction * rgb,
					vtkPiecewiseFunction * alpha );
  static void GetBlackBodyTransferFunction( vtkColorTransferFunction * rgb,
					    vtkPiecewiseFunction * alpha );
  static void GetBinaryMapTransferFunction( vtkColorTransferFunction * rgb,
                                            vtkPiecewiseFunction * alpha );

  vtkTransferFunctionPresets();
  ~vtkTransferFunctionPresets();

 private:
};





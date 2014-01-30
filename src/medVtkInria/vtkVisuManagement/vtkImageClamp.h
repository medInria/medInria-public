/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medVtkInriaExport.h>

#include <vtkThreadedImageAlgorithm.h>

/**
   \class vtkImageClamp vtkImageClamp.h "vtkImageClamp.h"
   \brief This class is a convenient image process that \"clamps\" an image between two extrem values

   This algorithm takes a scalar image as input and outputs an image, which scalar range
   matches user defined extrem values by calling SetClampMinimumValue() and SetClampMaximumValue()

   Use SetClampRatio() to define a voxel percentage to remove at the beginning and the end
   of the scalar histogram. Default value is 1\%. The value has to be between 0.0 and 1.0.
   The method ClampOverFlowOn() makes sure that outside values will be set to min- (or max-) value,
   
   Use SetOutputScalarTypeTo[...]() to choose the output type, Default.

   \author: Nicolas Toussaint, INRIA

   \see
   vtkImageFuse vtkImageAlgorithm
*/



class MEDVTKINRIA_EXPORT vtkImageClamp : public vtkThreadedImageAlgorithm
{
public:
  static vtkImageClamp *New();
  vtkTypeRevisionMacro(vtkImageClamp,vtkThreadedImageAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent);


  // Description:
  // Set the desired output scalar type. The result of the shift 
  // and scale operations is cast to the type specified.
  vtkSetMacro(OutputScalarType, int);
  vtkGetMacro(OutputScalarType, int);
  void SetOutputScalarTypeToDouble()
    {this->SetOutputScalarType(VTK_DOUBLE);}
  void SetOutputScalarTypeToFloat()
    {this->SetOutputScalarType(VTK_FLOAT);}
  void SetOutputScalarTypeToLong()
    {this->SetOutputScalarType(VTK_LONG);}
  void SetOutputScalarTypeToUnsignedLong()
    {this->SetOutputScalarType(VTK_UNSIGNED_LONG);};
  void SetOutputScalarTypeToInt()
    {this->SetOutputScalarType(VTK_INT);}
  void SetOutputScalarTypeToUnsignedInt()
    {this->SetOutputScalarType(VTK_UNSIGNED_INT);}
  void SetOutputScalarTypeToShort()
    {this->SetOutputScalarType(VTK_SHORT);}
  void SetOutputScalarTypeToUnsignedShort()
    {this->SetOutputScalarType(VTK_UNSIGNED_SHORT);}
  void SetOutputScalarTypeToChar()
    {this->SetOutputScalarType(VTK_CHAR);}
  void SetOutputScalarTypeToUnsignedChar()
    {this->SetOutputScalarType(VTK_UNSIGNED_CHAR);}

  // Description:
  // When the ClampOverflow flag is on, the data is thresholded so that
  // the output value does not exceed the max or min of the data type.
  // By default, ClampOverflow is off.
  vtkSetMacro(ClampOverflow, int);
  vtkGetMacro(ClampOverflow, int);
  vtkBooleanMacro(ClampOverflow, int);

  vtkSetMacro(ClampMinimumValue, double);
  vtkGetMacro(ClampMinimumValue, double);
  vtkSetMacro(ClampMaximumValue, double);
  vtkGetMacro(ClampMaximumValue, double);
  vtkSetMacro(ClampRatio, double);
  vtkGetMacro(ClampRatio, double);

  double* GetWindow()
  { return this->Window; }
  
  
protected:
  vtkImageClamp();
  ~vtkImageClamp();

  double ClampRatio;
  double ClampMinimumValue;
  double ClampMaximumValue;  
  int OutputScalarType;
  int ClampOverflow;

  double Window[2];
  
  virtual int RequestInformation(vtkInformation*,
                                 vtkInformationVector** inputVector,
                                 vtkInformationVector* outputVector);

  virtual void ThreadedRequestData(vtkInformation*,
                                   vtkInformationVector**,
                                   vtkInformationVector*,
                                   vtkImageData*** inData,
                                   vtkImageData** outData,
                                   int outExt[6],
                                   int threadId);
private:
  vtkImageClamp(const vtkImageClamp&);  // Not implemented.
  void operator=(const vtkImageClamp&);  // Not implemented.
};



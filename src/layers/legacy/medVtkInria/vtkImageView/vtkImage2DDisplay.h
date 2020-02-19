#pragma once
/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2020. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medVtkImageInfo.h>

#include <vtkColorTransferFunction.h>
#include <vtkImageActor.h>
#include <vtkImageData.h>
#include <vtkImageMapToColors.h>
#include <vtkLookupTable.h>
#include <vtkPiecewiseFunction.h>

class vtkImage2DDisplay : public vtkObject
{
public:
  static vtkImage2DDisplay * New();
  vtkTypeMacro (vtkImage2DDisplay, vtkObject);

  virtual void SetInputData(vtkImageData *pi_poVtkImage);

  void SetInputProducer(vtkAlgorithmOutput *pi_poAlgorithmOutput);

  virtual vtkLookupTable * GetLookupTable() const;

  virtual vtkImageActor* GetImageActor() { return this->ImageActor; }

  virtual vtkImageMapToColors* GetWindowLevel() const { return this->WindowLevel; }

  virtual vtkImageAlgorithm* GetInputProducer() const { return this->InputProducer; }

  vtkSetMacro(ColorWindow, double);
  vtkGetMacro(ColorWindow,double);
  vtkSetMacro(ColorLevel, double);
  vtkGetMacro(ColorLevel,double);

  virtual vtkColorTransferFunction* GetColorTransferFunction() { return this->ColorTransferFunction; }
  void SetColorTransferFunction ( vtkColorTransferFunction* function);
  void SetOpacityTransferFunction(vtkPiecewiseFunction* opacity);
  virtual vtkPiecewiseFunction* GetOpacityTransferFunction() { return this->OpacityTransferFunction; }
  virtual medVtkImageInfo* GetMedVtkImageInfo();

  virtual bool isInputSet();

  vtkGetMacro(UseLookupTable,bool);
  vtkSetMacro(UseLookupTable, bool);

protected:
  vtkImage2DDisplay();
  ~vtkImage2DDisplay() = default;

private:
  vtkSmartPointer<vtkImageMapToColors>        WindowLevel;
  vtkSmartPointer<vtkImageAlgorithm>          InputProducer;
  vtkSmartPointer<vtkImageActor>              ImageActor;
  double                                      ColorWindow;
  double                                      ColorLevel;
  bool                                        UseLookupTable;
  vtkSmartPointer<vtkColorTransferFunction >  ColorTransferFunction;
  vtkSmartPointer<vtkPiecewiseFunction>       OpacityTransferFunction;
  medVtkImageInfo                             m_sVtkImageInfo;


  vtkImage2DDisplay (const vtkImage2DDisplay&);
  void operator=(const vtkImage2DDisplay&);
};

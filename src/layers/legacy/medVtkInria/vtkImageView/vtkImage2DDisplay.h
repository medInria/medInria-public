#pragma once
/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2020. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <vtkImageData.h>
#include <vtkImageActor.h>
#include <vtkImageMapToColors.h>
#include <vtkLookupTable.h>
#include <vtkColorTransferFunction.h>
#include <vtkPiecewiseFunction.h>
#include <vtkSetGet.h>
#include "medVtkImageInfo.h"


class vtkImage2DDisplay : public vtkObject
{
public:
  static vtkImage2DDisplay * New();
  vtkTypeMacro (vtkImage2DDisplay, vtkObject);

  //virtual void SetInput(vtkAlgorithmOutput*  pi_poVtkAlgoPort);

  virtual void SetInput(vtkImageData *pi_poVtkImage);

  void SetInputProducer(vtkImageAlgorithm *inputImageAlgorithm);

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
  ~vtkImage2DDisplay();

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

#pragma once
#include <medVtkFibersDataPluginExport.h>

#include <vtkCommand.h>
#include <vtkPolyData.h>
#include <vtkAlgorithmOutput.h>

#include <vtkLimitFibersToVOI.h>
#include <vtkLimitFibersToROI.h>

/**
   vtkFibersManagerCallback declaration & implementation
 */
class MEDVTKFIBERSDATAPLUGIN_EXPORT vtkFibersManagerCallback: public vtkCommand
{

 public:
  static vtkFibersManagerCallback* New()
  { return new vtkFibersManagerCallback; }

  virtual void Execute ( vtkObject *caller, unsigned long, void* );

  vtkPolyData* GetOutput() const
  { return this->FiberLimiter->GetOutput(); }

  vtkAlgorithmOutput* GetOutputPort() const
  {
      return this->FiberLimiter->GetOutputPort();
  }

  vtkLimitFibersToVOI* GetFiberLimiter() const
  { return this->FiberLimiter; }
  
  vtkLimitFibersToROI* GetROIFiberLimiter() const
  { return this->ROIFiberLimiter; }
  
    
 protected:
  vtkFibersManagerCallback()
  {
    this->FiberLimiter    = vtkLimitFibersToVOI::New();
    this->ROIFiberLimiter = vtkLimitFibersToROI::New();
    this->FiberLimiter->SetInputConnection ( this->ROIFiberLimiter->GetOutputPort() );    
  }
  ~vtkFibersManagerCallback()
  {
    this->FiberLimiter->Delete();
    this->ROIFiberLimiter->Delete();
  }
  
 private:
  vtkLimitFibersToVOI* FiberLimiter;
  vtkLimitFibersToROI* ROIFiberLimiter;
};





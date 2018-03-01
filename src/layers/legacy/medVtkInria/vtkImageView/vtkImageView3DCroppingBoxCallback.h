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

#include <vtkCommand.h>
#include <vtkVolumeMapper.h>
#include <vtkSetGet.h>
#include <vtkObjectFactory.h>

class MEDVTKINRIA_EXPORT vtkImageView3DCroppingBoxCallback: public vtkCommand
{

 public:
  static vtkImageView3DCroppingBoxCallback* New()
  { return new vtkImageView3DCroppingBoxCallback; }

  virtual void Execute ( vtkObject *caller, unsigned long event, void*vtkNotUsed(callData) );

  void SetVolumeMapper (vtkVolumeMapper* mapper)
  {
    this->VolumeMapper = mapper;
  }
  vtkVolumeMapper* GetVolumeMapper() const
  {
    return this->VolumeMapper;
  }
  
  
 protected:
  vtkImageView3DCroppingBoxCallback()
  {
    this->VolumeMapper = 0;
  }
  ~vtkImageView3DCroppingBoxCallback(){};
  
 private:
  
  vtkVolumeMapper* VolumeMapper;
  
};




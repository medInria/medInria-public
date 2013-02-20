/*=========================================================================

Program:   vtkINRIA3D
Module:    $Id: vtkImageView3DCroppingBoxCallback.h 1080 2009-02-18 13:27:02Z acanale $
Language:  C++
Author:    $Author: acanale $
Date:      $Date: 2009-02-18 14:27:02 +0100 (mer, 18 fév 2009) $
Version:   $Revision: 1080 $

Copyright (c) 2007 INRIA - Asclepios Project. All rights reserved.
See Copyright.txt for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef _vtk_ImageView3DCroppingBoxCallback_h_
#define _vtk_ImageView3DCroppingBoxCallback_h_

#include "vtkImageViewExport.h"

#include <vtkCommand.h>
#include <vtkVolumeMapper.h>
#include <vtkSetGet.h>
#include <vtkObjectFactory.h>

class VTK_IMAGEVIEW_EXPORT vtkImageView3DCroppingBoxCallback: public vtkCommand
{

 public:
  static vtkImageView3DCroppingBoxCallback* New()
  { return new vtkImageView3DCroppingBoxCallback; }

  virtual void Execute ( vtkObject *caller, unsigned long event, void*vtkNotUsed(callData) );

  void SetVolumeMapper (vtkVolumeMapper* mapper)
  {
    this->VolumeMapper = mapper;
  }
  vtkVolumeMapper* GetVolumeMapper (void) const
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


#endif

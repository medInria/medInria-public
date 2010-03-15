/*=========================================================================

Program:   vtkINRIA3D
Module:    $Id: vtkImage3DImagePlaneCallback.h 1080 2009-02-18 13:27:02Z acanale $
Language:  C++
Author:    $Author: acanale $
Date:      $Date: 2009-02-18 14:27:02 +0100 (Wed, 18 Feb 2009) $
Version:   $Revision: 1080 $

Copyright (c) 2007 INRIA - Asclepios Project. All rights reserved.
See Copyright.txt for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
// version vtkRenderingAddOn
#ifndef _vtk_Image3DImagePlaneCallback_h_
#define _vtk_Image3DImagePlaneCallback_h_

#include "vtkINRIA3DConfigure.h"

#include <vtkCommand.h>
#include <vtkImageReslice.h>
#include <vtkMatrix4x4.h>


class vtkViewImage2D;


class VTK_RENDERINGADDON_EXPORT vtkImage3DImagePlaneCallback: public vtkCommand
{

 public:
  
  static vtkImage3DImagePlaneCallback* New()
  { return new vtkImage3DImagePlaneCallback; }

  virtual void Execute ( vtkObject *caller, unsigned long, void* );

  /*
  void SetViewImage2D (vtkViewImage2D* view)
  {
    this->ViewImage2D = view;
    this->FirstRender = true;
    }*/

  vtkImageData* GetOutput (void) const
  {
    return this->Reslice->GetOutput();
  }
  

  virtual void Reset (void)
  {
    this->Reslice->SetInput (NULL);
    //this->FirstRender = true;
  }

  vtkImageReslice* GetReslice()
  {
    return this->Reslice;
  }
  vtkMatrix4x4* GetMatrix()
  {
    return this->ResliceAxes;
  }
  
  

  
 protected:
 vtkImage3DImagePlaneCallback()
 {
   /*
   this->ViewImage2D = 0;
   this->FirstRender = true;
   */
   this->Reslice     = vtkImageReslice::New();
   this->ResliceAxes = vtkMatrix4x4::New();
 }
 ~vtkImage3DImagePlaneCallback()
 {
   this->Reslice->Delete();
   this->ResliceAxes->Delete();
 }

 private:
 /*
 vtkViewImage2D*   ViewImage2D;
 bool              FirstRender;*/
 
 vtkImageReslice*  Reslice;
 vtkMatrix4x4*     ResliceAxes;
 
};


#endif

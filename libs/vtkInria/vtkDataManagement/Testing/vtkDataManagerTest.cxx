/*=========================================================================

Program:   vtkINRIA3D
Module:    $Id: vtkDataManagerTest.cxx 880 2008-06-06 16:10:57Z filus $
Language:  C++
Author:    $Author: filus $
Date:      $Date: 2008-06-06 18:10:57 +0200 (Fri, 06 Jun 2008) $
Version:   $Revision: 880 $

Copyright (c) 2007 INRIA - Asclepios Project. All rights reserved.
See Copyright.txt for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include <vtkDataManager.h>
#include <vtkMetaImageData.h>
#include <vtkMetaSurfaceMesh.h>
#include <vtkMetaVolumeMesh.h>

#include <vtkImageData.h>
#include <vtkImageSource.h>
#include <vtkMetaVolumeMesh.h>
#include <vtkMetaDataSetSequence.h>


int vtkDataManagerTest(int argc, char* argv[])
{

    
  
  vtkDataManager::New();

  
  
  return 0;
}

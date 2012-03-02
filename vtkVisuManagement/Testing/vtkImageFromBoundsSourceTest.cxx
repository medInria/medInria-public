/*=========================================================================

 Program:   vtkINRIA3D
 Module:    $Id: SynchronizedViewsTest.cxx 1211 2009-07-29 14:57:22Z filus $
 Language:  C++
 Author:    $Author: filus $
 Date:      $Date: 2009-07-29 15:57:22 +0100 (Wed, 29 Jul 2009) $
 Version:   $Revision: 1211 $

 Copyright (c) 2007 INRIA - Asclepios Project. All rights reserved.
 See Copyright.txt for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notices for more information.

 =========================================================================*/
#include <vtkConeSource.h>
#include <vtkImageData.h>
#include <vtkMatrix4x4.h>
#include <vtkSmartPointer.h>

#include <vtkImageFromBoundsSource.h>

#include <cmath>
#include <cstdlib>
#include <iostream>

int vtkImageFromBoundsSourceTest(int argc, char *argv[])
{
    int status = EXIT_SUCCESS;
    vtkSmartPointer<vtkImageFromBoundsSource> imageFrombounds;
    imageFrombounds = vtkSmartPointer<vtkImageFromBoundsSource>::New();

    vtkSmartPointer<vtkConeSource> coneSource( vtkSmartPointer<vtkConeSource>::New());

    coneSource->SetHeight(30.);
    coneSource->SetRadius(13.);
    coneSource->SetResolution(20);
    coneSource->SetCenter(20,30,50);

    // One day it would be nice to be able to do this:
    // imageFrombounds->SetInput(coneSource->GetOutput());

    double coneBounds[6];
    coneSource->Update();
    coneSource->GetOutput()->GetBounds(coneBounds);

    imageFrombounds->SetOutputImageBounds(coneBounds);

    vtkSmartPointer<vtkImageData> imageOp = imageFrombounds->GetOutput();
    imageOp->Update();

    double imageBounds[6];
    imageFrombounds->GetOutput()->GetBounds(imageBounds);

    const double tolerance = 0.5;
    for ( int i(0); i<6; ++i ) {
        if ( std::fabs( coneBounds[i] - imageBounds[i] ) > tolerance ) {
            std::cerr << "Bounds do not match " << i << " " 
                << coneBounds[i] << " , " << imageBounds[i] << std::endl;
            status = EXIT_FAILURE;
        }
    }
    return status;
}

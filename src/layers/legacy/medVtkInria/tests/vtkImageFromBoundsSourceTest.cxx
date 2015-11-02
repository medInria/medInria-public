/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

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

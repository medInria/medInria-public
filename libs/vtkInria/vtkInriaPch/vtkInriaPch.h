/* vtkPch.h ---
 *
 * Author: John Stark
 * Copyright (C) 2011 - John Stark, Inria.
 * Created: August 2011
 * Version: $Id$
 * Last-Updated:
 *           By: John Stark
 *     Update #: 1
 */

/* Commentary:
 * Precompiled header file
 */

/* Change log:
 *
 */


#ifndef VTKPCH_H
#define VTKPCH_H

//VTK
#include <vtkActor.h>
#include <vtkActor2D.h>
#include <vtkCamera.h>
#include <vtkCommand.h>
#include <vtkgl.h>
#include <vtkImageActor.h>
#include <vtkImageData.h>
#include <vtkMath.h>
#include <vtkMatrix4x4.h>
#include <vtkObject.h>
#include <vtkPolyDataMapper2D.h>
#include <vtkRenderWindow.h>
#include <vtkSmartPointer.h>
#include <vtkTransform.h>

#ifdef vtkINRIA3D_USE_ITK
//ITK
#include <itkConstNeighborhoodIterator.h>
#include <itkImageRegionConstIterator.h>
#include <itkImageRegionIterator.h>
#include <itkImageToImageFilter.h>
#include <itkObjectFactory.h>
#endif // vtkINRIA3D_USE_ITK

//STL / C++ bindings for C
#include <cmath>
#include <fstream>
#include <iostream>
#include <list>
#include <sstream>
#include <string>
#include <vector>

#endif //VTKPCH_H

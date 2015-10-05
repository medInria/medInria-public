/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

namespace medImageView
{
//TODO: find a better name than orientation - RDE.
enum Orientation
{
    VIEW_ORIENTATION_AXIAL,
    VIEW_ORIENTATION_SAGITTAL,
    VIEW_ORIENTATION_CORONAL,
    VIEW_ORIENTATION_3D,
    VIEW_ALL_ORIENTATION
};
}

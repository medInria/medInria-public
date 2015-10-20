/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include "medDiffusionAbstractToolBox.h"

medDiffusionAbstractToolBox::medDiffusionAbstractToolBox(QWidget *parent) : medToolBox(parent)
{
}

medDiffusionAbstractToolBox::~medDiffusionAbstractToolBox()
{
}

medDiffusionScalarMapsAbstractToolBox::medDiffusionScalarMapsAbstractToolBox(QWidget *parent) : medDiffusionAbstractToolBox(parent)
{
}

medDiffusionScalarMapsAbstractToolBox::~medDiffusionScalarMapsAbstractToolBox()
{
}

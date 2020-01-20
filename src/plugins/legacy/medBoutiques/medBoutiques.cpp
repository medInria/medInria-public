/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2018. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medBoutiques.h>

#include <dtkCoreSupport/dtkAbstractProcessFactory.h>
#include <dtkCoreSupport/dtkAbstractDataFactory.h>

#include <itkImage.h>
#include <itkDiscreteGaussianImageFilter.h>
#include <itkSmoothingRecursiveGaussianImageFilter.h>

// /////////////////////////////////////////////////////////////////
// medBoutiques
// /////////////////////////////////////////////////////////////////

bool medBoutiques::registered()
{
    return dtkAbstractProcessFactory::instance()->registerProcessType("medBoutiques", createmedBoutiques);
}

QString medBoutiques::description() const
{
    return "medBoutiques";
}

dtkAbstractProcess * createmedBoutiques()
{
    return new medBoutiques;
}

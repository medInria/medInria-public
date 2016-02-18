/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medAbstractProcess.h>
#include <dtkCore/dtkAbstractObject.h>

medAbstractProcess::medAbstractProcess(medAbstractProcess * parent) : dtkAbstractProcess(*new medAbstractProcessPrivate(this), parent)
{
}

medAbstractProcess::medAbstractProcess(const medAbstractProcess& other) : dtkAbstractProcess(*new medAbstractProcessPrivate(*other.d_func()), other)
{
    
}

medAbstractProcess::~medAbstractProcess()
{
}


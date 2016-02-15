/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medAbstractShrinkFilterProcess.h>

#include <medAbstractImageData.h>
#include <medMetaDataKeys.h>

#include <medIntParameter.h>

class medAbstractShrinkFilterProcessPrivate
{
public:
    medIntParameter* shrinkFactor1;
    medIntParameter* shrinkFactor2;
    medIntParameter* shrinkFactor3;
};

medAbstractShrinkFilterProcess::medAbstractShrinkFilterProcess(QObject *parent): medAbstractSingleFilterOperationProcess(parent),
    d(new medAbstractShrinkFilterProcessPrivate)
{
    d->shrinkFactor1 = new medIntParameter("shrinkFactor1", this);
    d->shrinkFactor2 = new medIntParameter("shrinkFactor2", this);
    d->shrinkFactor3 = new medIntParameter("shrinkFactor3", this);
}

medAbstractShrinkFilterProcess::~medAbstractShrinkFilterProcess()
{

}

medIntParameter* medAbstractShrinkFilterProcess::shrinkFactor1()
{
    return d->shrinkFactor1;
}

medIntParameter* medAbstractShrinkFilterProcess::shrinkFactor2()
{
    return d->shrinkFactor2;
}

medIntParameter* medAbstractShrinkFilterProcess::shrinkFactor3()
{
    return d->shrinkFactor3;
}


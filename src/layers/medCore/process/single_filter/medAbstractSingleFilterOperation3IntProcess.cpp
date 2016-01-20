/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medAbstractSingleFilterOperation3IntProcess.h>

#include <medAbstractImageData.h>
#include <medMetaDataKeys.h>

#include <medIntParameter.h>

class medAbstractSingleFilterOperation3IntProcessPrivate
{
public:
    medIntParameter* x;
    medIntParameter* y;
    medIntParameter* z;
};

medAbstractSingleFilterOperation3IntProcess::medAbstractSingleFilterOperation3IntProcess(QObject *parent): medAbstractSingleFilterOperationProcess(parent),
    d(new medAbstractSingleFilterOperation3IntProcessPrivate)
{
    d->x = new medIntParameter("x", this);
    d->y = new medIntParameter("y", this);
    d->z = new medIntParameter("z", this);
}

medAbstractSingleFilterOperation3IntProcess::~medAbstractSingleFilterOperation3IntProcess()
{

}

medIntParameter* medAbstractSingleFilterOperation3IntProcess::x()
{
    return d->x;
}

medIntParameter* medAbstractSingleFilterOperation3IntProcess::y()
{
    return d->y;
}

medIntParameter* medAbstractSingleFilterOperation3IntProcess::z()
{
    return d->z;
}


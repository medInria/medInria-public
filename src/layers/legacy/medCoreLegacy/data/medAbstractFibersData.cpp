/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medAbstractFibersData.h>
#include <dtkLog/dtkLog.h>

// /////////////////////////////////////////////////////////////////
// medAbstractFibersDataPrivate
// /////////////////////////////////////////////////////////////////

class medAbstractFibersDataPrivate
{
public:
};

// /////////////////////////////////////////////////////////////////
// medAbstractFibersData
// /////////////////////////////////////////////////////////////////

medAbstractFibersData::medAbstractFibersData(void) : medAbstractData(), d(new medAbstractFibersDataPrivate)
{
}

medAbstractFibersData::medAbstractFibersData(const  medAbstractFibersData& other): medAbstractData(other), d(new medAbstractFibersDataPrivate)
{
}

medAbstractFibersData::~medAbstractFibersData(void)
{
    delete d;
    d = NULL;
}

void *medAbstractFibersData::fibers(void)
{
    return this->data();
}

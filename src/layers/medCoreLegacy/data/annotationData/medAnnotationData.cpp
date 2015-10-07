/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medAnnotationData.h>

#include <medAbstractData.h>

class medAnnotationData::medAnnotationDataPrivate
{
public:
};

medAnnotationData::medAnnotationData()
    : medAttachedData(),
    d( new medAnnotationDataPrivate )
{
}

medAnnotationData::~medAnnotationData()
{
    delete d;
    d = NULL;
}








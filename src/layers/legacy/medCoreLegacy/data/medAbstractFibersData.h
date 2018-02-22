/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2018. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medAbstractData.h>

#include <medCoreLegacyExport.h>

class medAbstractFibersDataPrivate;

class MEDCORELEGACY_EXPORT medAbstractFibersData : public medAbstractData
{
    Q_OBJECT

public:
             medAbstractFibersData();
             medAbstractFibersData(const  medAbstractFibersData& other);
    virtual ~medAbstractFibersData();

    void *fibers();

private:
    medAbstractFibersDataPrivate *d;
};



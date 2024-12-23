#pragma once
/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2020. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medAbstractData.h>

class medAbstractFibersDataPrivate;

class MEDCORE_EXPORT medAbstractFibersData : public medAbstractData
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



/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <dtkCore/dtkAbstractDataFactory.h>
#include <medAbstractData.h>


class MEDCORE_EXPORT medAbstractDataFactory : public dtkAbstractDataFactory
{
public:
    typedef medAbstractData           *(*medAbstractDataCreator)         (void);


public:
    static medAbstractDataFactory *instance();

    bool registerDataType         (const QString& type,  medAbstractDataCreator          func);
    bool registerDataType         (const QString& type,  medAbstractDataCreator          func, const QString& nameInterface);
    /*bool registerDataType         (const QString& type,  medAbstractData *(*func)());
    bool registerDataType         (const QString& type,  medAbstractData *(*func)() , const QString& nameInterface);*/

protected:
     medAbstractDataFactory();

public slots:
    medAbstractData            *create   (const QString& type);

private:
    static medAbstractDataFactory *s_instance;


private:
    bool registerDataType         (const QString& type,  dtkAbstractDataCreator          func);
    bool registerDataType         (const QString& type,  dtkAbstractDataCreator          func, const QString& nameInterface);
};



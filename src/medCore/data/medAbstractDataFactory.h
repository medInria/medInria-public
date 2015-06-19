/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <dtkCoreSupport/dtkAbstractDataFactory.h>
#include <medAbstractData.h>


class MEDCORE_EXPORT medAbstractDataFactory : public dtkAbstractDataFactory
{
public:
    static medAbstractDataFactory *instance();

    template <typename dataType>
    bool registerDataType() {
        return dtkAbstractDataFactory::registerDataType(dataType::staticIdentifier(),
                                                        create<dataType>);
    }

public slots:
    medAbstractData *create(const QString& type);

protected:
     medAbstractDataFactory();

private:
    static medAbstractDataFactory *s_instance;


    template < typename T >
    static dtkAbstractData* create() {
        return ( new T() );
    }

private:
    bool registerDataType(const QString& type, dtkAbstractDataCreator func);
    bool registerDataType(const QString& type, dtkAbstractDataCreator func, const QString& nameInterface);
};

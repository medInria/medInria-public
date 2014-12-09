/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medAbstractDataSourceFactory.h>
#include <medAbstractDataSource.h>

class medAbstractDataSourceFactoryPrivate
{
public:
    medAbstractDataSourceFactory::medAbstractDataSourceCreatorHash dataSourceCreators;
};

medAbstractDataSourceFactory *medAbstractDataSourceFactory::instance(void)
{
    if(!s_instance)
        s_instance = new medAbstractDataSourceFactory;

    return s_instance;
}

bool medAbstractDataSourceFactory::registerDataSource(QString type, medAbstractDataSourceCreator func)
{
    if(!d->dataSourceCreators.contains(type)) {
        d->dataSourceCreators.insert(type, func);
        return true;
    }

    return false;
}

QList<QString> medAbstractDataSourceFactory::dataSources(void)
{
    return d->dataSourceCreators.keys();
}

medAbstractDataSource *medAbstractDataSourceFactory::create(QString type,QWidget * parent)
{
    if(!d->dataSourceCreators.contains(type))
        return NULL;

    medAbstractDataSource *conf = d->dataSourceCreators[type](parent);

    return conf;
}

medAbstractDataSourceFactory::medAbstractDataSourceFactory(void) : dtkAbstractFactory(), d(new medAbstractDataSourceFactoryPrivate)
{
}

medAbstractDataSourceFactory::~medAbstractDataSourceFactory(void)
{
    delete d;
    d = NULL;
}

medAbstractDataSourceFactory *medAbstractDataSourceFactory::s_instance = NULL;

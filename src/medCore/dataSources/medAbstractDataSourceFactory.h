/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medCoreExport.h>
#include <dtkCore/dtkAbstractFactory.h>
#include <QtCore>

class medAbstractDataSource;
class medAbstractDataSourceFactoryPrivate;

/**
 * @class medSourceDataPluginFactory
 * @brief Dynamic source plugins (e.g. Shanoir, PACS, ...) factory
 * medAbstractSourceDataPlugin register to this factory.
 **/
class MEDCORE_EXPORT medAbstractDataSourceFactory : public dtkAbstractFactory
{
    Q_OBJECT

public:
    typedef medAbstractDataSource *(*medAbstractDataSourceCreator)(QWidget*);

    typedef QHash<QString, medAbstractDataSourceCreator> medAbstractDataSourceCreatorHash;

    /** Static accessor to the instance of the factory */
    static medAbstractDataSourceFactory *instance();

    /** Method called by plugins to register themselves */
    bool registerDataSource(QString type, medAbstractDataSourceCreator func);

    /** Returns a list of available source data plugins */
    QList<QString> dataSources();

public slots:
    /** Method to instantiate a plugin */
    medAbstractDataSource *create(QString type,QWidget* parent = NULL);
protected:
    medAbstractDataSourceFactory();
    ~medAbstractDataSourceFactory();

private:
    static medAbstractDataSourceFactory *s_instance;

    medAbstractDataSourceFactoryPrivate *d;
};



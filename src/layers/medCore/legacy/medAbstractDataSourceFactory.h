#pragma once
/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2020. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <dtkCoreSupport/dtkAbstractFactory.h>

#include <QtCore>

#include <medCoreExport.h>

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
    QList<QString> dataSourcePlugins();

public slots:
    /** Method to instantiate a plugin */
    medAbstractDataSource *create(QString type,QWidget* parent = nullptr);

protected:
    medAbstractDataSourceFactory();
    ~medAbstractDataSourceFactory();

private:
    static medAbstractDataSourceFactory *s_instance;

    medAbstractDataSourceFactoryPrivate *d;
};

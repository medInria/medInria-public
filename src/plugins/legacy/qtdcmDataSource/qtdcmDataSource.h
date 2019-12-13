/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2018. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medAbstractDataSource.h>

#include <qtdcmDataSourcePluginExport.h>

class qtdcmDataSourcePrivate;

class QTDCMDATASOURCEPLUGIN_EXPORT qtdcmDataSource : public medAbstractDataSource
{
    Q_OBJECT

public:
    qtdcmDataSource();
    virtual ~qtdcmDataSource();

    virtual QString description() const;

    static bool registered();

    QWidget *mainViewWidget();
    QWidget *sourceSelectorWidget();
    QString tabName();

    QList<medToolBox*> getToolBoxes();

public slots:
    void onSaveLocalSettings();
    void onImportFinished(QString directory);

private:
    qtdcmDataSourcePrivate *d;

    void initWidgets();
};

medAbstractDataSource *createQtdcmDataSource ( QWidget* );



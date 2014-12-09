/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medAbstractDataSource.h>

#include <medQtDcmDataSourceExport.h>

class medQtDcmDataSourcePrivate;

class medQtDcmDataSourcePLUGIN_EXPORT medQtDcmDataSource : public medAbstractDataSource
{
    Q_OBJECT

public:
    medQtDcmDataSource();
    virtual ~medQtDcmDataSource();

    virtual QString description() const;

    static bool registered();

    QWidget *mainViewWidget();
    QWidget *sourceSelectorWidget();
    QString tabName();

    QList<medToolBox*> getToolBoxes();

public slots:
    void onSaveLocalSettings();
    void onSerieMoved(QString directory);

private:
    medQtDcmDataSourcePrivate *d;

    void initWidgets();
};

medAbstractDataSource *createmedQtDcmDataSource ( QWidget* );



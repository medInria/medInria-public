/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medToolBox.h>
#include <medCoreExport.h>

class medBrowserPacsHostToolBoxPrivate;

class MEDCORE_EXPORT medBrowserPacsHostToolBox : public medToolBox
{
    Q_OBJECT

public:
     medBrowserPacsHostToolBox(QWidget *parent = 0);
    ~medBrowserPacsHostToolBox();

    void readSettings();
    void writeSettings();

    QString title();
    QString port();

protected slots:
    void onSettingsApplied();

private:
    medBrowserPacsHostToolBoxPrivate *d;
};



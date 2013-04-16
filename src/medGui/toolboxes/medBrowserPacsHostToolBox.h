/*=========================================================================

 MedInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medToolBox.h>
#include "medGuiExport.h"

class medBrowserPacsHostToolBoxPrivate;

class MEDGUI_EXPORT medBrowserPacsHostToolBox : public medToolBox
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



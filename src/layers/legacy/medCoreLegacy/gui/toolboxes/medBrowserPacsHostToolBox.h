#pragma once
/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2020. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medCoreLegacyExport.h>
#include <medToolBox.h>

class medBrowserPacsHostToolBoxPrivate;

class MEDCORELEGACY_EXPORT medBrowserPacsHostToolBox : public medToolBox
{
    Q_OBJECT

public:
     medBrowserPacsHostToolBox(QWidget *parent = nullptr);
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

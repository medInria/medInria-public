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

class medBrowserPacsSearchToolBoxPrivate;

class MEDCORE_EXPORT medBrowserPacsSearchToolBox : public medToolBox
{
    Q_OBJECT

public:
     medBrowserPacsSearchToolBox(QWidget *parent = 0);
    ~medBrowserPacsSearchToolBox();

signals:
    void search(QString query);

protected slots:
    void onSearch();

private:
    medBrowserPacsSearchToolBoxPrivate *d;
};



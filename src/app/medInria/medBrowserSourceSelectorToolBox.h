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

class medBrowserSourceSelectorToolBoxPrivate;

class medBrowserSourceSelectorToolBox : public medToolBox
{
    Q_OBJECT

public:
     medBrowserSourceSelectorToolBox(QWidget *parent = 0);
    ~medBrowserSourceSelectorToolBox();

	void addTab(QString tabName, QWidget *widget, QString description = "");

	void setCurrentTab(int current);

signals:
    void indexChanged(int);

private:
    medBrowserSourceSelectorToolBoxPrivate *d;
};



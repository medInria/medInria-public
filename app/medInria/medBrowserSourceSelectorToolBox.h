/* medBrowserSourceSelectorToolBox.h ---
 *
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Thu May 13 15:19:01 2010 (+0200)
 * Version: $Id$
 * Last-Updated: Wed Nov 10 10:24:03 2010 (+0100)
 *           By: Julien Wintz
 *     Update #: 16
 */

/* Commentary:
 *
 */

/* Change log:
 *
 */

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



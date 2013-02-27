/* medBrowserPacsSearchToolBox.h ---
 *
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Tue Oct  5 15:45:46 2010 (+0200)
 * Version: $Id$
 * Last-Updated: Wed Oct  6 10:39:31 2010 (+0200)
 *           By: Julien Wintz
 *     Update #: 14
 */

/* Commentary:
 *
 */

/* Change log:
 *
 */

#ifndef medBrowserPacsSearchToolBox_H
#define medBrowserPacsSearchToolBox_H

#include <medToolBox.h>
#include "medGuiExport.h"

class medBrowserPacsSearchToolBoxPrivate;

class MEDGUI_EXPORT medBrowserPacsSearchToolBox : public medToolBox
{
    Q_OBJECT

public:
     medBrowserPacsSearchToolBox(QWidget *parent = 0);
    ~medBrowserPacsSearchToolBox(void);

signals:
    void search(QString query);

protected slots:
    void onSearch(void);

private:
    medBrowserPacsSearchToolBoxPrivate *d;
};

#endif

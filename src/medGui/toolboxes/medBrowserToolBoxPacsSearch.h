/* medBrowserToolBoxPacsSearch.h ---
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

#ifndef medBrowserToolBoxPacsSEARCH_H
#define medBrowserToolBoxPacsSEARCH_H

#include <medToolBox.h>
#include "medGuiExport.h"

class medBrowserToolBoxPacsSearchPrivate;

class MEDGUI_EXPORT medBrowserToolBoxPacsSearch : public medToolBox
{
    Q_OBJECT

public:
     medBrowserToolBoxPacsSearch(QWidget *parent = 0);
    ~medBrowserToolBoxPacsSearch(void);

signals:
    void search(QString query);

protected slots:
    void onSearch(void);

private:
    medBrowserToolBoxPacsSearchPrivate *d;
};

#endif

/* medBrowserPacsHostToolBox.h ---
 *
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Tue Oct  5 15:45:46 2010 (+0200)
 * Version: $Id$
 * Last-Updated: Wed Oct  6 11:36:22 2010 (+0200)
 *           By: Julien Wintz
 *     Update #: 9
 */

/* Commentary:
 *
 */

/* Change log:
 *
 */

#ifndef medBrowserPacsHostToolBox_H
#define medBrowserPacsHostToolBox_H

#include <medToolBox.h>
#include "medGuiExport.h"

class medBrowserPacsHostToolBoxPrivate;

class MEDGUI_EXPORT medBrowserPacsHostToolBox : public medToolBox
{
    Q_OBJECT

public:
     medBrowserPacsHostToolBox(QWidget *parent = 0);
    ~medBrowserPacsHostToolBox(void);

    void readSettings(void);
    void writeSettings(void);

    QString title(void);
    QString port(void);

protected slots:
    void onSettingsApplied(void);

private:
    medBrowserPacsHostToolBoxPrivate *d;
};

#endif

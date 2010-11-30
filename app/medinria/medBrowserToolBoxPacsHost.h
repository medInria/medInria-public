/* medBrowserToolBoxPacsHost.h ---
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

#ifndef MEDBROWSERTOOLBOXPACSHOST_H
#define MEDBROWSERTOOLBOXPACSHOST_H

#include <medGui/medToolBox.h>

class medBrowserToolBoxPacsHostPrivate;

class MEDGUI_EXPORT medBrowserToolBoxPacsHost : public medToolBox
{
    Q_OBJECT

public:
     medBrowserToolBoxPacsHost(QWidget *parent = 0);
    ~medBrowserToolBoxPacsHost(void);

    void readSettings(void);
    void writeSettings(void);

    QString title(void);
    QString port(void);

protected slots:
    void onSettingsApplied(void);

private:
    medBrowserToolBoxPacsHostPrivate *d;
};

#endif

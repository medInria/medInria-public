/* medToolBoxPacsHost.h --- 
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

#ifndef MEDTOOLBOXPACSHOST_H
#define MEDTOOLBOXPACSHOST_H

#include "medToolBox.h"

class medToolBoxPacsHostPrivate;

class MEDGUI_EXPORT medToolBoxPacsHost : public medToolBox
{
    Q_OBJECT

public:
     medToolBoxPacsHost(QWidget *parent = 0);
    ~medToolBoxPacsHost(void);

    void readSettings(void);
    void writeSettings(void);

    QString title(void);
    QString port(void);

protected slots:
    void onSettingsApplied(void);

private:
    medToolBoxPacsHostPrivate *d;
};

#endif

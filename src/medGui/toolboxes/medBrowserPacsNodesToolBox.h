/* medBrowserPacsNodesToolBox.h ---
 *
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Tue Oct  5 15:46:09 2010 (+0200)
 * Version: $Id$
 * Last-Updated: Wed Oct  6 11:40:43 2010 (+0200)
 *           By: Julien Wintz
 *     Update #: 11
 */

/* Commentary:
 *
 */

/* Change log:
 *
 */

#ifndef medBrowserPacsNodesToolBox_H
#define medBrowserPacsNodesToolBox_H

#include <medToolBox.h>
#include "medGuiExport.h"

class medBrowserPacsNodesToolBoxPrivate;

class MEDGUI_EXPORT medBrowserPacsNodesToolBox : public medToolBox
{
    Q_OBJECT

public:
     medBrowserPacsNodesToolBox(QWidget *parent = 0);
    ~medBrowserPacsNodesToolBox();

    void readSettings();

    void writeSettings();

signals:
    void nodesUpdated();
    void echoRequest();

public slots:
    void addNode();
    void remNode();
    void onEchoResponse(QVector<bool>);

private:

    void fillWidget( QList<QVariant> nodes );

    medBrowserPacsNodesToolBoxPrivate *d;
};

#endif

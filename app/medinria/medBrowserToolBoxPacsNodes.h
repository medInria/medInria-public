/* medBrowserToolBoxPacsNodes.h ---
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

#ifndef medBrowserToolBoxPacsNODES_H
#define medBrowserToolBoxPacsNODES_H

#include <medGui/medToolBox.h>

class medBrowserToolBoxPacsNodesPrivate;

class MEDGUI_EXPORT medBrowserToolBoxPacsNodes : public medToolBox
{
    Q_OBJECT

public:
     medBrowserToolBoxPacsNodes(QWidget *parent = 0);
    ~medBrowserToolBoxPacsNodes(void);

    void readSettings(void);
    void writeSettings(void);

public slots:
    void addNode(void);
    void remNode(void);
    void echo(void);

private:
    medBrowserToolBoxPacsNodesPrivate *d;
};

#endif

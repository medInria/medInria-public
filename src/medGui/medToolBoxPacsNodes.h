/* medToolBoxPacsNodes.h --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Tue Oct  5 15:46:09 2010 (+0200)
 * Version: $Id$
 * Last-Updated: Tue Oct  5 17:18:23 2010 (+0200)
 *           By: Julien Wintz
 *     Update #: 10
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#ifndef MEDTOOLBOXPACSNODES_H
#define MEDTOOLBOXPACSNODES_H

#include "medToolBox.h"

class medToolBoxPacsNodesPrivate;

class MEDGUI_EXPORT medToolBoxPacsNodes : public medToolBox
{
    Q_OBJECT

public:
     medToolBoxPacsNodes(QWidget *parent = 0);
    ~medToolBoxPacsNodes(void);

    void readSettings(void);
    void writeSettings(void);

    void setHostTitle(const QString& title);
    void setHostAddress(const QString& address);
    void setHostPort(const QString& port);

public slots:
    void addNode(void);
    void remNode(void);
    void echo(void);

private:
    medToolBoxPacsNodesPrivate *d;
};

#endif

/* medViewerToolBoxWorkspace.h --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Fri Feb 19 09:02:09 2010 (+0100)
 * Version: $Id$
 * Last-Updated: Wed Nov 10 10:58:10 2010 (+0100)
 *           By: Julien Wintz
 *     Update #: 8
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#ifndef MEDVIEWERTOOLBOXWORKSPACE_H
#define MEDVIEWERTOOLBOXWORKSPACE_H

#include <medToolBox.h>

class medViewerToolBoxWorkspacePrivate;

class medViewerToolBoxWorkspace : public medToolBox
{
    Q_OBJECT

public:
     medViewerToolBoxWorkspace(QWidget *parent = 0);
    ~medViewerToolBoxWorkspace(void);

    void addWorkspace(QString name);

signals:
    void workspaceChanged(QString);

private:
    medViewerToolBoxWorkspacePrivate *d;
};

#endif

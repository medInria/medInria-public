/* medVisualizationWorkspace.h ---
 *
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Thu May 13 14:11:43 2010 (+0200)
 * Version: $Id$
 * Last-Updated: Thu May 13 14:22:37 2010 (+0200)
 *           By: Julien Wintz
 *     Update #: 5
 */

/* Commentary:
 *
 */

/* Change log:
 *
 */

#ifndef medVisualizationWorkspace_H
#define medVisualizationWorkspace_H

#include <QtCore>

#include <medWorkspace.h>


class medVisualizationWorkspacePrivate;

class medVisualizationWorkspace : public medWorkspace
{
    Q_OBJECT

public:
     medVisualizationWorkspace(QWidget *parent = 0);
    ~medVisualizationWorkspace(void);

    virtual void setupViewContainerStack();

    virtual QString identifier()  const;
    virtual QString description() const;
    static bool isUsable();

public slots:
    /**
     * @brief Connects toolboxes to the current container
     *
     * @param name the container name
    */
    virtual void connectToolboxesToCurrentContainer(const QString &name);

private:
    medVisualizationWorkspacePrivate *d;
};

#endif

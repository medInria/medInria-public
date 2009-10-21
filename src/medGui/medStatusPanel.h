/* medStatusPanel.h --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Sun Oct 11 13:00:53 2009 (+0200)
 * Version: $Id$
 * Last-Updated: Wed Oct 21 15:23:06 2009 (+0200)
 *           By: Julien Wintz
 *     Update #: 19
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#ifndef MEDSTATUSPANEL_H
#define MEDSTATUSPANEL_H

#include "medGuiExport.h"

#include <QFrame>

// /////////////////////////////////////////////////////////////////
// medStatusPanel
// /////////////////////////////////////////////////////////////////

class medStatusPanelPrivate;

class MEDGUI_EXPORT medStatusPanel : public QWidget
{
    Q_OBJECT

public:
     medStatusPanel(QWidget *parent = 0);
    ~medStatusPanel(void);

    void addSpacer(void);
    void addWidget(QWidget *widget);

    void removeWidget(QWidget *widget);

protected:
    void paintEvent(QPaintEvent *event);

private:
    medStatusPanelPrivate *d;
};

#endif

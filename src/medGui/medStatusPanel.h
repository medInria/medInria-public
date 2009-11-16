/* medStatusPanel.h --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Sun Oct 11 13:00:53 2009 (+0200)
 * Version: $Id$
 * Last-Updated: Tue Nov  3 09:35:26 2009 (+0100)
 *           By: Julien Wintz
 *     Update #: 20
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

class MEDGUI_EXPORT medStatusPanel : public QFrame
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

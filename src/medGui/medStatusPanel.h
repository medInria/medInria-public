/* medStatusPanel.h --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Sun Oct 11 13:00:53 2009 (+0200)
 * Version: $Id$
 * Last-Updated: Tue Oct 20 09:58:45 2009 (+0200)
 *           By: Julien Wintz
 *     Update #: 17
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#ifndef MEDSTATUSPANEL_H
#define MEDSTATUSPANEL_H

#include <QFrame>

// /////////////////////////////////////////////////////////////////
// medStatusPanel
// /////////////////////////////////////////////////////////////////

class medStatusPanelPrivate;

class medStatusPanel : public QWidget
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

/* medStatusPanel.h --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Sun Oct 11 13:00:53 2009 (+0200)
 * Version: $Id$
 * Last-Updated: Mon Oct 12 18:38:57 2009 (+0200)
 *           By: Julien Wintz
 *     Update #: 14
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
// medStatusWidget
// /////////////////////////////////////////////////////////////////

// class medStatusWidgetPrivate;

// class medStatusWidget : public QWidget
// {
//     Q_OBJECT

// public:
//      medStatusWidget(QWidget *parent = 0);
//     ~medStatusWidget(void);

//     void addField(const QString& field, QString value);
//     void setField(const QString& field, QString value);

// private:
//     medStatusWidgetPrivate *d;
// };

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

protected:
    void paintEvent(QPaintEvent *event);

private:
    medStatusPanelPrivate *d;
};

#endif

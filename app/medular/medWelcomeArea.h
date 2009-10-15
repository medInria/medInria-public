/* medWelcomeArea.h --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Mon Oct  5 10:47:56 2009 (+0200)
 * Version: $Id$
 * Last-Updated: Fri Oct  9 18:07:01 2009 (+0200)
 *           By: Julien Wintz
 *     Update #: 2
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#ifndef MEDWELCOMEAREA_H
#define MEDWELCOMEAREA_H

#include <QtCore>
#include <QtGui>

class medWelcomeAreaPrivate;

class medWelcomeArea : public QWidget
{
    Q_OBJECT

public:
     medWelcomeArea(QWidget *parent = 0);
    ~medWelcomeArea(void);

protected:
    void paintEvent(QPaintEvent *event);

protected slots:
    void linkClicked(const QUrl &url);

private:
    medWelcomeAreaPrivate *d;
};

#endif // MEDWELCOMEAREA_H

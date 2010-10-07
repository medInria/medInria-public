/* medStatusQuitButton.cpp --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Thu Oct  7 11:45:34 2010 (+0200)
 * Version: $Id$
 * Last-Updated: Thu Oct  7 11:55:28 2010 (+0200)
 *           By: Julien Wintz
 *     Update #: 20
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#include "medStatusQuitButton.h"

#include <QtGui>

medStatusQuitButton::medStatusQuitButton(QWidget *parent) : QWidget(parent)
{
    QLabel *icon = new QLabel(this);
    icon->setPixmap(QPixmap(":/icons/quit.png"));

    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(icon);
}

medStatusQuitButton::~medStatusQuitButton(void)
{

}

QSize medStatusQuitButton::sizeHint(void) const
{
    return QSize(16, 16);
}

void medStatusQuitButton::mousePressEvent(QMouseEvent *event)
{
    qApp->quit();
}

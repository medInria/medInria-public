/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medButton.h>

#include <QtGui>
#include <QtWidgets>

class medButtonPrivate
{
public:
    QLabel * icon;
};

/**
 * @brief Instantiates the button from a resource path.
 *
 * @param parent
 * @param resourceLocation
 * @param toolTip
*/
medButton::medButton( QWidget *parent, QString resourceLocation, QString toolTip ):
    QWidget(parent), d(new medButtonPrivate)
{
    d->icon = new QLabel(this);
    d->icon->setPixmap(QPixmap(resourceLocation));

    QHBoxLayout * layout = new QHBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(d->icon);
    setToolTip(toolTip);
}

/**
 * @brief Instantiates the button from an existing pixmap.
 *
 * @param parent
 * @param pixmap
 * @param toolTip
*/
medButton::medButton( QWidget *parent, QPixmap pixmap, QString toolTip ):
    QWidget(parent),d(new medButtonPrivate)
{
    d->icon = new QLabel(this);
    d->icon->setPixmap(pixmap);

    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(d->icon);
    setToolTip(toolTip);
}

medButton::~medButton( void )
{
    delete d;
}

/**
 * @brief Returns the pixmap's size.
 * @return QSize
*/
QSize medButton::sizeHint( void ) const
{
    return d->icon->sizeHint();
}

/**
 * @brief Sets a new existing pixmap as an icon.
 *
 * @param icon
*/
void medButton::setIcon(QPixmap icon)
{
    d->icon->setPixmap(icon);
}


void medButton::mousePressEvent( QMouseEvent *event )
{
    Q_UNUSED(event)
    emit triggered();
}

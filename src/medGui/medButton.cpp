#include "medButton.h"

#include <QtGui>

class medButtonPrivate
{
public:
    QLabel * icon;
};


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

QSize medButton::sizeHint( void ) const
{
    return d->icon->size();
}

void medButton::setIcon(QPixmap icon)
{
    d->icon->setPixmap(icon);
}


void medButton::mousePressEvent( QMouseEvent *event )
{
    emit triggered();
}

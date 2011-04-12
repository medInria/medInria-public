#include "medButton.h"

#include <QtGui>

medButton::medButton( QWidget *parent, QString resourceLocation, QString toolTip ): QWidget(parent)
{
    QLabel *icon = new QLabel(this);
    icon->setPixmap(QPixmap(resourceLocation));

    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(icon);
    setToolTip(toolTip);
}

medButton::~medButton( void )
{

}

QSize medButton::sizeHint( void ) const
{
    return QSize(16, 16);
}

void medButton::mousePressEvent( QMouseEvent *event )
{
    emit triggered();
}

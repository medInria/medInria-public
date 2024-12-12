/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2020. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medToolBoxHeader.h>

class medToolBoxHeaderPrivate
{
public:
    QString title;
    QPoint titleOffset;
};

medToolBoxHeader::medToolBoxHeader(QWidget *parent) : QFrame(parent), d(new medToolBoxHeaderPrivate)
{
    d->title = "Untitled";
    d->titleOffset = QPoint( 0, 0 );

    QBoxLayout* layout = new QBoxLayout(QBoxLayout::LeftToRight,this);
    layout->setMargin(0);
}

medToolBoxHeader::~medToolBoxHeader(void)
{
    delete d;
    d = nullptr;
}

/**
 * @brief Sets the header's title.
 *
 * @param title
*/
void medToolBoxHeader::setTitle(const QString& title)
{
    d->title = title;
}


/**
 * @brief Returns the title of the header.
 *
 * @return const QString
 */
QString medToolBoxHeader::title() const
{
    return d->title;
}

/**
 * @brief Sets the offset of the header's title from upper left corner.
 *
 * @param titleOffset
*/
void medToolBoxHeader::setTitleOffset(const QPoint & titleOffset)
{
    d->titleOffset = titleOffset;
}

/**
 * @brief Paints the header.
 *
 * @param event
*/
void medToolBoxHeader::paintEvent(QPaintEvent *event)
{
    QFrame::paintEvent(event);

    QRectF rect = this->contentsRect();

    QPainter painter(this);
    painter.translate( d->titleOffset );

    painter.drawText(rect, Qt::AlignCenter, d->title);
    painter.end();
}

/**
 * @brief Emits the triggered() signal.
 *
 * @param event
*/
void medToolBoxHeader::mousePressEvent( QMouseEvent *event )
{
    Q_UNUSED(event);
    emit triggered();
}

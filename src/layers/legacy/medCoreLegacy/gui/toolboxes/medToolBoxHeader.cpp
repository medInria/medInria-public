/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medToolBoxHeader.h>
#include <medButton.h>

#include <dtkCoreSupport/dtkGlobal.h>

class medToolBoxHeaderPrivate
{
public:
    QString title;
    QPoint titleOffset;
    medButton* about;
    static QPixmap* png;
    static const QString tooltip;
};


const QString medToolBoxHeaderPrivate::tooltip = QObject::tr("About this plugin");
QPixmap * medToolBoxHeaderPrivate::png = NULL;

medToolBoxHeader::medToolBoxHeader(QWidget *parent) : QFrame(parent), d(new medToolBoxHeaderPrivate)
{
    if (!d->png)  d->png = new QPixmap(":icons/information.png");
    d->title = "Untitled";
    d->titleOffset = QPoint( 0, 0 );
//    d->about = NULL;
    QBoxLayout* layout = new QBoxLayout(QBoxLayout::LeftToRight,this);
    layout->setMargin(0);

    d->about = new medButton(this,*(d->png),
                             medToolBoxHeaderPrivate::tooltip);
    layout->addStretch();
    layout->addWidget(d->about);
    d->about->hide();

}

medToolBoxHeader::~medToolBoxHeader(void)
{
    delete d;

    d = NULL;
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
 * @brief
 *
 * @param visible
 */
void medToolBoxHeader::setAboutButtonVisibility(bool visible)
{
    d->about->setVisible(visible);
}

/**
 * @brief Emits the triggered() signal.
 *
 * @param event
*/
void medToolBoxHeader::mousePressEvent( QMouseEvent *event )
{
    emit triggered();
}

/**
 * @brief Returns a pointer to the aboutButton.
 *
 */
medButton * medToolBoxHeader::aboutButton()
{
    return d->about;
}

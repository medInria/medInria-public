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

#include <dtkCore/dtkGlobal.h>

class medToolBoxHeaderPrivate
{
public:
    QString title;
    QLabel * titleLabel;
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
    d->titleLabel = new QLabel(d->title);
    QBoxLayout* layout = new QBoxLayout(QBoxLayout::LeftToRight,this);
    layout->setMargin(0);

    d->about = new medButton(this,*(d->png), medToolBoxHeaderPrivate::tooltip);
    layout->addWidget(d->titleLabel);
    layout->addStretch();
    layout->addWidget(d->about);
    d->about->hide();

}

medToolBoxHeader::~medToolBoxHeader(void)
{
    delete d;

    d = NULL;
}

QSize medToolBoxHeader::sizeHint(void) const
{
    return QSize(100, 32);
}

void medToolBoxHeader::setTitle(const QString& title)
{
    d->title = title;
    d->titleLabel->setText(d->title);
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

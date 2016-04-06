/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medQtView.h>

#include <medViewFactory.h>
#include <medAbstractData.h>

#include <medAbstractNavigator.h>
#include <medAbstractViewNavigator.h>

#include <medAbstractLayeredViewNavigator.h>
#include <medAbstractLayeredViewInteractor.h>


#include <QLabel>

class medQtViewPrivate
{
public:
    QLabel *label;
};


medQtView::medQtView(QObject *parent): d(new medQtViewPrivate)
{
    d->label = new QLabel("prout");
    d->label->setScaledContents(true);
    d->label->setVisible(true);
}

medQtView::~medQtView()
{
    delete d;
}

medAbstractLayeredViewInteractor * medQtView::primaryInteractor(medAbstractData* data)
{
    dtkDebug()<< "primaryInteractor(medAbstractData* data)";
    return 0;
}
 QList<medAbstractInteractor *> medQtView::extraInteractors(medAbstractData* data)
{
    dtkDebug()<< "extraInteractor(medAbstractData* data)";
    return QList<medAbstractInteractor *>();
}
 medAbstractLayeredViewInteractor * medQtView::primaryInteractor(unsigned int layer)
{
    dtkDebug()<< "primaryInteractor(unsigned int layer)";
    return 0;
}

 QList<medAbstractInteractor *> medQtView::extraInteractors(unsigned int layer)
{
    dtkDebug()<< "extraInteractor(unsigned int layer)";
    return QList<medAbstractInteractor *>();
}

 medAbstractLayeredViewNavigator * medQtView::primaryNavigator()
{
    dtkDebug()<< "primaryNavigator()";
    return 0;
}

 QList<medAbstractNavigator *> medQtView::extraNavigators()
{
    dtkDebug()<< "extraNavigator()";
    return QList<medAbstractNavigator *>();
}

QWidget* medQtView::viewWidget()
{
    return d->label;
}

QWidget* medQtView::navigatorWidget()
{
    return 0;
}

QWidget* medQtView::mouseInteractionWidget()
{
    return 0;
}

QWidget* medQtView::buildToolBarWidget()
{
    return 0;
}

bool medQtView::initialiseInteractors(medAbstractData* data)
{
    return true;
}

bool medQtView::initialiseNavigators()
{
    return true;
}

void medQtView::removeInteractors(medAbstractData *data)
{

}


medViewBackend* medQtView::backend() const
{
    return 0;
}

bool medQtView::registered()
{
    return medViewFactory::instance()->registerView<medQtView>("medQtView", QStringList() << "prout");
}

QString medQtView::s_description()
{
    static const QString description = "medQtView";
    return description;
}

QString medQtView::description( void ) const
{
    return s_description();
}

void medQtView::reset()
{
    dtkDebug() << "reset()";
}

void medQtView::render()
{
    dtkDebug() << "render";
}

QImage medQtView::buildThumbnail(const QSize &size)
{
    QImage img = QImage(size, QImage::Format_ARGB32);
    img.fill(Qt::black);

    return img;
}

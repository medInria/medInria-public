/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medQtView.h>

#include <medLayeredViewFactory.h>
#include <medAbstractData.h>

#include <medAbstractNavigator.h>
#include <medAbstractViewNavigator.h>

#include <medAbstractLayeredViewNavigator.h>
#include <medAbstractLayeredViewInteractor.h>


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
    qDebug()<< "primaryInteractor(medAbstractData* data)";
    return 0;
}
 QList<medAbstractInteractor *> medQtView::extraInteractors(medAbstractData* data)
{
    qDebug()<< "extraInteractor(medAbstractData* data)";
    return QList<medAbstractInteractor *>();
}
 medAbstractLayeredViewInteractor * medQtView::primaryInteractor(unsigned int layer)
{
    qDebug()<< "primaryInteractor(unsigned int layer)";
    return 0;
}

 QList<medAbstractInteractor *> medQtView::extraInteractors(unsigned int layer)
{
    qDebug()<< "extraInteractor(unsigned int layer)";
    return QList<medAbstractInteractor *>();
}

 medAbstractLayeredViewNavigator * medQtView::primaryNavigator()
{
    qDebug()<< "primaryNavigator()";
    return 0;
}

 QList<medAbstractNavigator *> medQtView::extraNavigators()
{
    qDebug()<< "extraNavigator()";
    return QList<medAbstractNavigator *>();
}

QWidget* medQtView::viewWidget()
{
    return d->label;
}
QWidget* medQtView::toolBarWidget()
{
    return 0;
}

QWidget* medQtView::navigatorWidget()
{
    return 0;
}

QList<QWidget *> medQtView::layerWidgets()
{
    return QList<QWidget*>();
}

void medQtView::initialiseInteractors(medAbstractData* data)
{

}

void medQtView::initialiseNavigators()
{

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
    return medLayeredViewFactory::instance()->registerView<medQtView>("medQtView", QStringList() << "prout");
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
    qDebug() << "reset()";
}

QList<medAbstractParameter*> medQtView::navigatorsParameters()
{
    return QList<medAbstractParameter*>();
}

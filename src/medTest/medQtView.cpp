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
}

medQtView::~medQtView()
{
    delete d;
}

QWidget *medQtView::widget(void)
{
    return d->label;
}

 medAbstractLayeredViewInteractor * medQtView::primaryInteractor(medAbstractData* data)
{
    qDebug()<< "primaryInteractor(medAbstractData* data)";
    return 0;
}
 QList<medAbstractInteractor *> medQtView::extraInteractor(medAbstractData* data)
{
    qDebug()<< "extraInteractor(medAbstractData* data)";
    return QList<medAbstractInteractor *>();
}
 medAbstractLayeredViewInteractor * medQtView::primaryInteractor(unsigned int layer)
{
    qDebug()<< "primaryInteractor(unsigned int layer)";
    return 0;
}

 QList<medAbstractInteractor *> medQtView::extraInteractor(unsigned int layer)
{
    qDebug()<< "extraInteractor(unsigned int layer)";
    return QList<medAbstractInteractor *>();
}

 medAbstractLayeredViewNavigator * medQtView::primaryNavigator()
{
    qDebug()<< "primaryNavigator()";
    return 0;
}

 QList<medAbstractNavigator *> medQtView::extraNavigator()
{
    qDebug()<< "extraNavigator()";
    return QList<medAbstractNavigator *>();
}

QWidget* medQtView::receiverWidget()
{
    return 0;
}
QWidget* medQtView::toolBar()
{
    return 0;
}

QWidget* medQtView::toolBox()
{
    return 0;
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

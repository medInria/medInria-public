/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include "medQtView.h"

#include <dtkCore/dtkAbstractViewFactory.h>
#include <dtkCore/dtkAbstractData.h>

#include <medAbstractData.h>

class medQtViewPrivate
{
public:
    QLabel *label;
    dtkSmartPointer<dtkAbstractData> data;
};

static dtkAbstractView* createMedQtView()
{
    return new medQtView;
}

medQtView::medQtView(): medAbstractView(), d(new medQtViewPrivate)
{
    d->label = new QLabel;
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

void medQtView::setData (dtkAbstractData *data, int layer)
{
    this->setData (data);
}

void medQtView::setData (dtkAbstractData *data)
{
    d->data = data;
    if (data->description()=="medQtDataImage") {
         QImage image = *( reinterpret_cast< QImage *>( data->data() ) );
         if (!image.isNull())
             d->label->setPixmap ( QPixmap::fromImage(image) );
    }
}

void *medQtView::data(void)
{
    return d->data;
}

void *medQtView::data(int layer)
{
    return this->data();
}

bool medQtView::registered()
{
    return dtkAbstractViewFactory::instance()->registerViewType(s_description(), createMedQtView);
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

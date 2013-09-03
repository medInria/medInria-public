/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include "v3dViewImageInteractor.h"

#include <dtkCore/dtkAbstractData.h>
#include <dtkCore/dtkAbstractView.h>
#include <dtkCore/dtkAbstractViewFactory.h>

#include <medVtkView.h>

class v3dViewImageInteractorPrivate
{
public:
    v3dView * view;
    QList<dtkAbstractData*> dataList;
};


v3dViewImageInteractor::v3dViewImageInteractor(): medAbstractVtkViewInteractor(), d(new v3dViewImageInteractorPrivate)
{
    d->view = NULL;
}

v3dViewImageInteractor::~v3dViewImageInteractor()
{
    this->disable();

    delete d;
    d = 0;
}

QString v3dViewImageInteractor::description() const
{
    return tr("Interactor displaying images");
}


QString v3dViewImageInteractor::identifier() const
{
    return "v3dViewImageInteractor";
}


QStringList v3dViewImageInteractor::handled() const
{
    return QStringList () << v3dView::s_identifier() << medVtkView::s_identifier();
}


bool v3dViewImageInteractor::isDataTypeHandled(QString dataType) const
{
    if (dataType.contains("Image"))
        return true;
    
    return false;
}

bool v3dViewImageInteractor::registered()
{
    return dtkAbstractViewFactory::instance()->registerViewInteractorType("v3dViewImageInteractor",
                                                                          QStringList () << v3dView::s_identifier() << medVtkView::s_identifier(),
                                                                          createv3dViewImageInteractor);
}


void v3dViewImageInteractor::setData(dtkAbstractData *data)
{
    if(data->identifier().contains("Image"))
        d->dataList.append(data);
}


void v3dViewImageInteractor::setView(dtkAbstractView *view)
{
    if (v3dView *v3dview = dynamic_cast<v3dView*>(view) )
        d->view = v3dview;
}

void v3dViewImageInteractor::enable()
{
    if (this->enabled())
        return;

    dtkAbstractViewInteractor::enable();
}

void v3dViewImageInteractor::disable()
{
    if (!this->enabled())
        return;

    dtkAbstractViewInteractor::disable();
}

void v3dViewImageInteractor::setOpacity(dtkAbstractData *data, double value)
{
    int layer = 0;
    foreach (dtkAbstractData *listData, d->dataList)
    {
        if (listData == data)
            break;
        
        layer++;
    }
    
    if (layer == d->dataList.size())
        return;
    
    d->view->onOpacityChanged(value, layer);

    d->view->update();
}

double v3dViewImageInteractor::opacity(dtkAbstractData *data) const
{
    int layer = 0;
    foreach (dtkAbstractData *listData, d->dataList)
    {
        if (listData == data)
            break;
        
        layer++;
    }
    
    if (layer == d->dataList.size())
        return 0;
    
    return d->view->opacity(layer);
}

void v3dViewImageInteractor::setVisible(dtkAbstractData *data, bool visible)
{
    int layer = 0;
    foreach (dtkAbstractData *listData, d->dataList)
    {
        if (listData == data)
            break;
        
        layer++;
    }
    
    if (layer == d->dataList.size())
        return;
    
    d->view->onVisibilityChanged(visible, layer);
    d->view->update();
}

bool v3dViewImageInteractor::isVisible(dtkAbstractData *data) const
{
    int layer = 0;
    foreach (dtkAbstractData *listData, d->dataList)
    {
        if (listData == data)
            break;
        
        layer++;
    }
    
    if (layer == d->dataList.size())
        return false;

    return d->view->visibility(layer);
}

// /////////////////////////////////////////////////////////////////
// Type instantiation
// /////////////////////////////////////////////////////////////////

dtkAbstractViewInteractor *createv3dViewImageInteractor()
{
    return new v3dViewImageInteractor;
}

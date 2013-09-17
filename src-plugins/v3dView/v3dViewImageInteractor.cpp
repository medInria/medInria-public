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
    //TODO: change this, itkTensorImage should not be handled by this interactor
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


    medListParameter *LUTParam = new medListParameter("LUT", data);
    QStringList LUTs = QStringList() << "Default" << "Black & White" << "Black & White Inversed"
                                     << "Spectrum" << "Hot Metal" << "Hot Green"
                                     << "Hot Iron" << "GE" << "Flow" << "Loni" << "Loni 2"
                                     << "Asymmetry" << "P-Value" << "Red Black Alpha"
                                     << "Green Black Alpha" << "Blue Black Alpha"
                                     << "Muscles & Bones" << "Bones" << "Red Vessels"
                                     << "Cardiac" << "Gray Rainbow" << "Stern" << "Black Body";
    LUTParam->setValues(LUTs);

    medListParameter *presetParam = new medListParameter("Preset", data);
    QStringList presets = QStringList() << "None" << "VR Muscles&Bones" << "Vascular I"
                                        << "Vascular II" << "Vascular III" << "Vascular IV"
                                        << "Standard" << "Soft" << "Soft on White"
                                        << "Soft on Blue" << "Red on White" << "Glossy" ;
    presetParam->setValues(presets);

    connect(LUTParam, SIGNAL(valueChanged(dtkAbstractData*,QString)), this, SLOT(setLUT(dtkAbstractData*,QString)));
    connect(presetParam, SIGNAL(valueChanged(dtkAbstractData*,QString)), this, SLOT(setPreset(dtkAbstractData*,QString)));

    parameters.insert(data, LUTParam);
    parameters.insert(data, presetParam);
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

void v3dViewImageInteractor::setLUT(dtkAbstractData * data, QString lut)
{
    d->view->setCurrentLayer(getLayer(data));
    d->view->onLookupTablePropertySet(lut);
    d->view->update();
}

QString v3dViewImageInteractor::LUT(dtkAbstractData * data) const
{

}

void v3dViewImageInteractor::setPreset(dtkAbstractData * data, QString preset)
{
    d->view->setCurrentLayer(getLayer(data));
    d->view->onPresetPropertySet(preset);
    d->view->update();
}

QString v3dViewImageInteractor::preset(dtkAbstractData * data) const
{

}

int v3dViewImageInteractor::getLayer(dtkAbstractData * data) const
{
    int layer = 0;

    foreach (dtkAbstractData *listData, d->dataList)
    {
        if (listData == data)
            return layer;

        layer++;
    }

    return -1;
}

// /////////////////////////////////////////////////////////////////
// Type instantiation
// /////////////////////////////////////////////////////////////////

dtkAbstractViewInteractor *createv3dViewImageInteractor()
{
    return new v3dViewImageInteractor;
}

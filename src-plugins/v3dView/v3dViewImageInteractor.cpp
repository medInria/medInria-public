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

#include <vtkColorTransferFunction.h>
#include <vtkPiecewiseFunction.h>

#include <medVtkView.h>
#include <vtkImageView2D.h>
#include <vtkImageView3D.h>
#include <vtkTransferFunctionPresets.h>
#include <vtkLookupTableManager.h>
#include <vtkImageViewCollection.h>

class v3dViewImageInteractorPrivate
{
public:
    v3dView * view;
    QList<dtkAbstractData*> dataList;
    vtkImageViewCollection *collection;
};


v3dViewImageInteractor::v3dViewImageInteractor(): medAbstractVtkViewInteractor(), d(new v3dViewImageInteractorPrivate)
{
    d->view = NULL;

    d->collection = vtkImageViewCollection::New();
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
    {
        d->view = v3dview;

        d->collection->RemoveAllItems();
        d->collection->AddItem ( d->view->view2d() );
        d->collection->AddItem ( d->view->view3d() );
    }
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
    int layer = getLayer(data);

    if (layer == d->dataList.size())
        return;
    
    d->view->view2d()->SetOpacity ( value, layer );
    d->view->view3d()->SetOpacity ( value, layer );

    d->view->update();
}

double v3dViewImageInteractor::opacity(dtkAbstractData *data) const
{
    int layer = getLayer(data);
    
    if (layer == d->dataList.size())
        return 0;
    
    return d->view->opacity(layer);
}

void v3dViewImageInteractor::setVisible(dtkAbstractData *data, bool visible)
{
    int layer = getLayer(data);
    
    if (layer == d->dataList.size())
        return;
    
    if ( visible )
    {
        d->view->view2d()->SetVisibility ( 1,layer );
        d->view->view3d()->SetVisibility ( 1,layer );
    }
    else
    {
        d->view->view2d()->SetVisibility ( 0,layer );
        d->view->view3d()->SetVisibility ( 0,layer );
    }

    d->view->update();
}

bool v3dViewImageInteractor::isVisible(dtkAbstractData *data) const
{
    int layer = getLayer(data);
    return d->view->visibility(layer);
}

void v3dViewImageInteractor::setLUT(dtkAbstractData * data, QString lut)
{
    setLUT( getLayer(data), lut);
    d->view->update();
}

void v3dViewImageInteractor::setLUT (int layer, const QString &value)
{
    typedef vtkTransferFunctionPresets Presets;
    vtkColorTransferFunction * rgb   = vtkColorTransferFunction::New();
    vtkPiecewiseFunction     * alpha = vtkPiecewiseFunction::New();
    Presets::GetTransferFunction ( value.toStdString(), rgb, alpha );

    if ( layer == 0 )
    {
        d->view->view2d()->SetTransferFunctions ( rgb, alpha, layer );
    }
    else
    {
        vtkLookupTable *lut = vtkLookupTableManager::GetLookupTable ( value.toStdString() );
        d->view->view2d()->SetLookupTable ( lut, layer );
        lut->Delete();
    }

    d->view->view3d()->SetTransferFunctions ( rgb, alpha, layer );
    d->view->view3d()->SetLookupTable(vtkLookupTableManager::GetLookupTable ( value.toStdString()), layer);

    rgb->Delete();
    alpha->Delete();
}


QString v3dViewImageInteractor::LUT(dtkAbstractData * data) const
{

}

void v3dViewImageInteractor::setPreset(dtkAbstractData * data, QString preset)
{
    setPreset( getLayer(data), preset);
    d->view->update();
}

void v3dViewImageInteractor::setPreset (int layer, const QString &preset )
{
    if ( preset == "None" )
    {
        // we reset the LUT and the ww/wl to the default values

        this->setLUT( layer, "Black & White" );

        double color[3] = {0.0, 0.0, 0.0};

        d->collection->SyncSetBackground ( color );
        d->collection->SyncResetWindowLevel(0);
    }
    else if ( preset == "VR Muscles&Bones" )
    {
        this->setLUT ( layer, "Muscles & Bones" );

        double color[3] = {0.0, 0.0, 0.0};

        d->collection->SyncSetBackground ( color );
        d->collection->SyncSetColorWindow ( 337.0, 0, 1 );
        d->collection->SyncSetColorLevel ( 1237.0, 0, 1 );
    }
    else if ( preset == "Vascular I" )
    {
        this->setLUT ( layer, "Stern" );

        double color[3] = {0.0, 0.0, 0.0};

        d->collection->SyncSetBackground ( color );
        d->collection->SyncSetColorWindow ( 388.8, 0, 1 );
        d->collection->SyncSetColorLevel ( 362.9, 0, 1 );
    }
    else if ( preset == "Vascular II" )
    {
        this->setLUT ( layer, "Red Vessels" );

        double color[3] = {0.0, 0.0, 0.0};

        d->collection->SyncSetBackground ( color );
        d->collection->SyncSetColorWindow ( 189.6, 0, 1 );
        d->collection->SyncSetColorLevel ( 262.3, 0, 1 );
    }
    else if ( preset == "Vascular III" )
    {
        this->setLUT ( layer, "Red Vessels" );

        double color[3] = {0.0, 0.0, 0.0};

        d->collection->SyncSetBackground ( color );
        d->collection->SyncSetColorWindow ( 284.4, 0, 1 );
        d->collection->SyncSetColorLevel ( 341.7, 0, 1 );
    }
    else if ( preset == "Vascular IV" )
    {
        this->setLUT ( layer, "Red Vessels" );

        double color[3] = {0.0, 0.0, 0.0};

        d->collection->SyncSetBackground ( color );
        d->collection->SyncSetColorWindow ( 272.5, 0, 1 );
        d->collection->SyncSetColorLevel ( 310.9, 0, 1 );
    }
    else if ( preset == "Standard" )
    {
        this->setLUT ( layer, "Muscles & Bones" );

        double color[3] = {0.0, 0.0, 0.0};

        d->collection->SyncSetBackground ( color );
        d->collection->SyncSetColorWindow ( 243.7, 0, 1 );
        d->collection->SyncSetColorLevel ( 199.6, 0, 1 );
    }
    else if ( preset == "Soft" )
    {
        this->setLUT ( layer, "Bones" );

        double color[3] = {0.0, 0.0, 0.0};

        d->collection->SyncSetBackground ( color );
        d->collection->SyncSetColorWindow ( 133.5, 0, 1 );
        d->collection->SyncSetColorLevel ( 163.4, 0, 1 );
    }
    else if ( preset == "Soft on White" )
    {
        this->setLUT ( layer, "Muscles & Bones" );

        double color[3] = {1.0,0.98820477724075317,0.98814374208450317};

        d->collection->SyncSetBackground ( color );
        d->collection->SyncSetColorWindow ( 449.3, 0, 1 );
        d->collection->SyncSetColorLevel ( 372.8, 0, 1 );
    }
    else if ( preset == "Soft on Blue" )
    {
        this->setLUT ( layer, "Muscles & Bones" );

        double color[3]={0.0, 0.27507439255714417, 0.26398107409477234};

        d->collection->SyncSetBackground ( color );
        d->collection->SyncSetColorWindow ( 449.3, 0, 1 );
        d->collection->SyncSetColorLevel ( 372.8, 0, 1 );
    }
    else if ( preset == "Red on White" )
    {
        this->setLUT ( layer, "Red Vessels" );

        double color[3]={1.0, 0.98820477724075317, 0.98814374208450317};

        d->collection->SyncSetBackground ( color );
        d->collection->SyncSetColorWindow ( 449.3, 0, 1 );
        d->collection->SyncSetColorLevel ( 372.8, 0, 1 );
    }
    else if ( preset == "Glossy" )
    {
        this->setLUT ( layer, "Bones" );

        double color[3] = {0.0, 0.0, 0.0};

        d->collection->SyncSetBackground ( color );
        d->collection->SyncSetColorWindow ( 133.5, 0, 1 );
        d->collection->SyncSetColorLevel ( 163.4, 0, 1 );
    }
    else
    {
        return; // to prevent trigger of event lutChanged()
    }
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

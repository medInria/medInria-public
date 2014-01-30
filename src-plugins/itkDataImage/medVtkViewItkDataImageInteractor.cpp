/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medVtkViewItkDataImageInteractor.h>


#include <vtkColorTransferFunction.h>
#include <vtkPiecewiseFunction.h>

#include <vtkImageView2D.h>
#include <vtkImageView3D.h>
#include <vtkTransferFunctionPresets.h>
#include <vtkLookupTableManager.h>
#include <vtkImageViewCollection.h>

#include <medVtkViewBackend.h>
#include <medAbstractData.h>
#include <medImageViewFactory.h>
#include <medStringListParameter.h>
#include <medIntParameter.h>
#include <medBoolParameter.h>



class medVtkViewItkDataImageInteractorPrivate
{
public:
    // views
    vtkImageView2D *view2d;
    vtkImageView3D *view3d;
    medAbstractData *data;

    unsigned int layer;

    medStringListParameter *lutParam;
    medStringListParameter *presetParam;
    medIntParameter *opacityParam;
    medBoolParameter *visibiltyParameter;


    medAbstractImageView *medVtkView;
};


medVtkViewItkDataImageInteractor::medVtkViewItkDataImageInteractor(medAbstractImageView* parent):
    medAbstractImageViewInteractor(parent), d(new medVtkViewItkDataImageInteractorPrivate)
{
    this->setParent(parent);

    d->medVtkView = parent;
    medVtkViewBackend* backend = dynamic_cast<medVtkViewBackend*>(parent->backend());
    d->view2d = backend->view2D;
    d->view3d = backend->view3D;

    d->data = NULL;
    d->layer = 0;

    d->lutParam = NULL;
    d->presetParam = NULL;
    d->opacityParam = NULL;
    d->visibiltyParameter = NULL;
}

medVtkViewItkDataImageInteractor::~medVtkViewItkDataImageInteractor()
{
    delete d;
    d = NULL;
}

QStringList medVtkViewItkDataImageInteractor::dataHandled()
{
    QStringList d = QStringList() << "itkDataImageChar3"
                                << "itkDataImageUChar3"
                                << "itkDataImageShort3"
                                << "itkDataImageUShort3"
                                << "itkDataImageInt3"
                                << "itkDataImageLong3"
                                << "itkDataImageULong3"
                                << "itkDataImageFloat3"
                                << "itkDataImageDouble3"
                                << "itkDataImageRGB3"
                                << "itkDataImageRGBA3"
                                << "itkDataImageVector3";
    return  d;
}

QStringList medVtkViewItkDataImageInteractor::handled() const
{
    return medVtkViewItkDataImageInteractor::dataHandled();
}

QString medVtkViewItkDataImageInteractor::description() const
{
    return tr("Interactor for ITK images");
}


QString medVtkViewItkDataImageInteractor::identifier() const
{
    return "medVtkViewItkDataImageInteractor";
}

bool medVtkViewItkDataImageInteractor::registered()
{
    medImageViewFactory *factory = medImageViewFactory::instance();
    return factory->registerInteractor<medVtkViewItkDataImageInteractor>("medVtkViewItkDataImageInteractor",
                                                                  QStringList () << "medVtkView" <<
                                                                  medVtkViewItkDataImageInteractor::dataHandled());
                    }


void medVtkViewItkDataImageInteractor::setData(medAbstractData *data)
{
    d->layer = d->medVtkView->layer(data);

    if (!(SetViewInput<itk::Image<char,3> >("itkDataImageChar3", data, d->layer) ||
          SetViewInput<itk::Image<unsigned char,3> >("itkDataImageUChar3", data, d->layer) ||
          SetViewInput<itk::Image<short,3> >("itkDataImageShort3", data, d->layer) ||
          SetViewInput<itk::Image<unsigned short,3> >("itkDataImageUShort3", data, d->layer) ||
          SetViewInput<itk::Image<int,3> >("itkDataImageInt3", data, d->layer) ||
          SetViewInput<itk::Image<unsigned,3> >("itkDataImageUInt3", data, d->layer) ||
          SetViewInput<itk::Image<long,3> >("itkDataImageLong3", data, d->layer) ||
          SetViewInput<itk::Image<unsigned long,3> >("itkDataImageULong3", data, d->layer) ||
          SetViewInput<itk::Image<float,3> >("itkDataImageFloat3", data ,d->layer) ||
          SetViewInput<itk::Image<double,3> >("itkDataImageDouble3", data, d->layer) ||
          SetViewInput<itk::Image<itk::RGBPixel<unsigned char>,3> >("itkDataImageRGB3", data, d->layer) ||
          SetViewInput<itk::Image<itk::RGBAPixel<unsigned char>,3> >("itkDataImageRGBA3", data, d->layer) ||
          SetViewInput<itk::Image<itk::Vector<unsigned char,3>,3> >("itkDataImageVector3", data, d->layer)))
    {
        qDebug() << "Unable to add data: " << data->identifier() << " to view " << this->identifier();
        return;
    }

    medStringListParameter *lutParam = new medStringListParameter("Lut", this);
    QStringList lut = QStringList() << "Default" << "Black & White" << "Black & White Inversed"
                                     << "Spectrum" << "Hot Metal" << "Hot Green"
                                     << "Hot Iron" << "GE" << "Flow" << "Loni" << "Loni 2"
                                     << "Asymmetry" << "P-Value" << "Red Black Alpha"
                                     << "Green Black Alpha" << "Blue Black Alpha"
                                     << "Muscles & Bones" << "Bones" << "Red Vessels"
                                     << "Cardiac" << "Gray Rainbow" << "Stern" << "Black Body";
    d->lutParam->addItems(lut);
    connect(d->presetParam, SIGNAL(valueChanged(QString)), this, SLOT(setPreset(QString)));


    medStringListParameter *presetParam = new medStringListParameter("Preset", this);
    QStringList presets = QStringList() << "None" << "VR Muscles&Bones" << "Vascular I"
                                        << "Vascular II" << "Vascular III" << "Vascular IV"
                                        << "Standard" << "Soft" << "Soft on White"
                                        << "Soft on Blue" << "Red on White" << "Glossy" ;
    d->presetParam->addItems(presets);

    connect(d->lutParam, SIGNAL(valueChanged(QString)), this, SLOT(setLut(QString)));

    d->opacityParam = new medIntParameter("Opacity", this);
    d->opacityParam->setRange(0, 100);
    d->opacityParam->setValue(100);
    connect(d->opacityParam, SIGNAL(valueChanged(int)), this, SLOT(setOpacity));

    d->visibiltyParameter = new medBoolParameter("Visibility", this);
    d->visibiltyParameter->setValue(true);
    connect(d->visibiltyParameter, SIGNAL(valueChanged(bool)), this, SLOT(setVisibility(bool)));
}


template <typename IMAGE>
bool medVtkViewItkDataImageInteractor::SetViewInput(const char* type, medAbstractData* data,const int layer)
{
    if (data->identifier() != type)
        return false;

    if (IMAGE* image = dynamic_cast<IMAGE*>((itk::Object*)(data->data())))
    {
        d->view2d->SetITKInput(image, d->layer);
        d->view3d->SetITKInput(image, d->layer);
        return true;
    }
    return false;
}


void medVtkViewItkDataImageInteractor::setOpacity(double value)
{
    double opacity = static_cast<double>(value) / 100.0;
    d->view2d->SetOpacity (opacity, d->layer);
    d->view3d->SetOpacity (opacity, d->layer);
}

double medVtkViewItkDataImageInteractor::opacity() const
{
     double opacity = static_cast<double>(d->opacityParam->value()) / 100.0;
     return opacity;
}

void medVtkViewItkDataImageInteractor::setVisibility(bool visible)
{
    if(visible)
    {
        d->view2d->SetVisibility(1, d->layer);
        d->view3d->SetVisibility(1, d->layer);
    }
    else
    {
        d->view2d->SetVisibility(0, d->layer);
        d->view3d->SetVisibility(0, d->layer);
    }
}

bool medVtkViewItkDataImageInteractor::visibility() const
{
    return d->visibiltyParameter->value();
}

QString medVtkViewItkDataImageInteractor::lut() const
{
    return d->lutParam->value();
}

void medVtkViewItkDataImageInteractor::setLut(QString value)
{
    typedef vtkTransferFunctionPresets Presets;
    vtkColorTransferFunction * rgb   = vtkColorTransferFunction::New();
    vtkPiecewiseFunction     * alpha = vtkPiecewiseFunction::New();

    Presets::GetTransferFunction(value.toStdString(), rgb, alpha );

    if (d->layer == 0)
        d->view2d->SetTransferFunctions(rgb, alpha, d->layer);
    else
    {
        //TODO: find out why its not the same process - RDE
        vtkLookupTable *lut = vtkLookupTableManager::GetLookupTable(value.toStdString());
        d->view2d->SetLookupTable(lut, d->layer);
        lut->Delete();
    }

    d->view3d->SetTransferFunctions(rgb, alpha, d->layer);
    d->view3d->SetLookupTable(vtkLookupTableManager::GetLookupTable(value.toStdString()), d->layer);

    rgb->Delete();
    alpha->Delete();
}

void medVtkViewItkDataImageInteractor::setPreset(QString preset)
{
    //TODO complete - RDE
//    if(preset == "None")
//    {
//        // we reset the LUT and the ww/wl to the default values

//        this->setLut(layer, "Black & White");

//        double color[3] = {0.0, 0.0, 0.0};

//        d->collection->SyncSetBackground ( color );
//        d->collection->SyncResetWindowLevel(0);
//    }
//    else if ( preset == "VR Muscles&Bones" )
//    {
//        this->setLUT ( layer, "Muscles & Bones" );

//        double color[3] = {0.0, 0.0, 0.0};

//        d->collection->SyncSetBackground ( color );
//        d->collection->SyncSetColorWindow ( 337.0, 0, 1 );
//        d->collection->SyncSetColorLevel ( 1237.0, 0, 1 );
//    }
//    else if ( preset == "Vascular I" )
//    {
//        this->setLut ( layer, "Stern" );

//        double color[3] = {0.0, 0.0, 0.0};

//        d->collection->SyncSetBackground ( color );
//        d->collection->SyncSetColorWindow ( 388.8, 0, 1 );
//        d->collection->SyncSetColorLevel ( 362.9, 0, 1 );
//    }
//    else if ( preset == "Vascular II" )
//    {
//        this->setLut ( layer, "Red Vessels" );

//        double color[3] = {0.0, 0.0, 0.0};

//        d->collection->SyncSetBackground ( color );
//        d->collection->SyncSetColorWindow ( 189.6, 0, 1 );
//        d->collection->SyncSetColorLevel ( 262.3, 0, 1 );
//    }
//    else if ( preset == "Vascular III" )
//    {
//        this->setLut ( layer, "Red Vessels" );

//        double color[3] = {0.0, 0.0, 0.0};

//        d->collection->SyncSetBackground ( color );
//        d->collection->SyncSetColorWindow ( 284.4, 0, 1 );
//        d->collection->SyncSetColorLevel ( 341.7, 0, 1 );
//    }
//    else if ( preset == "Vascular IV" )
//    {
//        this->setLut ( layer, "Red Vessels" );

//        double color[3] = {0.0, 0.0, 0.0};

//        d->collection->SyncSetBackground ( color );
//        d->collection->SyncSetColorWindow ( 272.5, 0, 1 );
//        d->collection->SyncSetColorLevel ( 310.9, 0, 1 );
//    }
//    else if ( preset == "Standard" )
//    {
//        this->setLUT ( layer, "Muscles & Bones" );

//        double color[3] = {0.0, 0.0, 0.0};

//        d->collection->SyncSetBackground ( color );
//        d->collection->SyncSetColorWindow ( 243.7, 0, 1 );
//        d->collection->SyncSetColorLevel ( 199.6, 0, 1 );
//    }
//    else if ( preset == "Soft" )
//    {
//        this->setLUT ( layer, "Bones" );

//        double color[3] = {0.0, 0.0, 0.0};

//        d->collection->SyncSetBackground ( color );
//        d->collection->SyncSetColorWindow ( 133.5, 0, 1 );
//        d->collection->SyncSetColorLevel ( 163.4, 0, 1 );
//    }
//    else if ( preset == "Soft on White" )
//    {
//        this->setLUT ( layer, "Muscles & Bones" );

//        double color[3] = {1.0,0.98820477724075317,0.98814374208450317};

//        d->collection->SyncSetBackground ( color );
//        d->collection->SyncSetColorWindow ( 449.3, 0, 1 );
//        d->collection->SyncSetColorLevel ( 372.8, 0, 1 );
//    }
//    else if ( preset == "Soft on Blue" )
//    {
//        this->setLUT ( layer, "Muscles & Bones" );

//        double color[3]={0.0, 0.27507439255714417, 0.26398107409477234};

//        d->collection->SyncSetBackground ( color );
//        d->collection->SyncSetColorWindow ( 449.3, 0, 1 );
//        d->collection->SyncSetColorLevel ( 372.8, 0, 1 );
//    }
//    else if ( preset == "Red on White" )
//    {
//        this->setLUT ( layer, "Red Vessels" );

//        double color[3]={1.0, 0.98820477724075317, 0.98814374208450317};

//        d->collection->SyncSetBackground ( color );
//        d->collection->SyncSetColorWindow ( 449.3, 0, 1 );
//        d->collection->SyncSetColorLevel ( 372.8, 0, 1 );
//    }
//    else if ( preset == "Glossy" )
//    {
//        this->setLUT ( layer, "Bones" );

//        double color[3] = {0.0, 0.0, 0.0};

//        d->collection->SyncSetBackground ( color );
//        d->collection->SyncSetColorWindow ( 133.5, 0, 1 );
//        d->collection->SyncSetColorLevel ( 163.4, 0, 1 );
//    }
//    else
//    {
//        return; // to prevent trigger of event lutChanged()
//    }
}

QString medVtkViewItkDataImageInteractor::preset() const
{
  return d->presetParam->value();
}


QWidget* medVtkViewItkDataImageInteractor::toolBarWidget()
{
    return new QWidget;
}

QWidget* medVtkViewItkDataImageInteractor::toolBoxWidget()
{
    return new QWidget;
}

QWidget* medVtkViewItkDataImageInteractor::layerWidget()
{
    return new QWidget;
}

void medVtkViewItkDataImageInteractor::setWindowLevel (double &window, double &level)
{
    qDebug() << "setWindowLevel"  << window << "," << level;
}

void medVtkViewItkDataImageInteractor::windowLevel(double &window, double &level)
{
    qDebug() << "windowLevel"  << window << "," << level;
}

void medVtkViewItkDataImageInteractor::moveToSliceAtPosition(const QVector3D &position)
{
    qDebug() << "moveToSliceAtPosition"  << position;
}


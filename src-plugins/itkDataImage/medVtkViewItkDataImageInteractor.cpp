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
#include <vtkRenderWindow.h>
#include <vtkImageData.h>

#include <QVTKWidget2.h>

#include <medVtkViewBackend.h>
#include <medAbstractData.h>
#include <medImageViewFactory.h>
#include <medStringListParameter.h>
#include <medIntParameter.h>
#include <medBoolParameter.h>
#include <medDoubleParameter.h>



class medVtkViewItkDataImageInteractorPrivate
{
public:
    // views
    vtkImageView2D *view2d;
    vtkImageView3D *view3d;
    vtkImageViewCollection *collection;
    medAbstractData *data;
    vtkRenderWindow *render;
    QVTKWidget2 *qvtkWidget;

    unsigned int layer;
    medAbstractImageView *medVtkView;

    medStringListParameter *lutParam;
    medStringListParameter *presetParam;
    medIntParameter *opacityParam;
    medBoolParameter *visibiltyParameter;
    medDoubleParameter *windowParameter;
    medDoubleParameter *levelParameter;
};


medVtkViewItkDataImageInteractor::medVtkViewItkDataImageInteractor(medAbstractImageView* parent):
    medAbstractImageViewInteractor(parent), d(new medVtkViewItkDataImageInteractorPrivate)
{
    d->medVtkView = parent;
    medVtkViewBackend* backend = static_cast<medVtkViewBackend*>(parent->backend());
    d->view2d = backend->view2D;
    d->view3d = backend->view3D;
    d->collection = vtkImageViewCollection::New();
    d->collection->AddItem(d->view2d);
    d->collection->AddItem(d->view3d);
    d->render = backend->renWin;
    d->qvtkWidget = backend->qvtkWidget;

    d->data = NULL;
    d->layer = 0;

    d->lutParam = NULL;
    d->presetParam = NULL;
    d->opacityParam = NULL;
    d->visibiltyParameter = NULL;
    d->windowParameter = NULL;
    d->levelParameter = NULL;
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

    d->lutParam = new medStringListParameter("Lut", this);
    QStringList lut = QStringList() << "Default" << "Black & White" << "Black & White Inversed"
                                     << "Spectrum" << "Hot Metal" << "Hot Green"
                                     << "Hot Iron" << "GE" << "Flow" << "Loni" << "Loni 2"
                                     << "Asymmetry" << "P-Value" << "Red Black Alpha"
                                     << "Green Black Alpha" << "Blue Black Alpha"
                                     << "Muscles & Bones" << "Bones" << "Red Vessels"
                                     << "Cardiac" << "Gray Rainbow" << "Stern" << "Black Body";
    d->lutParam->addItems(lut);
    connect(d->presetParam, SIGNAL(valueChanged(QString)), this, SLOT(setPreset(QString)));


    d->presetParam = new medStringListParameter("Preset", this);
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

    d->windowParameter = new medDoubleParameter("Window", this);
    d->levelParameter = new medDoubleParameter("Level", this);

//    d->view2d->ResetWindowLevel();

//    d->view2d->SetCurrentLayer(d->layer);

    d->view2d->ResetWindowLevel();
    double* range = d->view2d->GetInput()->GetScalarRange();
    double window = range[1]-range[0];
    double level = 0.5*(range[1]+range[0]);

    d->windowParameter->setRange(range[0], range[1]);
    d->levelParameter->setRange(range[0], range[1]);

    qDebug() << "range" << range[0] << " , " << range[1];
    qDebug() << "window " << window  << " level " <<level;

    connect(d->windowParameter, SIGNAL(valueChanged(double)), this, SLOT(setWindow(double)));
    connect(d->levelParameter, SIGNAL(valueChanged(double)), this, SLOT(setLevel(double)));

    d->windowParameter->setValue(window);
    d->levelParameter->setValue(level);
}


template <typename IMAGE>
bool medVtkViewItkDataImageInteractor::SetViewInput(const char* type, medAbstractData* data,const int layer)
{
    if (data->identifier() != type)
        return false;

    if (IMAGE* image = dynamic_cast<IMAGE*>((itk::Object*)(data->data())))
    {
        d->view2d->SetITKInput(image, layer);
        d->view3d->SetITKInput(image, layer);
        return true;
    }
    return false;
}


void medVtkViewItkDataImageInteractor::setOpacity(double value)
{
    double opacity = static_cast<double>(value) / 100.0;
    d->view3d->SetOpacity (opacity, d->layer);
    d->view2d->SetOpacity (opacity, d->layer);
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
    //TODO to complete wat for theo stuff ?) - RDE
    DTK_UNUSED(preset);
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
    d->windowParameter->setValue(window);
    d->levelParameter->setValue(level);
}

void medVtkViewItkDataImageInteractor::windowLevel(double &window, double &level)
{
    window = d->windowParameter->value();
    level = d->levelParameter->value();
}

void medVtkViewItkDataImageInteractor::moveToSliceAtPosition(const QVector3D &position)
{
    qDebug() << "moveToSliceAtPosition"  << position;
}

void medVtkViewItkDataImageInteractor::setWindow(double window)
{
    d->view2d->SetColorWindow(window, d->layer);
    d->view3d->SetColorWindow(window, d->layer);
}

void medVtkViewItkDataImageInteractor::setLevel(double level)
{
    d->view2d->SetColorLevel(level, d->layer);
    d->view3d->SetColorLevel(level, d->layer);
}

void medVtkViewItkDataImageInteractor::update()
{
    qDebug() << "update:" << this->view()->identifier() << " ask by " << this->identifier() << "from d->layer " << d->layer;
    if(d->medVtkView->is2D())
        d->view2d->Render();
    else
        d->view3d;

    d->qvtkWidget->update();
}

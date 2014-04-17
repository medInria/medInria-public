#include "medVtkViewItkVectorFieldInteractor.h"

#include <QVTKWidget.h>

#include <vtkPointData.h>
#include <vtkActor.h>
#include <vtkRenderer.h>
#include <vtkAssignAttribute.h>
#include <vtkVectorManager.h>
#include <vtkImageView2D.h>
#include <vtkImageView3D.h>

#include <itkVector.h>
#include <itkImage.h>
#include <itkImageToVTKImageFilter.h>

#include <medAbstractImageView.h>
#include <medViewFactory.h>
#include <medVtkViewBackend.h>
#include <medAbstractParameter.h>
#include <medDoubleParameter.h>
#include <medIntParameter.h>
#include <medStringListParameter.h>
#include <medBoolParameter.h>


const int ImageDimension = 3;
typedef itk::Image< itk::Vector< float, 3 >,  ImageDimension >   VectorFloatImageType;
typedef itk::Image< itk::Vector< double, 3 >,  ImageDimension >   VectorDoubleImageType;

typedef itk::ImageToVTKImageFilter< VectorFloatImageType > FloatFilterType;
typedef itk::ImageToVTKImageFilter< VectorDoubleImageType > DoubleFilterType;

class medVtkViewItkVectorFieldInteractorPrivate
{
public:
    medAbstractData* data;
    medAbstractImageView *view;
    vtkImageView2D *view2d;
    vtkImageView3D *view3d;
    vtkRenderWindow *render;
    vtkRenderer *renderer2d;
    vtkRenderer *renderer3d;

    QWidget *toolbox;

    QList <medAbstractParameter*> parameters;

    QImage thumbnail;

    VectorFloatImageType::Pointer    floatData;
    VectorDoubleImageType::Pointer   doubleData;
    FloatFilterType::Pointer         floatFilter;
    DoubleFilterType::Pointer        doubleFilter;

    vtkVectorManager                 *manager;

    double imageBounds[6];
};

medVtkViewItkVectorFieldInteractor::medVtkViewItkVectorFieldInteractor(medAbstractView *parent):
    medAbstractImageViewInteractor(parent),
    d(new medVtkViewItkVectorFieldInteractorPrivate)
{
    d->data    = 0;

    d->view = dynamic_cast<medAbstractImageView*>(parent);

    medVtkViewBackend* backend = static_cast<medVtkViewBackend*>(parent->backend());
    d->view2d = backend->view2D;
    d->view3d = backend->view3D;
    d->render = backend->renWin;
    d->renderer2d = backend->renderer2D;
    d->renderer3d = backend->renderer3D;

    d->manager = vtkVectorManager::New();
    d->manager->SetRenderWindowInteractor(d->render->GetInteractor(),d->renderer3d);

    connect(d->view, SIGNAL(positionViewedChanged(QVector3D)), this, SLOT(setPosition(QVector3D)));

    d->toolbox = NULL;

    for (int i=0; i<6; i++)
        d->imageBounds[i] = 0;
}


medVtkViewItkVectorFieldInteractor::~medVtkViewItkVectorFieldInteractor()
{
    delete d;
    d = 0;
}

QString medVtkViewItkVectorFieldInteractor::description() const
{
    return tr("Interactor helping display vector fields");
}

QString medVtkViewItkVectorFieldInteractor::identifier() const
{
    return "medVtkViewItkVectorFieldInteractor";
}

QStringList medVtkViewItkVectorFieldInteractor::handled() const
{
    return medVtkViewItkVectorFieldInteractor::dataHandled();
}

QStringList medVtkViewItkVectorFieldInteractor::dataHandled()
{
    QStringList datahandled = QStringList() << "itkDataImageVectorFloat3" << "itkDataImageVectorDouble3";
    return  datahandled;
}

bool medVtkViewItkVectorFieldInteractor::registered()
{
    medViewFactory *factory = medViewFactory::instance();
    factory->registerInteractor<medVtkViewItkVectorFieldInteractor>("medVtkViewItkVectorFieldInteractor",
                                                                     QStringList () << "medVtkView" <<
                                                                     medVtkViewItkVectorFieldInteractor::dataHandled());
    return true;
}

void medVtkViewItkVectorFieldInteractor::setData(medAbstractData *data)
{
    if (!data)
        return;

    QString identifier = data->identifier();

    d->data = data;

    vtkMatrix4x4 *mat = vtkMatrix4x4::New(); //start with identity matrix

    if( identifier.compare("itkDataImageVectorFloat3") == 0 )
    {
        typedef    itk::Vector< float, 3 >    InputPixelType;
        typedef itk::Image< InputPixelType,  ImageDimension >   ImageType;

        d->floatData = static_cast<ImageType* >(data->data());

        ImageType::DirectionType direction = d->floatData->GetDirection();
        ImageType::PointType i_origin =  d->floatData->GetOrigin();

        for (int i=0; i<3; i++)
            for (int k=0; k<3; k++)
                mat->SetElement(i,k, direction(i,k));

        double v_origin[4], v_origin2[4];
        for (int i=0; i<3; i++)
          v_origin[i] = i_origin[i];
        v_origin[3] = 1.0;
        mat->MultiplyPoint (v_origin, v_origin2);
        for (int i=0; i<3; i++)
          mat->SetElement (i, 3, v_origin[i]-v_origin2[i]);

        d->floatFilter = FloatFilterType::New();
        d->floatFilter->SetInput( d->floatData );
        d->floatFilter->Update();

        vtkImageData *vtkImage = d->floatFilter->GetOutput();

        d->manager->SetInput(vtkImage);
    }

    else if( identifier.compare("itkDataImageVectorDouble3") == 0 )
    {
        typedef    itk::Vector< double, 3 >    InputPixelType;
        typedef itk::Image< InputPixelType,  ImageDimension >   ImageType;

        d->doubleData = static_cast<ImageType* >(data->data());

        ImageType::DirectionType direction = d->doubleData->GetDirection();
        ImageType::PointType i_origin =  d->doubleData->GetOrigin();

        for (int i=0; i<3; i++)
            for (int k=0; k<3; k++)
                mat->SetElement(i,k, direction(i,k));

        double v_origin[4], v_origin2[4];
        for (int i=0; i<3; i++)
          v_origin[i] = i_origin[i];
        v_origin[3] = 1.0;
        mat->MultiplyPoint (v_origin, v_origin2);
        for (int i=0; i<3; i++)
          mat->SetElement (i, 3, v_origin[i]-v_origin2[i]);

        d->doubleFilter = DoubleFilterType::New();
        d->doubleFilter->SetInput( d->doubleData );
        d->doubleFilter->Update();

        vtkImageData *vtkImage = d->doubleFilter->GetOutput();

        d->manager->SetInput(vtkImage);
    }

    else return;

    d->manager->SetDirectionMatrix(mat);
    d->manager->ResetPosition();
    d->manager->Update();

    int *dim = d->manager->GetInput()->GetDimensions();
    d->view2d->SetInput(d->manager->GetVectorVisuManagerAxial()->GetActor(), d->view->layer(data), dim);
    d->view2d->SetInput(d->manager->GetVectorVisuManagerSagittal()->GetActor(), d->view->layer(data), dim);
    d->view2d->SetInput(d->manager->GetVectorVisuManagerCoronal()->GetActor(), d->view->layer(data), dim);

    setupParameters();

    update();
}

void medVtkViewItkVectorFieldInteractor::removeData()
{
    d->view2d->RemoveLayer(d->view->layer(d->data));
    d->manager->Delete();
}

void medVtkViewItkVectorFieldInteractor::setupParameters()
{
    medDoubleParameter *scaleFactor = new medDoubleParameter("Scale", this);
    scaleFactor->setRange(1,10);
    scaleFactor->setValue(1.0);

    medIntParameter *sampleRateControl = new medIntParameter("Sample Rate", this);
    sampleRateControl->setRange(1,10);
    sampleRateControl->setValue(1);

    medStringListParameter *colorMode = new medStringListParameter("Color mode", this);
    QStringList colorModes = QStringList() << "Vector Magnitude" << "Vector Direction";
    colorMode->addItems(colorModes);
    colorMode->setValue("Vector Magnitude");

    medBoolParameter *projection = new medBoolParameter("Projection", this);

    d->parameters.append(scaleFactor);
    d->parameters.append(sampleRateControl);
    d->parameters.append(colorMode);
    d->parameters.append(projection);

    connect(scaleFactor, SIGNAL(valueChanged(double)), this, SLOT(setScale(double)));
    connect(sampleRateControl,SIGNAL(valueChanged(int)),this,SLOT(setSampleRate(int)));
    connect(colorMode, SIGNAL(valueChanged(QString)), this, SLOT(setColorMode(QString)));
    connect(projection, SIGNAL(valueChanged(bool)), this, SLOT(setProjection(bool)));

    if(d->view->layer(d->data) == 0)
    {
        switch(d->view2d->GetViewOrientation())
        {
        case vtkImageView2D::VIEW_ORIENTATION_AXIAL:
            d->view->setOrientation(medImageView::VIEW_ORIENTATION_AXIAL);
            break;
        case vtkImageView2D::VIEW_ORIENTATION_SAGITTAL:
            d->view->setOrientation(medImageView::VIEW_ORIENTATION_SAGITTAL);
            break;
        case vtkImageView2D::VIEW_ORIENTATION_CORONAL:
            d->view->setOrientation(medImageView::VIEW_ORIENTATION_CORONAL);
            break;
        }
    }
}


void medVtkViewItkVectorFieldInteractor::setWindowLevel(double &window, double &level)
{
    //TODO
}

void medVtkViewItkVectorFieldInteractor::windowLevel(double &window, double &level)
{
    //TODO
}

void medVtkViewItkVectorFieldInteractor::setOpacity(double opacity)
{
    //TODO
}

double medVtkViewItkVectorFieldInteractor::opacity() const
{
    //TODO
    return 100;
}

void medVtkViewItkVectorFieldInteractor::setVisibility(bool visibility)
{
    int v = (visibility) ? 1 : 0;

    d->manager->GetVectorVisuManagerAxial()->GetActor()->SetVisibility(v);
    d->manager->GetVectorVisuManagerSagittal()->GetActor()->SetVisibility(v);
    d->manager->GetVectorVisuManagerCoronal()->GetActor()->SetVisibility(v);

    this->update();
}

bool medVtkViewItkVectorFieldInteractor::visibility() const
{
    return (d->manager->GetVectorVisuManagerAxial()->GetActor()->GetVisibility() == 1);
}

void medVtkViewItkVectorFieldInteractor::setScale(double scale)
{
    d->manager->SetGlyphScale((float)scale);
    this->update();
}

void medVtkViewItkVectorFieldInteractor::setSampleRate(int sampleRate)
{
    d->manager->SetSampleRate(sampleRate,sampleRate,sampleRate);
    this->update();
}

void medVtkViewItkVectorFieldInteractor::setColorMode(QString mode)
{
    if(mode == "Vector Magnitude")
        d->manager->SetColorMode(0);
    else if(mode == "Vector Direction")
        d->manager->SetColorMode(1);
    this->update();
}


void medVtkViewItkVectorFieldInteractor::setProjection(bool enable)
{
    d->manager->SetProjection(enable);
    this->update();
}


void medVtkViewItkVectorFieldInteractor::setPosition(const QVector3D& position)
{
    d->manager->SetCurrentPosition(position.x(), position.y(), position.z());
    this->update();
}

void medVtkViewItkVectorFieldInteractor::setShowAxial(bool show)
{
    if(show)
        d->manager->SetAxialSliceVisibility(1);
    else
        d->manager->SetAxialSliceVisibility(0);

    this->update();
}

void medVtkViewItkVectorFieldInteractor::setShowCoronal(bool show)
{
    if(show)
        d->manager->SetCoronalSliceVisibility(1);
    else
        d->manager->SetCoronalSliceVisibility(0);

    this->update();
}

void medVtkViewItkVectorFieldInteractor::setShowSagittal(bool show)
{
    if(show)
        d->manager->SetSagittalSliceVisibility(1);
    else
        d->manager->SetSagittalSliceVisibility(0);

    this->update();
}


QImage medVtkViewItkVectorFieldInteractor::generateThumbnail(const QSize &size)
{
    int w(size.width()), h(size.height());

    d->view2d->SetBackground(0.0, 0.0, 0.0);
    d->view2d->CursorFollowMouseOff();
    d->view2d->ShowImageAxisOff();
    d->view2d->ShowScalarBarOff();
    d->view2d->ShowAnnotationsOff();
    d->view2d->ShowRulerWidgetOff();

    vtkRenderWindow *renWin = vtkRenderWindow::New();
    renWin->SetOffScreenRendering(1);
    renWin->AddRenderer(d->renderer2d);
    d->view2d->SetRenderWindow(renWin);
    QVTKWidget *vtkWidget = dynamic_cast<QVTKWidget *>(d->view->viewWidget());
    if(!vtkWidget)
        return d->thumbnail;

    vtkWidget->SetRenderWindow(renWin);
    vtkWidget->resize(w,h);
    renWin->SetSize(w,h);
    d->view2d->Reset();
    renWin->Render();

    d->thumbnail = QPixmap::grabWidget(vtkWidget).toImage();

    renWin->Delete();

    return d->thumbnail;
}

void medVtkViewItkVectorFieldInteractor::moveToSliceAtPosition(const QVector3D &position)
{
    //TODO
}

void medVtkViewItkVectorFieldInteractor::moveToSlice(int slice)
{
    //TODO
}

QWidget* medVtkViewItkVectorFieldInteractor::layerWidget()
{
    return new QWidget;
}

QWidget* medVtkViewItkVectorFieldInteractor::toolBoxWidget()
{
    if(!d->toolbox)
    {
        d->toolbox = new QWidget;
        QFormLayout *layout = new QFormLayout(d->toolbox);
        foreach(medAbstractParameter *parameter, d->parameters)
            layout->addRow(parameter->getLabel(), parameter->getWidget());
    }
    return d->toolbox;
}

QWidget* medVtkViewItkVectorFieldInteractor::toolBarWidget()
{
    return new QWidget;
}

QList<medAbstractParameter*> medVtkViewItkVectorFieldInteractor::parameters()
{
    return d->parameters;
}

void medVtkViewItkVectorFieldInteractor::update()
{
    d->render->Render();
}

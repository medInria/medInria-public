/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

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
#include <medAbstractImageData.h>


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

    QList <medAbstractParameter*> parameters;

    VectorFloatImageType::Pointer    floatData;
    VectorDoubleImageType::Pointer   doubleData;
    FloatFilterType::Pointer         floatFilter;
    DoubleFilterType::Pointer        doubleFilter;

    vtkVectorManager                 *manager;

    medIntParameter *slicingParameter;

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

    d->manager = vtkVectorManager::New();
    d->manager->SetRenderWindowInteractor(d->render->GetInteractor(), d->view3d->GetRenderer());

    connect(d->view->positionBeingViewedParameter(), SIGNAL(valueChanged(QVector3D)), this, SLOT(setPosition(QVector3D)));

    for (int i=0; i<6; i++)
        d->imageBounds[i] = 0;

    d->slicingParameter = new medIntParameter("Slicing", this);
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

void medVtkViewItkVectorFieldInteractor::setInputData(medAbstractData *data)
{
    medAbstractInteractor::setInputData(data);

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
        d->floatFilter->UpdateOutputInformation();
        d->floatFilter->Update();

        vtkImageData *vtkImage = d->floatFilter->GetOutput();
        d->manager->SetInput(vtkImage);
    }

    else if( identifier.compare("itkDataImageVectorDouble3") == 0 )
    {
        typedef    itk::Vector< double, 3 >    InputPixelType;
        typedef itk::Image< InputPixelType,  ImageDimension >   ImageType;

        d->doubleData = static_cast<ImageType* >(data->data());

        d->doubleData->UpdateOutputInformation();
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
        d->doubleFilter->UpdateOutputInformation();
        d->doubleFilter->Update();

        vtkImageData *vtkImage = d->doubleFilter->GetOutput();
        d->manager->SetInput(vtkImage);
    }

    else return;
    d->manager->SetDirectionMatrix(mat);
    d->manager->ResetPosition();
    d->manager->Update();
    int *dim;
    dim = d->manager->GetInput()->GetDimensions();

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
    scaleFactor->setRange(0.001,10);
    scaleFactor->setValue(1.0);

    medIntParameter *sampleRateControl = new medIntParameter("Sample Rate", this);
    sampleRateControl->setRange(1,10);
    sampleRateControl->setValue(1);

    medStringListParameter *colorMode = new medStringListParameter("Color mode", this);
    colorMode->addItem("Vector Magnitude");
    colorMode->addItem("Vector Direction");
    colorMode->addItem("Red", medStringListParameter::createIconFromColor("red"));
    colorMode->addItem("Green", medStringListParameter::createIconFromColor("green"));
    colorMode->addItem("Blue", medStringListParameter::createIconFromColor("blue"));
    colorMode->addItem("Yellow", medStringListParameter::createIconFromColor("yellow"));
    colorMode->addItem("White", medStringListParameter::createIconFromColor("white"));
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


    //TODO - should be done automaticly from vtkImageView - RDE
//    if(d->view->layer(d->data) == 0)
//    {
//        switch(d->view2d->GetSliceOrientation())
//        {
//        case 0:
//            d->view->setOrientation(medImageView::VIEW_ORIENTATION_AXIAL);
//            break;
//        case 1:
//            d->view->setOrientation(medImageView::VIEW_ORIENTATION_CORONAL);
//            break;
//        case 2:
//            d->view->setOrientation(medImageView::VIEW_ORIENTATION_SAGITTAL);
//            break;
//        }
//    }

    connect(d->slicingParameter, SIGNAL(valueChanged(int)), this, SLOT(moveToSlice(int)));
    connect(d->view->positionBeingViewedParameter(), SIGNAL(valueChanged(QVector3D)), this, SLOT(updateSlicingParam()));
    connect(d->view, SIGNAL(orientationChanged()), this, SLOT(updatePlaneVisibility()));

    this->updateWidgets();
}


void medVtkViewItkVectorFieldInteractor::setWindowLevel(QHash<QString,QVariant>)
{
    //TODO
}

void medVtkViewItkVectorFieldInteractor::setOpacity(double opacity)
{
    //TODO
}

void medVtkViewItkVectorFieldInteractor::setVisibility(bool visibility)
{
    int v = (visibility) ? 1 : 0;

    d->manager->GetVectorVisuManagerAxial()->GetActor()->SetVisibility(v);
    d->manager->GetVectorVisuManagerSagittal()->GetActor()->SetVisibility(v);
    d->manager->GetVectorVisuManagerCoronal()->GetActor()->SetVisibility(v);

    this->update();
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
    if(mode == "Vector Magnitude") {
        d->manager->SetColorMode(vtkVectorVisuManager::ColorByVectorMagnitude);
    } else if(mode == "Vector Direction") {
        d->manager->SetColorMode(vtkVectorVisuManager::ColorByVectorDirection);
    } else {
        QColor color(mode.toLower()); // not great...
        double colorF[3]; color.getRgbF(&(colorF[0]), &(colorF[1]), &(colorF[2]));
        d->manager->SetUserColor(colorF);
        d->manager->SetColorMode(vtkVectorVisuManager::ColorByUserColor);
    }
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


void medVtkViewItkVectorFieldInteractor::setUpViewForThumbnail()
{
    d->view2d->Reset();
    d->view2d->SetBackground(0.0, 0.0, 0.0);
    d->view2d->CursorFollowMouseOff();
    d->view2d->ShowImageAxisOff();
    d->view2d->ShowScalarBarOff();
    d->view2d->ShowAnnotationsOff();
    d->view2d->ShowRulerWidgetOff();
}


void medVtkViewItkVectorFieldInteractor::moveToSlice(int slice)
{
    // TODO find a way to get woorldCoordinate for slice from vtkInria.
    //  instead of moving to the slice corresponding on the first layer dropped.
    if(d->view->is2D() && slice != d->view2d->GetSlice())
    {
        d->view2d->SetSlice(slice);
        d->view2d->Render();
    }
}

QWidget* medVtkViewItkVectorFieldInteractor::buildLayerWidget()
{
    return new QWidget;
}

QWidget* medVtkViewItkVectorFieldInteractor::buildToolBoxWidget()
{
    QWidget *toolbox = new QWidget;
    QFormLayout *layout = new QFormLayout(toolbox);
    foreach(medAbstractParameter *parameter, d->parameters)
        layout->addRow(parameter->getLabel(), parameter->getWidget());

    return toolbox;
}

QWidget* medVtkViewItkVectorFieldInteractor::buildToolBarWidget()
{
    d->slicingParameter->getSlider()->setOrientation(Qt::Horizontal);
    return d->slicingParameter->getSlider();
}

QList<medAbstractParameter*> medVtkViewItkVectorFieldInteractor::linkableParameters()
{
    QList <medAbstractParameter*> linkableParams = d->parameters;
    linkableParams << this->visibilityParameter();
    return linkableParams;
}

QList<medBoolParameter*> medVtkViewItkVectorFieldInteractor::mouseInteractionParameters()
{
    // no parameters related to mouse interactions
    return QList<medBoolParameter*>();
}

void medVtkViewItkVectorFieldInteractor::update()
{
    d->view->render();
}

void medVtkViewItkVectorFieldInteractor::updateWidgets()
{
    if(!d->view->is2D())
        d->slicingParameter->getSlider()->setEnabled(false);
    else
    {
        d->slicingParameter->getSlider()->setEnabled(true);
        this->updateSlicingParam();
    }
}

void medVtkViewItkVectorFieldInteractor::updateSlicingParam()
{
    if(!d->view->is2D())
        return;

    //TODO Should be set according to the real number of slice of this data and
    // not according to vtkInria (ie. first layer droped) - RDE

    // slice orientation may differ from view orientation. Adapt slider range accordingly.
//    int orientationId = d->view2d->GetSliceOrientation();
//    int *dim = d->manager->GetInput()->GetDimensions();

//    if (orientationId==vtkImageView2D::SLICE_ORIENTATION_XY)
//        d->slicingParameter->setRange(0, dim[2] - 1);
//    else if (orientationId==vtkImageView2D::SLICE_ORIENTATION_XZ)
//        d->slicingParameter->setRange (0, dim[1] - 1);
//    else if (orientationId==vtkImageView2D::SLICE_ORIENTATION_YZ)
//        d->slicingParameter->setRange (0, dim[0] - 1);

    d->slicingParameter->blockSignals(true);
    d->slicingParameter->setRange(d->view2d->GetSliceMin(), d->view2d->GetSliceMax());
    d->slicingParameter->blockSignals(false);

    d->slicingParameter->setValue(d->view2d->GetSlice());
}

void medVtkViewItkVectorFieldInteractor::updatePlaneVisibility()
{

    //TODO make it workes even if the SLICE_ORIENTATION is not X/Y
    //     (ie the acquisition orientaion of the image is something else than axial)
    //     - RDE

//    switch(d->view->orientation())
//    {
//        case medImageView::VIEW_ORIENTATION_3D:
//        d->manager->SetAxialSliceVisibility(0);
//        d->manager->SetCoronalSliceVisibility(0);
//        d->manager->SetSagittalSliceVisibility(1);
//        d->manager->SetProjection(false);
//        break;
//    case medImageView::VIEW_ORIENTATION_AXIAL:
//        d->manager->SetAxialSliceVisibility(1);
//        d->manager->SetCoronalSliceVisibility(1);
//        d->manager->SetSagittalSliceVisibility(1);
//        d->manager->SetProjection(true);
//        break;
//    case medImageView::VIEW_ORIENTATION_CORONAL:
//        d->manager->SetAxialSliceVisibility(1);
//        d->manager->SetCoronalSliceVisibility(1);
//        d->manager->SetSagittalSliceVisibility(1);
//        d->manager->SetProjection(true);
//        break;
//    case medImageView::VIEW_ORIENTATION_SAGITTAL:
//        d->manager->SetAxialSliceVisibility(1);
//        d->manager->SetCoronalSliceVisibility(1);
//        d->manager->SetSagittalSliceVisibility(1);
//        d->manager->SetProjection(true);
//        break;
//    }

//    this->update();
}

QString medVtkViewItkVectorFieldInteractor::name() const
{
    return "medVtkViewItkVectorFieldInteractor";
}

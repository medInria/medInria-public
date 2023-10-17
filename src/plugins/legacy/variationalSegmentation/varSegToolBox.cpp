/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2019. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <dtkCoreSupport/dtkAbstractProcessFactory.h>

#include <itkCastImageFilter.h>

#include <medAbstractData.h>
#include <medAbstractDataFactory.h>
#include <medAbstractLayeredView.h>
#include <medAbstractProcessLegacy.h>
#include <medDataManager.h>
#include <medPluginManager.h>
#include <medRunnableProcess.h>
#include <medTabbedViewContainers.h>
#include <medUtilities.h>
#include <medViewContainer.h>
#include <medVtkViewBackend.h>

#include <varSegToolBox.h>
#include <vtkCollection.h>
#include <vtkLandmarkWidget.h>
#include <vtkRenderWindowInteractor.h>

namespace mseg
{

class VarSegToolBoxPrivate
{
public:
    vtkLandmarkSegmentationController *controller;
    medAbstractLayeredView *currentView;
    QList<medAbstractLayeredView*> medViews;
    QList<vtkImageView2D*> *views2D;
    QList<vtkImageView3D*> *views3D;
    QPushButton *segButton;
    QPushButton *binaryImageButton;
    QPushButton *applyMaskButton;
    QPushButton *mprMode;
    dtkSmartPointer<medAbstractData> originalInput;
    dtkSmartPointer<medAbstractData> output;
    int inputSize[3];
    bool segOn;
    dtkSmartPointer <medAbstractProcessLegacy> process;
};

VarSegToolBox::VarSegToolBox(QWidget *parent )
    : medAbstractSelectableToolBox(parent)
    , d(new VarSegToolBoxPrivate)
{
    QWidget *displayWidget = new QWidget(this);
    this->addWidget(displayWidget);

    QVBoxLayout *layout = new QVBoxLayout(displayWidget);

    QLabel *inside = new QLabel(QString("Inside VOI: \n\tShift + right mouse button"), displayWidget);
    QLabel *outside = new QLabel(QString("On VOI: \n\tShift + left mouse button"), displayWidget);
    QLabel *on = new QLabel(QString("Outside VOI: \n\tShift + Ctrl + right mouse button \n\tor Shift + middle mouse button"), displayWidget);
    QLabel *deleteLabel = new QLabel(QString("To delete a landmark: \n\tCtrl + left mouse button on the landmark"), displayWidget);

    d->segButton = new QPushButton(tr("Start Segmentation"), displayWidget);
    d->segButton->setCheckable(true);
    d->segButton->setObjectName("segButton");

    d->binaryImageButton = new QPushButton(tr("Save Mask"), displayWidget);
    d->binaryImageButton->setToolTip("Import the current mask to the non persistent database");
    d->binaryImageButton->setObjectName("binaryImageButton");

    d->applyMaskButton = new QPushButton(tr("Apply and Save Segmentation"), displayWidget);
    d->applyMaskButton->setToolTip("Apply the mask to the image, save the segmentation");
    d->applyMaskButton->setObjectName("applyMaskButton");

    enableButtons(false);

    layout->addWidget(d->segButton);
    layout->addWidget(inside);
    layout->addWidget(outside);
    layout->addWidget(on);
    layout->addWidget(deleteLabel);
    layout->addWidget(d->binaryImageButton);
    layout->addWidget(d->applyMaskButton);

    connect(d->segButton,         SIGNAL(toggled(bool)), this, SLOT(segmentation(bool)), Qt::UniqueConnection);
    connect(d->binaryImageButton, SIGNAL(clicked()),     this, SLOT(addBinaryImage()),   Qt::UniqueConnection);
    connect(d->applyMaskButton,   SIGNAL(clicked()),     this, SLOT(applyMaskToImage()), Qt::UniqueConnection);

    d->controller = vtkLandmarkSegmentationController::New();
    d->output = medAbstractDataFactory::instance()->createSmartPointer("itkDataImageUChar3");
    d->currentView = nullptr;
    d->segOn = false;

    d->views2D = new QList<vtkImageView2D*>();
    d->views3D = new QList<vtkImageView3D*>();
}

VarSegToolBox::~VarSegToolBox()
{
    d->controller->Delete();
    delete d;
}

//static
medAbstractSelectableToolBox *VarSegToolBox::createInstance(QWidget *parent)
{
    return new VarSegToolBox(parent);
}

QString VarSegToolBox::s_description()
{
    static const QString desc = "Variational Segmentation Tool";
    return desc;
}

QString VarSegToolBox::s_identifier()
{
    static const QString id = "mseg::VarSegToolBox";
    return id;
}

QString VarSegToolBox::s_name()
{
    return "Variational Segmentation";
}

dtkPlugin* VarSegToolBox::plugin()
{
    return medPluginManager::instance().plugin("Variational Segmentation");
}

void VarSegToolBox::updateLandmarksRenderer(QString key, QString value)
{
    if (key != "Orientation")
    {
        return;
    }
    
    medAbstractView *v = qobject_cast<medAbstractView*>(this->sender());
    if (v != d->currentView || !d->controller)
    {
        return;
    }
    
    vtkRenderWindowInteractor *interactor = static_cast<medVtkViewBackend*>(v->backend())->renWin->GetInteractor();

    vtkCollection *landmarks = d->controller->GetTotalLandmarkCollection();
    landmarks->InitTraversal();
    vtkLandmarkWidget *l = vtkLandmarkWidget::SafeDownCast(landmarks->GetNextItemAsObject());
    
    while(l)
    {
        if (!d->controller->RemoveConstraint (l))
        {
            if ( (l->GetInteractor() == interactor))
            {
                if (value=="3D")
                {
                    l->On();
                }
                else if (d->controller->getMode3D()) // test if previous orientation was 3d
                {
                    if (l->GetCurrentRenderer())
                    {
                        l->Off();
                    }
                }
            }
        }
        
        l = vtkLandmarkWidget::SafeDownCast(landmarks->GetNextItemAsObject());
    }
    
    if (value == "3D")
    {
        d->controller->setMode3D(true);
    }
    else
    {
        d->controller->setMode3D(false);
    }
    
    landmarks->InitTraversal();
    l = vtkLandmarkWidget::SafeDownCast(landmarks->GetNextItemAsObject());
    while(l)
    {
        l->showOrHide2DWidget();
        l = vtkLandmarkWidget::SafeDownCast(landmarks->GetNextItemAsObject());
    }
}

void VarSegToolBox::addBinaryImage()
{
    typedef itk::Image<unsigned char, 3> binaryType;
    binaryType::Pointer img = d->controller->GetBinaryImage();

    if (img)
    {
        d->output->setData(img);

        medUtilities::setDerivedMetaData(d->output, d->originalInput, "VarSegMask");
        medDataManager::instance().importData(d->output, false);
    }
}

void VarSegToolBox::applyMaskToImage()
{
    if (d->currentView)
    {
        this->setToolBoxOnWaitStatus();

        d->process = qobject_cast<medAbstractProcessLegacy*>(dtkAbstractProcessFactory::instance()->create("medMaskApplication"));

        typedef itk::Image<unsigned char,3> binaryType;
        binaryType::Pointer img = d->controller->GetBinaryImage();

        if (img)
        {
            medAbstractData *maskData = medAbstractDataFactory::instance()->create( "itkDataImageUChar3" );
            maskData->setData(img);
            d->process->setInput(maskData, 0);
            d->process->setInput(static_cast<medAbstractData*>(d->currentView->layerData(0)), 1);

            medRunnableProcess *runProcess = new medRunnableProcess;
            runProcess->setProcess (d->process);
            connect (runProcess, SIGNAL (success  (QObject*)), this, SLOT(displayOutput()), Qt::UniqueConnection);
            this->addConnectionsAndStartJob(runProcess);
            enableOnProcessSuccessImportOutput(runProcess, false);
        }
        else
        {
            this->displayMessageError("The input data is empty");
            this->setToolBoxOnReadyToUse();
        }
    }
}

void VarSegToolBox::displayOutput()
{
    medUtilities::setDerivedMetaData(d->process->output(), d->originalInput, "VarSegApplied");
    medDataManager::instance().importData(d->process->output(), false);

    typedef itk::Image<unsigned char, 3> binaryType;
    binaryType::Pointer img = d->controller->GetBinaryImage();
    if (img)
    {
        d->output->setData(img);
        medUtilities::setDerivedMetaData(d->output, d->originalInput, "VarSegMask");
    }

    medTabbedViewContainers *tabbedViewContainers = getWorkspace()->tabbedViewContainers();
    medViewContainer *viewContainer = tabbedViewContainers->containersInTab(tabbedViewContainers->currentIndex()).at(0);
    viewContainer->removeView();
    viewContainer->setMultiLayered(true);
    viewContainer->addData(d->process->output());
}

void VarSegToolBox::updateView()
{
    medAbstractView *view = this->getWorkspace()->tabbedViewContainers()->getFirstSelectedContainerView();

    medAbstractLayeredView *layeredView = qobject_cast<medAbstractLayeredView*>(view);

    if (layeredView && !d->segOn)
    {
        // if the segmentation is activated we do not change the currentView.
        // The currentView is supposed to be the view in which the segmentation is being done.
        d->currentView = layeredView;

        // Toolbox does not work with meshes or vector images
        for (unsigned int i=0; i<d->currentView->layersCount(); ++i)
        {
            medAbstractData *data = d->currentView->layerData(i);
            if(!data || data->identifier().contains("vtkDataMesh")
                    || !data->identifier().contains("itkDataImage") //avoid medVtkFibersData also
                    || data->identifier().contains("itkDataImageVector"))
            {
                handleDisplayError(medAbstractProcessLegacy::DIMENSION_3D);
                d->currentView = nullptr;
                return;
            }
        }
    }
    else
    {
        d->currentView = nullptr;
    }
}

void VarSegToolBox::clear()
{
    endSegmentation();
}

void VarSegToolBox::startSegmentation()
{
    if (!d->currentView)
    {
        d->segButton->setChecked(false);
        this->displayMessageError("A data is needed in the view");
        return;
    }

    d->medViews.clear();
    d->views2D->clear();
    d->views3D->clear();

    connect(d->currentView, SIGNAL(propertySet(QString,QString)), this, SLOT(updateLandmarksRenderer(QString,QString)), Qt::UniqueConnection);
    connect(d->currentView, SIGNAL(closed()), this, SLOT(manageClosingView()), Qt::UniqueConnection);

    vtkCollection *interactorcollection = vtkCollection::New();
    interactorcollection->AddItem(static_cast<medVtkViewBackend*>(d->currentView->backend())->renWin->GetInteractor());
    d->controller->SetInteractorCollection(interactorcollection);
    interactorcollection->Delete();

    d->controller->EnabledOn();

    typedef vtkLandmarkSegmentationController::ImageType ImageType;
    ImageType::Pointer image;

    d->originalInput = d->currentView->layerData(0);

    if (!d->originalInput) return;

    if (d->originalInput->identifier() == "itkDataImageShort3")
    {
        image = castImageToType<short>(d->originalInput);
    }
    else if (d->originalInput->identifier() == "itkDataImageUShort3")
    {
        image = castImageToType<unsigned short>(d->originalInput);
    }
    else if (d->originalInput->identifier() == "itkDataImageDouble3")
    {
        image = castImageToType<double>(d->originalInput);
    }
    else if (d->originalInput->identifier() == "itkDataImageFloat3")
    {
        image = castImageToType<float>(d->originalInput);
    }
    else if (d->originalInput->identifier() == "itkDataImageInt3")
    {
        image = castImageToType<int>(d->originalInput);
    }
    else if (d->originalInput->identifier() == "itkDataImageUChar3")
    {
        image = castImageToType<unsigned char>(d->originalInput);
    }
    else
    {
        handleDisplayError(medAbstractProcessLegacy::PIXEL_TYPE);
        endSegmentation();
        return;
    }

    ImageType::Pointer smallerImage;

    ImageType::SizeType imageSize = image->GetLargestPossibleRegion().GetSize(); ;
    ImageType::SpacingType imageSpacing  = image->GetSpacing();

    double res = 25;
    ImageType::IndexType corner = {{0,0,0}};
    double smallestSpacing = std::min(imageSpacing[0], std::min(imageSpacing[1], imageSpacing[2]));
    double mSpacing[3];
    for (unsigned int i = 0; i < 3; i++)
    {
        mSpacing[i] = 100 * smallestSpacing / res;
    }

    int mDim[3];
    for (unsigned int i = 0; i < 3; i++)
    {
        mDim[i] = (int) (imageSize[i] * imageSpacing[i] / mSpacing[i]);
        d->inputSize[i]=imageSize[i];
    }
    d->controller->setOutputSize(d->inputSize[0], d->inputSize[1], d->inputSize[2]);

    ImageType::SpacingType NewSpacing;
    ImageType::SizeType NewSize;
    NewSpacing[0] = mSpacing[0];
    NewSpacing[1] = mSpacing[1];
    NewSpacing[2] = mSpacing[2];
    NewSize[0] = mDim[0];
    NewSize[1] = mDim[1];
    NewSize[2] = mDim[2];
    ImageType::RegionType region(corner,NewSize);
    smallerImage = ImageType::New();
    smallerImage->SetRegions(region);
    smallerImage->Allocate();
    smallerImage->SetDirection(image->GetDirection());
    smallerImage->SetOrigin(image->GetOrigin());
    smallerImage->SetSpacing(NewSpacing);
    
    d->controller->SetInput(smallerImage);
    
    d->medViews.append(d->currentView);
    d->views2D->append(static_cast<medVtkViewBackend*>(d->currentView->backend())->view2D);
    d->views3D->append(static_cast<medVtkViewBackend*>(d->currentView->backend())->view3D);

    for (int i = 0; i<d->medViews.size(); i++)
    {
        d->views2D->at(i)->AddDataSet (d->controller->GetOutput());
        d->views3D->at(i)->AddDataSet (d->controller->GetOutput());
    }

    d->controller->setViews2D(d->views2D);
    d->controller->setViews3D(d->views3D);
    
    enableButtons(true);
    d->segOn = true;
}

template <typename PixelType>
typename vtkLandmarkSegmentationController::ImageType::Pointer
VarSegToolBox::castImageToType(medAbstractData *inputData)
{
    typedef itk::Image<PixelType, 3> InputImage;
    typename InputImage::Pointer imgData = dynamic_cast< InputImage*>((itk::Object*)(inputData->data()));

    typedef itk::CastImageFilter< InputImage, vtkLandmarkSegmentationController::ImageType > CasterType;
    typename CasterType::Pointer caster = CasterType::New();
    caster->SetInput(imgData);
    caster->Update();
    return caster->GetOutput();
}

void VarSegToolBox::manageClosingView()
{
    d->currentView = nullptr;
    d->medViews.clear();
    endSegmentation();
}

void VarSegToolBox::enableButtons(bool isEnabled)
{
    d->binaryImageButton->setEnabled(isEnabled);
    d->applyMaskButton->setEnabled(isEnabled);
}

void VarSegToolBox::endSegmentation()
{
    if (d->segButton->isChecked())
    {
        d->segButton->setChecked(false);
    }

    d->segButton->setText("Start Segmentation");
    d->segOn = false;

    if (d->controller)
    {
        for (int i = 0; i<d->medViews.size(); i++)
        {
            if(d->currentView)
            {
                d->views2D->at(i)->RemoveDataSet (d->controller->GetOutput());
                d->views3D->at(i)->RemoveDataSet (d->controller->GetOutput());
            }
        }

        d->controller->EnabledOff();
        d->controller->DeleteAllLandmarks();
        d->controller->Delete();

        d->controller = vtkLandmarkSegmentationController::New();
        updateView();

        enableButtons(false);

        medAbstractView *view = this->getWorkspace()->tabbedViewContainers()->getFirstSelectedContainerView();
        if (view && (view->count()>0))
        {
            view->render();
        }
    }
}

void VarSegToolBox::segmentation(bool checked)
{
    if (checked)
    {
        d->segButton->setText("End Segmentation");
        startSegmentation();
    }
    else
    {
        endSegmentation();
    }
}

medAbstractData* VarSegToolBox::processOutput()
{
    return d->output;
}

} // namespace mseg


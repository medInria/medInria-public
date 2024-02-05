/*=========================================================================

 medInria

 Copyright (c) INRIA 2019. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/
#include "resliceToolBox.h"
#include "medResliceViewer.h"

#include <medAbstractImageData.h>
#include <medAbstractLayeredView.h>
#include <medDataManager.h>
#include <medDoubleParameterL.h>
#include <medMessageController.h>
#include <medPluginManager.h>
#include <medTabbedViewContainers.h>
#include <medToolBoxFactory.h>
#include <medViewContainer.h>
#include <medVtkViewBackend.h>

class resliceToolBoxPrivate
{
public:
    QPushButton *b_startReslice, *b_stopReslice, *b_saveImage, *b_reset;
    QComboBox *bySpacingOrSize;
    QLabel *helpBegin;
    medDoubleParameterL *spacingOrSizeX, *spacingOrSizeY, *spacingOrSizeZ;
    medResliceViewer *resliceViewer;
    dtkSmartPointer<medAbstractData> reformatedImage;
    QWidget *reformatOptions;
    std::array<double, 3> spacing;
    std::array<int, 3> dimensions;
    medAbstractData *originalImage;
};

resliceToolBox::resliceToolBox (QWidget *parent) : medAbstractSelectableToolBox (parent), d(new resliceToolBoxPrivate)
{
    QWidget *resliceToolBoxBody = new QWidget();
    this->addWidget(resliceToolBoxBody);

    QVBoxLayout *resliceToolBoxLayout =  new QVBoxLayout(resliceToolBoxBody);

    d->b_startReslice = new QPushButton("Start Reslice");
    d->b_startReslice->setCheckable(false);
    d->b_startReslice->setObjectName("startReformatButton");
    resliceToolBoxLayout->addWidget(d->b_startReslice);

    d->b_stopReslice = new QPushButton("Stop Reslice");
    d->b_stopReslice->setCheckable(false);
    d->b_stopReslice->setObjectName("stopReformatButton");

    d->b_saveImage = new QPushButton("Save Image");
    d->b_saveImage->setCheckable(false);
    d->b_saveImage->setObjectName("saveImageButton");

    // User can choose pixel or millimeter resample
    QHBoxLayout * resampleLayout = new QHBoxLayout();
    QLabel *bySpacingOrSizeLabel = new QLabel("Select your resample parameter ");
    d->bySpacingOrSize = new QComboBox();
    d->bySpacingOrSize->setObjectName("bySpacingOrSize");
    d->bySpacingOrSize->addItem("Spacing");
    d->bySpacingOrSize->addItem("Size");
    resampleLayout->addWidget(bySpacingOrSizeLabel);
    resampleLayout->addWidget(d->bySpacingOrSize);
    connect(d->bySpacingOrSize, SIGNAL(currentIndexChanged(const QString&)), this, SLOT(switchSpacingAndDimension(const QString&)));

    // Spinboxes of resample values
    QVBoxLayout *spacingSpinBoxLayout = new QVBoxLayout();

    QHBoxLayout *spacingSpinBoxLayoutX = new QHBoxLayout();
    spacingSpinBoxLayoutX->setAlignment(Qt::AlignCenter);
    d->spacingOrSizeX = new medDoubleParameterL("X", this);
    d->spacingOrSizeX->getSpinBox()->setAccessibleName("SpacingOrSizeX");
    d->spacingOrSizeX->setObjectName("SpacingOrSizeX");
    d->spacingOrSizeX->setRange(0,100);
    d->spacingOrSizeX->getSpinBox()->setSuffix(" mm");
    d->spacingOrSizeX->setSingleStep(0.1f);
    spacingSpinBoxLayoutX->addWidget(d->spacingOrSizeX->getLabel());
    spacingSpinBoxLayoutX->addWidget(d->spacingOrSizeX->getSpinBox());
    spacingSpinBoxLayout->addLayout(spacingSpinBoxLayoutX);

    QHBoxLayout *spacingSpinBoxLayoutY = new QHBoxLayout();
    spacingSpinBoxLayoutY->setAlignment(Qt::AlignCenter);
    d->spacingOrSizeY = new medDoubleParameterL("Y", this);
    d->spacingOrSizeY->getSpinBox()->setAccessibleName("SpacingOrSizeY");
    d->spacingOrSizeY->setObjectName("SpacingOrSizeY");
    d->spacingOrSizeY->setRange(0,100);
    d->spacingOrSizeY->getSpinBox()->setSuffix(" mm");
    d->spacingOrSizeY->setSingleStep(0.1f);
    spacingSpinBoxLayoutY->addWidget(d->spacingOrSizeY->getLabel());
    spacingSpinBoxLayoutY->addWidget(d->spacingOrSizeY->getSpinBox());
    spacingSpinBoxLayout->addLayout(spacingSpinBoxLayoutY);

    QHBoxLayout *spacingSpinBoxLayoutZ = new QHBoxLayout();
    spacingSpinBoxLayoutZ->setAlignment(Qt::AlignCenter);
    d->spacingOrSizeZ = new medDoubleParameterL("Z", this);
    d->spacingOrSizeZ->getSpinBox()->setAccessibleName("SpacingOrSizeZ");
    d->spacingOrSizeZ->setObjectName("SpacingOrSizeZ");
    d->spacingOrSizeZ->setRange(0,100);
    d->spacingOrSizeZ->getSpinBox()->setSuffix(" mm");
    d->spacingOrSizeZ->setSingleStep(0.1f);
    spacingSpinBoxLayoutZ->addWidget(d->spacingOrSizeZ->getLabel());
    spacingSpinBoxLayoutZ->addWidget(d->spacingOrSizeZ->getSpinBox());
    spacingSpinBoxLayout->addLayout(spacingSpinBoxLayoutZ);
    
    d->helpBegin = new QLabel("Drop a data in the view and click on 'Start Reslice'.");
    d->helpBegin->setObjectName("helpBegin");
    d->helpBegin->setStyleSheet("font: italic");
    resliceToolBoxLayout->addWidget(d->helpBegin);

    QLabel *help1 = new QLabel("To change the windowing left click on a 2D view.\n"
                               "To reset axes in a view press 'o'.\n"
                               "To reset windowing in a view press 'r'.\n"
                                );
    help1->setStyleSheet("font: italic");

    d->reformatOptions = new QWidget();
    QVBoxLayout *reformatOptionsLayout = new QVBoxLayout(d->reformatOptions);
    d->reformatOptions->setLayout(reformatOptionsLayout);
    d->reformatOptions->hide();

    resliceToolBoxLayout->addWidget(d->reformatOptions);
    reformatOptionsLayout->addWidget(help1);
    reformatOptionsLayout->addLayout(resampleLayout);
    reformatOptionsLayout->addLayout(spacingSpinBoxLayout);
    reformatOptionsLayout->addWidget(d->b_saveImage);
    reformatOptionsLayout->addWidget(d->b_stopReslice);
    resliceToolBoxBody->setLayout(resliceToolBoxLayout);

    // Connections
    connect(d->b_startReslice,SIGNAL(clicked()),this,SLOT(startReformat()));
    connect(d->b_stopReslice,SIGNAL(clicked()),this,SLOT(stopReformat()));

    d->resliceViewer = nullptr;
    d->originalImage = nullptr;
}
resliceToolBox::~resliceToolBox()
{
    delete d->resliceViewer;
    d->resliceViewer = nullptr;
    d->originalImage = nullptr;

    delete d;
    d = nullptr;
}

bool resliceToolBox::registered()
{
    return medToolBoxFactory::instance()->registerToolBox<resliceToolBox>();
}

dtkPlugin* resliceToolBox::plugin()
{
    return medPluginManager::instance().plugin("Reformat");
}

void resliceToolBox::startReformat()
{
    if (getWorkspace())
    {
        auto tabbedContainers = getWorkspace()->tabbedViewContainers();
        auto container = tabbedContainers->getFirstSelectedContainer();
        auto view = qobject_cast<medAbstractLayeredView*>(tabbedContainers->getFirstSelectedContainerView());

        if (view)
        {
            medAbstractData* data = view->layerData(view->currentLayer());
            bool is3D = false;

            // Toolbox does not work with meshes or vector images
            if (data->identifier().contains("itkDataImage") &&
                    !data->identifier().contains("Vector"))
            {
                if (dynamic_cast<medAbstractImageData *>(data)->Dimension() == 3)
                {
                    is3D = true;
                }

                if (view->layersCount() && is3D)
                {
                    d->helpBegin->hide();
                    d->reformatOptions->show();
                    d->b_startReslice->hide();

                    d->resliceViewer = new medResliceViewer(view, container);
                    d->resliceViewer->setToolBox(this);

                    container->removeView();
                    container->setDefaultWidget(d->resliceViewer->viewWidget());

                    connect(d->resliceViewer,SIGNAL(imageReformatedGenerated()),
                        this,SLOT(saveReformatedImage()), Qt::UniqueConnection);
                    connect(d->spacingOrSizeX, SIGNAL(valueChanged(double)), 
                        d->resliceViewer, SLOT(askedSpacingOrSizeChange(double)), Qt::UniqueConnection);
                    connect(d->spacingOrSizeY, SIGNAL(valueChanged(double)), 
                        d->resliceViewer, SLOT(askedSpacingOrSizeChange(double)), Qt::UniqueConnection);
                    connect(d->spacingOrSizeZ, SIGNAL(valueChanged(double)), 
                        d->resliceViewer, SLOT(askedSpacingOrSizeChange(double)), Qt::UniqueConnection);
                    connect(d->b_saveImage, SIGNAL(clicked()), 
                        d->resliceViewer, SLOT(saveImage()), Qt::UniqueConnection);

                    d->reformatedImage = nullptr;
                }
                else
                {
                    medMessageController::instance().showError(tr("Drop a 3D volume in the view"), 3000);
                }
            }
            else
            {
                medMessageController::instance().showError(tr("Drop a 3D volume in the view"), 3000);
            }
        }
        else
        {
            medMessageController::instance().showError(tr("Drop a 3D volume in the view"), 3000);
        }
    }
}

void resliceToolBox::stopReformat()
{
    if (getWorkspace() && d->resliceViewer)
    {
        d->bySpacingOrSize->setCurrentIndex(0); // init resample parameter
        d->helpBegin->show();
        d->reformatOptions->hide();
        d->b_startReslice->show();

        auto tabbedContainers = getWorkspace()->tabbedViewContainers();
        auto container = tabbedContainers->getFirstSelectedContainer();
        container->initializeDefaultWidget();

        if (d->originalImage)
        {
            container->addData(d->originalImage);
        }

        disconnect(d->resliceViewer);
        delete d->resliceViewer;
        d->resliceViewer = nullptr;

        QList<medToolBox*> toolBoxes = getWorkspace()->toolBoxes();
        for (int i = 0; i < toolBoxes.length(); i++)
        {
            toolBoxes[i]->show();
        }
    }
}

void resliceToolBox::clear()
{
    stopReformat();
}

void resliceToolBox::updateView()
{
    // Even if updateView is called because of an empty view, we want to keep the original data,
    // so the toolbox can be reset to the original one.
    medAbstractView* view = this->getWorkspace()->tabbedViewContainers()->getFirstSelectedContainerView();
    if (view)
    {
        medAbstractLayeredView *layeredView = qobject_cast<medAbstractLayeredView*>(view);
        if (auto data = dynamic_cast<medAbstractImageData*>(layeredView->layerData(layeredView->currentLayer())))
        {
            if (data->Dimension() == 3)
            {
                d->originalImage = data;

                // Copy original image information and display them
                vtkImageView2D *view2d = static_cast<medVtkViewBackend*>(layeredView->backend())->view2D;
                auto imageInfo = view2d->GetMedVtkImageInfo();
                memcpy(d->spacing.data(),    imageInfo->spacing,    3*sizeof(double));
                memcpy(d->dimensions.data(), imageInfo->dimensions, 3*sizeof(int));

                displayInfoOnCurrentView();
            }
        }
    }
}

void resliceToolBox::displayInfoOnCurrentView()
{
    if (d->bySpacingOrSize->currentIndex() == 0) // Spacing
    {
        d->spacingOrSizeX->setValue(d->spacing[0]);
        d->spacingOrSizeY->setValue(d->spacing[1]);
        d->spacingOrSizeZ->setValue(d->spacing[2]);
    }
    else // Dimension
    {
        d->spacingOrSizeX->setValue(d->dimensions[0]);
        d->spacingOrSizeY->setValue(d->dimensions[1]);
        d->spacingOrSizeZ->setValue(d->dimensions[2]);
    }
}

void resliceToolBox::saveReformatedImage()
{
    generateReformatedImage();
    if (d->reformatedImage && d->reformatedImage->data())
    {
        medDataManager::instance().importData(d->reformatedImage, false);
    }
}

void resliceToolBox::generateReformatedImage()
{
    if (d->resliceViewer)
    {
        d->reformatedImage = d->resliceViewer->getOutput();
    }
}

void resliceToolBox::switchSpacingAndDimension(const QString & value)
{
    if (value == "Spacing")
    {
        d->spacingOrSizeX->getSpinBox()->setSuffix(" mm");
        d->spacingOrSizeX->setRange(0, 100);
        d->spacingOrSizeX->setSingleStep(0.1f);
        d->spacingOrSizeY->getSpinBox()->setSuffix(" mm");
        d->spacingOrSizeY->setRange(0, 100);
        d->spacingOrSizeY->setSingleStep(0.1f);
        d->spacingOrSizeZ->getSpinBox()->setSuffix(" mm");
        d->spacingOrSizeZ->setRange(0, 100);
        d->spacingOrSizeZ->setSingleStep(0.1f);
    }
    else // Dimension
    {
        d->spacingOrSizeX->getSpinBox()->setSuffix(" px");
        d->spacingOrSizeX->setRange(0, 10000);
        d->spacingOrSizeX->setSingleStep(10);
        d->spacingOrSizeY->getSpinBox()->setSuffix(" px");
        d->spacingOrSizeY->setRange(0, 10000);
        d->spacingOrSizeY->setSingleStep(10);
        d->spacingOrSizeZ->getSpinBox()->setSuffix(" px");
        d->spacingOrSizeZ->setRange(0, 10000);
        d->spacingOrSizeZ->setSingleStep(10);
    }
    displayInfoOnCurrentView();
}

medAbstractData *resliceToolBox::processOutput()
{
    if (!d->reformatedImage)
    {
        generateReformatedImage();
    }
    return d->reformatedImage;
}

void resliceToolBox::changeButtonValue(QString buttonName, double value)
{
    if (buttonName == "SpacingOrSizeX")
    {
        d->spacingOrSizeX->setValue(value);
    }
    else if (buttonName == "SpacingOrSizeY")
    {
        d->spacingOrSizeY->setValue(value);
    }
    else if (buttonName == "SpacingOrSizeZ")
    {
        d->spacingOrSizeZ->setValue(value);
    }
}

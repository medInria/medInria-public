/*=========================================================================

 medInria

 Copyright (c) INRIA 2019. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/
#include "resliceToolBox.h"

#include <medAbstractImageData.h>
#include <medAbstractLayeredView.h>
#include <medDataManager.h>
#include <medDoubleParameterL.h>
#include <medMessageController.h>
#include <medPluginManager.h>
#include <medResliceViewer.h>
#include <medTabbedViewContainers.h>
#include <medToolBoxFactory.h>
#include <medViewContainer.h>
#include <medVtkViewBackend.h>

class resliceToolBoxPrivate
{
public:
    QPushButton *b_startReslice, *b_stopReslice, *b_saveImage, *b_reset;
    QComboBox *bySpacingOrDimension;
    QLabel *helpBegin;
    medDoubleParameterL *spacingX, *spacingY, *spacingZ;
    medAbstractLayeredView *currentView;
    medResliceViewer *resliceViewer;
    dtkSmartPointer<medAbstractData> reformatedImage;
    QWidget *reformatOptions;
};

resliceToolBox::resliceToolBox (QWidget *parent) : medAbstractSelectableToolBox (parent), d(new resliceToolBoxPrivate)
{
    // Fill the toolBox
    QWidget *resliceToolBoxBody = new QWidget(this);
    d->b_startReslice = new QPushButton("Start Reslice", resliceToolBoxBody);
    d->b_startReslice->setCheckable(false);
    d->b_startReslice->setObjectName("startReformatButton");
    d->b_stopReslice = new QPushButton("Stop Reslice", resliceToolBoxBody);
    d->b_stopReslice->setCheckable(false);
    d->b_stopReslice->setObjectName("stopReformatButton");
    d->b_saveImage = new QPushButton("Save Image", resliceToolBoxBody);
    d->b_saveImage->setCheckable(false);
    d->b_saveImage->setObjectName("saveImageButton");

    // User can choose pixel or millimeter resample
    QHBoxLayout * resampleLayout = new QHBoxLayout();
    QLabel *bySpacingOrDimensionLabel = new QLabel("Select your resample parameter ", resliceToolBoxBody);
    d->bySpacingOrDimension = new QComboBox(resliceToolBoxBody);
    d->bySpacingOrDimension->setObjectName("bySpacingOrDimension");
    d->bySpacingOrDimension->addItem("Spacing");
    d->bySpacingOrDimension->addItem("Dimension");
    resampleLayout->addWidget(bySpacingOrDimensionLabel);
    resampleLayout->addWidget(d->bySpacingOrDimension);
    connect(d->bySpacingOrDimension, SIGNAL(currentIndexChanged(const QString&)), this, SLOT(switchSpacingAndDimension(const QString&)));

    // Spinboxes of resample values
    QWidget *spinBoxes = new QWidget(resliceToolBoxBody);
    QVBoxLayout *spacingSpinBoxLayout = new QVBoxLayout(resliceToolBoxBody);

    QHBoxLayout *spacingSpinBoxLayoutX = new QHBoxLayout();
    spacingSpinBoxLayoutX->setAlignment(Qt::AlignCenter);
    d->spacingX = new medDoubleParameterL("X", this);
    d->spacingX->getSpinBox()->setAccessibleName("SpacingX");
    d->spacingX->setObjectName("SpacingX");
    d->spacingX->setRange(0,100);
    d->spacingX->getSpinBox()->setSuffix(" mm");
    d->spacingX->setSingleStep(0.1f);
    spacingSpinBoxLayoutX->addWidget(d->spacingX->getLabel());
    spacingSpinBoxLayoutX->addWidget(d->spacingX->getSpinBox());
    spacingSpinBoxLayout->addLayout(spacingSpinBoxLayoutX);

    QHBoxLayout *spacingSpinBoxLayoutY = new QHBoxLayout();
    spacingSpinBoxLayoutY->setAlignment(Qt::AlignCenter);
    d->spacingY = new medDoubleParameterL("Y", this);
    d->spacingY->getSpinBox()->setAccessibleName("SpacingY");
    d->spacingY->setObjectName("SpacingY");
    d->spacingY->setRange(0,100);
    d->spacingY->getSpinBox()->setSuffix(" mm");
    d->spacingY->setSingleStep(0.1f);
    spacingSpinBoxLayoutY->addWidget(d->spacingY->getLabel());
    spacingSpinBoxLayoutY->addWidget(d->spacingY->getSpinBox());
    spacingSpinBoxLayout->addLayout(spacingSpinBoxLayoutY);

    QHBoxLayout *spacingSpinBoxLayoutZ = new QHBoxLayout();
    spacingSpinBoxLayoutZ->setAlignment(Qt::AlignCenter);
    d->spacingZ = new medDoubleParameterL("Z", this);
    d->spacingZ->getSpinBox()->setAccessibleName("SpacingZ");
    d->spacingZ->setObjectName("SpacingZ");
    d->spacingZ->setRange(0,100);
    d->spacingZ->getSpinBox()->setSuffix(" mm");
    d->spacingZ->setSingleStep(0.1f);
    spacingSpinBoxLayoutZ->addWidget(d->spacingZ->getLabel());
    spacingSpinBoxLayoutZ->addWidget(d->spacingZ->getSpinBox());
    spacingSpinBoxLayout->addLayout(spacingSpinBoxLayoutZ);

    spinBoxes->setLayout(spacingSpinBoxLayout);

    QVBoxLayout *resliceToolBoxLayout =  new QVBoxLayout(resliceToolBoxBody);
    d->helpBegin = new QLabel("Drop a data in the view and click on 'Start Reslice'.", resliceToolBoxBody);
    d->helpBegin->setObjectName("helpBegin");
    d->helpBegin->setStyleSheet("font: italic");
    resliceToolBoxLayout->addWidget(d->helpBegin);

    QLabel *help1 = new QLabel("To change the windowing left click on a 2D view.", resliceToolBoxBody);
    QLabel *help2 = new QLabel("To reset axes in a view press 'o'.",               resliceToolBoxBody);
    QLabel *help3 = new QLabel("To reset windowing in a view press 'r'.\n",        resliceToolBoxBody);
    help1->setStyleSheet("font: italic");
    help2->setStyleSheet("font: italic");
    help3->setStyleSheet("font: italic");

    resliceToolBoxLayout->addWidget(d->b_startReslice);

    d->reformatOptions = new QWidget(resliceToolBoxBody);
    QVBoxLayout *reformatOptionsLayout = new QVBoxLayout(d->reformatOptions);
    d->reformatOptions->setLayout(reformatOptionsLayout);
    d->reformatOptions->hide();

    resliceToolBoxLayout->addWidget(d->reformatOptions);
    reformatOptionsLayout->addWidget(help1);
    reformatOptionsLayout->addWidget(help2);
    reformatOptionsLayout->addWidget(help3);
    reformatOptionsLayout->addLayout(resampleLayout);
    reformatOptionsLayout->addWidget(spinBoxes);
    reformatOptionsLayout->addWidget(d->b_saveImage);
    reformatOptionsLayout->addWidget(d->b_stopReslice);
    resliceToolBoxBody->setLayout(resliceToolBoxLayout);
    this->addWidget(resliceToolBoxBody);

    // Connections
    connect(d->b_startReslice,SIGNAL(clicked()),this,SLOT(startReformat()));
    connect(d->b_stopReslice,SIGNAL(clicked()),this,SLOT(stopReformat()));

    d->resliceViewer = nullptr;
    d->currentView   = nullptr;
}
resliceToolBox::~resliceToolBox()
{
    delete d->resliceViewer;
    d->resliceViewer = nullptr;
    d->currentView = nullptr;

    delete d;
    d = nullptr;
}

bool resliceToolBox::registered()
{
    return medToolBoxFactory::instance()->registerToolBox<resliceToolBox>();
}

dtkPlugin* resliceToolBox::plugin()
{
    medPluginManager* pm = medPluginManager::instance();
    dtkPlugin* plugin = pm->plugin ( "Reformat" );
    return plugin;
}

void resliceToolBox::startReformat()
{
    if (d->currentView && getWorkspace())
    {
        medAbstractData* data = d->currentView->layerData(d->currentView->currentLayer());
        bool is3D = false;
        if ((data->identifier().contains("itkDataImage"))
                && (dynamic_cast<medAbstractImageData *>(data)->Dimension() == 3))
        {
            is3D = true;
        }

        if (d->currentView->layersCount() && is3D)
        {
            d->helpBegin->hide();
            d->reformatOptions->show();
            d->b_startReslice->hide();

            d->resliceViewer = new medResliceViewer(d->currentView,
                                                    getWorkspace()->tabbedViewContainers());
            d->resliceViewer->setToolBox(this);
            getWorkspace()->tabbedViewContainers()->setAcceptDrops(false);
            connect(d->resliceViewer,SIGNAL(imageReformatedGenerated()),this,SLOT(saveReformatedImage()));
            medViewContainer * container = getWorkspace()->tabbedViewContainers()->insertNewTab(0, "Reslice");
            getWorkspace()->tabbedViewContainers()->setCurrentIndex(0);
            container->setDefaultWidget(d->resliceViewer->viewWidget());
            connect(container, SIGNAL(viewRemoved()),this, SLOT(stopReformat()), Qt::UniqueConnection);

            connect(d->spacingX, SIGNAL(valueChanged(double)), d->resliceViewer, SLOT(thickSlabChanged(double)));
            connect(d->spacingY, SIGNAL(valueChanged(double)), d->resliceViewer, SLOT(thickSlabChanged(double)));
            connect(d->spacingZ, SIGNAL(valueChanged(double)), d->resliceViewer, SLOT(thickSlabChanged(double)));
            connect(d->b_saveImage, SIGNAL(clicked()), d->resliceViewer, SLOT(saveImage()));

            d->reformatedImage = nullptr;

            // close the initial tab which is not needed anymore
            getWorkspace()->tabbedViewContainers()->removeTab(1);
            updateView();
        }
        else
        {
            medMessageController::instance()->showError(tr("Drop a 3D volume in the view"), 3000);
        }
    }
    else
    {
        medMessageController::instance()->showError(tr("Drop a 3D volume in the view"), 3000);
    }
}

void resliceToolBox::stopReformat()
{
    if (getWorkspace())
    {
        d->bySpacingOrDimension->setCurrentIndex(0); // init resample parameter
        d->helpBegin->show();
        d->reformatOptions->hide();
        d->b_startReslice->show();

        getWorkspace()->tabbedViewContainers()->removeTab(0);
        getWorkspace()->tabbedViewContainers()->insertNewTab(0, getWorkspace()->name());
        getWorkspace()->tabbedViewContainers()->setCurrentIndex(0);

        if (d->currentView)
        {
            getWorkspace()->tabbedViewContainers()->getFirstSelectedContainer()->addData(d->currentView->layerData(0));
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
    medAbstractView* view = this->getWorkspace()->tabbedViewContainers()->getFirstSelectedContainerView();

    if (view && d->currentView!= view)
    {
        medAbstractLayeredView *layeredView = qobject_cast<medAbstractLayeredView*>(view);
        if (dynamic_cast<medAbstractImageData*>(layeredView->layerData(layeredView->currentLayer())))
        {
            d->currentView = layeredView;
            displayInfoOnCurrentView();
        }
    }
}

void resliceToolBox::displayInfoOnCurrentView()
{
    vtkImageView2D *view2d = static_cast<medVtkViewBackend*>(d->currentView->backend())->view2D;

    if (d->bySpacingOrDimension->currentIndex() == 0) // Spacing
    {
        double *spacing = view2d->GetMedVtkImageInfo()->spacing;
        d->spacingX->setValue(spacing[0]);
        d->spacingY->setValue(spacing[1]);
        d->spacingZ->setValue(spacing[2]);
    }
    else // Dimension
    {
        int *dimension = view2d->GetMedVtkImageInfo()->dimensions;
        d->spacingX->setValue(dimension[0]);
        d->spacingY->setValue(dimension[1]);
        d->spacingZ->setValue(dimension[2]);
    }
}

void resliceToolBox::saveReformatedImage()
{
    generateReformatedImage();
    if (d->reformatedImage && d->reformatedImage->data())
    {
        medDataManager::instance()->importData(d->reformatedImage, false);
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
        d->spacingX->getSpinBox()->setSuffix(" mm");
        d->spacingX->setRange(0, 100);
        d->spacingX->setSingleStep(0.1f);
        d->spacingY->getSpinBox()->setSuffix(" mm");
        d->spacingY->setRange(0, 100);
        d->spacingY->setSingleStep(0.1f);
        d->spacingZ->getSpinBox()->setSuffix(" mm");
        d->spacingZ->setRange(0, 100);
        d->spacingZ->setSingleStep(0.1f);
    }
    else // Dimension
    {
        d->spacingX->getSpinBox()->setSuffix(" px");
        d->spacingX->setRange(0, 10000);
        d->spacingX->setSingleStep(10);
        d->spacingY->getSpinBox()->setSuffix(" px");
        d->spacingY->setRange(0, 10000);
        d->spacingY->setSingleStep(10);
        d->spacingZ->getSpinBox()->setSuffix(" px");
        d->spacingZ->setRange(0, 10000);
        d->spacingZ->setSingleStep(10);
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
    if (buttonName == "SpacingX")
    {
        d->spacingX->setValue(value);
    }
    else if (buttonName == "SpacingY")
    {
        d->spacingY->setValue(value);
    }
    else if (buttonName == "SpacingZ")
    {
        d->spacingZ->setValue(value);
    }
}

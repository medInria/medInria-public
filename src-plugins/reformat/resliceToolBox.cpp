#include "resliceToolBox.h"

#include <medAbstractImageData.h>
#include <medAbstractLayeredView.h>
#include <medDataManager.h>
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
    medComboBox *bySpacingOrDimension;
    QLabel *spacingXLab, *spacingYLab, *spacingZLab, *help0;
    QDoubleSpinBox *spacingX, *spacingY, *spacingZ;
    medAbstractLayeredView * currentView;
    medResliceViewer * resliceViewer;
    dtkSmartPointer<medAbstractData> reformatedImage;
    QWidget* reformatOptions;
};

resliceToolBox::resliceToolBox (QWidget *parent) : medAbstractSelectableToolBox (parent), d(new resliceToolBoxPrivate)
{
    this->setAboutPluginVisibility(false);
    this->setAboutPluginButton(this->plugin());

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
    QLabel* bySpacingOrDimensionLabel = new QLabel("Select your resample parameter:", resliceToolBoxBody);
    d->bySpacingOrDimension = new medComboBox(resliceToolBoxBody);
    d->bySpacingOrDimension->setObjectName("bySpacingOrDimension");
    d->bySpacingOrDimension->addItem("Spacing");
    d->bySpacingOrDimension->addItem("Dimension");
    resampleLayout->addWidget(bySpacingOrDimensionLabel);
    resampleLayout->addWidget(d->bySpacingOrDimension);
    connect(d->bySpacingOrDimension, SIGNAL(currentIndexChanged(const QString&)), this, SLOT(switchSpacingAndDimension(const QString&)));

    // Spinboxes of resample values
    QWidget * spinBoxes = new QWidget(resliceToolBoxBody);
    QVBoxLayout * spacingSpinBoxLayout = new QVBoxLayout(resliceToolBoxBody);
    d->spacingXLab = new QLabel("X : ");
    d->spacingX = new QDoubleSpinBox(resliceToolBoxBody);
    d->spacingX->setAccessibleName("SpacingX");
    d->spacingX->setObjectName("SpacingX");
    d->spacingX->setRange(0,100);
    d->spacingX->setSuffix(" mm");
    d->spacingX->setSingleStep(0.1f);
    spacingSpinBoxLayout->addWidget(d->spacingXLab);
    spacingSpinBoxLayout->addWidget(d->spacingX);
    d->spacingYLab = new QLabel("Y : ");
    d->spacingY = new QDoubleSpinBox(resliceToolBoxBody);
    d->spacingY->setAccessibleName("SpacingY");
    d->spacingY->setObjectName("SpacingY");
    d->spacingY->setRange(0,100);
    d->spacingY->setSuffix(" mm");
    d->spacingY->setSingleStep(0.1f);
    spacingSpinBoxLayout->addWidget(d->spacingYLab);
    spacingSpinBoxLayout->addWidget(d->spacingY);
    d->spacingZLab = new QLabel("Z : ");
    d->spacingZ = new QDoubleSpinBox(resliceToolBoxBody);
    d->spacingZ->setAccessibleName("SpacingZ");
    d->spacingZ->setObjectName("SpacingZ");
    d->spacingZ->setRange(0,100);
    d->spacingZ->setSuffix(" mm");
    d->spacingZ->setSingleStep(0.1f);
    spacingSpinBoxLayout->addWidget(d->spacingZLab);
    spacingSpinBoxLayout->addWidget(d->spacingZ);
    spinBoxes->setLayout(spacingSpinBoxLayout);

    QVBoxLayout *resliceToolBoxLayout =  new QVBoxLayout(resliceToolBoxBody);
    d->help0 = new QLabel("Open a data volume and launch:",resliceToolBoxBody);
    d->help0->setObjectName("help0");
    resliceToolBoxLayout->addWidget(d->help0);

    QLabel * help1 = new QLabel("To reset axes in a view press 'o'",resliceToolBoxBody);
    QLabel * help2 = new QLabel("To change the windowing left click on a 2D view",resliceToolBoxBody);
    QLabel * help3 = new QLabel("To reset windowing in a view press 'r'",resliceToolBoxBody);

    resliceToolBoxLayout->addWidget(d->b_startReslice);

    d->reformatOptions = new QWidget(resliceToolBoxBody);
    QVBoxLayout* reformatOptionsLayout = new QVBoxLayout(d->reformatOptions);
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

    d->resliceViewer  = 0;
    d->currentView     = 0;
}
resliceToolBox::~resliceToolBox()
{
    delete d->resliceViewer;
    d->resliceViewer = NULL;
    d->currentView = 0;

    delete d;
    d = NULL;
}

bool resliceToolBox::registered()
{
    return medToolBoxFactory::instance()->registerToolBox<resliceToolBox>();
}

dtkPlugin* resliceToolBox::plugin()
{
    medPluginManager* pm = medPluginManager::instance();
    dtkPlugin* plugin = pm->plugin ( "reslicePlugin" );
    return plugin;
}

void resliceToolBox::startReformat()
{
    if (d->currentView && getWorkspace())
    {
        d->help0->hide();
        d->reformatOptions->show();
        d->b_startReslice->hide();

        if (!d->currentView->layersCount()) return;

        d->resliceViewer = new medResliceViewer(d->currentView,getWorkspace()->stackedViewContainers());
        d->resliceViewer->setToolBox(this);
        getWorkspace()->stackedViewContainers()->setAcceptDrops(false);
        connect(d->resliceViewer,SIGNAL(imageReformatedGenerated()),this,SLOT(saveReformatedImage()));
        medViewContainer * container = getWorkspace()->stackedViewContainers()->insertContainerInTab(0,"Reslice");
        getWorkspace()->stackedViewContainers()->setCurrentIndex(0);
        container->setDefaultWidget(d->resliceViewer->viewWidget());
        connect(container, SIGNAL(viewRemoved()),this, SLOT(stopReformat()), Qt::UniqueConnection);

        connect(d->spacingX,SIGNAL(valueChanged(double)),d->resliceViewer,SLOT(thickSlabChanged(double)));
        connect(d->spacingY,SIGNAL(valueChanged(double)),d->resliceViewer,SLOT(thickSlabChanged(double)));
        connect(d->spacingZ,SIGNAL(valueChanged(double)),d->resliceViewer,SLOT(thickSlabChanged(double)));
        connect(d->b_saveImage,SIGNAL(clicked()),d->resliceViewer,SLOT(saveImage()));

        d->reformatedImage = 0;

        // close the initial tab which is not needed anymore
        getWorkspace()->stackedViewContainers()->removeTab(1);
        updateView();
    }
    else
    {
        medMessageController::instance()->showError(tr("Drop a volume in the view"),3000);
    }
}

void resliceToolBox::stopReformat()
{
    if (getWorkspace())
    {
        d->bySpacingOrDimension->setCurrentIndex(0); // init resample parameter
        d->help0->show();
        d->reformatOptions->hide();
        d->b_startReslice->show();

        getWorkspace()->stackedViewContainers()->removeTab(0);
        getWorkspace()->stackedViewContainers()->insertContainerInTab(0, getWorkspace()->name());
        getWorkspace()->stackedViewContainers()->setCurrentIndex(0);

        if (d->currentView)
        {
            getWorkspace()->stackedViewContainers()->getFirstSelectedContainer()->addData(d->currentView->layerData(0));
        }

        processOutput();
        disconnect(d->resliceViewer);
        delete d->resliceViewer;

        d->resliceViewer = 0;
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
    medAbstractView* view = this->getWorkspace()->stackedViewContainers()->getFirstSelectedContainerView();

    if (view && d->currentView!= view)
    {
        medAbstractLayeredView * layeredView = qobject_cast<medAbstractLayeredView*>(view);
        if (dynamic_cast<medAbstractImageData*>(layeredView->layerData(layeredView->currentLayer())))
        {
            d->currentView = layeredView;
            displayInfoOnCurrentView();
        }
    }
}

void resliceToolBox::displayInfoOnCurrentView()
{
    vtkImageView2D * view2d = static_cast<medVtkViewBackend*>(d->currentView->backend())->view2D;

    if (d->bySpacingOrDimension->currentIndex() == 0) // Spacing
    {
        double* spacing = view2d->GetInput()->GetSpacing();
        d->spacingX->setValue(spacing[0]);
        d->spacingY->setValue(spacing[1]);
        d->spacingZ->setValue(spacing[2]);
    }
    else // Dimension
    {
        int* dimension = view2d->GetInput()->GetDimensions();
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
        d->spacingX->setSuffix(" mm");
        d->spacingX->setRange(0,100);
        d->spacingX->setSingleStep(0.1f);
        d->spacingY->setSuffix(" mm");
        d->spacingY->setRange(0,100);
        d->spacingY->setSingleStep(0.1f);
        d->spacingZ->setSuffix(" mm");
        d->spacingZ->setRange(0,100);
        d->spacingZ->setSingleStep(0.1f);
    }
    else // Dimension
    {
        d->spacingX->setSuffix(" px");
        d->spacingX->setRange(0,10000);
        d->spacingX->setSingleStep(10);
        d->spacingY->setSuffix(" px");
        d->spacingY->setRange(0,10000);
        d->spacingY->setSingleStep(10);
        d->spacingZ->setSuffix(" px");
        d->spacingZ->setRange(0,10000);
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

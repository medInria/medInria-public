#include "reformatToolBox.h"
#include <QWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QMessageBox>
#include <QStringList>

#include <medToolBoxFactory.h>
#include <medPluginManager.h>
#include <medAbstractImageData.h>

#include <medDataManager.h>
#include <dtkCore/dtkAbstractDataFactory.h>
#include <medReformatViewer.h>
#include <medAbstractLayeredView.h>
#include <medTabbedViewContainers.h>
#include <QGroupBox>
#include <medVtkViewBackend.h>
#include <vtkImageView2D.h>
#include <vtkImageData.h>
#include <medSliderSpinboxPair.h>

class reformatToolBoxPrivate
{
public:
    QPushButton *b_startReformat, *b_stopReformat, *b_saveImage, *b_reset;
    medComboBox *bySpacingOrDimension;
    QLabel *spacingXLab, *spacingYLab, *spacingZLab, *help0;
    QDoubleSpinBox *spacingX, *spacingY, *spacingZ;
    medAbstractLayeredView * currentView;
    medAbstractWorkspace * workspace;
    medReformatViewer * reformatViewer;
    dtkSmartPointer<medAbstractData> reformatedImage;
    QWidget* reformatOptions;
};

reformatToolBox::reformatToolBox (QWidget *parent) : medSegmentationAbstractToolBox (parent), d(new reformatToolBoxPrivate)
{
    this->setTitle("Reformat/Resample");
    this->setAboutPluginVisibility(false);
    this->setAboutPluginButton(this->plugin());

    // Fill the toolBox
    QWidget *reformatToolBoxBody = new QWidget(this);
    d->b_startReformat = new QPushButton("Start Reformat/Resample", reformatToolBoxBody);
    d->b_startReformat->setCheckable(false);
    d->b_startReformat->setObjectName("startReformatButton");
    d->b_stopReformat = new QPushButton("Stop Reformat/Resample", reformatToolBoxBody);
    d->b_stopReformat->setCheckable(false);
    d->b_stopReformat->setObjectName("stopReformatButton");
    d->b_saveImage = new QPushButton("Save Image", reformatToolBoxBody);
    d->b_saveImage->setCheckable(false);
    d->b_saveImage->setObjectName("saveImageButton");

    // User can choose pixel or millimeter resample
    QHBoxLayout * resampleLayout = new QHBoxLayout();
    QLabel* bySpacingOrDimensionLabel = new QLabel("Select your resample parameter:", reformatToolBoxBody);
    d->bySpacingOrDimension = new medComboBox(reformatToolBoxBody);
    d->bySpacingOrDimension->setObjectName("bySpacingOrDimension");
    d->bySpacingOrDimension->addItem("Spacing");
    d->bySpacingOrDimension->addItem("Dimension");
    resampleLayout->addWidget(bySpacingOrDimensionLabel);
    resampleLayout->addWidget(d->bySpacingOrDimension);
    connect(d->bySpacingOrDimension, SIGNAL(currentIndexChanged(const QString&)), this, SLOT(switchSpacingAndDimension(const QString&)));

    // Spinboxes of resample values
    QWidget * spinBoxes = new QWidget(reformatToolBoxBody);
    QVBoxLayout * spacingSpinBoxLayout = new QVBoxLayout(reformatToolBoxBody);
    d->spacingXLab = new QLabel("X : ");
    d->spacingX = new QDoubleSpinBox(reformatToolBoxBody);
    d->spacingX->setAccessibleName("SpacingX");
    d->spacingX->setObjectName("SpacingX");
    d->spacingX->setRange(0,100);
    d->spacingX->setSuffix(" mm");
    d->spacingX->setSingleStep(0.1f);
    spacingSpinBoxLayout->addWidget(d->spacingXLab);
    spacingSpinBoxLayout->addWidget(d->spacingX);
    d->spacingYLab = new QLabel("Y : ");
    d->spacingY = new QDoubleSpinBox(reformatToolBoxBody);
    d->spacingY->setAccessibleName("SpacingY");
    d->spacingY->setObjectName("SpacingY");
    d->spacingY->setRange(0,100);
    d->spacingY->setSuffix(" mm");
    d->spacingY->setSingleStep(0.1f);
    spacingSpinBoxLayout->addWidget(d->spacingYLab);
    spacingSpinBoxLayout->addWidget(d->spacingY);
    d->spacingZLab = new QLabel("Z : ");
    d->spacingZ = new QDoubleSpinBox(reformatToolBoxBody);
    d->spacingZ->setAccessibleName("SpacingZ");
    d->spacingZ->setObjectName("SpacingZ");
    d->spacingZ->setRange(0,100);
    d->spacingZ->setSuffix(" mm");
    d->spacingZ->setSingleStep(0.1f);
    spacingSpinBoxLayout->addWidget(d->spacingZLab);
    spacingSpinBoxLayout->addWidget(d->spacingZ);
    spinBoxes->setLayout(spacingSpinBoxLayout);

    QVBoxLayout *reformatToolBoxLayout =  new QVBoxLayout(reformatToolBoxBody);
    d->help0 = new QLabel("Open a data volume and launch:",reformatToolBoxBody);
    d->help0->setObjectName("help0");
    reformatToolBoxLayout->addWidget(d->help0);

    QLabel * help1 = new QLabel("To reset axes in a view press 'o'",reformatToolBoxBody);
    QLabel * help2 = new QLabel("To change the windowing left click on a 2D view",reformatToolBoxBody);
    QLabel * help3 = new QLabel("To reset windowing in a view press 'r'",reformatToolBoxBody);

    reformatToolBoxLayout->addWidget(d->b_startReformat);

    d->reformatOptions = new QWidget(reformatToolBoxBody);
    QVBoxLayout* reformatOptionsLayout = new QVBoxLayout(d->reformatOptions);
    d->reformatOptions->setLayout(reformatOptionsLayout);
    d->reformatOptions->hide();

    reformatToolBoxLayout->addWidget(d->reformatOptions);
    reformatOptionsLayout->addWidget(help1);
    reformatOptionsLayout->addWidget(help2);
    reformatOptionsLayout->addWidget(help3);
    reformatOptionsLayout->addLayout(resampleLayout);
    reformatOptionsLayout->addWidget(spinBoxes);
    reformatOptionsLayout->addWidget(d->b_saveImage);
    reformatOptionsLayout->addWidget(d->b_stopReformat);
    reformatToolBoxBody->setLayout(reformatToolBoxLayout);
    this->addWidget(reformatToolBoxBody);

    // Connections
    connect(d->b_startReformat,SIGNAL(clicked()),this,SLOT(startReformat()));
    connect(d->b_stopReformat,SIGNAL(clicked()),this,SLOT(stopReformat()));

    d->reformatViewer  = 0;
    d->currentView     = 0;
}
reformatToolBox::~reformatToolBox()
{
    delete d->reformatViewer;
    d->reformatViewer = NULL;
    d->currentView = 0;

    delete d;
    d = NULL;
}

bool reformatToolBox::registered()
{
    return medToolBoxFactory::instance()->registerToolBox<reformatToolBox>();
}

dtkPlugin* reformatToolBox::plugin()
{
    medPluginManager* pm = medPluginManager::instance();
    dtkPlugin* plugin = pm->plugin ( "reformatPlugin" );
    return plugin;
}

void reformatToolBox::startReformat()
{
    if (d->currentView && d->workspace)
    {
        d->help0->hide();
        d->reformatOptions->show();
        d->b_startReformat->hide();

        if (!d->currentView->layersCount()) return;

        d->reformatViewer = new medReformatViewer(d->currentView,d->workspace->stackedViewContainers());
        d->reformatViewer->setToolBox(this);
        d->workspace->stackedViewContainers()->setAcceptDrops(false);
        connect(d->reformatViewer,SIGNAL(imageReformatedGenerated()),this,SLOT(saveReformatedImage()));
        medViewContainer * container = d->workspace->stackedViewContainers()->insertContainerInTab(0,"Reformat");
        d->workspace->stackedViewContainers()->setCurrentIndex(0);
        container->setDefaultWidget(d->reformatViewer->viewWidget());
        d->workspace->stackedViewContainers()->lockTabs();

        connect(d->spacingX,SIGNAL(valueChanged(double)),d->reformatViewer,SLOT(thickSlabChanged(double)));
        connect(d->spacingY,SIGNAL(valueChanged(double)),d->reformatViewer,SLOT(thickSlabChanged(double)));
        connect(d->spacingZ,SIGNAL(valueChanged(double)),d->reformatViewer,SLOT(thickSlabChanged(double)));
        connect(d->b_saveImage,SIGNAL(clicked()),d->reformatViewer,SLOT(saveImage()));

        QList<medToolBox*> toolBoxes = d->workspace->toolBoxes();
        for (int i = 0; i < toolBoxes.length(); i++)
        {
            if (toolBoxes[i] != this)
            {
                toolBoxes[i]->hide();
            }
        }
        d->reformatedImage = 0;

        // close the initial tab which is not needed anymore
        d->workspace->stackedViewContainers()->unlockTabs();
        d->workspace->stackedViewContainers()->removeTab(1);
        updateView();
    }
    else
    {
        medMessageController::instance()->showError(tr("Reformat needs a volume"),3000);
    }
}

void reformatToolBox::stopReformat()
{
    if (d->workspace)
    {
        d->help0->show();
        d->reformatOptions->hide();
        d->b_startReformat->show();

        d->workspace->stackedViewContainers()->unlockTabs();
        d->workspace->stackedViewContainers()->removeTab(0);
        d->workspace->stackedViewContainers()->insertContainerInTab(0, d->workspace->name());
        d->workspace->stackedViewContainers()->setCurrentIndex(0);
        processOutput();
        disconnect(d->reformatViewer);
        delete d->reformatViewer;

        d->reformatViewer = 0;
        QList<medToolBox*> toolBoxes = d->workspace->toolBoxes();
        for (int i = 0; i < toolBoxes.length(); i++)
        {
            toolBoxes[i]->show();
        }
        d->currentView = NULL;
    }
}

void reformatToolBox::setWorkspace(medAbstractWorkspace * workspace)
{
    d->workspace = workspace;
    //connect(d->workspace->stackedViewContainers(),SIGNAL(currentChanged(const QString&)),this,SLOT(actOnContainerChange(const QString&)));
    medTabbedViewContainers * containers = workspace->stackedViewContainers();
    QObject::connect(containers,SIGNAL(containersSelectedChanged()),this,SLOT(updateView()));
    updateView();
}

void reformatToolBox::updateView()
{
    medTabbedViewContainers * containers = d->workspace->stackedViewContainers();
    QList<medViewContainer*> containersInTabSelected =  containers->containersInTab(containers->currentIndex());
    medAbstractView *view=NULL;
    for(int i=0;i<containersInTabSelected.length();i++)
    {
        if (containersInTabSelected[i]->isSelected())
        {
            view = containersInTabSelected[i]->view();
            break;
        }
    }

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

void reformatToolBox::actOnContainerChange(const QString & name)
{
//    if (!qobject_cast<reformatWorkspace*>(d->workspace))
//        return;
//    if (name == "Reformat")
//        qobject_cast<reformatWorkspace*>(d->workspace)->showViewPropertiesToolBox(false);
//    else
//        qobject_cast<reformatWorkspace*>(d->workspace)->showViewPropertiesToolBox(true);
}

void reformatToolBox::displayInfoOnCurrentView()
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

void reformatToolBox::saveReformatedImage()
{
    generateReformatedImage();
    if (d->reformatedImage && d->reformatedImage->data())
    {
        medDataManager::instance()->importData(d->reformatedImage, false);
    }
}

void reformatToolBox::generateReformatedImage()
{
    d->reformatedImage = d->reformatViewer->getOutput();
}

void reformatToolBox::switchSpacingAndDimension(const QString & value)
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

medAbstractData *reformatToolBox::processOutput()
{
    if (!d->reformatedImage)
    {
        generateReformatedImage();
    }
    return d->reformatedImage;
}

void reformatToolBox::changeButtonValue(QString buttonName, double value)
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

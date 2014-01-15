#include "medVectorFieldToolBox.h"
#include <medToolBoxFactory.h>
#include <v3dViewVectorFieldInteractor.h>
#include <medGui/commonWidgets/medSliderSpinboxPair.h>

class medVectorFieldToolBoxPrivate
{
public:
    QDoubleSpinBox          *scaleFactor;
    medSliderSpinboxPair    *sampleRateControl;
    QComboBox               *scaleMode;
    QComboBox               *colorMode;
    QCheckBox               *projection;

    QCheckBox               *hideShowAxialCheckBox;
    QCheckBox               *hideShowCoronalCheckBox;
    QCheckBox               *hideShowSagittalCheckBox;

    v3dViewVectorFieldInteractor *interactor;
    v3dView * view;

};

medVectorFieldToolBox::medVectorFieldToolBox(QWidget *parent)
    : medToolBox(parent)
    , d(new medVectorFieldToolBoxPrivate)
{
    this->setTitle("Vector Field View");
    this->setValidDataTypes(QStringList() << "itkDataImageVectorFloat3" << "itkDataImageVectorDouble3");

    QWidget* displayWidget = new QWidget(this);

    d->scaleFactor = new QDoubleSpinBox;
    d->scaleFactor->setValue(1.0);

    //  Control sample rate
    d->sampleRateControl = new medSliderSpinboxPair(displayWidget);
    d->sampleRateControl->setMinimum(1);
    d->sampleRateControl->setMaximum(10);
    d->sampleRateControl->setValue(1);


    d->colorMode = new QComboBox;
    QStringList colorModes = QStringList() << "Vector Magnitude" << "Vector Direction";
    d->colorMode->addItems(colorModes);
    d->colorMode->setCurrentIndex(0);

    d->projection = new QCheckBox;

    // hide or show axial, coronal, and sagittal
    QString hideShowAxesToolTip = tr("Hide or show a specific axis.");
    QLabel* hideShowAxesLabel = new QLabel(tr("Hide/show:"));
    hideShowAxesLabel->setToolTip(hideShowAxesToolTip);
    d->hideShowAxialCheckBox = new QCheckBox("Axial", displayWidget);
    d->hideShowAxialCheckBox->setToolTip(hideShowAxesToolTip);
    d->hideShowCoronalCheckBox = new QCheckBox("Coronal", displayWidget);
    d->hideShowCoronalCheckBox->setToolTip(hideShowAxesToolTip);
    d->hideShowSagittalCheckBox = new QCheckBox("Sagittal", displayWidget);
    d->hideShowSagittalCheckBox->setToolTip(hideShowAxesToolTip);

    d->hideShowAxialCheckBox->setChecked(true);
    d->hideShowCoronalCheckBox->setChecked(true);
    d->hideShowSagittalCheckBox->setChecked(true);

    QHBoxLayout* slicesLayout = new QHBoxLayout;
    slicesLayout->addWidget(d->hideShowAxialCheckBox);
    slicesLayout->addWidget(d->hideShowCoronalCheckBox);
    slicesLayout->addWidget(d->hideShowSagittalCheckBox);

    QFormLayout *layout = new QFormLayout(displayWidget);
    layout->addRow("Scale Factor", d->scaleFactor );
    layout->addRow(tr("Sample rate:"),d->sampleRateControl);
    layout->addRow("Color Mode", d->colorMode );
    layout->addRow("Activate Projection", d->projection );
    layout->addRow(hideShowAxesLabel, slicesLayout);

    this->addWidget(displayWidget);

    connect(d->scaleFactor, SIGNAL(editingFinished()), this, SLOT(setScaleFactor()));
    connect(d->sampleRateControl,SIGNAL(valueChanged(int)),this,SLOT(setSampleRate(int)));
    connect(d->colorMode, SIGNAL(currentIndexChanged(int)), this, SLOT(setColorMode(int)));
    connect(d->projection, SIGNAL(toggled(bool)), this, SLOT(setProjection(bool)));
    connect(d->hideShowAxialCheckBox, SIGNAL(stateChanged(int)), this, SLOT(setShowAxial(int)));
    connect(d->hideShowCoronalCheckBox, SIGNAL(stateChanged(int)),this, SLOT(setShowCoronal(int)));
    connect(d->hideShowSagittalCheckBox, SIGNAL(stateChanged(int)), this, SLOT(setShowSagittal(int)));

    this->hide();
}

medVectorFieldToolBox::~medVectorFieldToolBox()
{
    delete d;
    d = NULL;
}


void medVectorFieldToolBox::update (dtkAbstractView *view)
{
    medToolBox::update(view);
    if (!view)
    {
        d->view = 0;
        d->interactor = 0;
        return;
    }

    d->view = qobject_cast<v3dView*>(view);
    d->interactor = qobject_cast<v3dViewVectorFieldInteractor*>(d->view->dtkAbstractView::interactor("v3dViewVectorFieldInteractor"));
}


void medVectorFieldToolBox::setScaleFactor()
{
    double scale = d->scaleFactor->value();
    if(d->interactor)
        d->interactor->setScale(scale);
}

void medVectorFieldToolBox::setSampleRate (int sampleRate)
{
    if(d->interactor)
        d->interactor->setSampleRate(sampleRate);
}

void medVectorFieldToolBox::setColorMode(int mode)
{
    if(d->interactor)
        d->interactor->setColorMode(mode);
}

void medVectorFieldToolBox::setProjection(bool enable)
{
    if(d->interactor)
        d->interactor->setProjection(enable);
}

bool medVectorFieldToolBox::registered()
{
    return medToolBoxFactory::instance()->registerToolBox<medVectorFieldToolBox>("medVectorFieldToolBox",
                                                                                "medVectorFieldToolBox",
                                                                                "Vector View ToolBox",
                                                                                QStringList()<<"view"<<"vectors");
}

void medVectorFieldToolBox::setShowAxial(int checkBoxState)
{
    d->interactor->setShowAxial(checkBoxState == Qt::Checked);
}

void medVectorFieldToolBox::setShowCoronal(int checkBoxState)
{
    d->interactor->setShowCoronal(checkBoxState == Qt::Checked);
}

void medVectorFieldToolBox::setShowSagittal(int checkBoxState)
{
    d->interactor->setShowSagittal(checkBoxState == Qt::Checked);
}


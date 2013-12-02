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


    d->scaleMode = new QComboBox;
    QStringList scaleModes = QStringList() << "Scalar" << "Vector" << "VectorComponents";
    d->scaleMode->addItems(scaleModes);

    d->colorMode = new QComboBox;
    QStringList colorModes = QStringList() << "Scalar" << "Scale" << "Vector";
    d->colorMode->addItems(colorModes);

    d->projection = new QCheckBox;

    QFormLayout *layout = new QFormLayout(displayWidget);
    layout->addRow("Scale Factor", d->scaleFactor );
    layout->addRow(tr("Sample rate:"),d->sampleRateControl);
    layout->addRow("Scale Mode", d->scaleMode );
    layout->addRow("Color Mode", d->colorMode );
    layout->addRow("Activate Projection", d->projection );

    this->addWidget(displayWidget);

    connect(d->scaleFactor, SIGNAL(editingFinished()), this, SLOT(setScaleFactor()));
    connect(d->sampleRateControl,SIGNAL(valueChanged(int)),this,SLOT(setSampleRate(int)));
    connect(d->scaleMode, SIGNAL(currentIndexChanged(int)), this, SLOT(setScaleMode(int)));
    connect(d->colorMode, SIGNAL(currentIndexChanged(int)), this, SLOT(setColorMode(int)));
    connect(d->projection, SIGNAL(toggled(bool)), this, SLOT(setProjection(bool)));

    this->hide();
}

medVectorFieldToolBox::~medVectorFieldToolBox()
{
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

void medVectorFieldToolBox::setScaleMode(int mode)
{
    if(d->interactor)
        d->interactor->setScaleMode(mode);
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



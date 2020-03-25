#include "itkProcessTensorTractographyToolBox.h"

#include <medDataIndex.h>
#include <medDataManager.h>
#include <medPluginManager.h>
#include <medToolBoxFactory.h>
#include <medDropSite.h>
#include <medProgressionStack.h>
#include <medMetaDataKeys.h>

#include <itkProcessTensorTractography.h>

class itkProcessTensorTractographyToolBoxPrivate
{
public:
    double FA1Value;
    double FA2Value;
    double smoothnessValue;
    int samplingValue;
    int minLengthValue;
};


itkProcessTensorTractographyToolBox::itkProcessTensorTractographyToolBox(QWidget *parent) :
    medDiffusionAbstractToolBox(parent), d(new itkProcessTensorTractographyToolBoxPrivate)
{
    QWidget *widget = new QWidget(this);
    
    QGroupBox *tractographyBox = new QGroupBox (widget);
    tractographyBox->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);

    QString fa1ToolTip = tr("Start seeding fibers in voxels whose FA value is above the threshold.");
    QLabel *fa1Label = new QLabel(tr("Starting FA Threshold:"), tractographyBox);
    fa1Label->setToolTip(fa1ToolTip);
    QSlider *fa1Slider = new QSlider (Qt::Horizontal, tractographyBox);
    fa1Slider->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    fa1Slider->setFocusPolicy(Qt::NoFocus);
    fa1Slider->setRange (0, 1000);
    fa1Slider->setValue (300);
    fa1Slider->setToolTip(fa1ToolTip);
    
    QSpinBox *fa1SpinBox = new QSpinBox (tractographyBox);
    fa1SpinBox->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    fa1SpinBox->setRange (0, 1000);
    fa1SpinBox->setValue (300);
    fa1SpinBox->setToolTip(fa1ToolTip);
    
    QString fa2ToolTip = tr("Stop fiber's construction when reaching a voxel\nwhose FA value falls below the threshold.");
    QLabel *fa2Label = new QLabel(tr("Stopping FA Threshold:"), tractographyBox);
    fa2Label->setToolTip(fa2ToolTip);
    
    QSlider *fa2Slider = new QSlider (Qt::Horizontal, tractographyBox);
    fa2Slider->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    fa2Slider->setFocusPolicy(Qt::NoFocus);
    fa2Slider->setRange (0, 1000);
    fa2Slider->setValue (200);
    fa2Slider->setToolTip(fa2ToolTip);
    
    QSpinBox *fa2SpinBox = new QSpinBox (tractographyBox);
    fa2SpinBox->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    fa2SpinBox->setRange (0, 1000);
    fa2SpinBox->setValue (200);
    fa2SpinBox->setToolTip(fa2ToolTip);
    
    QString smothnessToolTip = tr("This value indicates how \"strictly\" the fiber follows the tensors.\nThe higher it is, the less the fiber gets deviated by the tensors.");
    QLabel *smoothnessLabel = new QLabel(tr("Smoothness:"), tractographyBox);
    smoothnessLabel->setToolTip(smothnessToolTip);
    QSlider *smoothnessSlider = new QSlider (Qt::Horizontal, tractographyBox);
    smoothnessSlider->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    smoothnessSlider->setFocusPolicy(Qt::NoFocus);
    smoothnessSlider->setRange (0, 100);
    smoothnessSlider->setValue (0);
    smoothnessSlider->setToolTip(smothnessToolTip);
    
    QSpinBox *smoothnessSpinBox = new QSpinBox (tractographyBox);
    smoothnessSpinBox->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    smoothnessSpinBox->setRange (0, 100);
    smoothnessSpinBox->setValue (0);
    smoothnessSpinBox->setToolTip(smothnessToolTip);
    
    QString minLengthToolTip = tr("At the final stage, fibers whose length is below the chosen amount (in millimetres) are deleted.");
    QLabel *minLengthLabel = new QLabel(tr("Minimum length:"), tractographyBox);
    minLengthLabel->setToolTip(minLengthToolTip);
    QSlider *minLengthSlider = new QSlider (Qt::Horizontal, tractographyBox);
    minLengthSlider->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    minLengthSlider->setFocusPolicy(Qt::NoFocus);
    minLengthSlider->setRange (0, 100);
    minLengthSlider->setValue (10);
    minLengthSlider->setToolTip(minLengthToolTip);
    
    QSpinBox *minLengthSpinBox = new QSpinBox (tractographyBox);
    minLengthSpinBox->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    minLengthSpinBox->setRange (0, 100);
    minLengthSpinBox->setValue (10);
    minLengthSpinBox->setToolTip(minLengthToolTip);
    
    QString samplingToolTip = tr("Fibers are seeded every X voxels, being X the sampling value.");
    QLabel *samplingLabel = new QLabel(tr("Sampling:"), tractographyBox);
    samplingLabel->setToolTip(samplingToolTip);
    QSlider *samplingSlider = new QSlider (Qt::Horizontal, tractographyBox);
    samplingSlider->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    samplingSlider->setRange (1, 50);
    samplingSlider->setValue (1);
    samplingSlider->setToolTip(samplingToolTip);
    
    QSpinBox *samplingSpinBox = new QSpinBox (tractographyBox);
    samplingSpinBox->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    samplingSpinBox->setRange (1, 50);
    samplingSpinBox->setValue (1);
    samplingSpinBox->setToolTip(samplingToolTip);
    
    QHBoxLayout *fa1Layout = new QHBoxLayout;
    fa1Layout->addWidget (fa1Slider);
    fa1Layout->addWidget (fa1SpinBox);
    
    QHBoxLayout *fa2Layout = new QHBoxLayout;
    fa2Layout->addWidget (fa2Slider);
    fa2Layout->addWidget (fa2SpinBox);
    
    QHBoxLayout *smoothnessLayout = new QHBoxLayout;
    smoothnessLayout->addWidget (smoothnessSlider);
    smoothnessLayout->addWidget (smoothnessSpinBox);
    
    QHBoxLayout *minLengthLayout = new QHBoxLayout;
    minLengthLayout->addWidget (minLengthSlider);
    minLengthLayout->addWidget (minLengthSpinBox);
    
    QHBoxLayout *samplingLayout = new QHBoxLayout;
    samplingLayout->addWidget (samplingSlider);
    samplingLayout->addWidget (samplingSpinBox);
    
    QVBoxLayout *tractographyBoxLayout = new QVBoxLayout (tractographyBox);
    tractographyBoxLayout->addWidget (fa1Label);
    tractographyBoxLayout->addLayout (fa1Layout);
    tractographyBoxLayout->addWidget (fa2Label);
    tractographyBoxLayout->addLayout (fa2Layout);
    tractographyBoxLayout->addWidget (smoothnessLabel);
    tractographyBoxLayout->addLayout (smoothnessLayout);
    tractographyBoxLayout->addWidget (minLengthLabel);
    tractographyBoxLayout->addLayout (minLengthLayout);
    tractographyBoxLayout->addWidget (samplingLabel);
    tractographyBoxLayout->addLayout (samplingLayout);
    
    QVBoxLayout *layout = new QVBoxLayout(widget);
    layout->addWidget (tractographyBox);
    
    connect (fa1Slider,        SIGNAL (valueChanged (int)), this,       SLOT (setFA1Value (int)));
    connect (fa1Slider,        SIGNAL (valueChanged (int)), fa1SpinBox, SLOT (setValue (int)));
    connect (fa1SpinBox,       SIGNAL (valueChanged (int)), fa1Slider,  SLOT (setValue (int)));
    
    connect (fa2Slider,        SIGNAL (valueChanged (int)), this,       SLOT (setFA2Value (int)));
    connect (fa2Slider,        SIGNAL (valueChanged (int)), fa2SpinBox, SLOT (setValue (int)));
    connect (fa2SpinBox,       SIGNAL (valueChanged (int)), fa2Slider,  SLOT (setValue (int)));
    
    connect (smoothnessSlider, SIGNAL (valueChanged (int)), this,              SLOT (setSmoothnessValue (int)));
    connect (smoothnessSlider, SIGNAL (valueChanged (int)), smoothnessSpinBox, SLOT (setValue (int)));
    connect (smoothnessSpinBox,SIGNAL (valueChanged (int)), smoothnessSlider,  SLOT (setValue (int)));
    
    connect (samplingSlider,   SIGNAL (valueChanged (int)), this,            SLOT (setSamplingValue (int)));
    connect (samplingSlider,   SIGNAL (valueChanged (int)), samplingSpinBox, SLOT (setValue (int)));
    connect (samplingSpinBox,  SIGNAL (valueChanged (int)), samplingSlider,  SLOT (setValue (int)));
    
    connect (minLengthSlider,  SIGNAL (valueChanged (int)), this,             SLOT (setMinLengthValue (int)));
    connect (minLengthSlider,  SIGNAL (valueChanged (int)), minLengthSpinBox, SLOT (setValue (int)));
    connect (minLengthSpinBox, SIGNAL (valueChanged (int)), minLengthSlider,  SLOT (setValue (int)));

    this->addWidget(widget);
    
    //Default parameters
    d->FA1Value = 0.3;
    d->FA2Value = 0.2;
    d->smoothnessValue = 0;
    d->samplingValue = 1;
    d->minLengthValue = 10;
}

itkProcessTensorTractographyToolBox::~itkProcessTensorTractographyToolBox(void)
{
    delete d;
    d = NULL;
}

bool itkProcessTensorTractographyToolBox::registered(void)
{
    return medToolBoxFactory::instance()->registerToolBox<itkProcessTensorTractographyToolBox>();
}

void itkProcessTensorTractographyToolBox::setFA1Value (int value)
{
    d->FA1Value = value / 1000.0;
}

void itkProcessTensorTractographyToolBox::setFA2Value (int value)
{
    d->FA2Value = value / 1000.0;
}

void itkProcessTensorTractographyToolBox::setSmoothnessValue (int value)
{
    d->smoothnessValue = value / 100.0;
}

void itkProcessTensorTractographyToolBox::setSamplingValue (int value)
{
    d->samplingValue = value;
}

void itkProcessTensorTractographyToolBox::setMinLengthValue (int value)
{
    d->minLengthValue = value;
}

QString itkProcessTensorTractographyToolBox::processName()
{
    return "itkProcessTensorTractography";
}

void itkProcessTensorTractographyToolBox::setProcessParameters(medAbstractDiffusionProcess *process)
{
    itkProcessTensorTractography *castProcess = dynamic_cast <itkProcessTensorTractography *> (process);
    
    if (!castProcess)
        return;
    
    castProcess->setFA1Value(d->FA1Value);
    castProcess->setFA2Value(d->FA2Value);
    castProcess->setSmoothnessValue(d->smoothnessValue);
    castProcess->setSamplingValue(d->samplingValue);
    castProcess->setMinLengthValue(d->minLengthValue);
}

dtkPlugin * itkProcessTensorTractographyToolBox::plugin() const
{
    medPluginManager* pm = medPluginManager::instance();
    dtkPlugin* plugin = pm->plugin ( "itkProcessTensorLogEuclideanPlugin" );
    return plugin;
}


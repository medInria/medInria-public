#include "itkProcessTensorEstimationToolBox.h"

#include <medDataIndex.h>
#include <medDataManager.h>
#include <medPluginManager.h>
#include <medToolBoxFactory.h>
#include <medDropSite.h>
#include <medProgressionStack.h>
#include <medMetaDataKeys.h>

#include <itkProcessTensorEstimation.h>

class itkProcessTensorEstimationToolBoxPrivate
{
public:
    QCheckBox *automaticBrainExtractionCheckBox;
    QSlider *bstSlider;
    
    int brainB0Threshold;
    bool automaticBrainExtraction;
    bool tensorSmoothing;
    QString smoothingType;
};


itkProcessTensorEstimationToolBox::itkProcessTensorEstimationToolBox(QWidget *parent) :
    medDiffusionAbstractToolBox(parent), d(new itkProcessTensorEstimationToolBoxPrivate)
{
    QWidget *widget = new QWidget(this);

    QGroupBox *estimationBox = new QGroupBox (widget);
    estimationBox->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
    estimationBox->setTitle(tr("Pre-Processing"));

    d->automaticBrainExtractionCheckBox = new QCheckBox(estimationBox);
    d->automaticBrainExtractionCheckBox->setText(tr("Automatic brain extraction"));
    d->automaticBrainExtractionCheckBox->setChecked (true);
    d->automaticBrainExtractionCheckBox->setToolTip(tr("Uncheck this box if you want to manually set the background removal threshold."));

    QString brtToolTip = tr("If \"Automatic brain extraction\" is unchecked, the algorithm will not\nconsider background values falling below the manually set threshold.");
    QLabel  *bstLabel  = new QLabel (tr ("Background removal threshold:"), estimationBox);
    bstLabel->setToolTip(brtToolTip);
    d->bstSlider = new QSlider(Qt::Horizontal, estimationBox);
    d->bstSlider->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    d->bstSlider->setFocusPolicy(Qt::NoFocus);
    d->bstSlider->setRange (0, 9999);
    d->bstSlider->setValue (100);
    d->bstSlider->setToolTip(brtToolTip);

    QSpinBox *bstSpinBox = new QSpinBox (estimationBox);
    bstSpinBox->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    bstSpinBox->setRange (0, 9999);
    bstSpinBox->setValue (100);
    bstSpinBox->setToolTip(brtToolTip);

    bstLabel->setEnabled   (false);
    d->bstSlider->setEnabled  (false);
    bstSpinBox->setEnabled (false);

    QGroupBox *smoothingBox = new QGroupBox (estimationBox);
    smoothingBox->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
    smoothingBox->setTitle (tr("Use smoothing"));
    smoothingBox->setCheckable (true);
    smoothingBox->setChecked (false);
    smoothingBox->setToolTip(tr("By activating this feature the algorithm will try to reduce noise, but might reduce its accuracy also."));

    QRadioButton *lowSmoothing  = new QRadioButton (smoothingBox);
    QRadioButton *medSmoothing  = new QRadioButton (smoothingBox);
    QRadioButton *highSmoothing = new QRadioButton (smoothingBox);
    lowSmoothing->setText(tr("Low"));
    lowSmoothing->setChecked(true);
    medSmoothing->setText(tr("Medium"));
    highSmoothing->setText(tr("High"));

    // layout
    QHBoxLayout *bstLayout = new QHBoxLayout;
    bstLayout->addWidget (d->bstSlider);
    bstLayout->addWidget (bstSpinBox);

    QVBoxLayout *estimationBoxLayout = new QVBoxLayout (estimationBox);
    estimationBoxLayout->addWidget (d->automaticBrainExtractionCheckBox);
    estimationBoxLayout->addWidget (bstLabel);
    estimationBoxLayout->addLayout (bstLayout);
    estimationBoxLayout->addWidget (smoothingBox);

    QHBoxLayout *smoothingBoxLayout = new QHBoxLayout (smoothingBox);
    smoothingBoxLayout->addWidget (lowSmoothing);
    smoothingBoxLayout->addWidget (medSmoothing);
    smoothingBoxLayout->addWidget (highSmoothing);
    
    QVBoxLayout *layout = new QVBoxLayout(widget);
    layout->addWidget (estimationBox);

    // connections
    connect (d->automaticBrainExtractionCheckBox, SIGNAL(toggled(bool)), bstLabel,      SLOT(setDisabled(bool)));
    connect (d->automaticBrainExtractionCheckBox, SIGNAL(toggled(bool)), d->bstSlider, SLOT(setDisabled(bool)));
    connect (d->automaticBrainExtractionCheckBox, SIGNAL(toggled(bool)), bstSpinBox,    SLOT(setDisabled(bool)));
    connect (d->automaticBrainExtractionCheckBox, SIGNAL(toggled(bool)), this,          SLOT(toggleAutomaticBrainExtraction(bool)));

    connect (d->bstSlider,    SIGNAL (valueChanged (int)), this,          SLOT (changeBSTSliderValue (int)));
    connect (d->bstSlider,    SIGNAL (valueChanged (int)), bstSpinBox,    SLOT (setValue (int)));
    connect (bstSpinBox,       SIGNAL (valueChanged (int)), d->bstSlider, SLOT (setValue (int)));

    connect (smoothingBox,  SIGNAL(toggled(bool)), this, SLOT(toggleTensorSmoothing(bool)));
    connect (lowSmoothing,  SIGNAL(toggled(bool)), this, SLOT(toggleLowSmoothing(bool)));
    connect (medSmoothing,  SIGNAL(toggled(bool)), this, SLOT(toggleMedSmoothing(bool)));
    connect (highSmoothing, SIGNAL(toggled(bool)), this, SLOT(toggleHighSmoothing(bool)));

    this->addWidget(widget);
    
    //Default parameters
    d->brainB0Threshold = 100;
    d->automaticBrainExtraction = true;
    d->tensorSmoothing = false;
    d->smoothingType = "low";
}

itkProcessTensorEstimationToolBox::~itkProcessTensorEstimationToolBox(void)
{
    delete d;
    d = NULL;
}

bool itkProcessTensorEstimationToolBox::registered(void)
{
    return medToolBoxFactory::instance()->registerToolBox<itkProcessTensorEstimationToolBox>();
}

QString itkProcessTensorEstimationToolBox::processName()
{
        return "itkProcessTensorEstimation";
}

void itkProcessTensorEstimationToolBox::setProcessParameters(medAbstractDiffusionProcess *process)
{
    itkProcessTensorEstimation *castProcess = dynamic_cast <itkProcessTensorEstimation *> (process);
    
    if (!castProcess)
        return;
    
    castProcess->setAutomaticBrainMasking(d->automaticBrainExtraction);
    if (!d->automaticBrainExtraction)
        castProcess->setB0Threshold(d->brainB0Threshold);

    castProcess->setTensorSmoothing(d->tensorSmoothing);
    if (d->tensorSmoothing)
        castProcess->setSmoothingLevel(d->smoothingType);
}

void itkProcessTensorEstimationToolBox::changeBSTSliderValue(int value)
{
    d->brainB0Threshold = value;
}

void itkProcessTensorEstimationToolBox::toggleAutomaticBrainExtraction(bool value)
{
    d->automaticBrainExtraction = value;
}

void itkProcessTensorEstimationToolBox::toggleTensorSmoothing(bool value)
{
    d->tensorSmoothing = value;
    d->smoothingType = "low";
}

void itkProcessTensorEstimationToolBox::toggleLowSmoothing(bool value)
{
    if (value)
        d->smoothingType = "low";
}

void itkProcessTensorEstimationToolBox::toggleMedSmoothing(bool value)
{
    if (value)
        d->smoothingType = "medium";
}

void itkProcessTensorEstimationToolBox::toggleHighSmoothing(bool value)
{
    if (value)
        d->smoothingType = "high";
}

dtkPlugin * itkProcessTensorEstimationToolBox::plugin() const
{
    medPluginManager* pm = medPluginManager::instance();
    dtkPlugin* plugin = pm->plugin ( "itkProcessTensorLogEuclideanPlugin" );
    return plugin;
}


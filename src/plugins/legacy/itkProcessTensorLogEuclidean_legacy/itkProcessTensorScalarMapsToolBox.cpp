#include "itkProcessTensorScalarMapsToolBox.h"

#include <dtkCore/dtkSmartPointer.h>
#include <dtkCore/dtkAbstractData.h>
#include <dtkCore/dtkAbstractProcess.h>
#include <dtkCore/dtkAbstractProcessFactory.h>
#include <dtkCore/dtkAbstractViewInteractor.h>

#include "itkProcessTensorScalarMaps.h"

#include <medDataIndex.h>
#include <medDataManager.h>
#include <medPluginManager.h>
#include <medToolBoxFactory.h>
#include <medDropSite.h>
#include <medProgressionStack.h>
#include <medMetaDataKeys.h>

class itkProcessTensorScalarMapsToolBoxPrivate
{
public:
    QSignalMapper *mapper;
    
    QString mapRequested;
};


itkProcessTensorScalarMapsToolBox::itkProcessTensorScalarMapsToolBox(QWidget *parent) :
    medDiffusionScalarMapsAbstractToolBox(parent), d(new itkProcessTensorScalarMapsToolBoxPrivate)
{
    QWidget *widget = new QWidget(this);

    QGroupBox *tensorToScalarBox = new QGroupBox (widget);
    tensorToScalarBox->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
    tensorToScalarBox->setTitle (tr("Diffusion tensor maps"));
    
    QPushButton *faButton = new QPushButton("", tensorToScalarBox);
    faButton->setToolTip(tr("Fractional Anisotropy"));
    faButton->setIconSize (QSize (50, 50));
    faButton->setFixedSize (60, 60);
    faButton->setIcon (QIcon(":/dtitrack/icons/fa.png"));
    
    QPushButton *facButton = new QPushButton("", tensorToScalarBox);
    facButton->setToolTip(tr("Coloured Fractional Anisotropy"));
    facButton->setIconSize (QSize (50, 50));
    facButton->setFixedSize (60, 60);
    facButton->setIcon (QIcon(":/dtitrack/icons/fac.png"));
    
    QPushButton *raButton = new QPushButton("", tensorToScalarBox);
    raButton->setToolTip(tr("Relative Anisotropy"));
    raButton->setIconSize (QSize (50, 50));
    raButton->setFixedSize (60, 60);
    raButton->setIcon (QIcon(":/dtitrack/icons/ra.png"));
    
    QPushButton *l1Button = new QPushButton("", tensorToScalarBox);
    l1Button->setToolTip(tr("Lambda 1: Largest Eigenvalue"));
    l1Button->setIconSize (QSize (50, 50));
    l1Button->setFixedSize (60, 60);
    l1Button->setIcon (QIcon(":/dtitrack/icons/l1.png"));
    
    QPushButton *l2Button = new QPushButton("", tensorToScalarBox);
    l2Button->setToolTip(tr("Lambda 2: Second Largest Eigenvalue"));
    l2Button->setIconSize (QSize (50, 50));
    l2Button->setFixedSize (60, 60);
    l2Button->setIcon (QIcon(":/dtitrack/icons/l2.png"));
    
    QPushButton *l3Button = new QPushButton("", tensorToScalarBox);
    l3Button->setToolTip(tr("Lambda 3: Smallest Eigenvalue"));
    l3Button->setIconSize (QSize (50, 50));
    l3Button->setFixedSize (60, 60);
    l3Button->setIcon (QIcon(":/dtitrack/icons/l3.png"));
    
    QPushButton *clButton = new QPushButton("", tensorToScalarBox);
    clButton->setToolTip(tr("Linear Coefficient"));
    clButton->setIconSize (QSize (50, 50));
    clButton->setFixedSize (60, 60);
    clButton->setIcon (QIcon(":/dtitrack/icons/cl.png"));
    
    QPushButton *cpButton = new QPushButton("", tensorToScalarBox);
    cpButton->setToolTip(tr("Planar Coefficient"));
    cpButton->setIconSize (QSize (50, 50));
    cpButton->setFixedSize (60, 60);
    cpButton->setIcon (QIcon(":/dtitrack/icons/cp.png"));
    
    QPushButton *csButton = new QPushButton("", tensorToScalarBox);
    csButton->setToolTip(tr("Spherical Coefficient"));
    csButton->setIconSize (QSize (50, 50));
    csButton->setFixedSize (60, 60);
    csButton->setIcon (QIcon(":/dtitrack/icons/cs.png"));
    
    QPushButton *adcButton = new QPushButton("", tensorToScalarBox);
    adcButton->setToolTip(tr("Average Diffusion Coefficient"));
    adcButton->setIconSize (QSize (50, 50));
    adcButton->setFixedSize (60, 60);
    adcButton->setIcon (QIcon(":/dtitrack/icons/adc.png"));
    
    QPushButton *vrButton = new QPushButton("", tensorToScalarBox);
    vrButton->setToolTip(tr("Volume Ratio"));
    vrButton->setIconSize (QSize (50, 50));
    vrButton->setFixedSize (60, 60);
    vrButton->setIcon (QIcon(":/dtitrack/icons/vr.png"));
    
    QPushButton *falButton = new QPushButton("", tensorToScalarBox);
    falButton->setToolTip(tr("Logarithmic Fractional Anisotropy"));
    falButton->setIconSize (QSize (50, 50));
    falButton->setFixedSize (60, 60);
    falButton->setIcon (QIcon(":/dtitrack/icons/fal.png"));
    
    QGridLayout *tensorToScalarBoxLayout = new QGridLayout (tensorToScalarBox);
    tensorToScalarBoxLayout->addWidget (faButton,  0, 0);
    tensorToScalarBoxLayout->addWidget (facButton, 0, 1);
    tensorToScalarBoxLayout->addWidget (raButton,  0, 2);
    tensorToScalarBoxLayout->addWidget (l1Button,  1, 0);
    tensorToScalarBoxLayout->addWidget (l2Button,  1, 1);
    tensorToScalarBoxLayout->addWidget (l3Button,  1, 2);
    tensorToScalarBoxLayout->addWidget (clButton,  2, 0);
    tensorToScalarBoxLayout->addWidget (cpButton,  2, 1);
    tensorToScalarBoxLayout->addWidget (csButton,  2, 2);
    tensorToScalarBoxLayout->addWidget (adcButton, 3, 0);
    tensorToScalarBoxLayout->addWidget (vrButton,  3, 1);
    tensorToScalarBoxLayout->addWidget (falButton, 3, 2);
    
    QVBoxLayout *layout = new QVBoxLayout(widget);
    layout->addWidget (tensorToScalarBox);
    
    // Setting button mappings
    d->mapper = new QSignalMapper (this);
    
    connect (faButton,  SIGNAL (clicked()), d->mapper, SLOT(map()));
    connect (adcButton, SIGNAL (clicked()), d->mapper, SLOT(map()));
    connect (raButton,  SIGNAL (clicked()), d->mapper, SLOT(map()));
    connect (vrButton,  SIGNAL (clicked()), d->mapper, SLOT(map()));
    connect (clButton,  SIGNAL (clicked()), d->mapper, SLOT(map()));
    connect (cpButton,  SIGNAL (clicked()), d->mapper, SLOT(map()));
    connect (csButton,  SIGNAL (clicked()), d->mapper, SLOT(map()));
    connect (l1Button,  SIGNAL (clicked()), d->mapper, SLOT(map()));
    connect (l2Button,  SIGNAL (clicked()), d->mapper, SLOT(map()));
    connect (l3Button,  SIGNAL (clicked()), d->mapper, SLOT(map()));
    connect (facButton, SIGNAL (clicked()), d->mapper, SLOT(map()));
    connect (falButton, SIGNAL (clicked()), d->mapper, SLOT(map()));
    
    d->mapper->setMapping(faButton, "FA");
    d->mapper->setMapping(adcButton, "ADC");
    d->mapper->setMapping(raButton, "RA");
    d->mapper->setMapping(vrButton, "VR");
    d->mapper->setMapping(clButton, "Cl");
    d->mapper->setMapping(cpButton, "Cp");
    d->mapper->setMapping(csButton, "Cs");
    d->mapper->setMapping(l1Button, "Lambda1");
    d->mapper->setMapping(l2Button, "Lambda2");
    d->mapper->setMapping(l3Button, "Lambda3");
    d->mapper->setMapping(facButton, "ColorFA");
    d->mapper->setMapping(falButton, "LogFA");
    
    connect (d->mapper, SIGNAL(mapped(QString)), this, SLOT(requestProcess(QString)));
    
    this->addWidget(widget);
}

itkProcessTensorScalarMapsToolBox::~itkProcessTensorScalarMapsToolBox(void)
{
    delete d;
    d = NULL;
}

bool itkProcessTensorScalarMapsToolBox::registered(void)
{
    return medToolBoxFactory::instance()->registerToolBox<itkProcessTensorScalarMapsToolBox>();
}

QString itkProcessTensorScalarMapsToolBox::processName()
{
    return "itkProcessTensorScalarMaps";
}

void itkProcessTensorScalarMapsToolBox::setProcessParameters(medAbstractDiffusionProcess *process)
{
    itkProcessTensorScalarMaps *castProcess = dynamic_cast <itkProcessTensorScalarMaps *> (process);

    if (!castProcess)
        return;
    
    castProcess->setTensorScalarMapRequested(d->mapRequested);
}

void itkProcessTensorScalarMapsToolBox::requestProcess(QString mapRequested)
{
    d->mapRequested = mapRequested;
    
    emit processStartRequested();
}

dtkPlugin * itkProcessTensorScalarMapsToolBox::plugin() const
{
    medPluginManager* pm = medPluginManager::instance();
    dtkPlugin* plugin = pm->plugin ( "itkProcessTensorLogEuclideanPlugin" );
    return plugin;
}


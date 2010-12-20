#include "medToolBoxFiberView.h"

class medToolBoxFiberViewPrivate
{
public:
	QComboBox    *colorCombo;
    QCheckBox    *displayCheckBox;
    QRadioButton *displayRadioPolylines;
    QRadioButton *displayRadioRibbons;
    QRadioButton *displayRadioTubes;
    QSlider      *radiusSlider;
};


medToolBoxFiberView::medToolBoxFiberView(QWidget *parent) : medToolBox(parent), d(new medToolBoxFiberViewPrivate)
{
    QWidget *displayWidget = new QWidget(this);
    
    QLabel *colorLabel = new QLabel("Color fibers by:", displayWidget);
    
    d->colorCombo = new QComboBox(displayWidget);
    d->colorCombo->addItem("Local orientation");
    d->colorCombo->addItem("Global orientation");
    d->colorCombo->addItem("Fractional anisotropy");
    
    QHBoxLayout *colorLayout = new QHBoxLayout;
    colorLayout->addWidget(colorLabel);
    colorLayout->addWidget(d->colorCombo);
    
    d->displayCheckBox = new QCheckBox("Use hardware acceleration", displayWidget);
    
    d->displayRadioPolylines = new QRadioButton("Display fibers as polylines", displayWidget);
    d->displayRadioRibbons = new QRadioButton("Display fibers as ribbons", displayWidget);
    d->displayRadioTubes = new QRadioButton("Display fibers as tubes", displayWidget);
    
    QButtonGroup *displayRadioGroup = new QButtonGroup(displayWidget);
    displayRadioGroup->addButton(d->displayRadioPolylines);
    displayRadioGroup->addButton(d->displayRadioRibbons);
    displayRadioGroup->addButton(d->displayRadioTubes);
    displayRadioGroup->setExclusive(true);
    
    QVBoxLayout *displayGroupLayout = new QVBoxLayout;
    displayGroupLayout->addWidget(d->displayRadioPolylines);
    displayGroupLayout->addWidget(d->displayRadioRibbons);
    displayGroupLayout->addWidget(d->displayRadioTubes);
    displayGroupLayout->addStretch(1);
    
    d->displayRadioPolylines->setChecked(true);
    
    QLabel *radiusLabel = new QLabel("Fibers radius:", displayWidget);
    
    d->radiusSlider = new QSlider(Qt::Horizontal, displayWidget);
    d->radiusSlider->setMinimum(1);
    d->radiusSlider->setMaximum(10);
    
    QHBoxLayout *radiusLayout = new QHBoxLayout;
    radiusLayout->addWidget(radiusLabel);
    radiusLayout->addWidget(d->radiusSlider);
    
    QVBoxLayout *displayLayout = new QVBoxLayout(displayWidget);
    displayLayout->addLayout(colorLayout);
    displayLayout->addWidget(d->displayCheckBox);
    displayLayout->addLayout(displayGroupLayout);
    displayLayout->addLayout(radiusLayout);
    
    connect (d->colorCombo,            SIGNAL(currentIndexChanged(int)), this, SIGNAL (fiberColorModeChanged (int)));
    connect (d->displayCheckBox,       SIGNAL(toggled(bool)),            this, SIGNAL (GPUActivated (bool)));
    connect (d->displayRadioPolylines, SIGNAL(toggled(bool)),            this, SIGNAL (lineModeSelected (bool)));
    connect (d->displayRadioRibbons,   SIGNAL(toggled(bool)),            this, SIGNAL (ribbonModeSelected (bool)));
    connect (d->displayRadioTubes,     SIGNAL(toggled(bool)),            this, SIGNAL (tubeModeSelected (bool)));
    connect (d->radiusSlider,          SIGNAL(valueChanged(int)),        this, SIGNAL (fiberRadiusSet(int)));
    
    this->setTitle("Fiber View");
    this->setWidget(displayWidget);
}

medToolBoxFiberView::~medToolBoxFiberView()
{
    delete d;
    d = NULL;
}

void medToolBoxFiberView::update (dtkAbstractView *view)
{
}

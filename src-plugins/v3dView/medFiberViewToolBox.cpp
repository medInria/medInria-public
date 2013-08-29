/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include "medFiberViewToolBox.h"

#include <medToolBoxFactory.h>
#include <v3dViewFiberInteractor.h>
#include <v3dView.h>

class medFiberViewToolBoxPrivate
{
public:
    QComboBox    *colorCombo;
    QCheckBox    *displayCheckBox;
    QRadioButton *displayRadioPolylines;
    QRadioButton *displayRadioRibbons;
    QRadioButton *displayRadioTubes;
    QSlider      *radiusSlider;

    v3dView * view;
    v3dViewFiberInteractor * interactor;
};


medFiberViewToolBox::medFiberViewToolBox(QWidget *parent) : medToolBox(parent), d(new medFiberViewToolBoxPrivate)
{
    d->view= 0;
    d->interactor = 0;

    QWidget *displayWidget = new QWidget(this);

    QString colorFibersToolTip = tr("Choose the coloring method of the fibers.");
    QLabel *colorLabel = new QLabel("Color fibers by:", displayWidget);
    colorLabel->setToolTip(colorFibersToolTip);

    d->colorCombo = new QComboBox(displayWidget);
    d->colorCombo->addItem("Local orientation");
    d->colorCombo->addItem("Global orientation");
    d->colorCombo->addItem("Fractional anisotropy");
    d->colorCombo->setToolTip(colorFibersToolTip);

    QHBoxLayout *colorLayout = new QHBoxLayout;
    colorLayout->addWidget(colorLabel);
    colorLayout->addWidget(d->colorCombo);

    d->displayCheckBox = new QCheckBox("Use hardware acceleration", displayWidget);
    d->displayCheckBox->setToolTip(tr("Select to use GPU hardware acceleration when possible."));

    d->displayRadioPolylines = new QRadioButton("Display fibers as polylines", displayWidget);
    d->displayRadioPolylines->setToolTip(tr("Use polylines to draw the fibers."));
    d->displayRadioRibbons = new QRadioButton("Display fibers as ribbons", displayWidget);
    d->displayRadioRibbons->setToolTip(tr("Use ribbons to draw the fibers."));
    d->displayRadioTubes = new QRadioButton("Display fibers as tubes", displayWidget);
    d->displayRadioTubes->setToolTip(tr("Use tubes to draw the fibers."));

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

    QString radiusToolTip = tr("Increase of decrease the radius of the fibers (except if there are being drawn with polylines).");
    QLabel *radiusLabel = new QLabel("Fibers radius:", displayWidget);
    radiusLabel->setToolTip(radiusToolTip);

    d->radiusSlider = new QSlider(Qt::Horizontal, displayWidget);
    d->radiusSlider->setMinimum(1);
    d->radiusSlider->setMaximum(10);
    d->radiusSlider->setToolTip(radiusToolTip);

    QHBoxLayout *radiusLayout = new QHBoxLayout;
    radiusLayout->addWidget(radiusLabel);
    radiusLayout->addWidget(d->radiusSlider);

    QVBoxLayout *displayLayout = new QVBoxLayout(displayWidget);
    displayLayout->addLayout(colorLayout);
    displayLayout->addWidget(d->displayCheckBox);
    displayLayout->addLayout(displayGroupLayout);
    displayLayout->addLayout(radiusLayout);

    connect (d->colorCombo,            SIGNAL(currentIndexChanged(int)), this, SLOT(setFiberColorMode(int)));
    connect (d->displayCheckBox,       SIGNAL(toggled(bool)),            this, SLOT(activateGPU(bool)));
    connect (d->displayRadioPolylines, SIGNAL(toggled(bool)),            this, SLOT(selectLineMode (bool)));
    connect (d->displayRadioRibbons,   SIGNAL(toggled(bool)),            this, SLOT(selectRibbonMode(bool)));
    connect (d->displayRadioTubes,     SIGNAL(toggled(bool)),            this, SLOT(selectTubeMode(bool)));
    connect (d->radiusSlider,          SIGNAL(valueChanged(int)),        this, SLOT(setFiberRadius(int)));

    this->setTitle("Fiber View");
    this->addWidget(displayWidget);

    this->setValidDataTypes(QStringList() << "v3dDataFibers");

    this->hide();
}

medFiberViewToolBox::~medFiberViewToolBox()
{
    delete d;
    d = NULL;
}

int medFiberViewToolBox::fiberRadius()
{
    return d->radiusSlider->value();
}

int medFiberViewToolBox::fiberColorMode()
{
    return d->colorCombo->currentIndex();
}

bool medFiberViewToolBox::isGPUActivated()
{
    return d->displayCheckBox->isChecked();
}

bool medFiberViewToolBox::isLineModeSelected()
{
    return d->displayRadioPolylines->isChecked();
}

bool medFiberViewToolBox::isRibbonModeSelected()
{
    return d->displayRadioRibbons->isChecked();
}

bool medFiberViewToolBox::isTubesModeSelected()
{
    return d->displayRadioTubes->isChecked();
}

void medFiberViewToolBox::setFiberColorMode(int index)
{
    if (index==0)
        d->interactor->setColorMode(v3dViewFiberInteractor::Local);
    else if (index==1)
        d->interactor->setColorMode(v3dViewFiberInteractor::Global);
    else if (index==2)
        d->interactor->setColorMode(v3dViewFiberInteractor::FA);
 }

void medFiberViewToolBox::activateGPU (bool value)
{
    d->interactor->activateGPU(value);
}

void medFiberViewToolBox::selectLineMode (bool value)
{
    if(value)
    {
        d->interactor->setRenderingMode(v3dViewFiberInteractor::Lines);
        d->view->update();
    }
}

void medFiberViewToolBox::selectRibbonMode (bool value)
{
    if(value)
    {
        d->interactor->setRenderingMode(v3dViewFiberInteractor::Ribbons);
        d->view->update();
    }
}

void medFiberViewToolBox::selectTubeMode (bool value)
{
    if(value)
    {
        d->interactor->setRenderingMode(v3dViewFiberInteractor::Tubes);
        d->view->update();
    }
}

void medFiberViewToolBox::setFiberRadius(int radius)
{
    d->interactor->setRadius(radius);
}

void medFiberViewToolBox::update (dtkAbstractView *view)
{
    medToolBox::update(view);
    if (!view)
    {
        d->view = 0;
        d->interactor = 0;
        return;
    }

    d->view = qobject_cast<v3dView*>(view);
    d->interactor = qobject_cast<v3dViewFiberInteractor*>(d->view->dtkAbstractView::interactor("v3dViewFiberInteractor"));
}

bool medFiberViewToolBox::registered()
{
    return medToolBoxFactory::instance()->registerToolBox<medFiberViewToolBox>("medFiberViewToolBox",
                                                                               "medFiberViewToolBox",
                                                                               "Fiber View ToolBox",
                                                                               QStringList()<<"view"<<"fibers");
}

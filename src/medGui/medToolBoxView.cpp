/* medToolBoxView.cpp --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Fri Feb 19 09:06:02 2010 (+0100)
 * Version: $Id$
 * Last-Updated: Fri Feb 19 10:27:55 2010 (+0100)
 *           By: Julien Wintz
 *     Update #: 18
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#include "medToolBoxView.h"

#include <dtkCore/dtkAbstractView.h>

class medToolBoxViewPrivate
{
public:
    QComboBox *foregroundLookupTableComboBox;
    QComboBox *backgroundLookupTableComboBox;
    QComboBox *presetComboBox;
    QCheckBox *scalarBarVisibilityCheckBox;
    QCheckBox *axisVisibilityCheckBox;

    QComboBox   *view3dModeComboBox;
    QComboBox   *view3dVRModeComboBox;
    QSlider     *view3dLODSlider;
    QPushButton *windowingPushButton;
    QPushButton *zoomingPushButton;
    QPushButton *slicingPushButton;
    QPushButton *measuringPushButton;
    QPushButton *croppingPushButton;
};

medToolBoxView::medToolBoxView(QWidget *parent) : medToolBox(parent), d(new medToolBoxViewPrivate)
{
    d->foregroundLookupTableComboBox = new QComboBox(this);
    d->foregroundLookupTableComboBox->setFocusPolicy(Qt::NoFocus);
    d->foregroundLookupTableComboBox->addItem("Default");
    d->foregroundLookupTableComboBox->addItem("Black&White");
    d->foregroundLookupTableComboBox->addItem("Black&WhiteInversed");
    d->foregroundLookupTableComboBox->addItem("Spectrum");
    d->foregroundLookupTableComboBox->addItem("HotMetal");
    d->foregroundLookupTableComboBox->addItem("GE");
    d->foregroundLookupTableComboBox->addItem("Loni");
    d->foregroundLookupTableComboBox->addItem("Loni2");
    d->foregroundLookupTableComboBox->addItem("Asymmetry");
    d->foregroundLookupTableComboBox->addItem("PValue");
    d->foregroundLookupTableComboBox->addItem("blueBlackAlpha");
    d->foregroundLookupTableComboBox->addItem("greenBlackAlpha");
    d->foregroundLookupTableComboBox->addItem("redBlackAlpha");
    d->foregroundLookupTableComboBox->addItem("Muscles&Bones");
    d->foregroundLookupTableComboBox->addItem("Red Vessels");
    d->foregroundLookupTableComboBox->addItem("Bones");
    d->foregroundLookupTableComboBox->addItem("Stern");

    d->backgroundLookupTableComboBox = new QComboBox(this);
    d->backgroundLookupTableComboBox->setFocusPolicy(Qt::NoFocus);
    d->backgroundLookupTableComboBox->addItem("Default");
    d->backgroundLookupTableComboBox->addItem("Black&White");
    d->backgroundLookupTableComboBox->addItem("Black&WhiteInversed");
    d->backgroundLookupTableComboBox->addItem("Spectrum");
    d->backgroundLookupTableComboBox->addItem("HotMetal");
    d->backgroundLookupTableComboBox->addItem("GE");
    d->backgroundLookupTableComboBox->addItem("Loni");
    d->backgroundLookupTableComboBox->addItem("Loni2");
    d->backgroundLookupTableComboBox->addItem("Asymmetry");
    d->backgroundLookupTableComboBox->addItem("PValue");
    d->backgroundLookupTableComboBox->addItem("blueBlackAlpha");
    d->backgroundLookupTableComboBox->addItem("greenBlackAlpha");
    d->backgroundLookupTableComboBox->addItem("redBlackAlpha");
    d->backgroundLookupTableComboBox->addItem("Muscles&Bones");
    d->backgroundLookupTableComboBox->addItem("Red Vessels");
    d->backgroundLookupTableComboBox->addItem("Bones");
    d->backgroundLookupTableComboBox->addItem("Stern");
    d->backgroundLookupTableComboBox->hide();

    d->presetComboBox = new QComboBox(this);
    d->presetComboBox->setFocusPolicy(Qt::NoFocus);
    d->presetComboBox->addItem("None");
    d->presetComboBox->addItem("VR Muscles&Bones");
    d->presetComboBox->addItem("Vascular I");
    d->presetComboBox->addItem("Vascular II");
    d->presetComboBox->addItem("Vascular III");
    d->presetComboBox->addItem("Vascular IV");
    d->presetComboBox->addItem("Standard");
    d->presetComboBox->addItem("Soft");
    d->presetComboBox->addItem("Soft on White");
    d->presetComboBox->addItem("Soft on Blue");
    d->presetComboBox->addItem("Red on White");
    d->presetComboBox->addItem("Glossy");

    d->view3dModeComboBox = new QComboBox(this);
    d->view3dModeComboBox->setFocusPolicy(Qt::NoFocus);
    d->view3dModeComboBox->addItem("VR");
    d->view3dModeComboBox->addItem("MIP - Maximum");
    d->view3dModeComboBox->addItem("MIP - Minimum");
    d->view3dModeComboBox->addItem("MPR");
    d->view3dModeComboBox->addItem("Off");

    d->view3dVRModeComboBox = new QComboBox(this);
    d->view3dVRModeComboBox->setFocusPolicy(Qt::NoFocus);
    d->view3dVRModeComboBox->addItem( "GPU" );
    d->view3dVRModeComboBox->addItem( "RayCastAndTexture" );
    d->view3dVRModeComboBox->addItem( "RayCast" );
    d->view3dVRModeComboBox->addItem( "Texture" );
    d->view3dVRModeComboBox->addItem( "Default" );

    d->view3dLODSlider = new QSlider (Qt::Horizontal, this);
    d->view3dLODSlider->setRange (0, 100);
    d->view3dLODSlider->setValue (100);

    d->windowingPushButton = new QPushButton("", this);
    d->windowingPushButton->setIcon (QIcon (":/icons/wlww.tiff"));
    d->windowingPushButton->setCheckable (true);
    d->windowingPushButton->setMinimumWidth ( 20 );
    d->zoomingPushButton   = new QPushButton("", this);
    d->zoomingPushButton->setIcon (QIcon (":/icons/zoom.tiff"));
    d->zoomingPushButton->setCheckable (true);
    d->slicingPushButton   = new QPushButton("", this);
    d->slicingPushButton->setIcon (QIcon (":/icons/stack.tiff"));
    d->slicingPushButton->setCheckable (true);
    d->measuringPushButton = new QPushButton("", this);
    d->measuringPushButton->setIcon (QIcon (":/icons/length.tiff"));
    d->measuringPushButton->setCheckable (true);

    d->croppingPushButton = new QPushButton ("", this);
    d->croppingPushButton->setIcon (QIcon (":/icons/cropping.png"));
    d->croppingPushButton->setCheckable (true);
    d->croppingPushButton->setMinimumWidth ( 20 );

    QButtonGroup *mouseGroup = new QButtonGroup (this);
    mouseGroup->addButton ( d->windowingPushButton );
    mouseGroup->addButton ( d->zoomingPushButton );
    mouseGroup->addButton ( d->slicingPushButton );
    mouseGroup->addButton ( d->measuringPushButton );
    mouseGroup->setExclusive (true);

    QHBoxLayout *mouseLayout = new QHBoxLayout;    
    mouseLayout->addWidget(d->windowingPushButton);
    mouseLayout->addWidget(d->slicingPushButton);
    mouseLayout->addWidget(d->zoomingPushButton);
    mouseLayout->addWidget(d->measuringPushButton);
        
    connect(d->foregroundLookupTableComboBox, SIGNAL(currentIndexChanged(QString)), this, SIGNAL(foregroundLookupTableChanged(QString)));
    connect(d->backgroundLookupTableComboBox, SIGNAL(currentIndexChanged(QString)), this, SIGNAL(backgroundLookupTableChanged(QString)));
    connect(d->presetComboBox,                SIGNAL(currentIndexChanged(QString)), this, SIGNAL(lutPresetChanged(QString)));
    connect(d->view3dModeComboBox,            SIGNAL(currentIndexChanged(QString)), this, SIGNAL(tdModeChanged(QString)));
    connect(d->view3dVRModeComboBox,          SIGNAL(currentIndexChanged(QString)), this, SIGNAL(tdVRModeChanged(QString)));
    connect(d->view3dLODSlider,               SIGNAL(valueChanged(int)),            this, SIGNAL(tdLodChanged(int)));
    connect(d->windowingPushButton,           SIGNAL(toggled(bool)),                this, SIGNAL(windowingChanged(bool)));
    connect(d->zoomingPushButton,             SIGNAL(toggled(bool)),                this, SIGNAL(zoomingChanged(bool)));
    connect(d->slicingPushButton,             SIGNAL(toggled(bool)),                this, SIGNAL(slicingChanged(bool)));
    connect(d->measuringPushButton,           SIGNAL(toggled(bool)),                this, SIGNAL(measuringChanged(bool)));
    connect(d->croppingPushButton,            SIGNAL(toggled(bool)),                this, SIGNAL(croppingChanged(bool)));

    d->scalarBarVisibilityCheckBox = new QCheckBox(this);
 
    connect(d->scalarBarVisibilityCheckBox, SIGNAL(toggled(bool)), this, SIGNAL(scalarBarVisibilityChanged(bool)));

    d->axisVisibilityCheckBox = new QCheckBox(this);

    connect(d->axisVisibilityCheckBox, SIGNAL(toggled(bool)), this, SIGNAL(axisVisibilityChanged(bool)));

    QWidget *viewToolBoxWidget = new QWidget;
    QWidget *view3dToolBoxWidget = new QWidget;
    QWidget *mouseToolBoxWidget = new QWidget;

    QFormLayout *lutToolBoxWidgetLayout = new QFormLayout(viewToolBoxWidget);
    lutToolBoxWidgetLayout->addRow("Color lut:", d->foregroundLookupTableComboBox);
    lutToolBoxWidgetLayout->addRow("Preset:", d->presetComboBox);
    lutToolBoxWidgetLayout->addRow("Show axis:", d->axisVisibilityCheckBox);
    lutToolBoxWidgetLayout->addRow("Show scalars:", d->scalarBarVisibilityCheckBox);
    lutToolBoxWidgetLayout->setFormAlignment(Qt::AlignHCenter);

    QFormLayout *view3dToolBoxWidgetLayout = new QFormLayout(view3dToolBoxWidget);
    view3dToolBoxWidgetLayout->addRow("3D Mode:", d->view3dModeComboBox);
    view3dToolBoxWidgetLayout->addRow("Renderer:", d->view3dVRModeComboBox);
    view3dToolBoxWidgetLayout->addRow("LOD:", d->view3dLODSlider);
    view3dToolBoxWidgetLayout->addRow("Cropping:", d->croppingPushButton);
    view3dToolBoxWidgetLayout->setFormAlignment(Qt::AlignHCenter);

    QFormLayout *mouseToolBoxWidgetLayout = new QFormLayout(mouseToolBoxWidget);
    mouseToolBoxWidgetLayout->addRow ("Type:", mouseLayout);
    mouseToolBoxWidgetLayout->setFormAlignment(Qt::AlignHCenter);
    
    medToolBoxTab *viewToolBoxTab = new medToolBoxTab(this);
    viewToolBoxTab->addTab(viewToolBoxWidget, "All");
    viewToolBoxTab->addTab(mouseToolBoxWidget, "2D");
    viewToolBoxTab->addTab(view3dToolBoxWidget, "3D");

    this->setTitle("View");
    this->setWidget(viewToolBoxTab);
}

medToolBoxView::~medToolBoxView(void)
{
    delete d;

    d = NULL;
}

void medToolBoxView::update(dtkAbstractView *view)
{
    d->foregroundLookupTableComboBox->blockSignals(true);
    d->foregroundLookupTableComboBox->setCurrentIndex(d->foregroundLookupTableComboBox->findText(view->property("LookupTable")));
    d->foregroundLookupTableComboBox->blockSignals(false);

    d->backgroundLookupTableComboBox->blockSignals(true);
    d->backgroundLookupTableComboBox->setCurrentIndex(d->backgroundLookupTableComboBox->findText(view->property("BackgroundLookupTable")));
    d->backgroundLookupTableComboBox->blockSignals(false);

    d->axisVisibilityCheckBox->blockSignals(true);
    d->axisVisibilityCheckBox->setChecked(view->property("ShowAxis") == "true");
    d->axisVisibilityCheckBox->blockSignals(false);

    d->scalarBarVisibilityCheckBox->blockSignals(true);
    d->scalarBarVisibilityCheckBox->setChecked(view->property("ScalarBarVisibility") == "true");
    d->scalarBarVisibilityCheckBox->blockSignals(false);

    d->windowingPushButton->blockSignals(true);
    d->zoomingPushButton->blockSignals(true);
    d->slicingPushButton->blockSignals(true);
    d->measuringPushButton->blockSignals(true);
    if( view->property("LeftClickInteraction")=="Windowing" )
      d->windowingPushButton->setChecked(true);
    if( view->property("LeftClickInteraction")=="Zooming" )
      d->zoomingPushButton->setChecked(true);
    if( view->property("LeftClickInteraction")=="Slicing" )
      d->slicingPushButton->setChecked(true);
    if( view->property("LeftClickInteraction")=="Measuring" )
      d->measuringPushButton->setChecked(true);
    d->windowingPushButton->blockSignals(false);
    d->zoomingPushButton->blockSignals(false);
    d->slicingPushButton->blockSignals(false);
    d->measuringPushButton->blockSignals(false);

    d->view3dModeComboBox->blockSignals(true);
    d->view3dModeComboBox->setCurrentIndex(d->view3dModeComboBox->findText(view->property("Mode")));
    d->view3dModeComboBox->blockSignals(false);

    d->view3dVRModeComboBox->blockSignals(true);
    d->view3dVRModeComboBox->setCurrentIndex(d->view3dVRModeComboBox->findText(view->property("VRMode")));
    d->view3dVRModeComboBox->blockSignals(false);

    d->presetComboBox->blockSignals(true);
    d->presetComboBox->setCurrentIndex(d->presetComboBox->findText(view->property("Preset")));
    d->presetComboBox->blockSignals(false);

    d->croppingPushButton->blockSignals(true);
    if( view->property("Cropping")=="true" )
        d->croppingPushButton->setChecked(true);
    else
        d->croppingPushButton->setChecked(false);
    d->croppingPushButton->blockSignals(false);
}

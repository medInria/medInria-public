/* medViewerArea.cpp --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Fri Sep 18 12:43:06 2009 (+0200)
 * Version: $Id$
 * Last-Updated: Tue Nov  3 09:11:08 2009 (+0100)
 *           By: Julien Wintz
 *     Update #: 497
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#include "medViewerArea.h"

#include <dtkCore/dtkAbstractViewFactory.h>
#include <dtkCore/dtkAbstractView.h>
#include <dtkCore/dtkAbstractDataFactory.h>
#include <dtkCore/dtkAbstractData.h>
#include <dtkCore/dtkAbstractDataReader.h>
#include <dtkCore/dtkGlobal.h>

#include <medCore/medDataIndex.h>
#include <medCore/medDataManager.h>

#include <medSql/medDatabaseController.h>
#include <medSql/medDatabaseNavigator.h>

#include <medGui/medClutEditor.h>
#include <medGui/medLayoutChooser.h>
#include <medGui/medToolBox.h>
#include <medGui/medToolBoxContainer.h>
#include <medGui/medViewContainer.h>

#include <QtGui>
#include <QtSql>

// /////////////////////////////////////////////////////////////////
// medViewerAreaStack
// /////////////////////////////////////////////////////////////////

class medViewerAreaStackPrivate
{
public:
    int id;

    medViewContainer *container_single;
    medViewContainer *container_multi;
    medViewContainer *container_custom;
};

medViewerAreaStack::medViewerAreaStack(QWidget *parent) : QStackedWidget(parent), d(new medViewerAreaStackPrivate)
{
    d->container_single = new medViewContainer(this);
    d->container_multi = new medViewContainer(this);
    d->container_custom = new medViewContainer(this);

    this->addWidget(d->container_single);
    this->addWidget(d->container_multi);
    this->addWidget(d->container_custom);

    this->setCurrentIndex(0);

    connect(d->container_single, SIGNAL(focused(dtkAbstractView*)), this, SIGNAL(focused(dtkAbstractView*)));
    connect(d->container_multi,  SIGNAL(focused(dtkAbstractView*)), this, SIGNAL(focused(dtkAbstractView*)));
    connect(d->container_custom, SIGNAL(focused(dtkAbstractView*)), this, SIGNAL(focused(dtkAbstractView*)));
}

medViewerAreaStack::~medViewerAreaStack(void)
{
    delete d;

    d = NULL;
}

void medViewerAreaStack::setPatientId(int id)
{
    d->id = id;
}

medViewContainer *medViewerAreaStack::current(void)
{
    if(this->currentIndex() == 0)
        return d->container_single;

    if(this->currentIndex() == 1)
        return d->container_multi;

    if(this->currentIndex() == 2)
        return d->container_custom;

    return NULL;
}

// /////////////////////////////////////////////////////////////////
// medViewerArea
// /////////////////////////////////////////////////////////////////

class medViewerAreaPrivate
{
public:
    medToolBoxContainer *toolbox_container;

    medDatabaseNavigator *navigator;

    QStackedWidget *stack;

    // toolbox elements

    QComboBox *patientComboBox;

    QComboBox *foregroundLookupTableComboBox;
    QComboBox *backgroundLookupTableComboBox;
    QComboBox *presetComboBox;
    QCheckBox *scalarBarVisibilityCheckBox;
    QCheckBox *axisVisibilityCheckBox;

    QComboBox   *view3dModeComboBox;
    QSlider     *view3dLODSlider;
    QPushButton *windowingPushButton;
    QPushButton *zoomingPushButton;
    QPushButton *slicingPushButton;
    QPushButton *measuringPushButton;
    QPushButton *croppingPushButton;
  
    // view containers hash

    QHash<int, medViewerAreaStack *> view_stacks;

    // status elements

    QProgressBar *progress;
};

medViewerArea::medViewerArea(QWidget *parent) : QWidget(parent), d(new medViewerAreaPrivate)
{
    d->stack = new QStackedWidget(this);
    d->stack->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    d->patientComboBox = new QComboBox(this);
    d->patientComboBox->setFocusPolicy(Qt::NoFocus);
    connect(d->patientComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onPatientIndexChanged(int)));

    // Setting up patient chooser

    QWidget *patientToolBoxWidget = new QWidget;

    QHBoxLayout *patientToolBoxWidgetLayout = new QHBoxLayout(patientToolBoxWidget);
    patientToolBoxWidgetLayout->addWidget(d->patientComboBox);

    medToolBox *patientToolBox = new medToolBox(this);
    patientToolBox->setTitle("Patient");
    patientToolBox->setWidget(patientToolBoxWidget);

    // Setting up layout chooser

    medLayoutChooser *layoutChooser = new medLayoutChooser;

    connect(layoutChooser, SIGNAL(selected(int,int)), this, SLOT(split(int,int)));

//    QWidgetAction *layoutAction = new QWidgetAction(0);
//    layoutAction->setDefaultWidget(layoutChooser);
//
//    QMenu *layoutMenu = new QMenu;
//    layoutMenu->addAction(layoutAction);
//
//    QPushButton *singleLayoutButton = new QPushButton("Single", this);
//    QPushButton *multiLayoutButton = new QPushButton("Multi", this);
//    QPushButton *customLayoutButton = new QPushButton("Custom", this);
//    customLayoutButton->setMenu(layoutMenu);
//
//    QWidget *layoutToolBoxWidget = new QWidget;
//
//    QHBoxLayout *layoutToolBoxWidgetLayout = new QHBoxLayout(layoutToolBoxWidget);
//    layoutToolBoxWidgetLayout->addWidget(singleLayoutButton);
//    layoutToolBoxWidgetLayout->addWidget(multiLayoutButton);
//    layoutToolBoxWidgetLayout->addWidget(customLayoutButton);

    QWidget *layoutToolBoxChooserPage = new QWidget(this);

    QHBoxLayout *layoutToolBoxChooserPageLayout = new QHBoxLayout(layoutToolBoxChooserPage);
    layoutToolBoxChooserPageLayout->setContentsMargins(0, 3, 0, 0);
    layoutToolBoxChooserPageLayout->setAlignment(Qt::AlignHCenter | Qt::AlignCenter);
    layoutToolBoxChooserPageLayout->addWidget(layoutChooser);

    medToolBoxTab *layoutToolBoxTab = new medToolBoxTab(this);
    layoutToolBoxTab->addTab(new QWidget, "Single");
    layoutToolBoxTab->addTab(new QWidget, "Multi");
    layoutToolBoxTab->addTab(layoutToolBoxChooserPage, "Custom");

    connect(layoutToolBoxTab, SIGNAL(currentChanged(int)), this, SLOT(setStackIndex(int)));

    medToolBox *layoutToolBox = new medToolBox(this);
    layoutToolBox->setTitle("Layout");
    layoutToolBox->setWidget(layoutToolBoxTab);

    // Setting up view settings

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

    d->view3dLODSlider = new QSlider (Qt::Horizontal, this);
    d->view3dLODSlider->setRange (100, 1000);
    d->view3dLODSlider->setValue (1000);

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

        
    connect(d->foregroundLookupTableComboBox, SIGNAL(currentIndexChanged(QString)), this, SLOT(setupForegroundLookupTable(QString)));
    connect(d->backgroundLookupTableComboBox, SIGNAL(currentIndexChanged(QString)), this, SLOT(setupBackgroundLookupTable(QString)));
    connect(d->presetComboBox,                SIGNAL(currentIndexChanged(QString)), this, SLOT(setupLUTPreset(QString)));
    connect(d->view3dModeComboBox,            SIGNAL(currentIndexChanged(QString)), this, SLOT(setup3DMode(QString)));
    connect(d->view3dLODSlider,               SIGNAL(valueChanged(int)),            this, SLOT(setup3DLOD(int)));
    connect(d->windowingPushButton,           SIGNAL(toggled(bool)),                this, SLOT(setupWindowing(bool)));
    connect(d->zoomingPushButton,             SIGNAL(toggled(bool)),                this, SLOT(setupZooming(bool)));
    connect(d->slicingPushButton,             SIGNAL(toggled(bool)),                this, SLOT(setupSlicing(bool)));
    connect(d->measuringPushButton,           SIGNAL(toggled(bool)),                this, SLOT(setupMeasuring(bool)));
    connect(d->croppingPushButton,            SIGNAL(toggled(bool)),                this, SLOT(setupCropping(bool)));

    d->scalarBarVisibilityCheckBox = new QCheckBox(this);
 
    connect(d->scalarBarVisibilityCheckBox, SIGNAL(toggled(bool)), this, SLOT(setupScalarBarVisibility(bool)));

    d->axisVisibilityCheckBox = new QCheckBox(this);

    connect(d->axisVisibilityCheckBox, SIGNAL(toggled(bool)), this, SLOT(setupAxisVisibility(bool)));

    QWidget *viewToolBoxWidget = new QWidget;
    QWidget *view3dToolBoxWidget = new QWidget;
    QWidget *mouseToolBoxWidget = new QWidget;

    QFormLayout *lutToolBoxWidgetLayout = new QFormLayout(viewToolBoxWidget);
    lutToolBoxWidgetLayout->addRow("Color lut:", d->foregroundLookupTableComboBox);
    lutToolBoxWidgetLayout->addRow("Preset:", d->presetComboBox);
    //lutToolBoxWidgetLayout->addRow("Background lut:", d->backgroundLookupTableComboBox);
    lutToolBoxWidgetLayout->addRow("Show axis:", d->axisVisibilityCheckBox);
    lutToolBoxWidgetLayout->addRow("Show scalars:", d->scalarBarVisibilityCheckBox);
    lutToolBoxWidgetLayout->setFormAlignment(Qt::AlignHCenter);


    QFormLayout *view3dToolBoxWidgetLayout = new QFormLayout(view3dToolBoxWidget);
    view3dToolBoxWidgetLayout->addRow("3D Mode:", d->view3dModeComboBox);
    view3dToolBoxWidgetLayout->addRow("LOD:", d->view3dLODSlider);
    view3dToolBoxWidgetLayout->addRow("Cropping:", d->croppingPushButton);
    view3dToolBoxWidgetLayout->setFormAlignment(Qt::AlignHCenter);


    QFormLayout *mouseToolBoxWidgetLayout = new QFormLayout(mouseToolBoxWidget);
    mouseToolBoxWidgetLayout->addRow ("Type:", mouseLayout);
    mouseToolBoxWidgetLayout->setFormAlignment(Qt::AlignHCenter);
    

    medToolBox *viewToolBox = new medToolBox(this);
    viewToolBox->setTitle("View");
    viewToolBox->setWidget(viewToolBoxWidget);

    medToolBox *view3dToolBox = new medToolBox(this);
    view3dToolBox->setTitle("3D");
    view3dToolBox->setWidget(view3dToolBoxWidget);

    medToolBox *mouseToolBox = new medToolBox(this);
    mouseToolBox->setTitle("Mouse");
    mouseToolBox->setWidget(mouseToolBoxWidget);

    
    // Setting up toolbox container

    d->toolbox_container = new medToolBoxContainer(this);
    d->toolbox_container->setFixedWidth(300);
    d->toolbox_container->addToolBox(patientToolBox);
    d->toolbox_container->addToolBox(layoutToolBox);
    d->toolbox_container->addToolBox(viewToolBox);
    d->toolbox_container->addToolBox(mouseToolBox);
    d->toolbox_container->addToolBox(view3dToolBox);

    
    // Setting up view container

    QWidget *view_container = new QWidget(this);

    QVBoxLayout *view_container_layout = new QVBoxLayout(view_container);
    view_container_layout->setContentsMargins(0, 10, 0, 10);
    view_container_layout->addWidget(d->stack);

    // Setting up navigator container

    QFrame *navigator_container = new QFrame(this);
    navigator_container->setObjectName("medNavigatorContainer");
    navigator_container->setFixedWidth(186);

    d->navigator = new medDatabaseNavigator(navigator_container);
    connect(d->navigator, SIGNAL(seriesClicked(int)), this, SLOT(onSeriesIndexChanged(int)));

    QVBoxLayout *navigator_container_layout = new QVBoxLayout(navigator_container);
    navigator_container_layout->setContentsMargins(0, 0, 0, 0);
    navigator_container_layout->setSpacing(0);
    navigator_container_layout->addWidget(d->navigator);
    navigator_container_layout->setAlignment(Qt::AlignHCenter);

    // status elements

    d->progress = new QProgressBar(this);
    d->progress->setTextVisible(false);
    d->progress->setMinimum(0);
    d->progress->setMaximum(100);
    d->progress->setValue(0);

    // Setting up layout

    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    layout->addWidget(d->toolbox_container);
    layout->addWidget(view_container);
    layout->addWidget(navigator_container);

    // Setup from database
    this->setup();

    connect(medDatabaseController::instance(), SIGNAL(updated()), this, SLOT(setup()));
}

medViewerArea::~medViewerArea(void)
{
    delete d->toolbox_container;
    delete d->stack;
    delete d;

    d = NULL;
}

void medViewerArea::setup(QStatusBar *status)
{
    status->addWidget(d->progress);

    d->progress->show();
}

void medViewerArea::setdw(QStatusBar *status)
{
    status->removeWidget(d->progress);

    d->progress->hide();
}

void medViewerArea::setPatientIndex(int index)
{
    d->patientComboBox->setCurrentIndex(index);
}

void medViewerArea::setStudyIndex(int index)
{

}

void medViewerArea::setSeriesIndex(int index)
{

}

void medViewerArea::setImageIndex(int index)
{

}

void medViewerArea::setup(void)
{
    d->patientComboBox->clear();
    d->patientComboBox->addItem("Choose patient");

    QSqlQuery query(*(medDatabaseController::instance()->database()));

    query.prepare("SELECT name, id FROM patient");
    if(!query.exec())
        qDebug() << DTK_COLOR_FG_RED << query.lastError() << DTK_NOCOLOR;

    while(query.next())
        d->patientComboBox->addItem(query.value(0).toString(), query.value(1));
}

void medViewerArea::split(int rows, int cols)
{
    if (d->view_stacks.count())
        d->view_stacks.value(d->patientComboBox->currentIndex())->current()->current()->split(rows, cols);
}

void medViewerArea::onPatientIndexChanged(int id)
{
    medDataIndex index = medDatabaseController::instance()->indexForStudy(id);

    if(!index.isValid())
        return;

    // Setup view container

    medViewerAreaStack *view_stack;

    if(!d->view_stacks.contains(id)) {
        view_stack = new medViewerAreaStack(this);
        view_stack->setPatientId(id);
        connect(view_stack, SIGNAL(focused(dtkAbstractView*)), this, SLOT(onViewFocused(dtkAbstractView*)));
        d->view_stacks.insert(id, view_stack);
        d->stack->addWidget(view_stack);
    } else {
        view_stack = d->view_stacks.value(id);
    }

    d->stack->setCurrentWidget(view_stack);

    // Setup navigator

    d->navigator->onPatientClicked(id);
}

void medViewerArea::onStudyIndexChanged(int id)
{
    medDataIndex index = medDatabaseController::instance()->indexForStudy(id);

    if(!index.isValid())
        return;

    Q_UNUSED(index);
}

void medViewerArea::onSeriesIndexChanged(int id)
{
    medDataIndex index = medDatabaseController::instance()->indexForSeries(id);

    if(!index.isValid())
        return;

    dtkAbstractData *data = medDatabaseController::instance()->read(index);

    if(data)
        medDataManager::instance()->insert(index, data);

    if (data) {

        dtkAbstractView *view = dtkAbstractViewFactory::instance()->create("v3dView");

        if (!view)
            return;

        view->setData(data);
        view->reset();

        d->view_stacks.value(d->patientComboBox->currentIndex())->current()->current()->setView(view);
        d->view_stacks.value(d->patientComboBox->currentIndex())->current()->current()->setFocus(Qt::MouseFocusReason);
    }
}

void medViewerArea::onImageIndexChanged(int id)
{
    medDataIndex index = medDatabaseController::instance()->indexForStudy(id);

    if(!index.isValid())
        return;

    Q_UNUSED(index);
}

void medViewerArea::onViewFocused(dtkAbstractView *view)
{
    d->foregroundLookupTableComboBox->blockSignals (true);
    d->foregroundLookupTableComboBox->setCurrentIndex(d->foregroundLookupTableComboBox->findText(view->property("LookupTable")));
    d->foregroundLookupTableComboBox->blockSignals (false);

    d->backgroundLookupTableComboBox->blockSignals (true);
    d->backgroundLookupTableComboBox->setCurrentIndex(d->backgroundLookupTableComboBox->findText(view->property("BackgroundLookupTable")));
    d->backgroundLookupTableComboBox->blockSignals (false);

    d->axisVisibilityCheckBox->blockSignals (true);
    d->axisVisibilityCheckBox->setChecked(view->property("ShowAxis") == "true");
    d->axisVisibilityCheckBox->blockSignals (false);

    d->scalarBarVisibilityCheckBox->blockSignals (true);
    d->scalarBarVisibilityCheckBox->setChecked(view->property("ScalarBarVisibility") == "true");
    d->scalarBarVisibilityCheckBox->blockSignals (false);

    d->windowingPushButton->blockSignals (true);
    d->zoomingPushButton->blockSignals (true);
    d->slicingPushButton->blockSignals (true);
    d->measuringPushButton->blockSignals (true);
    if( view->property("LeftClickInteraction")=="Windowing" )
      d->windowingPushButton->setChecked (true);
    if( view->property("LeftClickInteraction")=="Zooming" )
      d->zoomingPushButton->setChecked (true);
    if( view->property("LeftClickInteraction")=="Slicing" )
      d->slicingPushButton->setChecked (true);
    if( view->property("LeftClickInteraction")=="Measuring" )
      d->measuringPushButton->setChecked (true);
    d->windowingPushButton->blockSignals (false);
    d->zoomingPushButton->blockSignals (false);
    d->slicingPushButton->blockSignals (false);
    d->measuringPushButton->blockSignals (false);

    d->view3dModeComboBox->blockSignals (true);
    d->view3dModeComboBox->setCurrentIndex(d->view3dModeComboBox->findText(view->property("Mode")));
    d->view3dModeComboBox->blockSignals (false);

    d->presetComboBox->blockSignals (true);
    d->presetComboBox->setCurrentIndex(d->presetComboBox->findText(view->property("Preset")));
    d->presetComboBox->blockSignals (false);

    d->croppingPushButton->blockSignals (true);
    if( view->property("Cropping")=="true" )
        d->croppingPushButton->setChecked(true);
    else
        d->croppingPushButton->setChecked(false);
    d->croppingPushButton->blockSignals (false);
}

// layout settings

void medViewerArea::setStackIndex(int index)
{
    if (d->view_stacks.count())
        d->view_stacks.value(d->patientComboBox->currentIndex())->setCurrentIndex(index);
}

// view settings

void medViewerArea::setupForegroundLookupTable(QString table)
{
    if(!d->view_stacks.count())
        return;

    if(dtkAbstractView *view =  d->view_stacks.value(d->patientComboBox->currentIndex())->current()->current()->view()) {
        view->setProperty("LookupTable", table);
	view->update();
    }
    else
        qDebug() << "Unable to retrieve view";
}

void medViewerArea::setupBackgroundLookupTable(QString table)
{
    if(!d->view_stacks.count())
        return;
    
    if(dtkAbstractView *view =  d->view_stacks.value(d->patientComboBox->currentIndex())->current()->current()->view()) {
        view->setProperty("BackgroundLookupTable", table);
	view->update();
    }
}

void medViewerArea::setupAxisVisibility(bool visible)
{
    if(!d->view_stacks.count())
        return;
  
    if(dtkAbstractView *view = d->view_stacks.value(d->patientComboBox->currentIndex())->current()->current()->view()) {
        visible ? view->setProperty("ShowAxis", "true") : view->setProperty("ShowAxis", "false");
	view->update();
    }
}

void medViewerArea::setupScalarBarVisibility(bool visible)
{
    if(!d->view_stacks.count())
        return;
  
    if(dtkAbstractView *view =  d->view_stacks.value(d->patientComboBox->currentIndex())->current()->current()->view()) {
        visible ? view->setProperty("ScalarBarVisibility", "true") : view->setProperty("ScalarBarVisibility", "false");
	view->update();
    }
}

void medViewerArea::setup3DMode (QString table)
{
    if(!d->view_stacks.count())
        return;
  
    if(dtkAbstractView *view =  d->view_stacks.value(d->patientComboBox->currentIndex())->current()->current()->view()) {
        view->setProperty("Mode", table);
        view->update();
    }
}

void medViewerArea::setupLUTPreset (QString table)
{
    if(!d->view_stacks.count())
        return;
  
    if(dtkAbstractView *view =  d->view_stacks.value(d->patientComboBox->currentIndex())->current()->current()->view()) {
        view->setProperty("Preset", table);
	view->update();
    }
}

void medViewerArea::setup3DLOD (int value)
{
  qDebug() << __func__;
}

void medViewerArea::setupWindowing (bool checked)
{
    if(!d->view_stacks.count())
        return;

    if(dtkAbstractView *view =  d->view_stacks.value(d->patientComboBox->currentIndex())->current()->current()->view()) {
        view->setProperty("LeftClickInteraction", "Windowing");
	view->update();
    }
}

void medViewerArea::setupZooming (bool checked)
{
    if(!d->view_stacks.count())
        return;
  
    if(dtkAbstractView *view =  d->view_stacks.value(d->patientComboBox->currentIndex())->current()->current()->view()) {
        view->setProperty("LeftClickInteraction", "Zooming");
	view->update();
    }
}

void medViewerArea::setupSlicing (bool checked)
{
    if(!d->view_stacks.count())
        return;
  
    if(dtkAbstractView *view =  d->view_stacks.value(d->patientComboBox->currentIndex())->current()->current()->view()) {
        view->setProperty("LeftClickInteraction", "Slicing");
	view->update();
    }
}

void medViewerArea::setupMeasuring (bool checked)
{
    if(!d->view_stacks.count())
        return;
  
    if(dtkAbstractView *view =  d->view_stacks.value(d->patientComboBox->currentIndex())->current()->current()->view()) {
        view->setProperty("LeftClickInteraction", "Measuring");
	view->update();
    }
}

void medViewerArea::setupCropping (bool checked)
{
    if(!d->view_stacks.count())
        return;
  
    if(dtkAbstractView *view =  d->view_stacks.value(d->patientComboBox->currentIndex())->current()->current()->view()) {
        checked ? view->setProperty("Cropping", "true") : view->setProperty("Cropping", "false");
	view->update();
    }
}

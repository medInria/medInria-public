/* medViewerArea.cpp --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Fri Sep 18 12:43:06 2009 (+0200)
 * Version: $Id$
 * Last-Updated: Tue Jun 15 22:13:09 2010 (+0200)
 *           By: Julien Wintz
 *     Update #: 946
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#include "medViewerArea.h"
#include "medViewerAreaStack.h"
#include "medViewerConfiguration.h"
#include "medViewerConfigurator.h"

#include <dtkCore/dtkAbstractViewFactory.h>
#include <dtkCore/dtkAbstractView.h>
#include <dtkCore/dtkAbstractDataFactory.h>
#include <dtkCore/dtkAbstractData.h>
#include <dtkCore/dtkAbstractDataReader.h>
#include <dtkCore/dtkGlobal.h>

#include <medCore/medDataIndex.h>
#include <medCore/medDataManager.h>
#include <medCore/medViewManager.h>

#include <medSql/medDatabaseController.h>
#include <medSql/medDatabaseNavigator.h>

#include <medGui/medToolBox.h>
#include <medGui/medToolBoxContainer.h>
#include <medGui/medToolBoxLayout.h>
#include <medGui/medToolBoxPatient.h>
#include <medGui/medToolBoxDiffusion.h>
#include <medGui/medToolBoxRegistration.h>
#include <medGui/medToolBoxView.h>
#include <medGui/medViewContainer.h>
#include <medGui/medViewContainerCustom.h>
#include <medGui/medViewContainerMulti.h>
#include <medGui/medViewContainerSingle.h>

#include <QtGui>
#include <QtSql>

// /////////////////////////////////////////////////////////////////
// medViewerArea
// /////////////////////////////////////////////////////////////////

class medViewerAreaPrivate
{
public:
    medToolBoxPatient *patientToolBox;
    medToolBoxLayout *layoutToolBox;
    medToolBoxView *viewToolBox;
    medToolBoxDiffusion *diffusionToolBox;
    medToolBoxRegistration *registrationToolBox;
    medToolBoxContainer *toolbox_container;

    medDatabaseNavigator *navigator;

    QStackedWidget *stack;
  
    QHash<int, medViewerAreaStack *> view_stacks;

public:
    int current_patient;
    int current_container;
    int current_container_preset;
};

medViewerArea::medViewerArea(QWidget *parent) : QWidget(parent), d(new medViewerAreaPrivate)
{
    // -- Internal logic

    d->current_patient = -1;

    // -- User interface setup

    d->stack = new QStackedWidget(this);
    d->stack->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    // -- Patient toolbox --

    d->patientToolBox = new medToolBoxPatient(this);

    connect(d->patientToolBox, SIGNAL(patientIndexChanged(int)), this, SLOT(switchToPatient(int)));
    
    // -- Layout toolbox --

    d->layoutToolBox = new medToolBoxLayout(this);

    connect(d->layoutToolBox, SIGNAL(modeChanged(int)), this, SLOT(switchToContainer(int)));
    connect(d->layoutToolBox, SIGNAL(split(int, int)), this, SLOT(split(int, int)));
    connect(d->layoutToolBox, SIGNAL(presetClicked(int)), this, SLOT(switchToContainerPreset(int)));

    // -- View toolbox --
    
    d->viewToolBox = new medToolBoxView(this);

    connect(d->viewToolBox, SIGNAL(foregroundLookupTableChanged(QString)), this, SLOT(setupForegroundLookupTable(QString)));
    connect(d->viewToolBox, SIGNAL(backgroundLookupTableChanged(QString)), this, SLOT(setupBackgroundLookupTable(QString)));
    connect(d->viewToolBox, SIGNAL(lutPresetChanged(QString)), this, SLOT(setupLUTPreset(QString)));
    connect(d->viewToolBox, SIGNAL(tdModeChanged(QString)), this, SLOT(setup3DMode(QString)));
    connect(d->viewToolBox, SIGNAL(tdVRModeChanged(QString)), this, SLOT(setup3DVRMode(QString)));
    connect(d->viewToolBox, SIGNAL(tdLodChanged(int)), this, SLOT(setup3DLOD(int)));
    connect(d->viewToolBox, SIGNAL(windowingChanged(bool)), this, SLOT(setupWindowing(bool)));
    connect(d->viewToolBox, SIGNAL(zoomingChanged(bool)), this, SLOT(setupZooming(bool)));
    connect(d->viewToolBox, SIGNAL(slicingChanged(bool)), this, SLOT(setupSlicing(bool)));
    connect(d->viewToolBox, SIGNAL(measuringChanged(bool)), this, SLOT(setupMeasuring(bool)));
    connect(d->viewToolBox, SIGNAL(croppingChanged(bool)), this, SLOT(setupCropping(bool)));
    connect(d->viewToolBox, SIGNAL(scalarBarVisibilityChanged(bool)), this, SLOT(setupScalarBarVisibility(bool)));
    connect(d->viewToolBox, SIGNAL(axisVisibilityChanged(bool)), this, SLOT(setupAxisVisibility(bool)));
    connect(d->viewToolBox, SIGNAL(rulerVisibilityChanged(bool)), this, SLOT(setupRulerVisibility(bool)));

    // -- Diffusion toolbox --

    d->diffusionToolBox = new medToolBoxDiffusion(this);
    d->diffusionToolBox->setVisible(false);

    // -- Registration toolbox --

    d->registrationToolBox = new medToolBoxRegistration(this);
    d->registrationToolBox->setVisible(false);

    connect(d->registrationToolBox, SIGNAL(setupLayoutCompare()), this, SLOT(setupLayoutCompare()));
    connect(d->registrationToolBox, SIGNAL(setupLayoutFuse()), this, SLOT(setupLayoutFuse()));

    // Setting up toolbox container

    d->toolbox_container = new medToolBoxContainer(this);
    d->toolbox_container->setFixedWidth(320);
    d->toolbox_container->addToolBox(d->patientToolBox);
    d->toolbox_container->addToolBox(d->layoutToolBox);
    d->toolbox_container->addToolBox(d->viewToolBox);
    d->toolbox_container->addToolBox(d->diffusionToolBox);
    d->toolbox_container->addToolBox(d->registrationToolBox);
    
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

    QVBoxLayout *navigator_container_layout = new QVBoxLayout(navigator_container);
    navigator_container_layout->setContentsMargins(0, 0, 0, 0);
    navigator_container_layout->setSpacing(0);
    navigator_container_layout->addWidget(d->navigator);
    navigator_container_layout->setAlignment(Qt::AlignHCenter);

    // Setting up layout

    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    layout->addWidget(d->toolbox_container);
    layout->addWidget(view_container);
    layout->addWidget(navigator_container);

    // Setting up database

    this->setup();

    connect(medDatabaseController::instance(), SIGNAL(updated()), this, SLOT(setup()));

    // Setting up visualization configuration

    medViewerConfiguration *visualizationConfiguration = new medViewerConfiguration(this);
    visualizationConfiguration->attach(d->layoutToolBox, true);
    visualizationConfiguration->attach(d->viewToolBox, true);
    visualizationConfiguration->attach(0); // Single container when set up

    medViewerConfigurator::instance()->addConfiguration("Visualization", visualizationConfiguration);

    // Setting up diffusion configuration

    medViewerConfiguration *diffusionConfiguration = new medViewerConfiguration(this);
    diffusionConfiguration->attach(d->layoutToolBox, false);
    diffusionConfiguration->attach(d->viewToolBox, true);
    diffusionConfiguration->attach(d->diffusionToolBox, true);
    diffusionConfiguration->attach(0); // Single when set up

    medViewerConfigurator::instance()->addConfiguration("Diffusion", diffusionConfiguration);

    // Setting up registration configuration

    medViewerConfiguration *registrationConfiguration = new medViewerConfiguration(this);
    registrationConfiguration->attach(d->layoutToolBox, false);
    registrationConfiguration->attach(d->viewToolBox, true);
    registrationConfiguration->attach(d->registrationToolBox, true);
    registrationConfiguration->attach(3); // Registration compare container when set up

    medViewerConfigurator::instance()->addConfiguration("Registration", registrationConfiguration);
}

medViewerArea::~medViewerArea(void)
{
    delete d;

    d = NULL;
}

//! Status bar setup
/*! 
 *  Called whenever the viewer area is switched to. Add viewer area
 *  specific status widgets on the left of the status bar.
 */

void medViewerArea::setup(QStatusBar *status)
{

}

//! Status bar setdown
/*! 
 *  Called whenever the viewer area is left. Remove viewer area
 *  specific status widgets from the left of the status bar.
 */

void medViewerArea::setdw(QStatusBar *status)
{

}

//! Viewer area setup.
/*! 
 *  This methods sets the patient toolbox up by retreiving the list of
 *  partients from the database, add items using their names, and the
 *  database patient id as a user data for each item. Beware, the
 *  index of the patient in the combo box does not necessarily
 *  corresponds to the one of the patient in the database.
 */

void medViewerArea::setup(void)
{
    d->patientToolBox->clear();
    d->patientToolBox->addItem("Choose patient", -1);

    QSqlQuery query(*(medDatabaseController::instance()->database()));

    query.prepare("SELECT name, id FROM patient");
    if(!query.exec())
        qDebug() << DTK_COLOR_FG_RED << query.lastError() << DTK_NO_COLOR;

    while(query.next())
        d->patientToolBox->addItem(query.value(0).toString(), query.value(1));
}

//! Split the currently displayed custom container.
/*! 
 *  This slots make the connection between the layout toolbox gui and
 *  the actual custom view container.
 */

void medViewerArea::split(int rows, int cols)
{
    if (d->view_stacks.count())
        d->view_stacks.value(d->current_patient)->current()->split(rows, cols);
}

//! Open data corresponding to index \param index.
/*! 
 * 
 */

void medViewerArea::open(const medDataIndex& index)
{
    if(!((medDataIndex)index).isValid())
        return;

    this->switchToPatient(index.patientId());

    dtkAbstractData *data = NULL;
    dtkAbstractView *view = NULL;

    if(!data)
        data = medDataManager::instance()->data(index);

    if(!data)
        data = medDatabaseController::instance()->read(index);

    if(!data)
        return;

    medDataManager::instance()->insert(index, data);

    if(!view)
        view = d->view_stacks.value(d->current_patient)->current()->current()->view();

    if(!view)
        view = dtkAbstractViewFactory::instance()->create("v3dView");
    
    if(!view)
        return;

    medViewManager::instance()->insert(index, view);
    
    view->setData(data);
    view->reset();
    
    d->view_stacks.value(d->current_patient)->current()->current()->setView(view);
    d->view_stacks.value(d->current_patient)->current()->current()->setFocus(Qt::MouseFocusReason);
}

//! Open file on the local filesystem.
/*! 
 * 
 */

void medViewerArea::open(const QString& file)
{
    if (file.isEmpty())
        return;

    QFileInfo fileInfo(file);
    
    typedef dtkAbstractDataFactory::dtkAbstractDataTypeHandler dtkAbstractDataTypeHandler;
    
    QList<dtkAbstractDataTypeHandler> readers = dtkAbstractDataFactory::instance()->readers();

    dtkAbstractData* data = 0;
    
    for (int i=0; i<readers.size(); i++) {
        dtkAbstractDataReader* dataReader = dtkAbstractDataFactory::instance()->reader(readers[i].first, readers[i].second);
	if (dataReader->canRead(fileInfo.filePath())) {
	    dataReader->read(fileInfo.filePath());
	    data = dataReader->data();
	    delete dataReader;
	    break;
	}
    }

    if (!data) {
        qDebug() << "Cannot read data";
	return;
    }
        
    int id = d->patientToolBox->patientIndex(fileInfo.baseName());

    if (id == -1) {
        d->patientToolBox->addItem(fileInfo.baseName());
        id = d->patientToolBox->patientIndex(fileInfo.baseName());
    }
    
    if (id == -1)
        return;
    
    this->switchToPatient(id);

    dtkAbstractView *view = this->currentContainerFocused()->view();

    if(!view)    
        view = dtkAbstractViewFactory::instance()->create("v3dView");
    
    if(!view)
        return;

    view->setData(data);
    view->reset();
    
    this->currentContainerFocused()->setView(view);
    this->currentContainerFocused()->setFocus(Qt::MouseFocusReason);

    // towards thumbnail generation

    d->navigator->reset();
    d->navigator->addThumbnail(data->thumbnail());
}

//! Switch the view area layout to the one of patient with database index \param index.
/*! 
 * 
 */

void medViewerArea::switchToPatient(int id)
{
    if(id < 0)
        return;

    d->current_patient = id;

    // Setup view container

    medViewerAreaStack *view_stack;

    if(!d->view_stacks.contains(d->current_patient)) {
        view_stack = new medViewerAreaStack(this);
        view_stack->setPatientId(d->current_patient);
        connect(view_stack, SIGNAL(dropped(medDataIndex)), this, SLOT(open(medDataIndex)));
        connect(view_stack, SIGNAL(focused(dtkAbstractView*)), this, SLOT(onViewFocused(dtkAbstractView*)));
        d->view_stacks.insert(d->current_patient, view_stack);
        d->stack->addWidget(view_stack);
    } else {
        view_stack = d->view_stacks.value(d->current_patient);
    }

    d->stack->setCurrentWidget(view_stack);

    // Setup navigator

    d->navigator->onPatientClicked(d->current_patient);

    // Setup patient toolbox

    d->patientToolBox->setPatientIndex(id);
}

//! Set stack index.
/*! 
 *  This method actually allows one to switch between the
 *  single/multi/custom modes for the currently displayed view
 *  stack. A view stack is composed of a single/custom/multi layout.
 */

void medViewerArea::switchToContainer(int index)
{
    if(index < 0)
        return;

    d->current_container = index;

    if (d->view_stacks.count())
        if (d->view_stacks.value(d->current_patient))
	    d->view_stacks.value(d->current_patient)->setCurrentIndex(index);
}

//! Set custom view preset
/*! 
 *  Presets are defined in src/medGui/medViewContainerCustom.
 */

void medViewerArea::switchToContainerPreset(int index)
{
    if(index < 0)
        return;

    if (d->view_stacks.count()) {
        if (d->view_stacks.value(d->current_patient)) {
	    if(medViewContainerCustom *custom = dynamic_cast<medViewContainerCustom *>(d->view_stacks.value(d->current_patient)->custom())) {
                custom->setPreset(index);
            }
        }
    }
}

//! View focused callback. 
/*! 
 *  This method updates the toolboxes according to the focused view.
 */

void medViewerArea::onViewFocused(dtkAbstractView *view)
{
    d->viewToolBox->update(view);
    d->diffusionToolBox->update(view);
}

//! Returns the currently displayed stack. 
/*! 
 *  A stack is a set a view containers for a given patient.
 */

medViewerAreaStack *medViewerArea::currentStack(void)
{
    return d->view_stacks.value(d->current_patient);
}

//! Returns the currently displayed container of the currently displayed stack.
/*! 
 * 
 */

medViewContainer *medViewerArea::currentContainer(void)
{
    return d->view_stacks.value(d->current_patient)->current();
}

//! Returns the currently focused child container.
/*! 
 *  Note that container are hierarchical structures. This methods
 *  returns a container that can contain a view.
 */

medViewContainer *medViewerArea::currentContainerFocused(void)
{
    return d->view_stacks.value(d->current_patient)->current()->current();
}

// view settings

void medViewerArea::setupForegroundLookupTable(QString table)
{
    if(!d->view_stacks.count())
        return;

    if(dtkAbstractView *view =  d->view_stacks.value(d->current_patient)->current()->current()->view()) {
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
    
    if(dtkAbstractView *view =  d->view_stacks.value(d->current_patient)->current()->current()->view()) {
        view->setProperty("BackgroundLookupTable", table);
	view->update();
    }
}

void medViewerArea::setupAxisVisibility(bool visible)
{
    if(!d->view_stacks.count())
        return;
  
    if(dtkAbstractView *view = d->view_stacks.value(d->current_patient)->current()->current()->view()) {
        visible ? view->setProperty("ShowAxis", "true") : view->setProperty("ShowAxis", "false");
	view->update();
    }
}

void medViewerArea::setupScalarBarVisibility(bool visible)
{
    if(!d->view_stacks.count())
        return;
  
    if(dtkAbstractView *view =  d->view_stacks.value(d->current_patient)->current()->current()->view()) {
        visible ? view->setProperty("ScalarBarVisibility", "true") : view->setProperty("ScalarBarVisibility", "false");
	view->update();
    }
}

void medViewerArea::setupRulerVisibility(bool visible)
{
    if(!d->view_stacks.count())
        return;
  
    if(dtkAbstractView *view =  d->view_stacks.value(d->current_patient)->current()->current()->view()) {
        visible ? view->setProperty("ShowRuler", "true") : view->setProperty("ShowRuler", "false");
	view->update();
    }
}

void medViewerArea::setup3DMode(QString mode)
{
    if(!d->view_stacks.count())
        return;
  
    if(dtkAbstractView *view =  d->view_stacks.value(d->current_patient)->current()->current()->view()) {
        view->setProperty("Mode", mode);
        view->update();
    }
}

void medViewerArea::setup3DVRMode(QString mode)
{
    if(!d->view_stacks.count())
        return;
  
    if(dtkAbstractView *view =  d->view_stacks.value(d->patientToolBox->patientIndex())->current()->current()->view()) {
        view->setProperty("VRMode", mode);
        view->update();
    }
}

void medViewerArea::setupLUTPreset(QString table)
{
    if(!d->view_stacks.count())
        return;
  
    if(dtkAbstractView *view =  d->view_stacks.value(d->current_patient)->current()->current()->view()) {
        view->setProperty("Preset", table);
	view->update();
    }
}

void medViewerArea::setup3DLOD(int value)
{
    if(!d->view_stacks.count())
        return;

    if(dtkAbstractView *view =  d->view_stacks.value(d->current_patient)->current()->current()->view()) {
        view->setMetaData("LOD", QString::number(value));
	view->update();
    }
}

void medViewerArea::setupWindowing(bool checked)
{
    if(!d->view_stacks.count())
        return;

    if(dtkAbstractView *view =  d->view_stacks.value(d->current_patient)->current()->current()->view()) {
        view->setProperty("LeftClickInteraction", "Windowing");
	view->update();
    }
}

void medViewerArea::setupZooming(bool checked)
{
    if(!d->view_stacks.count())
        return;
  
    if(dtkAbstractView *view =  d->view_stacks.value(d->current_patient)->current()->current()->view()) {
        view->setProperty("LeftClickInteraction", "Zooming");
	view->update();
    }
}

void medViewerArea::setupSlicing(bool checked)
{
    if(!d->view_stacks.count())
        return;
  
    if(dtkAbstractView *view =  d->view_stacks.value(d->current_patient)->current()->current()->view()) {
        view->setProperty("LeftClickInteraction", "Slicing");
	view->update();
    }
}

void medViewerArea::setupMeasuring(bool checked)
{
    if(!d->view_stacks.count())
        return;
  
    if(dtkAbstractView *view =  d->view_stacks.value(d->current_patient)->current()->current()->view()) {
        view->setProperty("LeftClickInteraction", "Measuring");
	view->update();
    }
}

void medViewerArea::setupCropping(bool checked)
{
    if(!d->view_stacks.count())
        return;
  
    if(dtkAbstractView *view =  d->view_stacks.value(d->current_patient)->current()->current()->view()) {
        checked ? view->setProperty("Cropping", "true") : view->setProperty("Cropping", "false");
	view->update();
    }
}

void medViewerArea::setupLayoutCompare(void)
{
    if(!d->view_stacks.count())
        return;

    d->view_stacks.value(d->current_patient)->setCurrentIndex(3);
}

void medViewerArea::setupLayoutFuse(void)
{
    if(!d->view_stacks.count())
        return;

    d->view_stacks.value(d->current_patient)->setCurrentIndex(4);
	
    if (d->registrationToolBox->fuseView()) {
        d->view_stacks.value(d->current_patient)->current()->setView(d->registrationToolBox->fuseView());
        d->view_stacks.value(d->current_patient)->current()->setFocus(Qt::MouseFocusReason);
    }
}

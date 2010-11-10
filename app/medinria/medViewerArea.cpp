/* medViewerArea.cpp --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Fri Sep 18 12:43:06 2009 (+0200)
 * Version: $Id$
 * Last-Updated: Wed Nov 10 15:33:44 2010 (+0100)
 *           By: Julien Wintz
 *     Update #: 1054
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#include "medViewerArea.h"
#include "medViewerArea_p.h"
#include "medViewerAreaStack.h"
#include "medViewerConfiguration.h"
#include "medViewerConfigurator.h"
#include "medViewerToolBoxConfiguration.h"
#include "medViewerToolBoxDiffusion.h"
#include "medViewerToolBoxLayout.h"
#include "medViewerToolBoxPatient.h"
#include "medViewerToolBoxView.h"

#include <dtkCore/dtkAbstractViewFactory.h>
#include <dtkCore/dtkAbstractView.h>
#include <dtkCore/dtkAbstractDataFactory.h>
#include <dtkCore/dtkAbstractData.h>
#include <dtkCore/dtkAbstractDataReader.h>
#include <dtkCore/dtkGlobal.h>

#include <dtkVr/dtkVrHeadRecognizer.h>
#include <dtkVr/dtkVrGestureRecognizer.h>

#include <medCore/medDataIndex.h>
#include <medCore/medDataManager.h>
#include <medCore/medViewManager.h>
#include <medCore/medAbstractView.h>

#include <medSql/medDatabaseController.h>
#include <medSql/medDatabaseNonPersitentItem.h>
#include <medSql/medDatabaseNonPersitentController.h>
#include <medSql/medDatabaseNavigator.h>

#include <medGui/medClutEditor.h>
#include <medGui/medToolBox.h>
#include <medGui/medToolBoxContainer.h>
#include <medGui/medToolBoxRegistration.h>
#include <medGui/medViewContainer.h>
#include <medGui/medViewContainerCustom.h>
#include <medGui/medViewContainerMulti.h>
#include <medGui/medViewContainerSingle.h>
#include <medGui/medViewPool.h>


#include <QtGui>
#include <QtSql>

// /////////////////////////////////////////////////////////////////
// medViewerArea
// /////////////////////////////////////////////////////////////////

medViewerArea::medViewerArea(QWidget *parent) : QWidget(parent), d(new medViewerAreaPrivate)
{
    // -- Internal logic

    d->current_patient = -1;

    // -- User interface setup

    d->stack = new QStackedWidget(this);
    d->stack->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    // -- Patient toolbox --

    d->patientToolBox = new medViewerToolBoxPatient(this);

    connect(d->patientToolBox, SIGNAL(patientIndexChanged(int)), this, SLOT(switchToPatient(int)));

    // -- Configuration toolbox --

    d->configurationToolBox = new medViewerToolBoxConfiguration(this);
    d->configurationToolBox->addConfiguration("Visualization");
    d->configurationToolBox->addConfiguration("Registration");
    d->configurationToolBox->addConfiguration("Diffusion");

    connect(d->configurationToolBox, SIGNAL(configurationChanged(QString)), medViewerConfigurator::instance(), SLOT(setConfiguration(QString)));
    
    // -- Layout toolbox --

    d->layoutToolBox = new medViewerToolBoxLayout(this);

    connect(d->layoutToolBox, SIGNAL(modeChanged(int)), this, SLOT(switchToContainer(int)));
    connect(d->layoutToolBox, SIGNAL(split(int, int)), this, SLOT(split(int, int)));
    connect(d->layoutToolBox, SIGNAL(presetClicked(int)), this, SLOT(switchToContainerPreset(int)));

    // -- View toolbox --
    
    d->viewToolBox = new medViewerToolBoxView(this);

    connect(d->viewToolBox, SIGNAL(foregroundLookupTableChanged(QString)), this, SLOT(setupForegroundLookupTable(QString)));
    // connect(d->viewToolBox, SIGNAL(backgroundLookupTableChanged(QString)), this, SLOT(setupBackgroundLookupTable(QString)));
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
    connect(d->viewToolBox, SIGNAL(annotationsVisibilityChanged(bool)), this, SLOT(setupAnnotationsVisibility(bool)));

    // -- Diffusion toolbox --

    d->diffusionToolBox = new medViewerToolBoxDiffusion(this);
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
    d->toolbox_container->addToolBox(d->configurationToolBox);
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
    layout->addWidget(navigator_container);
    layout->addWidget(view_container);
    layout->addWidget(d->toolbox_container);

    // Setting up database

    this->setup();

    connect(medDatabaseController::instance(), SIGNAL(updated()), this, SLOT(setup()));
    connect(medDatabaseNonPersitentController::instance(), SIGNAL(updated()), this, SLOT(setup()));

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

    // action for transfer function
    QAction * transFunAction = new QAction("Toggle Tranfer Function Widget", this);
    transFunAction->setShortcut(Qt::ControlModifier + Qt::ShiftModifier + Qt::Key_L);
    transFunAction->setCheckable(true);
    transFunAction->setChecked(false);
    connect(transFunAction, SIGNAL(toggled(bool)), this, SLOT(bringUpTransFun(bool)));

    this->addAction(transFunAction);
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

    // Setting up persitent data

    QSqlQuery query(*(medDatabaseController::instance()->database()));

    query.prepare("SELECT name, id FROM patient");
    if(!query.exec())
        qDebug() << DTK_COLOR_FG_RED << query.lastError() << DTK_NO_COLOR;

    while(query.next())
        d->patientToolBox->addItem(query.value(0).toString(), query.value(1));

    // Setting up non persitent data

    QList<QString> patientList;
    foreach(medDatabaseNonPersitentItem *item, medDatabaseNonPersitentController::instance()->items()) {
        if (!patientList.contains (item->name())) {
	    d->patientToolBox->addItem(item->name(), item->index().patientId());
	    patientList.append (item->name());
        }
    }
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

    if(((medDataIndex)index).isValidForSeries()) {

    dtkAbstractData *data = NULL;
    dtkAbstractView *view = NULL;

    if(!data)
        data = medDataManager::instance()->data(index);

    if(!data)
        data = medDatabaseNonPersitentController::instance()->data(index);

    if(!data)
        data = medDatabaseController::instance()->read(index);

    if(!data)
        return;

    medDataManager::instance()->insert(index, data);

    if(!view)
        view = d->view_stacks.value(d->current_patient)->current()->current()->view();

    if(!view)
        view = dtkAbstractViewFactory::instance()->create("v3dView");
    
    if(!view) {
        qDebug() << "Unable to create a v3dView";
        return;
    }

    medViewManager::instance()->insert(index, view);
    
    view->setData(data);
    view->reset(); // called in view_stacks -> setView but seems necessary with the streaming approach

    QMutexLocker ( &d->mutex );
    d->view_stacks.value(d->current_patient)->current()->current()->setView(view);
    d->view_stacks.value(d->current_patient)->current()->current()->setFocus(Qt::MouseFocusReason);
	
	
    return;

    }

    if(((medDataIndex)index).isValidForPatient()) {

    this->switchToPatient(index.patientId());
    this->switchToContainer(1);

    QSqlQuery stQuery(*(medDatabaseController::instance()->database()));
    stQuery.prepare("SELECT * FROM study WHERE patient = :id");
    stQuery.bindValue(":id", index.patientId());
    if(!stQuery.exec())
	qDebug() << DTK_COLOR_FG_RED << stQuery.lastError() << DTK_NO_COLOR;

    while(stQuery.next()) {
      
        QSqlQuery seQuery(*(medDatabaseController::instance()->database()));
        seQuery.prepare("SELECT * FROM series WHERE study = :id");
        seQuery.bindValue(":id", stQuery.value(0));
        if(!seQuery.exec())
	    qDebug() << DTK_COLOR_FG_RED << seQuery.lastError() << DTK_NO_COLOR;
        
        while(seQuery.next())
	    this->open(medDataIndex(index.patientId(), stQuery.value(0).toInt(), seQuery.value(0).toInt()));
    }

    }
}

//! Open file on the local filesystem.
/*! 
 * 
 */

void medViewerArea::open(const QString& file)
{
    this->open(medDatabaseNonPersitentController::instance()->read(file));
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

    // Setup layout toolbox

    
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

#include <dtkVr/dtkVrController.h>

//! View focused callback. 
/*! 
 *  This method updates the toolboxes according to the focused view.
 */

void medViewerArea::onViewFocused(dtkAbstractView *view)
{
    // set head recognizer

    static dtkVrHeadRecognizer *head_recognizer = NULL;

    if(dtkApplicationArgumentsContain(qApp, "--tracker")) {

        if(!head_recognizer) {
            head_recognizer = new dtkVrHeadRecognizer;
            head_recognizer->startConnection(QUrl(dtkApplicationArgumentsValue(qApp, "--tracker")));
        }

        if(view->property("Orientation") == "3D")
            head_recognizer->setView(view);
        else
            head_recognizer->setView(NULL);
    }

    // set gesture recognizer

    static dtkVrGestureRecognizer *gesture_recognizer = NULL;

    if(dtkApplicationArgumentsContain(qApp, "--tracker")) {

        if(!gesture_recognizer) {
            gesture_recognizer = new dtkVrGestureRecognizer;
            gesture_recognizer->startConnection(QUrl(dtkApplicationArgumentsValue(qApp, "--tracker")));
        }

        gesture_recognizer->setView(view);
        gesture_recognizer->setReceiver(static_cast<medAbstractView *>(view)->receiverWidget());
    }

    // Update toolboxes

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

    if(medViewPool *pool = d->view_stacks.value(d->current_patient)->current()->pool()) {
        pool->setViewProperty("LookupTable", table);
    }
}
/*
void medViewerArea::setupBackgroundLookupTable(QString table)
{
    if(!d->view_stacks.count())
        return;
    
    if(medViewPool *pool = d->view_stacks.value(d->current_patient)->current()->pool()) {
        pool->setViewProperty("BackgroundLookupTable", table);
    }
}
*/
void medViewerArea::setupAxisVisibility(bool visible)
{
    if(!d->view_stacks.count())
        return;

    if(medViewPool *pool = d->view_stacks.value(d->current_patient)->current()->pool()) {
        visible ? pool->setViewProperty("ShowAxis", "true") : pool->setViewProperty("ShowAxis", "false");
    }
}

void medViewerArea::setupScalarBarVisibility(bool visible)
{
    if(!d->view_stacks.count())
        return;
  
    if(medViewPool *pool = d->view_stacks.value(d->current_patient)->current()->pool()) {
        visible ? pool->setViewProperty("ShowScalarBar", "true") : pool->setViewProperty("ShowScalarBar", "false");
    }
}

void medViewerArea::setupRulerVisibility(bool visible)
{
    if(!d->view_stacks.count())
        return;
  
    if(medViewPool *pool = d->view_stacks.value(d->current_patient)->current()->pool()) {
        visible ? pool->setViewProperty("ShowRuler", "true") : pool->setViewProperty("ShowRuler", "false");
    }
}

void medViewerArea::setupAnnotationsVisibility(bool visible)
{
    if(!d->view_stacks.count())
        return;
  
    if(medViewPool *pool = d->view_stacks.value(d->current_patient)->current()->pool()) {
        visible ? pool->setViewProperty("ShowAnnotations", "true") : pool->setViewProperty("ShowAnnotations", "false");
    }
}

void medViewerArea::setup3DMode(QString mode)
{
    if(!d->view_stacks.count())
        return;
  
    if(medViewPool *pool = d->view_stacks.value(d->current_patient)->current()->pool()) {
        pool->setViewProperty("3DMode", mode);
    }
}

void medViewerArea::setup3DVRMode(QString mode)
{
    if(!d->view_stacks.count())
        return;
  
    if(medViewPool *pool = d->view_stacks.value(d->current_patient)->current()->pool()) {
        pool->setViewProperty("Renderer", mode);
    }
}

void medViewerArea::setupLUTPreset(QString table)
{
    if(!d->view_stacks.count())
        return;
  
    if(medViewPool *pool = d->view_stacks.value(d->current_patient)->current()->pool()) {
        pool->setViewProperty("Preset", table);
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

    if(medViewPool *pool = d->view_stacks.value(d->current_patient)->current()->pool()) {
        pool->setViewProperty("MouseInteraction", "Windowing");
    }
}

void medViewerArea::setupZooming(bool checked)
{
    if(!d->view_stacks.count())
        return;
  
    if(medViewPool *pool = d->view_stacks.value(d->current_patient)->current()->pool()) {
        pool->setViewProperty("MouseInteraction", "Zooming");
    }
}

void medViewerArea::setupSlicing(bool checked)
{
    if(!d->view_stacks.count())
        return;
  
    if(medViewPool *pool = d->view_stacks.value(d->current_patient)->current()->pool()) {
        pool->setViewProperty("MouseInteraction", "Slicing");
    }
}

void medViewerArea::setupMeasuring(bool checked)
{
    if(!d->view_stacks.count())
        return;
  
    if(medViewPool *pool = d->view_stacks.value(d->current_patient)->current()->pool()) {
        pool->setViewProperty("MouseInteraction", "Measuring");
    }
}

void medViewerArea::setupCropping(bool checked)
{
    if(!d->view_stacks.count())
        return;
  
    if(medViewPool *pool = d->view_stacks.value(d->current_patient)->current()->pool()) {
        checked ? pool->setViewProperty("Cropping", "true") : pool->setViewProperty("Cropping", "false");
    }
}

void medViewerArea::bringUpTransFun(bool checked)
{
    if (!checked)
    {
        if (d->transFun !=NULL )
        {
            delete d->transFun ;
            d->transFun=NULL;
        }
      return;
    }
    if(!d->view_stacks.count())
        return;
  
    if(dtkAbstractView *view = d->view_stacks.value(d->current_patient)->current()->current()->view()) {

      d->transFun = new medClutEditor(NULL);
      d->transFun->setWindowModality( Qt::WindowModal );
      d->transFun->setWindowFlags(Qt::Tool|Qt::WindowStaysOnTopHint);

      d->transFun->setData(static_cast<dtkAbstractData *>(view->data()));
      d->transFun->setView(dynamic_cast<medAbstractView*>(view));

      d->transFun->show();
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

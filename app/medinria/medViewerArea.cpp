/* medViewerArea.cpp --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Fri Sep 18 12:43:06 2009 (+0200)
 * Version: $Id$
 * Last-Updated: Wed Nov 10 16:15:44 2010 (+0100)
 *           By: Julien Wintz
 *     Update #: 1061
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#include "medViewerArea.h"
#include "medViewerArea_p.h"
// #include "medViewerConfigurator.h"
#include "medViewerToolBoxConfiguration.h"

#include "medGui/medViewContainerStack.h"
#include "medGui/medViewerToolBoxLayout.h"
#include "medViewerToolBoxPatient.h"
#include "medGui/medViewerToolBoxView.h"

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
#include <medSql/medDatabaseNavigatorController.h>

#include <medGui/medClutEditor.h>
#include <medGui/medToolBox.h>
#include <medGui/medToolBoxContainer.h>
#include <medGui/medToolBoxRegistration.h>
#include <medGui/medToolBoxDiffusion.h>
#include <medGui/medViewContainer.h>
#include <medGui/medViewContainerCustom.h>
#include <medGui/medViewContainerMulti.h>
#include <medGui/medViewContainerSingle.h>
#include <medGui/medViewPool.h>
#include <medGui/medViewerConfiguration.h>
#include <medGui/medViewerConfigurationFactory.h>

#include <QtGui>
#include <QtSql>

// /////////////////////////////////////////////////////////////////
// medViewerArea
// /////////////////////////////////////////////////////////////////

medViewerArea::medViewerArea(QWidget *parent) : QWidget(parent), d(new medViewerAreaPrivate)
{
    d->layout = 0;

    // -- Internal logic

    d->current_patient = -1;
    d->configurations = new QHash<QString,medViewerConfiguration *>();
    d->current_configuration_name = "";
    d->current_configuration = 0;

    // -- User interface setup

    d->stack = new QStackedWidget(this);
    d->stack->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);


    // Setting up toolbox container

    d->toolbox_container = new medToolBoxContainer(this);

    // Setting up view container

    d->view_container = new QWidget(this);

    QVBoxLayout *view_container_layout = new QVBoxLayout(d->view_container);
    view_container_layout->setContentsMargins(0, 10, 0, 10);
    view_container_layout->addWidget(d->stack);

    // Setting up navigator container

    d->navigator_container = new QFrame(this);
    d->navigator_container->setObjectName("medNavigatorContainer");
    
    d->navigator = 0;//new medDatabaseNavigator(d->navigator_container);

    d->navigator_container_layout = 0;

    //action for transfer function
    QAction * transFunAction =
      new QAction("Toggle Tranfer Function Widget", this);
    transFunAction->setShortcut(Qt::ControlModifier + Qt::ShiftModifier +
				Qt::Key_L);
    transFunAction->setCheckable( true );
    transFunAction->setChecked( false );
    connect(transFunAction, SIGNAL(toggled(bool)),
	    this, SLOT(bringUpTransferFunction(bool)));

    this->addAction(transFunAction);
    
    d->toolboxPatient = new medViewerToolBoxPatient(this);
    d->toolboxPatient->setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed);//, QSizePolicy::Minimum);
    d->toolboxPatient->setFixedWidth(186);
    
    connect (d->toolboxPatient, SIGNAL(patientIndexChanged(int)), this, SLOT(switchToPatient(int)));
}

medViewerArea::~medViewerArea(void)
{
    //TODO: delete
    delete d->configurations;
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

    medViewContainerStack *view_stack;

    if(!d->view_stacks.contains(d->current_patient)) {
        view_stack = new medViewContainerStack(this);
        connect(view_stack, SIGNAL(dropped(medDataIndex)), this, SLOT(open(medDataIndex)));
        connect(view_stack, SIGNAL(focused(dtkAbstractView*)), this, SLOT(onViewFocused(dtkAbstractView*)));
        d->view_stacks.insert(d->current_patient, view_stack);
        d->stack->addWidget(view_stack);
    }
    else {
        view_stack = d->view_stacks.value(d->current_patient);
    }

    if (d->current_configuration) {
        d->current_configuration->setupViewContainerStack( view_stack );
        switchToContainer (d->current_configuration->viewLayoutType());
        switchToContainerPreset (d->current_configuration->customLayoutType());
    }
    
    d->stack->setCurrentWidget(view_stack);

    // Setup navigator

    if (d->navigator)
        d->navigator->onPatientClicked(d->current_patient);

    // Setup patient toolbox
    //TODO emit a signal to the Patient Toolbox
    //emit (setPatientIndex(id));

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
    
    if (d->current_configuration)
        d->current_configuration->setViewLayoutType(index);
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
    
    if (d->current_configuration)
        d->current_configuration->setCustomLayoutType(index);
}

void medViewerArea::addToolBox(medToolBox *toolbox)
{
    d->toolbox_container->addToolBox(toolbox);
}

void medViewerArea::removeToolBox(medToolBox *toolbox)
{
    d->toolbox_container->removeToolBox(toolbox);
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
    QList<medToolBox *> toolboxes = d->toolbox_container->toolBoxes();
    foreach( medToolBox *tb, toolboxes)
        tb->update(view);
    
    connect (view, SIGNAL(lutChanged()), this, SLOT(updateTransferFunction()));

    this->updateTransferFunction();
}

//! Returns the currently displayed stack. 
/*! 
 *  A stack is a set a view containers for a given patient.
 */

medViewContainerStack *medViewerArea::currentStack(void)
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
/*
void medViewerArea::setupForegroundLookupTable(QString table)
{
    if(!d->view_stacks.count())
        return;

    if ( medViewPool *pool = this->currentContainer()->pool() )
        pool->setViewProperty("LookupTable", table);

    this->updateTransferFunction();
}
void medViewerArea::setupLUTPreset(QString table)
{
    if(!d->view_stacks.count())
        return;
  
    if ( medViewPool *pool = this->currentContainer()->pool() ) {
        pool->setViewProperty("Preset", table);
    }

    this->updateTransferFunction();
}
*/

void medViewerArea::bringUpTransferFunction(bool checked)
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
  
    if ( dtkAbstractView *view = this->currentContainerFocused()->view() ) {

      d->transFun = new medClutEditor(NULL);
      d->transFun->setWindowModality( Qt::WindowModal );
      d->transFun->setWindowFlags(Qt::Tool|Qt::WindowStaysOnTopHint);

      // d->transFun->setData(static_cast<dtkAbstractData *>(view->data()));
      d->transFun->setView(dynamic_cast<medAbstractView*>(view));

      d->transFun->show();
    }
}

void medViewerArea::updateTransferFunction()
{
    dtkAbstractView * view = this->currentContainerFocused()->view();
    if ( d->transFun && view ) {
	// d->transFun->setData( static_cast<dtkAbstractData *>( view->data() ) );
	d->transFun->setView( dynamic_cast<medAbstractView *>( view ), true );
	d->transFun->update();
    }
}
//TODO: don't know why it's been removed from .h file...
//void medViewerArea::setupLayoutCompare(void)
//{
//    if(!d->view_stacks.count())
//        return;

//    d->view_stacks.value(d->current_patient)->setCurrentIndex(3);
//}

//TODO: move this to the configuration
//void medViewerArea::setupLayoutFuse(void)
//{
//    if(!d->view_stacks.count())
//        return;

//    d->view_stacks.value(d->current_patient)->setCurrentIndex(4);
	
//    if (d->registrationToolBox->fuseView()) {
//        this->currentContainer()->setView(d->registrationToolBox->fuseView());
//        this->currentContainer()->setFocus(Qt::MouseFocusReason);
//    }
//}


void medViewerArea::setupConfiguration(QString name)
{
    if (d->current_configuration_name == name)
        return;
    
    medViewerConfiguration *conf = NULL;
    
    if (d->configurations->contains(name))
        conf = d->configurations->operator[](name);
    else {
        if (conf = medViewerConfigurationFactory::instance()->createConfiguration(name))
            d->configurations->insert(name, conf);
        else
            qDebug()<< "Configuration" << name << "couldn't be created";
    }

    if (!conf)
        return;
    
    d->current_configuration = conf;

    //clean toolboxes
    d->toolbox_container->clear();

    if (d->layout)
        delete d->layout;
    
    if (d->navigator_container_layout)
        delete d->navigator_container_layout;
    
    //setup orientation
    switch (conf->layoutType()){
        case medViewerConfiguration::TopDbBottomTb:
        case medViewerConfiguration::TopTbBottomDb:
           {
              d->toolbox_container->setOrientation(medToolBoxContainer::Horizontal);
              d->toolbox_container->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
              d->toolbox_container->setFixedHeight(200);
              medDatabaseNavigatorController::instance()->setOrientation( Qt::Horizontal );
              d->navigator_container_layout = new QHBoxLayout(d->navigator_container);
              d->navigator_container_layout->setAlignment(Qt::AlignVCenter|Qt::AlignLeft);
              d->navigator_container->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
              d->navigator_container->setFixedHeight(186);
              d->layout = new QVBoxLayout(this);
              d->layout->setContentsMargins(0, 0, 0, 0);
              d->layout->setSpacing(0);
           }
            break;
            
        case medViewerConfiguration::LeftDbRightTb:
        case medViewerConfiguration::LeftTbRightDb:
        default:
           {
              d->toolbox_container->setOrientation(medToolBoxContainer::Vertical);
              d->toolbox_container->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
              d->toolbox_container->setFixedWidth(320);
              medDatabaseNavigatorController::instance()->setOrientation( Qt::Vertical );
              d->navigator_container_layout = new QVBoxLayout(d->navigator_container);
              d->navigator_container_layout->setAlignment(Qt::AlignHCenter|Qt::AlignTop);    
              d->navigator_container->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
              d->navigator_container->setFixedWidth(186);
              d->layout = new QHBoxLayout(this);
              d->layout->setContentsMargins(0, 0, 0, 0);
              d->layout->setSpacing(0);
           }
    }

    //setup database visibility
    d->navigator_container->setVisible( conf->isDatabaseVisible());

    // Setting up navigator
    
    if (d->navigator)
        delete d->navigator;
    d->navigator = new medDatabaseNavigator(d->navigator_container);
    d->navigator->onPatientClicked(d->current_patient);
    
    d->navigator_container_layout->setContentsMargins(0, 0, 0, 0);
    d->navigator_container_layout->setSpacing(0);
    d->navigator_container_layout->addWidget(d->toolboxPatient);
    d->navigator_container_layout->addWidget(d->navigator);

    // setup layout type
    if (d->view_stacks.contains(d->current_patient)) {
        conf->setupViewContainerStack( d->view_stacks[d->current_patient] );
    }
    switchToContainer(conf->viewLayoutType());

    if (conf->viewLayoutType() == medViewContainer::Custom) {
        switchToContainerPreset(conf->customLayoutType());
    }
    
    // add toolboxes
    foreach (medToolBox * toolbox, conf->toolBoxes() ) {
        this->addToolBox(toolbox);
        toolbox->show();
    }
    
    //setup layout Toolbox Visibility
    conf->isLayoutToolBoxVisible()?conf->showLayoutToolBox():conf->hideLayoutToolBox();

    d->layout->addWidget(d->navigator_container);
    d->layout->addWidget(d->view_container);
    d->layout->addWidget(d->toolbox_container);
    
    connect(conf, SIGNAL(layoutModeChanged(int)),   this, SLOT(switchToContainer(int)));
    connect(conf, SIGNAL(layoutSplit(int,int)),     this, SLOT(split(int,int)));
    connect(conf, SIGNAL(layoutPresetClicked(int)), this, SLOT(switchToContainerPreset(int)));
    connect(conf,SIGNAL(toolboxAdded(medToolBox*)),this,SLOT(addToolBox(medToolBox*)));
    connect(conf,SIGNAL(toolboxRemoved(medToolBox*)),this,SLOT(removeToolBox(medToolBox*)));

    d->current_configuration_name = name;
    
    this->updateGeometry();
}

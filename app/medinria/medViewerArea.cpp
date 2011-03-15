/* medViewerArea.cpp --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Fri Sep 18 12:43:06 2009 (+0200)
 * Version: $Id$
 * Last-Updated: Mon Dec 20 17:24:07 2010 (+0100)
 *           By: Julien Wintz
 *     Update #: 1062
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#include "medViewerArea.h"
#include "medViewerArea_p.h"

#include <dtkCore/dtkAbstractViewFactory.h>
#include <dtkCore/dtkAbstractView.h>
#include <dtkCore/dtkAbstractDataFactory.h>
#include <dtkCore/dtkAbstractData.h>
#include <dtkCore/dtkAbstractDataReader.h>
#include <dtkCore/dtkGlobal.h>

#include <dtkVr/dtkVrHeadRecognizer.h>
#include <dtkVr/dtkVrGestureRecognizer.h>

#include <medCore/medSettingsManager.h>
#include <medCore/medDataIndex.h>
#include <medCore/medDataManager.h>
#include <medCore/medViewManager.h>
#include <medCore/medAbstractView.h>

#include <medSql/medDatabaseController.h>
#include <medSql/medDatabaseNonPersistentItem.h>
#include <medSql/medDatabaseNonPersistentController.h>
#include <medSql/medDatabaseNavigator.h>
#include <medSql/medDatabaseNavigatorController.h>

#include <medGui/medClutEditor.h>
#include <medGui/medToolBox.h>
#include <medGui/medToolBoxContainer.h>
#include <medGui/medViewContainer.h>
#include <medGui/medViewContainerCustom.h>
#include <medGui/medViewContainerMulti.h>
#include <medGui/medViewContainerSingle.h>
#include <medGui/medViewPool.h>
#include <medGui/medViewerConfigurationFactory.h>
#include <medGui/medToolBoxDiffusion.h>
#include <medGui/medToolBoxRegistration.h>
#include <medGui/medViewContainerStack.h>
#include <medGui/medViewerToolBoxLayout.h>
#include <medViewerToolBoxPatient.h>
#include <medGui/medViewerToolBoxView.h>
#include <medGui/medViewerToolBoxViewProperties.h>


#include <QtGui>
#include <QtSql>
#include <QPropertyAnimation>
#include <QEasingCurve>

// /////////////////////////////////////////////////////////////////
// medViewerArea
// /////////////////////////////////////////////////////////////////

medViewerArea::medViewerArea(QWidget *parent) : QWidget(parent), d(new medViewerAreaPrivate)
{
    // -- Internal logic
    d->current_patient = -1;
    d->current_configuration_name = "";
    d->current_configuration = 0;
    d->current_layout = medViewerConfiguration::LeftDbRightTb;

    d->splitter = new QSplitter(this);
    d->splitter->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
    d->splitter->setHandleWidth(2);
    // -- User interface setup

    d->stack = new QStackedWidget(this);
    d->stack->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    
    d->toolboxPatient = new medViewerToolBoxPatient(this);
    d->toolboxPatient->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);//, QSizePolicy::Minimum);
    d->toolboxPatient->setFixedWidth(176); // 186 - 10

    
    // Setting up toolbox container

    d->toolbox_container = new medToolBoxContainer(this);
    d->toolbox_container->setOrientation(Qt::Vertical);
    d->toolbox_container->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Expanding);
    d->toolbox_container->setMinimumWidth(320);

    // Setting up view container

    d->view_container = new QWidget(this);

    QVBoxLayout *view_container_layout = new QVBoxLayout(d->view_container);
    view_container_layout->setContentsMargins(0, 10, 0, 10);
    view_container_layout->addWidget(d->stack);


    // Setting up navigator container

    d->navigator_container = new QFrame(this);
    d->navigator_container->setObjectName("medNavigatorContainer");
    d->navigator_container->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
    d->navigator_container->setFixedWidth(186);

    // Setting up navigator
    
    medDatabaseNavigatorController::instance()->setOrientation( Qt::Vertical );
    d->navigator = new medDatabaseNavigator(d->navigator_container);

    d->navigator_animation = new QPropertyAnimation (d->navigator, "geometry");
    d->navigator_animation->setDuration (500);
    d->navigator_animation->setEasingCurve (QEasingCurve::OutQuad);

    // d->navigator_container_layout = 0;
    d->navigator_container_layout = new QGridLayout(d->navigator_container);
    d->navigator_container_layout->setAlignment(Qt::AlignHCenter|Qt::AlignTop);
    d->navigator_container_layout->setContentsMargins(0, 0, 0, 0);
    d->navigator_container_layout->setSpacing(0);
    d->navigator_container_layout->addWidget(d->toolboxPatient, 0, 0);
    d->navigator_container_layout->addWidget(d->navigator, 1, 0);

    //Set up viewer layout
    QHBoxLayout *layout = new QHBoxLayout;
    layout->addWidget(d->splitter);
    setLayout(layout);
    d->splitter->addWidget(d->navigator_container);
    d->splitter->addWidget(d->view_container);
    d->splitter->addWidget(d->toolbox_container);

    //restore previous splitter position.
    d->restoreSplitterSize(Qt::Horizontal);

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
    
    connect (d->toolboxPatient, SIGNAL(patientIndexChanged(int)), this, SLOT(switchToPatient(int)));
}

medViewerArea::~medViewerArea(void)
{

    d->saveSplitterSize(d->current_configuration->layoutType());

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
    if (d->current_configuration && 
        d->current_configuration->currentViewContainer())
        d->current_configuration->currentViewContainer()->split(rows, cols);
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
        
        // the data-manager should be used to read data
        data = medDataManager::instance()->data(index);
        if ( !data )
            return;
        
        if(!view) 
        {
            if (d->current_configuration->currentViewContainer() &&
                d->current_configuration->currentViewContainer()->current())
                view = d->current_configuration->currentViewContainer()->current()->view();
        }
        
        if(!view)
        {
            view = dtkAbstractViewFactory::instance()->create("v3dView");
            connect (view, SIGNAL(closed()), this, SLOT(onViewClosed()));
        }
        
        if(!view)
        {
            qDebug() << "Unable to create a v3dView";
            return;
        }
        
        medViewManager::instance()->insert(index, view);
        
        
        this->onViewFocused(view);
        view->setData(data);
        
        QMutexLocker ( &d->mutex );
        if (d->current_configuration->currentViewContainer()) 
        {
            d->current_configuration->currentViewContainer()->setUpdatesEnabled (false);
            d->current_configuration->currentViewContainer()->setDisabled (true);
            
            if (d->current_configuration->currentViewContainer()->current()) {
                d->current_configuration->currentViewContainer()->current()->setView(view);
                d->current_configuration->currentViewContainer()->current()->setFocus(Qt::MouseFocusReason);
            }
            
            view->reset();
            view->update();
            
            d->current_configuration->currentViewContainer()->setDisabled (false);
            d->current_configuration->currentViewContainer()->setUpdatesEnabled (true);
        }
        
        return;
    }
    
    if(((medDataIndex)index).isValidForPatient()) {
        
        this->setupConfiguration("Visualization");
        //TODO: check next 2 lines.
        //this->switchToPatient(index.patientId());
        this->switchToContainer("Multi");
        
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
    this->open(medDatabaseNonPersistentController::instance()->import(file));
}

void medViewerArea::onViewClosed(void)
{
    if (dtkAbstractView *view = dynamic_cast<dtkAbstractView*> (this->sender())) {        
        QList<medToolBox *> toolboxes = d->toolbox_container->toolBoxes();
        foreach( medToolBox *tb, toolboxes)
            tb->update(NULL);
        
        medDataIndex index = medViewManager::instance()->index( view );
        medViewManager::instance()->remove(index, view); // deletes the view
    }
}

//! Switch the view area layout to the one of patient with database index \param index.
/*! 
 * 
 */

void medViewerArea::switchToPatient(int id)
{
    qDebug()<<"SWITCHING";
    if(id < 0 || d->current_patient==id)
        return;

    d->current_patient = id;

    // Setup navigator

    if (d->navigator) {
        d->navigator->onPatientClicked(d->current_patient);
        
        QRect endGeometry = d->navigator->geometry();
        QRect startGeometry = endGeometry;
        if (d->navigator->orientation()==Qt::Vertical)
            startGeometry.setY (endGeometry.y()+1000);
        else
            startGeometry.setX (endGeometry.x()+1000);
        
        d->navigator_animation->setStartValue(startGeometry);
        d->navigator_animation->setEndValue(endGeometry);
        d->navigator_animation->start();
    }

    // Setup patient toolbox
    //TODO emit a signal to the Patient Toolbox
    d->toolboxPatient->setPatientIndex (id);

    // Setup layout toolbox
}

void medViewerArea::switchToStackedViewContainers(medViewContainerStack* stack)
{
    if(!stack )
    {
        qDebug() << "No stack to switch to";
        return;
    }
       
    if (-1 == d->stack->indexOf(stack))
    {   
        //TODO: Maybe unconnect previous ones??
        connect(stack, SIGNAL(dropped(medDataIndex)), this, SLOT(open(medDataIndex)));
        connect(stack, SIGNAL(focused(dtkAbstractView*)),
                this,  SLOT(onViewFocused(dtkAbstractView*)));
        qDebug() << "add stack of Containers to the stackWidget";
        d->stack->addWidget(stack);
    }
    else
    {
        qDebug() << "No switch needed";
    }
    d->stack->setCurrentWidget(stack);
}

//! Set stack index.
/*! 
 *  This method actually allows one to switch between the
 *  single/multi/custom modes for the currently displayed view
 *  stack. A view stack is composed of a single/custom/multi layout.
 */

void medViewerArea::switchToContainer(const QString& name)
{
//    if (d->current_patient_container.contains (d->current_patient))
//        if (d->current_patient_container[d->current_patient]==index)
//        return;

    
    
//    d->current_patient_container[d->current_patient] = index;
    
//    if(index < 0)
//        return;

//    if (d->view_stacks.count())
//      if (d->view_stacks.value(d->current_patient)) {
//          d->view_stacks.value(d->current_patient)->setCurrentIndex(index);
//        //this->currentContainer()->setFocus(Qt::MouseFocusReason);
//     }
    
    if (d->current_configuration)
    {
        if (d->current_configuration->currentViewContainer() &&
            d->current_configuration->currentViewContainer()== 
            d->current_configuration->stackedViewContainers()->container(name))
            return;
        qDebug() << "switching from" << 
                d->current_configuration->currentViewContainerName() << 
                "to configuration" << name;
        
        d->current_configuration->setCurrentViewContainer(name);
        d->current_configuration->currentViewContainer()->setFocus(Qt::MouseFocusReason);
    }
    else
    {
        qDebug() << "no currentConfiguration";
    }
}

//! Set custom view preset
/*! 
 *  Presets are defined in src/medGui/medViewContainerCustom.
 */

void medViewerArea::switchToContainerPreset(int index)
{
    if(index < 0)
        return;

    if (d->current_configuration && 
        d->current_configuration->currentViewContainer())
    {
        if(medViewContainerCustom *custom = dynamic_cast<medViewContainerCustom *>(
                d->current_configuration->currentViewContainer())) {
                custom->setPreset(index);
                d->current_configuration->setCustomLayoutType(index);
            }
    }    
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

////! Returns the currently displayed container of the currently displayed stack.
///*! 
// * 
// */

//medViewContainer *medViewerArea::currentContainer(void)
//{
//    return d->view_stacks.value(d->current_patient)->current();
//}

//! Returns the currently focused child container.
/*! 
 *  Note that container are hierarchical structures. This methods
 *  returns a container that can contain a view.
 */

medViewContainer *medViewerArea::currentContainerFocused(void)
{
    return d->current_configuration->currentViewContainer()->current();
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
    if(!d->current_configuration->currentViewContainer())
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


void medViewerArea::setupConfiguration(QString name)
{
    qDebug() << "setupConfiguration to :" << name;
    if (d->current_configuration_name == name)
        return;
    
    medViewerConfiguration *conf = NULL;
    
    if (d->configurations.contains(name))
        conf = d->configurations[name];
    else {
        if (conf = medViewerConfigurationFactory::instance()->createConfiguration(name)) {
            connect(d->toolboxPatient, SIGNAL(patientIndexChanged(int)), conf, SLOT(patientChanged(int)));
            d->configurations.insert(name, conf);
        }
        else
            qDebug()<< "Configuration" << name << "couldn't be created";
    }

    if (!conf)
        return;

    d->current_configuration = conf;
    d->current_configuration_name = name;

    //clean toolboxes
    d->toolbox_container->hide();
    d->toolbox_container->clear();
    
    //setup layout
    switchToLayout (conf->layoutType());
    
    // setup layout type
    conf->setupViewContainerStack();

    switchToStackedViewContainers(conf->stackedViewContainers());

    if (dynamic_cast<medViewContainerCustom *>(conf->currentViewContainer())) {
        switchToContainerPreset(conf->customLayoutType());
    }

    //setup database visibility
    d->navigator_container->setVisible( conf->isDatabaseVisible() );

    // add toolboxes
    
    foreach (medToolBox * toolbox, conf->toolBoxes() ) {
        this->addToolBox(toolbox);
        toolbox->show();
    }
    
    //setup layout Toolbox Visibility
    conf->isLayoutToolBoxVisible()?conf->showLayoutToolBox():conf->hideLayoutToolBox();
    
    d->toolbox_container->setVisible( conf->areToolBoxesVisible() );

    /*
      if (d->toolbox_container->toolBoxes().count()) {
      QPropertyAnimation *animation = new QPropertyAnimation(d->toolbox_container, "geometry");
      animation->setDuration(500);
      if (d->toolbox_container->orientation()==medToolBoxContainer::Vertical)  {
      animation->setStartValue(QRect(d->toolbox_container->x(), 1000, d->toolbox_container->width(), d->toolbox_container->height()));
      animation->setEndValue(QRect(d->toolbox_container->x(), 0, d->toolbox_container->width(), d->toolbox_container->height()));
      }
      else {
      animation->setStartValue(QRect(1000, d->toolbox_container->y(), d->toolbox_container->width(), d->toolbox_container->height()));
      animation->setEndValue(QRect(0, d->toolbox_container->y(), d->toolbox_container->width(), d->toolbox_container->height()));
      }
      animation->setEasingCurve(QEasingCurve::OutQuad);
      animation->start();
      }*/
    
    connect(conf, SIGNAL(layoutModeChanged(const QString&)),
            this, SLOT(switchToContainer(const QString&)));
    connect(conf, SIGNAL(layoutSplit(int,int)),       this, SLOT(split(int,int)));
    connect(conf, SIGNAL(layoutPresetClicked(int)),   this, SLOT(switchToContainerPreset(int)));
    connect(conf, SIGNAL(toolboxAdded(medToolBox*)),  this, SLOT(addToolBox(medToolBox*)));
    connect(conf, SIGNAL(toolboxRemoved(medToolBox*)),this, SLOT(removeToolBox(medToolBox*)));
}

void medViewerArea::switchToLayout (medViewerConfiguration::LayoutType layout)
{
    if (d->current_layout==layout)
        return;


    //save previous splitter size
    d->saveSplitterSize(d->current_layout);

    d->current_layout = layout;

    //setup orientation
    switch (layout){
        case medViewerConfiguration::TopDbBottomTb:
        case medViewerConfiguration::TopTbBottomDb:
           {

         d->navigator_container_layout->removeWidget ( d->toolboxPatient );
         d->navigator_container_layout->removeWidget ( d->navigator );

         d->navigator->setOrientation (Qt::Horizontal);
         
         d->navigator_container_layout->addWidget (d->toolboxPatient, 0, 0);
         d->navigator_container_layout->addWidget (d->navigator, 0, 1);
             //width must be fixed or the navigator doesn't grow
             //back when changing orientation again
             d->navigator_container->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
             d->navigator_container->setFixedHeight(186);
             d->navigator_container->setFixedWidth(QWIDGETSIZE_MAX);
         
         d->toolbox_container->setOrientation(Qt::Horizontal);
             d->toolbox_container->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
             d->toolbox_container->setMinimumHeight(200);
             d->toolbox_container->setFixedWidth(QWIDGETSIZE_MAX);
             d->restoreSplitterSize(Qt::Vertical);
           } 
            break;
            
        case medViewerConfiguration::LeftDbRightTb:
        case medViewerConfiguration::LeftTbRightDb:
        default:
           {

         d->navigator_container_layout->removeWidget ( d->toolboxPatient );
         d->navigator_container_layout->removeWidget ( d->navigator );

         d->navigator->setOrientation (Qt::Vertical);
         
         d->navigator_container_layout->addWidget (d->toolboxPatient, 0, 0);
         d->navigator_container_layout->addWidget (d->navigator, 1, 0);

             d->navigator_container->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
             d->navigator_container->setFixedWidth(186);
             d->navigator_container->setFixedHeight(QWIDGETSIZE_MAX);
          
         d->toolbox_container->setOrientation(Qt::Vertical);
             d->toolbox_container->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Expanding);
             d->toolbox_container->setMinimumWidth(320);
             d->toolbox_container->setMinimumHeight (QWIDGETSIZE_MAX);
             d->restoreSplitterSize(Qt::Horizontal);
           }

    }


    switch (layout){
        case medViewerConfiguration::TopDbBottomTb:
        case medViewerConfiguration::LeftDbRightTb:
            d->splitter->insertWidget(0,d->navigator_container);
            d->splitter->insertWidget(2,d->toolbox_container);
        break;
        
        case medViewerConfiguration::TopTbBottomDb:
        case medViewerConfiguration::LeftTbRightDb:
        default:
            d->splitter->insertWidget(0,d->toolbox_container);
            d->splitter->insertWidget(2,d->navigator_container);
        break;	    
    }

}


void medViewerAreaPrivate::saveSplitterSize(medViewerConfiguration::LayoutType layout)
{
    if (layout == medViewerConfiguration::TopDbBottomTb ||
        layout == medViewerConfiguration::TopTbBottomDb)
    {
        medSettingsManager::instance()->setValue("application","ViewerSplitterSizeVertical",
            splitter->saveState());
    }
    else
    {
        medSettingsManager::instance()->setValue("application","ViewerSplitterSizeHorizontal",
            splitter->saveState());
    }

}

void medViewerAreaPrivate::restoreSplitterSize(Qt::Orientation orientation)
{
    if (orientation == Qt::Horizontal)
    {

        if (!splitter->restoreState(medSettingsManager::instance()->value("application",
            "ViewerSplitterSizeHorizontal").toByteArray()))
        {
            //viewcontainer size
            int containerSize = QWIDGETSIZE_MAX -
                navigator->minimumWidth()-
                toolbox_container->minimumWidth();
            QList<int> sizes;
            sizes.append(navigator->minimumWidth());
            sizes.append(containerSize);
            sizes.append(toolbox_container->minimumWidth());
            splitter->setSizes(sizes);
        }
        splitter->setOrientation(Qt::Horizontal);

    }
    else
    {
        if (!splitter->restoreState(medSettingsManager::instance()->value("application",
            "ViewerSplitterSizeVertical").toByteArray()))
        {
            //viewcontainer size
            int containerSize = QWIDGETSIZE_MAX -
                navigator->minimumHeight() -
                toolbox_container->minimumHeight();
            QList<int> sizes;
            sizes.append(navigator->minimumHeight());
            sizes.append(containerSize);
            sizes.append(toolbox_container->minimumHeight());
            splitter->setSizes(sizes);
        }
        splitter->setOrientation(Qt::Vertical);

    }

}

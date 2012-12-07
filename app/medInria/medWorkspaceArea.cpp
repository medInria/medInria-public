#include "medWorkspaceArea.h"
#include "medWorkspaceArea_p.h"

#include <dtkCore/dtkAbstractViewFactory.h>
#include <dtkCore/dtkAbstractView.h>
#include <dtkCore/dtkAbstractDataFactory.h>
#include <dtkCore/dtkAbstractData.h>
#include <dtkCore/dtkAbstractDataReader.h>
#include <dtkCore/dtkGlobal.h>

#include <dtkVr/dtkVrHeadRecognizer.h>
#include <dtkVr/dtkVrGestureRecognizer.h>

#include <medAbstractDbController.h>
#include <medSettingsManager.h>
#include <medDataIndex.h>
#include <medDataManager.h>
#include <medViewManager.h>
#include <medAbstractView.h>
#include <medMetaDataKeys.h>

#include <medDatabaseNavigator.h>
#include <medDatabaseNavigatorController.h>
#include <medDatabaseNonPersistentController.h>
#include <medDatabaseController.h>

#include <medClutEditor.h>
#include <medToolBox.h>
#include <medToolBoxContainer.h>
#include <medViewContainer.h>
#include <medViewContainerCustom.h>
#include <medMultiViewContainer.h>
#include <medSingleViewContainer.h>
#include <medViewPool.h>
#include <medWorkspaceFactory.h>
#include <medToolBoxDiffusion.h>
#include <medToolBoxRegistration.h>
#include <medTabbedViewContainers.h>
#include <medViewerToolBoxLayout.h>
#include <medViewerToolBoxPatient.h>
#include <medViewerToolBoxViewProperties.h>

#include <QtGui>
#include <QPropertyAnimation>
#include <QEasingCurve>

// /////////////////////////////////////////////////////////////////
// medWorkspaceArea
// /////////////////////////////////////////////////////////////////

medWorkspaceArea::medWorkspaceArea(QWidget *parent) : QWidget(parent), d(new medWorkspaceAreaPrivate)
{
    // -- Internal logic
    d->current_patient = medDataIndex();
    d->current_workspace_name = "";
    d->current_workspace = 0;
    d->current_layout = medViewerWorkspace::LeftDbRightTb;

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
    view_container_layout->setContentsMargins(0, 0, 0, 0);
    view_container_layout->addWidget(d->stack);


    // Setting up navigator container
    d->navigator_container = new QFrame(this);
    d->navigator_container->setObjectName("medNavigatorContainer");
    d->navigator_container->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
    d->navigator_container->setFixedWidth(186);

    // Setting up navigator
    medDatabaseNavigatorController::instance()->setOrientation( Qt::Vertical );
    d->navigator = new medDatabaseNavigator(d->navigator_container);

    // First argument is the target, last is the parent.
    d->navigator_animation = new QPropertyAnimation (d->navigator, "geometry", d->view_container);
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
    QAction * transFunAction = new QAction("Toggle Tranfer Function Widget", this);
    transFunAction->setShortcut(Qt::ControlModifier + Qt::ShiftModifier + Qt::Key_L);
    transFunAction->setCheckable( true );
    transFunAction->setChecked( false );
    connect(transFunAction, SIGNAL(toggled(bool)),
        this, SLOT(bringUpTransferFunction(bool)));

    this->addAction(transFunAction);

    connect (d->toolboxPatient,          SIGNAL (patientIndexChanged(const medDataIndex&)),
        this, SLOT(switchToPatient(const medDataIndex&)));

    //Avoid double triggering between update and dataAdded/Removed.
    //And dataRemoved is triggered too early: the data has not been actually removed yet.
    connect (medDataManager::instance(), SIGNAL (dataAdded (const medDataIndex&)), d->navigator,
             SLOT (updateNavigator (const medDataIndex&)));
    connect (medDataManager::instance(), SIGNAL (dataRemoved (const medDataIndex&)), d->navigator,
             SLOT (updateNavigator (const medDataIndex&)));

    connect (medDataManager::instance(), SIGNAL (dataAdded (const medDataIndex&)), d->toolboxPatient,
             SLOT (setupDatabase ()));
    connect (medDataManager::instance(), SIGNAL (dataRemoved (const medDataIndex&)), d->toolboxPatient,
             SLOT (setupDatabase ()));

    connect (medDataManager::instance(), SIGNAL (dataAdded (const medDataIndex&)), d->toolboxPatient,
             SLOT (setPatientIndex (const medDataIndex&)));
    connect (medDataManager::instance(), SIGNAL (dataRemoved (const medDataIndex&)), d->toolboxPatient,
             SLOT (setPatientIndex (const medDataIndex&)));

/*
//------------- MEM LEAK TEST BEGIN -----------------//
    int memusage = 0;
    int leak = 0;
    // creating one that loads the dll
    medAbstractView* dummy = dynamic_cast<medAbstractView*>(dtkAbstractViewFactory::instance()->create("v3dView"));
    dtkAbstractViewFactory::instance()->destroy(dummy);

    int beforeMem = medDataManager::getProcessMemoryUsage();
    for (int i = 0; i < 20; i++)
    {
        memusage = medDataManager::getProcessMemoryUsage();
        medAbstractView* view = dynamic_cast<medAbstractView*>(dtkAbstractViewFactory::instance()->create("v3dView"));
        dtkAbstractViewFactory::instance()->destroy(view);
        leak = medDataManager::getProcessMemoryUsage() - memusage;
        qDebug() << "leaking: " << leak / 1000 << " Kbytes";
    }
    int afterMem = medDataManager::getProcessMemoryUsage();
    qDebug() << "total leakage" << (afterMem-beforeMem)  / 1000 << " Kbytes";

//--------------MEM LEAK TEST END ------------------//
*/


}

medWorkspaceArea::~medWorkspaceArea(void)
{

    d->saveSplitterSize(d->current_workspace->layoutType());

    delete d;

    d = NULL;
}

void medWorkspaceArea::setup(QStatusBar *status)
{

}

void medWorkspaceArea::setdw(QStatusBar *status)
{

}

void medWorkspaceArea::split(int rows, int cols)
{
    medViewContainer * root = this->currentRootContainer();
    if ( root != NULL )
        root->split(rows, cols);
}

bool medWorkspaceArea::openInTab(const medDataIndex &index)
{
    if(!((medDataIndex)index).isValid())
        return false;

    // For the moment switch to visualization, later we will be cleverer
    this->setupWorkspace("Visualization");

    if (!this->currentRootContainer()->views().isEmpty())
    {
        medDataManager *dataManager = medDataManager::instance();
        medAbstractDbController *dbc = dataManager->controllerForDataSource(index.dataSourceId());
        QString createdName = dbc->metaData(index,medMetaDataKeys::PatientName.key());
        createdName = d->current_workspace->addMultiContainer(createdName);
        d->current_workspace->stackedViewContainers()->setContainer(createdName);
    }
    else
        d->current_workspace->stackedViewContainers()->changeCurrentContainerType("Multi");

    return this->open(index);
}

bool medWorkspaceArea::open(const medDataIndex& index)
{
    if(!((medDataIndex)index).isValid())
        return false;



    if(((medDataIndex)index).isValidForSeries())
    {
        dtkSmartPointer<dtkAbstractData> data;
        dtkSmartPointer<medAbstractView> view;

        /** test to check for reference counting problems, here the datamanager should be able to drop the data */
        //medDataManager::instance()->data(index);
        //medDataManager::instance()->clearCache();
        //return;


        // the data-manager should be used to read data
        //We can'see wy the dataManager had signals blocked here: put it back on
        //only if it solves an obvious bug.
//        medDataManager::instance()->blockSignals (true);
        data = medDataManager::instance()->data(index);
//        medDataManager::instance()->blockSignals (false);
        if ( data.isNull() )
        {
            return false;
        }

        //get the root container, to see if there is an available view to dump our data in.
        medViewContainer * root = this->currentRootContainer();
        if ( root != NULL )
        {
            view = qobject_cast<medAbstractView*>(root->view());
        }
        bool newView = view.isNull();
        if( newView)
        {
            //container empty, or multi with no extendable view
            view = qobject_cast<medAbstractView*>(dtkAbstractViewFactory::instance()->createSmartPointer("v3dView"));
            connect (view, SIGNAL(closed()), this, SLOT(onViewClosed()));
            connect (view, SIGNAL(dataRemoved(int )), this, SLOT(onDataRemoved(int )));
        }

        if( view.isNull() ) {
            qWarning() << "Unable to create a v3dView";
            return false;
        }

        // add the view to the viewManager. Not much used nowadays
        medViewManager::instance()->insert(index, view);

        // set the data to the view
        if (!root->multiLayer())
        {
            //TODO: move the whole opening of indices to the medViewContainer itself one day.
            view->removeOverlay(0);
            view->setSharedDataPointer(data,0);
            newView = true;
        }
        else
        {
            view->setSharedDataPointer(data);
        }

        // call update
        QMutexLocker ( &d->mutex );
        if ( root != NULL )
        {
            //set the view to the current container
            root->setView(view);
            //only call reset if the view is a new one or with only one layer.
            if (newView)
            {
                qDebug() << "reset view";
                view->reset();
            }
            view->update();
//            qDebug() <<  QApplication::focusWidget();
        }
        this->switchToPatient(index);
        return true;
    }

    if(index.isValidForPatient())
    {
        // For the moment switch to visualization, later we will be cleverer
        this->setupWorkspace("Visualization");
        this->switchToContainer("Multi");

        medDataManager *dataManager = medDataManager::instance();
        medAbstractDbController *dbc = dataManager->controllerForDataSource(index.dataSourceId());

        QList<medDataIndex> studiesForSource = dbc->studies(index);
        bool succeeded = true;
        for ( QList<medDataIndex>::const_iterator studyIt(studiesForSource.begin()); studyIt != studiesForSource.end(); ++studyIt) {
            QList<medDataIndex> seriesForSource = dbc->series((*studyIt));

            for ( QList<medDataIndex>::const_iterator seriesIt(seriesForSource.begin()); seriesIt != seriesForSource.end(); ++seriesIt) {
                bool couldOpen = this->open(*seriesIt);
                succeeded = couldOpen && succeeded;
            }
        }
        return succeeded;
    }
    return true;
}

void medWorkspaceArea::openInTab(const QString& file)
{
    medDataManager::instance()->importNonPersistent (file);
}

void medWorkspaceArea::open(const QString& file)
{
    medDataManager::instance()->importNonPersistent (file);
}

//void medWorkspaceArea::onFileOpened(const medDataIndex &index)
//{
//    qDebug()<< "onFileOpened";
//    this->open(index);
//}

void medWorkspaceArea::onFileOpenedInTab(const medDataIndex &index)
{
//    qDebug()<<"onFileOpenInTab";
    this->openInTab(index);
}

void medWorkspaceArea::onViewClosed(void)
{
    if (medAbstractView *view = dynamic_cast<medAbstractView*> (this->sender())) {
        QList<medToolBox *> toolboxes = d->toolbox_container->toolBoxes();
        foreach( medToolBox *tb, toolboxes)
            tb->update(NULL);

        QList<medDataIndex> indices = medViewManager::instance()->indices( view );
        foreach (medDataIndex index, indices)
            medViewManager::instance()->remove(index, view); // deletes the view
    }
}

void medWorkspaceArea::onDataRemoved(int layer)
{
    //JGG qDebug()<< "Removing layer ";
    Q_UNUSED(layer);
    if (medAbstractView *view = dynamic_cast<medAbstractView*> (this->sender())) {
        QList<medToolBox *> toolboxes = d->toolbox_container->toolBoxes();
        foreach( medToolBox *tb, toolboxes)
            tb->update(view);
    }
}


void medWorkspaceArea::switchToPatient(const medDataIndex& id )
{
    if(!id.isValid()  || d->current_patient.patientId() == id.patientId() ||
       id.patientId() < 0)
        return;


    if (d->current_patient.isValid())
    {
        //clear the confs if needed:
        medSettingsManager * mnger = medSettingsManager::instance();
        bool clear = mnger->value("system","clearOnPatientChange",QVariant(false)).toBool();
        if (clear)
        {

            if (medDataManager::instance()->nonPersistentDataCount())
            {
                // warn the user that previous results might be discarded
                switch(QMessageBox::information( this, "System message",
                        "Changing patient will discard unsaved data. Continue?",
                        "Yes", "No", "Save data in database", 0, 1) )
                {
                case 0:
                    medDataManager::instance()->clearNonPersistentData();
                    emit (clearOnPatientChange());
                    break;

                case 2:
                    medDataManager::instance()->storeNonPersistentDataToDatabase();
                    emit (clearOnPatientChange());
                    break;

                case 1:
                default:
                    //not switching
                    //set the patient toolbox back to the current patient
                    d->toolboxPatient->blockSignals (true);
                    d->toolboxPatient->setPatientIndex(d->current_patient);
                    d->toolboxPatient->blockSignals (false);
                    return;
                    break;
                }
            }
            else
            {
                //there is no hanging data: let's clear
                emit (clearOnPatientChange());
            }

        }
    }


    d->current_patient = id;

    // Setup navigator

    if (d->navigator) {
        d->navigator->onItemClicked(d->current_patient);
        // We can't use the animation here:
        // if a volume is opened from the browser
        // and the workspaceArea has never been shown, the navigator goes outside
        // of the screen.
//        QRect endGeometry = d->navigator->geometry();
//        QRect startGeometry = endGeometry;

//        if (d->navigator->orientation()==Qt::Vertical)
//            startGeometry.setY (endGeometry.y()+1000);
//        else
//            startGeometry.setX (endGeometry.x()+1000);
//        qDebug()<<"startGeometry:" << startGeometry;
//        qDebug()<<"endGeometry"<< endGeometry;
//        d->navigator_animation->setStartValue(startGeometry);
//        d->navigator_animation->setEndValue(endGeometry);
//        d->navigator_animation->start();
    }

    // Setup patient toolbox
    d->toolboxPatient->blockSignals (true);
    d->toolboxPatient->setPatientIndex (
                medDataIndex::makePatientIndex(id.dataSourceId(),
                                               id.patientId()));
    d->toolboxPatient->blockSignals (false);
}

void medWorkspaceArea::switchToStackedViewContainers(medTabbedViewContainers* stack)
{
    if(!stack )
    {
        qWarning() << "No stack to switch to";
        return;
    }

    if (-1 == d->stack->indexOf(stack))
    {
        connect(stack, SIGNAL(dropped(medDataIndex)), this, SLOT(open(medDataIndex)));
        connect(stack, SIGNAL(focused(dtkAbstractView*)),
                this,  SLOT(onViewFocused(dtkAbstractView*)));
        d->stack->addWidget(stack);
    }
    d->stack->setCurrentWidget(stack);
}


void medWorkspaceArea::switchToContainer(const QString& name)
{
//    qDebug() << "switching from"
//             << d->current_workspace->currentViewContainerName()
//             << d->current_workspace->currentViewContainer()
//             << "to container" << name
//             << d->current_workspace->stackedViewContainers()->container(name);
    if (d->current_workspace)
    {
        medViewContainer * root = this->currentRootContainer();

//        root->current()->clearFocus();
        root->current()->setFocus(Qt::MouseFocusReason);
        if ( (root==NULL) || (root ==
             d->current_workspace->stackedViewContainers()->container(name)) )
        {
//            qDebug() << "same conf do nothing";
            //same conf, do nothing
            return;
        }

        d->current_workspace->setCurrentViewContainer(name);
        root->setFocus(Qt::MouseFocusReason);
    }
    else
    {
        //should not happen
        qWarning() << "no currentWorkspace";
    }
}


void medWorkspaceArea::switchToContainerPreset(int index)
{
    if(index < 0)
        return;

    medViewContainer * root = this->currentRootContainer();
    if ( root != NULL ) {
        medViewContainerCustom *custom =
            dynamic_cast<medViewContainerCustom *>( root );
        if ( custom ) {
            custom->setPreset(index);
            d->current_workspace->setCustomPreset(index);
        }
    }
}

void medWorkspaceArea::addToolBox(medToolBox *toolbox)
{
    d->toolbox_container->addToolBox(toolbox);
}

void medWorkspaceArea::removeToolBox(medToolBox *toolbox)
{
    d->toolbox_container->removeToolBox(toolbox);
}

void medWorkspaceArea::onViewFocused(dtkAbstractView *view)
{
    // set head recognizer
//    qDebug() << "medWorkspaceAreaOnViewFocused";
    if (view)
    { //Note to Julien from Ben: not sure the head recognizer works for view==NULL, so I put it inside this iftake it out if needed.
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
        //update tranfer function.
        connect (view, SIGNAL(lutChanged()),
                 this, SLOT(updateTransferFunction()), Qt::UniqueConnection);
        this->updateTransferFunction();
    }

    // Update toolboxes
    QList<medToolBox *> toolboxes = d->toolbox_container->toolBoxes();
    foreach( medToolBox *tb, toolboxes)
    {
        tb->update(view);
    }
}

medViewContainer *medWorkspaceArea::currentRootContainer(void)
{
    if ( d->current_workspace == NULL )
        return NULL;

    return d->current_workspace->currentViewContainer();
}

medViewContainer *medWorkspaceArea::currentContainerFocused(void)
{
    medViewContainer * root = this->currentRootContainer();
    if ( root == NULL )
        return NULL;

    return root->current();
}

// view settings
/*
void medWorkspaceArea::setupForegroundLookupTable(QString table)
{
    if(!d->view_stacks.count())
        return;

    if ( medViewPool *pool = this->currentContainer()->pool() )
        pool->setViewProperty("LookupTable", table);

    this->updateTransferFunction();
}
void medWorkspaceArea::setupLUTPreset(QString table)
{
    if(!d->view_stacks.count())
        return;

    if ( medViewPool *pool = this->currentContainer()->pool() ) {
        pool->setViewProperty("Preset", table);
    }

    this->updateTransferFunction();
}
*/

void medWorkspaceArea::bringUpTransferFunction(bool checked)
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

    medViewContainer * current = this->currentContainerFocused();
    if ( current == NULL )
        return;

    if ( dtkAbstractView *view = current->view() ) {
      d->transFun = new medClutEditor(NULL);
      d->transFun->setWindowModality( Qt::WindowModal );
      d->transFun->setWindowFlags(Qt::Tool|Qt::WindowStaysOnTopHint);

      // d->transFun->setData(static_cast<dtkAbstractData *>(view->data()));
      d->transFun->setView(dynamic_cast<medAbstractView*>(view));

      d->transFun->show();
    }
}

void medWorkspaceArea::updateTransferFunction()
{
    medViewContainer * current = this->currentContainerFocused();
    if ( current == NULL )
        return;

    dtkAbstractView * view = current->view();
    if ( d->transFun != NULL && view != NULL ) {
    // d->transFun->setData( static_cast<dtkAbstractData *>( view->data() ) );
        d->transFun->setView( dynamic_cast<medAbstractView *>( view ), true );
        d->transFun->update();
    }
}


void medWorkspaceArea::setupWorkspace(QString name)
{
    //    qDebug() << "setupWorkspace to :" << name;
    if (d->current_workspace_name == name)
        return;

    // clear the current config if needed
    /*
    if (d->current_workspace) {
        medSettingsManager * mnger = medSettingsManager::instance();
        bool clear = mnger->value("system","clearOnPatientChange",QVariant(false)).toBool();
        if (clear)
        {
            qDebug()<<"clearing the containers on workspace switch";
            d->current_workspace->clear();
        }
    }
*/

    medViewerWorkspace *workspace = NULL;

    if (d->workspaces.contains(name))
        workspace = d->workspaces[name];
    else {
        if (workspace = medWorkspaceFactory::instance()->createWorkspace(name, this)) {
            connect(this, SIGNAL(clearOnPatientChange()), workspace, SLOT(clear()));
            d->workspaces.insert(name, workspace);
        }
        else
            qWarning()<< "Workspace" << name << "couldn't be created";
    }

    if (!workspace)
        return;

    d->current_workspace = workspace;
    d->current_workspace_name = name;

    //clean toolboxes
    d->toolbox_container->hide();
    d->toolbox_container->clear();

    //setup layout
    switchToLayout (workspace->layoutType());

    // setup layout type
    //clear the confs if needed:
    medSettingsManager * mnger = medSettingsManager::instance();
    bool clear = mnger->value("system","clearOnPatientChange",QVariant(false)).toBool();
    if (clear)
    {
        qDebug()<<"clearing the containers on workspace switch";
        workspace->clear();
    }
    else
    {
        workspace->setupViewContainerStack();
    }

    switchToStackedViewContainers(workspace->stackedViewContainers());

    if (qobject_cast<medViewContainerCustom *>(workspace->currentViewContainer())) {
        switchToContainerPreset(workspace->customLayoutPreset());
    }

    //setup database visibility
    d->navigator_container->setVisible( workspace->isDatabaseVisible() );

    // add toolboxes

    foreach (medToolBox * toolbox, workspace->toolBoxes() ) {
        this->addToolBox(toolbox);
        toolbox->show();
    }

    d->toolbox_container->setVisible( workspace->areToolBoxesVisible() );

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

//    connect(workspace->stackedViewContainers(), SIGNAL(currentChanged(const QString&)),
//            this, SLOT(switchToContainer(const QString&)), Qt::UniqueConnection);
    connect(workspace, SIGNAL(layoutSplit(int,int)),       this, SLOT(split(int,int)), Qt::UniqueConnection);
    connect(workspace, SIGNAL(layoutPresetClicked(int)),   this, SLOT(switchToContainerPreset(int)), Qt::UniqueConnection);
    connect(workspace, SIGNAL(toolboxAdded(medToolBox*)),  this, SLOT(addToolBox(medToolBox*)), Qt::UniqueConnection);
    connect(workspace, SIGNAL(toolboxRemoved(medToolBox*)),this, SLOT(removeToolBox(medToolBox*)), Qt::UniqueConnection);
}

void medWorkspaceArea::switchToLayout (medViewerWorkspace::LayoutType layout)
{
    if (d->current_layout==layout)
        return;


    //save previous splitter size
    d->saveSplitterSize(d->current_layout);

    d->current_layout = layout;

    //setup orientation
    switch (layout){
        case medViewerWorkspace::TopDbBottomTb:
        case medViewerWorkspace::TopTbBottomDb:
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

        case medViewerWorkspace::LeftDbRightTb:
        case medViewerWorkspace::LeftTbRightDb:
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
        case medViewerWorkspace::TopDbBottomTb:
        case medViewerWorkspace::LeftDbRightTb:
            d->splitter->insertWidget(0,d->navigator_container);
            d->splitter->insertWidget(2,d->toolbox_container);
        break;

        case medViewerWorkspace::TopTbBottomDb:
        case medViewerWorkspace::LeftTbRightDb:
        default:
            d->splitter->insertWidget(0,d->toolbox_container);
            d->splitter->insertWidget(2,d->navigator_container);
        break;
    }

}


void medWorkspaceAreaPrivate::saveSplitterSize(medViewerWorkspace::LayoutType layout)
{
    if (layout == medViewerWorkspace::TopDbBottomTb ||
        layout == medViewerWorkspace::TopTbBottomDb)
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

void medWorkspaceAreaPrivate::restoreSplitterSize(Qt::Orientation orientation)
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

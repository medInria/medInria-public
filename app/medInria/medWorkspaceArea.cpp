/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medWorkspaceArea.h>
#include <medWorkspaceArea_p.h>


#include <medAbstractDataFactory.h>
#include <medAbstractData.h>
#include <dtkCore/dtkAbstractDataReader.h>
#include <dtkCore/dtkGlobal.h>

#include <dtkVr/dtkVrHeadRecognizer.h>
#include <dtkVr/dtkVrGestureRecognizer.h>

#include <medAbstractDbController.h>
#include <medSettingsManager.h>
#include <medDataIndex.h>
#include <medDataManager.h>
#include <medViewManager.h>
#include <medAbstractImageView.h>
#include <medMetaDataKeys.h>

#include <medDatabaseNavigator.h>
#include <medDatabaseNavigatorController.h>
#include <medDatabaseNonPersistentController.h>
#include <medDatabaseController.h>
#include <medViewFactory.h>
#include <medAbstractView.h>

#include <medClutEditor.h>
#include <medToolBox.h>
#include <medToolBoxContainer.h>
#include <medViewContainer.h>
#include <medCustomViewContainer.h>
#include <medMultiViewContainer.h>
#include <medSingleViewContainer.h>
#include <medWorkspaceFactory.h>
#include <medTabbedViewContainers.h>
#include <medVisualizationLayoutToolBox.h>
#include <medPatientSelectorToolBox.h>

#include <QtGui>
#include <QPropertyAnimation>
#include <QEasingCurve>

// /////////////////////////////////////////////////////////////////
// medWorkspaceArea
// /////////////////////////////////////////////////////////////////

medWorkspaceArea::medWorkspaceArea(QWidget *parent) : QWidget(parent), d(new medWorkspaceAreaPrivate)
{
    // -- Internal logic
    d->currentPatient = medDataIndex();
    d->currentWorkspaceName = "";
    d->currentWorkspace = 0;
    d->currentLayout = medWorkspace::LeftDbRightTb;

    d->splitter = new QSplitter(this);
    d->splitter->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
    d->splitter->setHandleWidth(2);

    // -- User interface setup
    d->stack = new QStackedWidget(this);
    d->stack->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    d->patientToolBox = new medPatientSelectorToolBox(this);
    d->patientToolBox->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);//, QSizePolicy::Minimum);
    d->patientToolBox->setFixedWidth(200); // 156 - 10


    // Setting up toolbox container
    d->toolBoxContainer = new medToolBoxContainer(this);
    d->toolBoxContainer->setOrientation(Qt::Vertical);
    d->toolBoxContainer->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Expanding);
    d->toolBoxContainer->setMinimumWidth(340);

    // Setting up view container
    d->viewContainer = new QWidget(this);
    QVBoxLayout *viewContainerLayout = new QVBoxLayout(d->viewContainer);
    viewContainerLayout->setContentsMargins(0, 0, 0, 0);
    viewContainerLayout->addWidget(d->stack);

    // Setting up navigator container
    d->navigatorContainer = new QFrame(this);
    d->navigatorContainer->setObjectName("medNavigatorContainer");
    d->navigatorContainer->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
    d->navigatorContainer->setFixedWidth(200);

    // Setting up navigator
    medDatabaseNavigatorController::instance()->setOrientation( Qt::Vertical );
    d->navigator = new medDatabaseNavigator(d->navigatorContainer);

    // First argument is the target, last is the parent.
    d->navigatorAnimation = new QPropertyAnimation (d->navigator, "geometry", d->viewContainer);
    d->navigatorAnimation->setDuration (500);
    d->navigatorAnimation->setEasingCurve (QEasingCurve::OutQuad);

    // d->navigatorContainerLayout = 0;
    d->navigatorContainerLayout = new QGridLayout(d->navigatorContainer);
    d->navigatorContainerLayout->setAlignment(Qt::AlignHCenter|Qt::AlignTop);
    d->navigatorContainerLayout->setContentsMargins(0, 0, 0, 0);
    d->navigatorContainerLayout->setSpacing(0);
    d->navigatorContainerLayout->addWidget(d->patientToolBox, 0, 0);
    d->navigatorContainerLayout->addWidget(d->navigator, 1, 0);

    //Set up viewer layout
    QHBoxLayout *layout = new QHBoxLayout;
    layout->addWidget(d->splitter);
    layout->setContentsMargins(1, 0, 1, 0);
    setLayout(layout);
    d->splitter->addWidget(d->navigatorContainer);
    d->splitter->addWidget(d->viewContainer);
    d->splitter->addWidget(d->toolBoxContainer);

    //restore previous splitter position.
    d->restoreSplitterSize(Qt::Horizontal);

    //action for transfer function
    QAction * transFunAction = new QAction("Toggle Tranfer Function Widget", this);
    transFunAction->setShortcut(Qt::CTRL + Qt::Key_H);
    transFunAction->setCheckable( true );
    transFunAction->setChecked( false );
    connect(transFunAction, SIGNAL(toggled(bool)),
        this, SLOT(bringUpTransferFunction(bool)));

    this->addAction(transFunAction);

    connect (d->patientToolBox,          SIGNAL (patientIndexChanged(const medDataIndex&)),
        this, SLOT(switchToPatient(const medDataIndex&)));

    //Avoid double triggering between update and dataAdded/Removed.
    //And dataRemoved is triggered too early: the data has not been actually removed yet.
    connect (medDataManager::instance(), SIGNAL (dataAdded (const medDataIndex&)),
             d->navigator, SLOT (updateNavigator (const medDataIndex&)));
    connect (medDataManager::instance(), SIGNAL (dataRemoved (const medDataIndex&)),
             d->navigator, SLOT (updateNavigator (const medDataIndex&)), Qt::QueuedConnection);

    connect (medDataManager::instance(), SIGNAL (dataAdded (const medDataIndex&)),
             d->patientToolBox, SLOT (setupDatabase ()));
    connect (medDataManager::instance(), SIGNAL (dataRemoved (const medDataIndex&)),
             d->patientToolBox, SLOT (setupDatabase ()));

    connect (medDataManager::instance(), SIGNAL (dataAdded (const medDataIndex&)),
             d->patientToolBox, SLOT (setPatientIndex (const medDataIndex&)));
    connect (medDataManager::instance(), SIGNAL (dataRemoved (const medDataIndex&)),
             d->patientToolBox, SLOT (setPatientIndex (const medDataIndex&)));
}

medWorkspaceArea::~medWorkspaceArea(void)
{

    d->saveSplitterSize(d->currentWorkspace->layoutType());

    delete d;

    d = NULL;
}

void medWorkspaceArea::setup(QStatusBar *status)
{

}

void medWorkspaceArea::setdw(QStatusBar *status)
{

}

QPixmap medWorkspaceArea::grabScreenshot()
{
    return QPixmap::grabWidget(this->currentRootContainer());
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
        createdName = d->currentWorkspace->addDefaultTypeContainer(createdName);
        d->currentWorkspace->stackedViewContainers()->setContainer(createdName);
    }
    
    medSettingsManager * mnger = medSettingsManager::instance();
    QString defaultLayout = mnger->value("startup","default_container_layout",
                                  "Multi").toString();
    
    if ((this->currentRootContainer()->views().isEmpty()))
    {
        medDataManager *dataManager = medDataManager::instance();
        d->currentWorkspace->stackedViewContainers()->changeCurrentContainerType(defaultLayout);
        medAbstractDbController *dbc = dataManager->controllerForDataSource(index.dataSourceId());
        QString realName = dbc->metaData(index,medMetaDataKeys::PatientName.key());
        d->currentWorkspace->stackedViewContainers()->changeCurrentContainerName(realName);
    }
    
    return this->open(index);
}

bool medWorkspaceArea::open(const medDataIndex& index)
{
    bool succeeded = false;

    if( !index.isValid() )
        return false;
    if( index.isValidForSeries() )
    {
        //get the root container, to see if there is an available view to dump our data in.
        medViewContainer * root = this->currentRootContainer();
        connect(this, SIGNAL(sliceSelected(int)), root, SIGNAL(sliceSelected(int)));
        succeeded = root->open(index);
        this->switchToPatient(index);
    }
    else if( index.isValidForPatient() )
    {
        // For the moment switch to visualization, later we will be cleverer
        this->setupWorkspace("Visualization");
        this->switchToContainer("Multi");

        medDataManager *dataManager = medDataManager::instance();
        medAbstractDbController *dbc = dataManager->controllerForDataSource(index.dataSourceId());

        QList<medDataIndex> studiesForSource = dbc->studies(index);
        succeeded = true;
        for ( QList<medDataIndex>::const_iterator studyIt(studiesForSource.begin()); studyIt != studiesForSource.end(); ++studyIt) {
            QList<medDataIndex> seriesForSource = dbc->series((*studyIt));

            for ( QList<medDataIndex>::const_iterator seriesIt(seriesForSource.begin()); seriesIt != seriesForSource.end(); ++seriesIt) {
                bool couldOpen = this->open(*seriesIt);
                succeeded = couldOpen && succeeded;
            }
        }
    }
    
    return succeeded;

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
    medAbstractImageView * medView = dynamic_cast<medAbstractImageView *> (this->sender());
    if (medView) {
       onViewClosed(medView);
    }
}

void medWorkspaceArea::onViewClosed(medAbstractView *view)
{ 
    medAbstractImageView * medView = dynamic_cast<medAbstractImageView *> (view);
    if( medView )
    {
        QList<medToolBox *> toolboxes = d->toolBoxContainer->toolBoxes();
        foreach( medToolBox *tb, toolboxes)
            tb->update(NULL);

        QList<medDataIndex> indices = medViewManager::instance()->indices( medView );
        foreach (medDataIndex index, indices)
            medViewManager::instance()->remove(index, medView); // deletes the view
    }
}

void medWorkspaceArea::onDataRemoved(int layer)
{
    Q_UNUSED(layer);
    medAbstractImageView * medView = dynamic_cast<medAbstractImageView *> (this->sender());
    if (medView) {
        QList<medToolBox *> toolboxes = d->toolBoxContainer->toolBoxes();
        foreach( medToolBox *tb, toolboxes)
            tb->update(medView);
    }
}


void medWorkspaceArea::switchToPatient(const medDataIndex& id )
{
    if(!id.isValid()  || d->currentPatient.patientId() == id.patientId() ||
       id.patientId() < 0)
        return;


    if (d->currentPatient.isValid())
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
                    d->patientToolBox->blockSignals (true);
                    d->patientToolBox->setPatientIndex(d->currentPatient);
                    d->patientToolBox->blockSignals (false);
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


    d->currentPatient = id;

    // Setup navigator

    if (d->navigator) {
        d->navigator->onItemClicked(d->currentPatient);
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
//        d->navigatorAnimation->setStartValue(startGeometry);
//        d->navigatorAnimation->setEndValue(endGeometry);
//        d->navigatorAnimation->start();
    }

    // Setup patient toolbox
    d->patientToolBox->blockSignals (true);
    d->patientToolBox->setPatientIndex (
                medDataIndex::makePatientIndex(id.dataSourceId(),
                                               id.patientId()));
    d->patientToolBox->blockSignals (false);
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
        connect(stack, SIGNAL(focused(medAbstractView*)),
                this,  SLOT(onViewFocused(medAbstractView*)));
        connect (stack, SIGNAL(viewRemoved(medAbstractView *)),
                 this, SLOT(onViewClosed(medAbstractView *)));
        d->stack->addWidget(stack);
    }
    d->stack->setCurrentWidget(stack);
}


void medWorkspaceArea::switchToContainer(const QString& name)
{
//    qDebug() << "switching from"
//             << d->currentWorkspace->currentViewContainerName()
//             << d->currentWorkspace->currentViewContainer()
//             << "to container" << name
//             << d->currentWorkspace->stackedViewContainers()->container(name);
    if (d->currentWorkspace)
    {
        medViewContainer * root = this->currentRootContainer();

//        root->current()->clearFocus();
        root->current()->setFocus(Qt::MouseFocusReason);
        if ( (root==NULL) || (root ==
             d->currentWorkspace->stackedViewContainers()->container(name)) )
        {
//            qDebug() << "same conf do nothing";
            //same conf, do nothing
            return;
        }

        d->currentWorkspace->setCurrentViewContainer(name);
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
        medCustomViewContainer *custom =
            dynamic_cast<medCustomViewContainer *>( root );
        if ( custom ) {
            custom->setPreset(index);
            d->currentWorkspace->setCustomPreset(index);
        }
    }
}

void medWorkspaceArea::addToolBox(medToolBox *toolbox)
{
    d->toolBoxContainer->addToolBox(toolbox);
}

void medWorkspaceArea::removeToolBox(medToolBox *toolbox)
{
    d->toolBoxContainer->removeToolBox(toolbox);
}

void medWorkspaceArea::onViewFocused(medAbstractView *view)
{
//    // set head recognizer
//    //    qDebug() << "medWorkspaceAreaOnViewFocused";
//    if(view)
//    { //Note to Julien from Ben: not sure the head recognizer works for view==NULL, so I put it inside this iftake it out if needed.
//        static dtkVrHeadRecognizer *head_recognizer = NULL;

//        if(dtkApplicationArgumentsContain(qApp, "--tracker")) {

//            if(!head_recognizer) {
//                head_recognizer = new dtkVrHeadRecognizer;
//                head_recognizer->startConnection(QUrl(dtkApplicationArgumentsValue(qApp, "--tracker")));
//            }

//            if(view->property("Orientation") == "3D")
//                head_recognizer->setView(view);
//            else
//                head_recognizer->setView(NULL);
//        }

//        // set gesture recognizer

//        static dtkVrGestureRecognizer *gesture_recognizer = NULL;

//        if(dtkApplicationArgumentsContain(qApp, "--tracker")) {

//            if(!gesture_recognizer) {
//                gesture_recognizer = new dtkVrGestureRecognizer;
//                gesture_recognizer->startConnection(QUrl(dtkApplicationArgumentsValue(qApp, "--tracker")));
//            }

//            gesture_recognizer->setView(view);
//            gesture_recognizer->setReceiver(static_cast<medAbstractImageView *>(view)->viewWidget());
//        }
//        //update tranfer function.
//        connect (view, SIGNAL(lutChanged()),
//                 this, SLOT(updateTransferFunction()), Qt::UniqueConnection);
//        this->updateTransferFunction();
//    }

    if(!view)
        return;

    // Update toolboxes
    QList<medToolBox *> toolboxes = d->toolBoxContainer->toolBoxes();
    foreach( medToolBox *tb, toolboxes)
    {
        tb->update(view);
    }
}

medViewContainer *medWorkspaceArea::currentRootContainer(void)
{
    if ( d->currentWorkspace == NULL )
        return NULL;

    return d->currentWorkspace->currentViewContainer();
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

    if ( medAbstractView *view = current->view() ) {
      d->transFun = new medClutEditor(NULL);
      d->transFun->setWindowModality( Qt::WindowModal );
      d->transFun->setWindowFlags(Qt::Tool|Qt::WindowStaysOnTopHint);

      // d->transFun->setData(static_cast<medAbstractData *>(view->data()));
      d->transFun->setView(dynamic_cast<medAbstractImageView*>(view));

      d->transFun->show();
    }
}

void medWorkspaceArea::updateTransferFunction()
{
    medViewContainer * current = this->currentContainerFocused();
    if ( current == NULL )
        return;

    medAbstractView * view = current->view();
    if ( d->transFun != NULL && view != NULL ) {
    // d->transFun->setData( static_cast<medAbstractData *>( view->data() ) );
        d->transFun->setView( dynamic_cast<medAbstractImageView *>( view ), true );
        d->transFun->update();
    }
}


void medWorkspaceArea::setupWorkspace(QString name)
{
    //    qDebug() << "setupWorkspace to :" << name;
    if (d->currentWorkspaceName == name)
        return;

    // clear the current config if needed
    /*
    if (d->currentWorkspace) {
        medSettingsManager * mnger = medSettingsManager::instance();
        bool clear = mnger->value("system","clearOnPatientChange",QVariant(false)).toBool();
        if (clear)
        {
            qDebug()<<"clearing the containers on workspace switch";
            d->currentWorkspace->clear();
        }
    }
*/

    medWorkspace *workspace = NULL;

    if (d->workspaces.contains(name))
        workspace = d->workspaces[name];
    else
    {
        workspace = medWorkspaceFactory::instance()->createWorkspace(name, this);
        if (workspace)
        {
            connect(this, SIGNAL(clearOnPatientChange()), workspace, SLOT(clear()));
            d->workspaces.insert(name, workspace);
        }
        else
            qWarning()<< "Workspace " << name << " couldn't be created";
    }

    if (!workspace)
        return;

    d->currentWorkspace = workspace;
    d->currentWorkspaceName = name;

    //clean toolboxes
    d->toolBoxContainer->hide();
    d->toolBoxContainer->clear();

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

    if (qobject_cast<medCustomViewContainer *>(workspace->currentViewContainer())) {
        switchToContainerPreset(workspace->customLayoutPreset());
    }

    //setup database visibility
    d->navigatorContainer->setVisible( workspace->isDatabaseVisible() );

    // add toolboxes

    foreach (medToolBox * toolbox, workspace->toolBoxes() ) {
        this->addToolBox(toolbox);
        toolbox->show();
    }

    d->toolBoxContainer->setVisible( workspace->areToolBoxesVisible() );

    /*
      if (d->toolBoxContainer->toolBoxes().count()) {
      QPropertyAnimation *animation = new QPropertyAnimation(d->toolBoxContainer, "geometry");
      animation->setDuration(500);
      if (d->toolBoxContainer->orientation()==medToolBoxContainer::Vertical)  {
      animation->setStartValue(QRect(d->toolBoxContainer->x(), 1000, d->toolBoxContainer->width(), d->toolBoxContainer->height()));
      animation->setEndValue(QRect(d->toolBoxContainer->x(), 0, d->toolBoxContainer->width(), d->toolBoxContainer->height()));
      }
      else {
      animation->setStartValue(QRect(1000, d->toolBoxContainer->y(), d->toolBoxContainer->width(), d->toolBoxContainer->height()));
      animation->setEndValue(QRect(0, d->toolBoxContainer->y(), d->toolBoxContainer->width(), d->toolBoxContainer->height()));
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

    // double-click on a thumbnail launches its visualization in the current workspace

    connect (medDataManager::instance(), SIGNAL(openRequested(const medDataIndex&)),
        this, SLOT(open(const medDataIndex&)), Qt::UniqueConnection);
}

void medWorkspaceArea::switchToLayout (medWorkspace::LayoutType layout)
{
    if (d->currentLayout==layout)
        return;


    //save previous splitter size
    d->saveSplitterSize(d->currentLayout);

    d->currentLayout = layout;

    //setup orientation
    switch (layout){
        case medWorkspace::TopDbBottomTb:
        case medWorkspace::TopTbBottomDb:
           {

         d->navigatorContainerLayout->removeWidget ( d->patientToolBox );
         d->navigatorContainerLayout->removeWidget ( d->navigator );

         d->navigator->setOrientation (Qt::Horizontal);

         d->navigatorContainerLayout->addWidget (d->patientToolBox, 0, 0);
         d->navigatorContainerLayout->addWidget (d->navigator, 0, 1);
             //width must be fixed or the navigator doesn't grow
             //back when changing orientation again
             d->navigatorContainer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
             d->navigatorContainer->setFixedHeight(156);
             d->navigatorContainer->setFixedWidth(QWIDGETSIZE_MAX);

         d->toolBoxContainer->setOrientation(Qt::Horizontal);
             d->toolBoxContainer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
             d->toolBoxContainer->setMinimumHeight(200);
             d->toolBoxContainer->setFixedWidth(QWIDGETSIZE_MAX);
             d->restoreSplitterSize(Qt::Vertical);
           }
            break;

        case medWorkspace::LeftDbRightTb:
        case medWorkspace::LeftTbRightDb:
        default:
           {

         d->navigatorContainerLayout->removeWidget ( d->patientToolBox );
         d->navigatorContainerLayout->removeWidget ( d->navigator );

         d->navigator->setOrientation (Qt::Vertical);

         d->navigatorContainerLayout->addWidget (d->patientToolBox, 0, 0);
         d->navigatorContainerLayout->addWidget (d->navigator, 1, 0);

             d->navigatorContainer->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
             d->navigatorContainer->setFixedWidth(156);
             d->navigatorContainer->setFixedHeight(QWIDGETSIZE_MAX);

         d->toolBoxContainer->setOrientation(Qt::Vertical);
             d->toolBoxContainer->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Expanding);
             d->toolBoxContainer->setMinimumWidth(320);
             d->toolBoxContainer->setMinimumHeight (QWIDGETSIZE_MAX);
             d->restoreSplitterSize(Qt::Horizontal);
           }

    }


    switch (layout){
        case medWorkspace::TopDbBottomTb:
        case medWorkspace::LeftDbRightTb:
            d->splitter->insertWidget(0,d->navigatorContainer);
            d->splitter->insertWidget(2,d->toolBoxContainer);
        break;

        case medWorkspace::TopTbBottomDb:
        case medWorkspace::LeftTbRightDb:
        default:
            d->splitter->insertWidget(0,d->toolBoxContainer);
            d->splitter->insertWidget(2,d->navigatorContainer);
        break;
    }

}


void medWorkspaceAreaPrivate::saveSplitterSize(medWorkspace::LayoutType layout)
{
    if (layout == medWorkspace::TopDbBottomTb ||
        layout == medWorkspace::TopTbBottomDb)
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
                toolBoxContainer->minimumWidth();
            QList<int> sizes;
            sizes.append(navigator->minimumWidth());
            sizes.append(containerSize);
            sizes.append(toolBoxContainer->minimumWidth());
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
                toolBoxContainer->minimumHeight();
            QList<int> sizes;
            sizes.append(navigator->minimumHeight());
            sizes.append(containerSize);
            sizes.append(toolBoxContainer->minimumHeight());
            splitter->setSizes(sizes);
        }
        splitter->setOrientation(Qt::Vertical);

    }

}

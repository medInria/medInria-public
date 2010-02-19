/* medViewerArea.cpp --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Fri Sep 18 12:43:06 2009 (+0200)
 * Version: $Id$
 * Last-Updated: Fri Feb 19 18:24:26 2010 (+0100)
 *           By: Julien Wintz
 *     Update #: 563
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

#include <medGui/medToolBox.h>
#include <medGui/medToolBoxContainer.h>
#include <medGui/medToolBoxConfiguration.h>
#include <medGui/medToolBoxLayout.h>
#include <medGui/medToolBoxPatient.h>
#include <medGui/medToolBoxDiffusion.h>
#include <medGui/medToolBoxRegistration.h>
#include <medGui/medToolBoxView.h>
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
    medToolBoxPatient *patientToolBox;
    medToolBoxConfiguration *configurationToolBox;
    medToolBoxLayout *layoutToolBox;
    medToolBoxView *viewToolBox;
    medToolBoxDiffusion *diffusionToolBox;
    medToolBoxRegistration *registrationToolBox;
    medToolBoxContainer *toolbox_container;

    medDatabaseNavigator *navigator;

    QStackedWidget *stack;
  
    // view containers hash

    QHash<int, medViewerAreaStack *> view_stacks;

    // status elements

    QProgressBar *progress;
};

medViewerArea::medViewerArea(QWidget *parent) : QWidget(parent), d(new medViewerAreaPrivate)
{
    d->stack = new QStackedWidget(this);
    d->stack->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    // -- Patient toolbox --

    d->patientToolBox = new medToolBoxPatient(this);

    connect(d->patientToolBox, SIGNAL(patientIndexChanged(int)), this, SLOT(onPatientIndexChanged(int)));

    // -- Configuration toolbox --

    d->configurationToolBox = new medToolBoxConfiguration(this);

    // connect(d->configurationToolBox, SIGNAL(configurationChanged(QString)), this, SLOT());

    // -- Layout toolbox --

    d->layoutToolBox = new medToolBoxLayout(this);

    connect(d->layoutToolBox, SIGNAL(modeChanged(int)), this, SLOT(setStackIndex(int)));
    connect(d->layoutToolBox, SIGNAL(split(int, int)), this, SLOT(split(int, int)));

    // -- View toolbox --
    
    d->viewToolBox = new medToolBoxView(this);

    connect(d->viewToolBox, SIGNAL(foregroundLookupTableChanged(QString)), this, SLOT(setupForegroundLookupTable(QString)));
    connect(d->viewToolBox, SIGNAL(backgroundLookupTableChanged(QString)), this, SLOT(setupBackgroundLookupTable(QString)));
    connect(d->viewToolBox, SIGNAL(lutPresetChanged(QString)), this, SLOT(setupLUTPreset(QString)));
    connect(d->viewToolBox, SIGNAL(tdModeChanged(QString)), this, SLOT(setup3DMode(QString)));
    //connect(d->viewToolBox, SIGNAL(tdLodChanged(int)), this, SLOT(setup3DLOD(int)));
    connect(d->viewToolBox, SIGNAL(windowingChanged(bool)), this, SLOT(setupWindowing(bool)));
    connect(d->viewToolBox, SIGNAL(zoomingChanged(bool)), this, SLOT(setupZooming(bool)));
    connect(d->viewToolBox, SIGNAL(slicingChanged(bool)), this, SLOT(setupSlicing(bool)));
    connect(d->viewToolBox, SIGNAL(measuringChanged(bool)), this, SLOT(setupMeasuring(bool)));
    connect(d->viewToolBox, SIGNAL(croppingChanged(bool)), this, SLOT(setupCropping(bool)));
    connect(d->viewToolBox, SIGNAL(scalarBarVisibilityChanged(bool)), this, SLOT(setupScalarBarVisibility(bool)));
    connect(d->viewToolBox, SIGNAL(axisVisibilityChanged(bool)), this, SLOT(setupAxisVisibility(bool)));

    // -- Diffusion toolbox --

    d->diffusionToolBox = new medToolBoxDiffusion(this);

    // -- Registration toolbox --

    d->registrationToolBox = new medToolBoxRegistration(this);
    
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
    d->patientToolBox->setPatientIndex(index);
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
    d->patientToolBox->clear();
    d->patientToolBox->addItem("Choose patient");

    QSqlQuery query(*(medDatabaseController::instance()->database()));

    query.prepare("SELECT name, id FROM patient");
    if(!query.exec())
        qDebug() << DTK_COLOR_FG_RED << query.lastError() << DTK_NOCOLOR;

    while(query.next())
        d->patientToolBox->addItem(query.value(0).toString(), query.value(1));
}

void medViewerArea::split(int rows, int cols)
{
    if (d->view_stacks.count())
        d->view_stacks.value(d->patientToolBox->patientIndex())->current()->current()->split(rows, cols);
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

    if (data)
        medDataManager::instance()->insert(index, data);

    if (data) {

        dtkAbstractView *view = dtkAbstractViewFactory::instance()->create("v3dView");

        if (!view)
            return;

	connect(d->viewToolBox, SIGNAL(tdLodChanged(int)), view, SLOT(onVRQualitySet(int)));
	
        view->setData(data);
        view->reset();

        d->view_stacks.value(d->patientToolBox->patientIndex())->current()->current()->setView(view);
        d->view_stacks.value(d->patientToolBox->patientIndex())->current()->current()->setFocus(Qt::MouseFocusReason);
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
    d->viewToolBox->update(view);
}

// layout settings

void medViewerArea::setStackIndex(int index)
{
    if (d->view_stacks.count())
        d->view_stacks.value(d->patientToolBox->patientIndex())->setCurrentIndex(index);
}

// view settings

void medViewerArea::setupForegroundLookupTable(QString table)
{
    if(!d->view_stacks.count())
        return;

    if(dtkAbstractView *view =  d->view_stacks.value(d->patientToolBox->patientIndex())->current()->current()->view()) {
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
    
    if(dtkAbstractView *view =  d->view_stacks.value(d->patientToolBox->patientIndex())->current()->current()->view()) {
        view->setProperty("BackgroundLookupTable", table);
	view->update();
    }
}

void medViewerArea::setupAxisVisibility(bool visible)
{
    if(!d->view_stacks.count())
        return;
  
    if(dtkAbstractView *view = d->view_stacks.value(d->patientToolBox->patientIndex())->current()->current()->view()) {
        visible ? view->setProperty("ShowAxis", "true") : view->setProperty("ShowAxis", "false");
	view->update();
    }
}

void medViewerArea::setupScalarBarVisibility(bool visible)
{
    if(!d->view_stacks.count())
        return;
  
    if(dtkAbstractView *view =  d->view_stacks.value(d->patientToolBox->patientIndex())->current()->current()->view()) {
        visible ? view->setProperty("ScalarBarVisibility", "true") : view->setProperty("ScalarBarVisibility", "false");
	view->update();
    }
}

void medViewerArea::setup3DMode (QString table)
{
    if(!d->view_stacks.count())
        return;
  
    if(dtkAbstractView *view =  d->view_stacks.value(d->patientToolBox->patientIndex())->current()->current()->view()) {
        view->setProperty("Mode", table);
        view->update();
    }
}

void medViewerArea::setupLUTPreset (QString table)
{
    if(!d->view_stacks.count())
        return;
  
    if(dtkAbstractView *view =  d->view_stacks.value(d->patientToolBox->patientIndex())->current()->current()->view()) {
        view->setProperty("Preset", table);
	view->update();
    }
}

void medViewerArea::setup3DLOD (int value)
{
}

void medViewerArea::setupWindowing (bool checked)
{
    if(!d->view_stacks.count())
        return;

    if(dtkAbstractView *view =  d->view_stacks.value(d->patientToolBox->patientIndex())->current()->current()->view()) {
        view->setProperty("LeftClickInteraction", "Windowing");
	view->update();
    }
}

void medViewerArea::setupZooming (bool checked)
{
    if(!d->view_stacks.count())
        return;
  
    if(dtkAbstractView *view =  d->view_stacks.value(d->patientToolBox->patientIndex())->current()->current()->view()) {
        view->setProperty("LeftClickInteraction", "Zooming");
	view->update();
    }
}

void medViewerArea::setupSlicing (bool checked)
{
    if(!d->view_stacks.count())
        return;
  
    if(dtkAbstractView *view =  d->view_stacks.value(d->patientToolBox->patientIndex())->current()->current()->view()) {
        view->setProperty("LeftClickInteraction", "Slicing");
	view->update();
    }
}

void medViewerArea::setupMeasuring (bool checked)
{
    if(!d->view_stacks.count())
        return;
  
    if(dtkAbstractView *view =  d->view_stacks.value(d->patientToolBox->patientIndex())->current()->current()->view()) {
        view->setProperty("LeftClickInteraction", "Measuring");
	view->update();
    }
}

void medViewerArea::setupCropping (bool checked)
{
    if(!d->view_stacks.count())
        return;
  
    if(dtkAbstractView *view =  d->view_stacks.value(d->patientToolBox->patientIndex())->current()->current()->view()) {
        checked ? view->setProperty("Cropping", "true") : view->setProperty("Cropping", "false");
	view->update();
    }
}

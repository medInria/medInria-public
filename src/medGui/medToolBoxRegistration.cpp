/* medToolBoxRegistration.cpp --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Fri Feb 19 09:06:02 2010 (+0100)
 * Version: $Id$
 * Last-Updated: Sat Mar  6 11:54:58 2010 (+0100)
 *           By: Julien Wintz
 *     Update #: 215
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#include "medDropSite.h"
#include "medToolBoxRegistration.h"

#include <dtkCore/dtkAbstractDataFactory.h>
#include <dtkCore/dtkAbstractData.h>
#include <dtkCore/dtkAbstractDataImage.h>
#include <dtkCore/dtkAbstractProcessFactory.h>
#include <dtkCore/dtkAbstractProcess.h>
#include <dtkCore/dtkAbstractViewFactory.h>
#include <dtkCore/dtkAbstractView.h>
#include <dtkCore/dtkAbstractViewInteractor.h>

#include <medCore/medDataManager.h>
#include <medCore/medViewManager.h>

#include <QtGui>

class medToolBoxRegistrationPrivate
{
public:
    medDropSite *processDropSiteFixed;
    medDropSite *processDropSiteMoving;
	
    QRadioButton *blendRadio;
    QRadioButton *checkerboardRadio;

    dtkAbstractView *fixedView;
    dtkAbstractView *movingView;
    dtkAbstractView *fuseView;

    dtkAbstractDataImage *fixedData;
    dtkAbstractDataImage *movingData;
};

medToolBoxRegistration::medToolBoxRegistration(QWidget *parent) : medToolBox(parent), d(new medToolBoxRegistrationPrivate)
{
    d->fuseView = dtkAbstractViewFactory::instance()->create("v3dView");
    if (d->fuseView)
        d->fuseView->enableInteractor("v3dViewFuseInteractor");
    
    d->fixedData  = 0;
    d->movingData = 0;
    d->fixedView  = 0;
    d->movingView = 0;
    
    // Process page

    QWidget *processPage = new QWidget;
    
    d->processDropSiteFixed  = new medDropSite(processPage);
    d->processDropSiteFixed->setText("Fixed");

    d->processDropSiteMoving = new medDropSite(processPage);
    d->processDropSiteMoving->setText("Moving");

    QHBoxLayout *processDropSiteLayout = new QHBoxLayout;
    processDropSiteLayout->addWidget(d->processDropSiteFixed);
    processDropSiteLayout->addWidget(d->processDropSiteMoving);

    QPushButton *processRunButton = new QPushButton("Run", processPage);

    QVBoxLayout *processLayout = new QVBoxLayout(processPage);
    processLayout->addLayout(processDropSiteLayout);
    processLayout->addWidget(processRunButton);

    connect(processRunButton, SIGNAL(clicked()), this, SLOT(run()));

    // Layout page

    QWidget *layoutPage = new QWidget;

    QPushButton *layoutButtonCompare = new QPushButton("Compare", layoutPage);
    layoutButtonCompare->setCheckable(true);
    layoutButtonCompare->setChecked(true);

    QPushButton *layoutButtonFuse = new QPushButton("Fuse", layoutPage);
    layoutButtonFuse->setCheckable(true);
    layoutButtonFuse->setChecked(false);

    QButtonGroup *layoutButtonGroup = new QButtonGroup(layoutPage);
    layoutButtonGroup->addButton(layoutButtonCompare);
    layoutButtonGroup->addButton(layoutButtonFuse);
    layoutButtonGroup->setExclusive(true);

    QSlider *layoutFuseSlider = new QSlider(Qt::Horizontal, layoutPage);
    layoutFuseSlider->setRange(1, 100);
    layoutFuseSlider->setValue(50);
    if (d->fuseView)
        if (dtkAbstractViewInteractor *interactor = d->fuseView->interactor("v3dViewFuseInteractor")) {
            connect(layoutFuseSlider, SIGNAL(valueChanged(int)), interactor, SLOT(onBlendAlphaValueSet(int)));
            connect(layoutFuseSlider, SIGNAL(valueChanged(int)), interactor, SLOT(onCheckerboardDivisionCountValueSet(int)));
        }
    
    d->blendRadio = new QRadioButton("Blend", layoutPage);
    d->checkerboardRadio = new QRadioButton("Checkerboard", layoutPage);
    d->blendRadio->setChecked(true);

    QButtonGroup *radioGroup = new QButtonGroup(this);
    radioGroup->addButton(d->blendRadio);
    radioGroup->addButton(d->checkerboardRadio);
    radioGroup->setExclusive(true);	

    QHBoxLayout *radioGroupLayout = new QHBoxLayout;
    radioGroupLayout->addWidget(d->blendRadio);
    radioGroupLayout->addWidget(d->checkerboardRadio);
    
    QHBoxLayout *layoutButtonLayout = new QHBoxLayout;
    layoutButtonLayout->addWidget(layoutButtonCompare);
    layoutButtonLayout->addWidget(layoutButtonFuse);

    QVBoxLayout *layoutLayout = new QVBoxLayout(layoutPage);
    layoutLayout->addLayout(layoutButtonLayout);
    layoutLayout->addLayout(radioGroupLayout);
    layoutLayout->addWidget(layoutFuseSlider);

    connect(layoutButtonCompare, SIGNAL(clicked()), this, SIGNAL(setupLayoutCompare()));
    connect(layoutButtonFuse, SIGNAL(clicked()), this, SIGNAL(setupLayoutFuse()));
    
    connect(d->blendRadio, SIGNAL(toggled(bool)), this, SLOT(onBlendModeSet(bool)));
    connect(d->checkerboardRadio, SIGNAL(toggled(bool)), this, SLOT(onCheckerboardModeSet(bool)));

    connect (d->processDropSiteFixed,  SIGNAL (objectDropped ()), this, SLOT (onFixedImageDropped ()));
    connect (d->processDropSiteMoving, SIGNAL (objectDropped ()), this, SLOT (onMovingImageDropped ()));
    
    // /////////////////////////////////////////////////////////////////
    // Setup
    // /////////////////////////////////////////////////////////////////

    medToolBoxTab *tab = new medToolBoxTab(this);
    tab->addTab(processPage, "Process");
    tab->addTab(layoutPage, "Layout");

    this->setTitle("Registration");
    this->setWidget(tab);
}

medToolBoxRegistration::~medToolBoxRegistration(void)
{
    delete d;

    d = NULL;
}

dtkAbstractView *medToolBoxRegistration::fuseView(void)
{
    return d->fuseView;
}

void medToolBoxRegistration::run(void)
{
    if (!d->fixedData || !d->movingData || !d->movingView)
        return;

    dtkAbstractProcess *process = dtkAbstractProcessFactory::instance()->create("itkProcessRegistration");
    if (!process)
        return;
    
    process->setInput(d->fixedData,  0);
    process->setInput(d->movingData, 1);
    process->run();
    
    dtkAbstractData *output = process->output();

    if(output) {
        d->movingView->setData(output);
	d->movingView->reset();
        d->movingView->update();
	
	if (d->fuseView) {
	    if (dtkAbstractViewInteractor *interactor = d->fuseView->interactor("v3dViewFuseInteractor")) {
		interactor->setData(output, 1);
		d->fuseView->reset();
		d->fuseView->update();
	    }
	}
    }
}

void medToolBoxRegistration::onBlendModeSet(bool value)
{
    if (value)
        if (d->fuseView)
            if (dtkAbstractViewInteractor *interactor = d->fuseView->interactor("v3dViewFuseInteractor")) {
                interactor->setProperty("FusionStyle", "blend");
                d->fuseView->update();
            }
}

void medToolBoxRegistration::onCheckerboardModeSet(bool value)
{
    if (value)
        if (d->fuseView)
            if (dtkAbstractViewInteractor *interactor = d->fuseView->interactor("v3dViewFuseInteractor")) {
                interactor->setProperty("FusionStyle", "checkerboard");
                d->fuseView->update();
            }
}

void medToolBoxRegistration::onFixedImageDropped (void)
{
    medDataIndex index = d->processDropSiteFixed->index();
  
    if (!index.isValid())
        return;

    d->fixedData = dynamic_cast<dtkAbstractDataImage*>( medDataManager::instance()->data(index) );
  
    if (!d->fixedData)
        return;

    d->fixedView = medViewManager::instance()->views(index).first();

    if(!d->fixedView) {
        qDebug() << "Unable to retrieve fixed view";
	return;
    }

    if (d->movingView)
        d->fixedView->link(d->movingView);

    if (d->fuseView)
        if (dtkAbstractViewInteractor *interactor = d->fuseView->interactor("v3dViewFuseInteractor")) {
	    interactor->setData(d->fixedData,  0);
	    if (d->movingData &&
		d->fixedData->xDimension()==d->movingData->xDimension() &&
		d->fixedData->yDimension()==d->movingData->yDimension() &&
		d->fixedData->zDimension()==d->movingData->zDimension()) {
	              interactor->setData(d->fixedData,   0);
		      interactor->setData(d->movingData,  1);
		      d->fuseView->reset();
		      d->fuseView->update();
	    }
	}
}

void medToolBoxRegistration::onMovingImageDropped (void)
{
    medDataIndex index = d->processDropSiteMoving->index();
  
    if (!index.isValid())
        return;

    d->movingData = dynamic_cast<dtkAbstractDataImage*>( medDataManager::instance()->data(index) );
  
    if (!d->movingData)
        return;

    d->movingView = medViewManager::instance()->views(d->processDropSiteMoving->index()).first();

    if(!d->movingView) {
        qDebug() << "Unable to retrieve moving view";
	return;
    }

    if (d->fixedView)
        d->fixedView->link(d->movingView);

    if (d->fuseView)
        if (dtkAbstractViewInteractor *interactor = d->fuseView->interactor("v3dViewFuseInteractor")) {
	    if (d->fixedData &&
		d->fixedData->xDimension()==d->movingData->xDimension() &&
		d->fixedData->yDimension()==d->movingData->yDimension() &&
		d->fixedData->zDimension()==d->movingData->zDimension()) {
	            interactor->setData(d->fixedData,   0);
	            interactor->setData(d->movingData,  1);
		    d->fuseView->reset();
		    d->fuseView->update();
	    }
	}
}

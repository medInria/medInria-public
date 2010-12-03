/* medToolBoxRegistration.cpp --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Fri Feb 19 09:06:02 2010 (+0100)
 * Version: $Id$
 * Last-Updated: Wed Nov 10 16:32:30 2010 (+0100)
 *           By: Julien Wintz
 *     Update #: 272
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#include "medDropSite.h"
#include "medToolBoxTab.h"
#include "medToolBoxRegistration.h"
#include "medToolBoxRegistrationCustom.h"

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

#include <medGui/medToolBoxFactory.h>

#include <QtGui>

class medToolBoxRegistrationPrivate
{
public:
    medDropSite *processDropSiteFixed;
    medDropSite *processDropSiteMoving;
	
    QRadioButton *blendRadio;
    QRadioButton *checkerboardRadio;
    QPushButton * saveImageButton;
    QPushButton * saveTransButton;
    QComboBox *toolboxes;
    dtkAbstractView *fixedView;
    dtkAbstractView *movingView;
    dtkAbstractView *fuseView;

    dtkAbstractDataImage *fixedData;
    dtkAbstractDataImage *movingData;

    medToolBoxRegistrationCustom * customToolBox;
};

medToolBoxRegistration::medToolBoxRegistration(QWidget *parent) : medToolBox(parent), d(new medToolBoxRegistrationPrivate)
{
    d->fuseView = dtkAbstractViewFactory::instance()->create("v3dView");

    if (d->fuseView)
        d->fuseView->enableInteractor("v3dViewFuseInteractor");
    
    d->fixedData  = NULL;
    d->movingData = NULL;
    d->fixedView  = NULL;
    d->movingView = NULL;
    
    // Process page

    QWidget *processPage = new QWidget(this);
    
    d->processDropSiteFixed  = new medDropSite(processPage);
    d->processDropSiteFixed->setText("Fixed");

    d->processDropSiteMoving = new medDropSite(processPage);
    d->processDropSiteMoving->setText("Moving");

    QHBoxLayout *processDropSiteLayout = new QHBoxLayout(this);
    processDropSiteLayout->addWidget(d->processDropSiteFixed);
    processDropSiteLayout->addWidget(d->processDropSiteMoving);

    d->saveImageButton = new QPushButton(tr("Save Image"),this);
    connect (d->saveImageButton, SIGNAL(clicked()), this, SLOT(onSaveImage()));
    d->saveTransButton = new QPushButton(tr("Save Transformation"),this);
    connect (d->saveTransButton, SIGNAL(clicked()), this, SLOT(onSaveTrans()));


    // --- Setting up custom toolboxes list ---

    d->toolboxes = new QComboBox(this);
    d->toolboxes->addItem("Choose algorithm");

    foreach(QString toolbox, medToolBoxFactory::instance()->registrationToolBoxes())
        d->toolboxes->addItem(toolbox, toolbox);

    connect(d->toolboxes, SIGNAL(activated(const QString&)), this, SLOT(onToolBoxChosen(const QString&)));

    // ---
    
    QVBoxLayout *processLayout = new QVBoxLayout(processPage);
    processLayout->addLayout(processDropSiteLayout);
    processLayout->addWidget(d->toolboxes);
    processLayout->addWidget(d->saveImageButton);
    processLayout->addWidget(d->saveTransButton);
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

    d->customToolBox = NULL;
}

medToolBoxRegistration::~medToolBoxRegistration(void)
{
    delete d;

    d = NULL;
}

dtkAbstractView *medToolBoxRegistration::fixedView(void)
{
    return d->fixedView;
}

dtkAbstractView *medToolBoxRegistration::movingView(void)
{
    return d->movingView;
}

dtkAbstractView *medToolBoxRegistration::fuseView(void)
{
    return d->fuseView;
}

dtkAbstractDataImage *medToolBoxRegistration::fixedData(void)
{
    return d->fixedData;
}

dtkAbstractDataImage *medToolBoxRegistration::movingData(void)
{
    return d->movingData;
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

    if (d->movingView) {
        d->fixedView->link(d->movingView);
	d->movingView->update();
    }

    if (d->fuseView)
        if (dtkAbstractViewInteractor *interactor = d->fuseView->interactor("v3dViewFuseInteractor")) {
	    if (d->movingData) {
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

    d->movingData = dynamic_cast<dtkAbstractDataImage*>(medDataManager::instance()->data(index));
  
    if (!d->movingData)
        return;

    d->movingView = medViewManager::instance()->views(d->processDropSiteMoving->index()).first();

    if(!d->movingView) {
        qDebug() << "Unable to retrieve moving view";
	return;
    }

    if (d->fixedView) {
        d->fixedView->link(d->movingView);
	d->movingView->update();
    }

    if (d->fuseView) {
        if (dtkAbstractViewInteractor *interactor = d->fuseView->interactor("v3dViewFuseInteractor")) {
	    if (d->fixedData) {
                interactor->setData(d->fixedData,   0);
                interactor->setData(d->movingData,  1);
                d->fuseView->reset();
                d->fuseView->update();
	    }
	}
    }
}

void medToolBoxRegistration::onToolBoxChosen(const QString& id)
{

    medToolBoxRegistrationCustom *toolbox = medToolBoxFactory::instance()->createCustomRegistrationToolBox(id);

    if(!toolbox) {
        qDebug() << "Unable to instanciate" << id << "toolbox";
        return;
    }

    toolbox->setRegistrationToolBox(this);
    //get rid of old toolBox
    if (d->customToolBox)
    {
        emit removeToolBox(d->customToolBox);
        delete d->customToolBox;
    }
    d->customToolBox = toolbox;
    emit addToolBox(toolbox);
}

void medToolBoxRegistration::onSaveImage()
{
    QString fileName = QFileDialog(this, tr("Save Image"),
                               QDir::homePath(),
                               tr("Nifty (*.nii);;Analyse (*.);;Nrrd (*.nrrd);;VTK (*.vtk);;All supported files ()"));
    qDebug() << fileName;

}

void medToolBoxRegistration::onSaveTrans()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save Transformation"),
                               QDir::homePath(),
                               tr("Transformation (*. *.xpm *.jpg)"));
    qDebug() << fileName;
}

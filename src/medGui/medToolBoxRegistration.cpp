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
#include <medCore/medMessageController.h>
#include <medGui/medToolBoxFactory.h>

#include <QtGui>

class medToolBoxRegistrationPrivate
{
public:
    medDropSite *processDropSiteFixed;
    medDropSite *processDropSiteMoving;
	
    QRadioButton *blendRadio;
    QRadioButton *checkerboardRadio;
    QSlider *layoutFuseSlider;

    QPushButton * saveImageButton;
    QPushButton * saveTransButton;
    QComboBox *toolboxes;
    dtkAbstractView *fixedView;
    dtkAbstractView *movingView;
    dtkAbstractView *fuseView;

    dtkAbstractDataImage *fixedData;
    dtkAbstractDataImage *movingData;

    dtkAbstractProcess * process;

    medToolBoxRegistrationCustom * customToolBox;
};

medToolBoxRegistration::medToolBoxRegistration(QWidget *parent) : medToolBox(parent), d(new medToolBoxRegistrationPrivate)
{
    d->fuseView = 0; 
    
    d->fixedData  = NULL;
    d->movingData = NULL;
    d->fixedView  = NULL;
    d->movingView = NULL;
    d->process = NULL;
    // Process page

    QWidget *processPage = new QWidget(this);
    
    d->processDropSiteFixed  = new medDropSite(processPage);
    d->processDropSiteFixed->setText("Fixed");

    d->processDropSiteMoving = new medDropSite(processPage);
    d->processDropSiteMoving->setText("Moving");

    QHBoxLayout *processDropSiteLayout = new QHBoxLayout;
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

    d->layoutFuseSlider = new QSlider(Qt::Horizontal, layoutPage);
    d->layoutFuseSlider->setRange(1, 100);
    d->layoutFuseSlider->setValue(50);

    
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
    layoutLayout->addWidget(d->layoutFuseSlider);

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
    tab->addTab(processPage, tr("Process"));
    tab->addTab(layoutPage, tr("Layout"));

    this->setTitle(tr("Registration"));
    this->setTabWidget(tab);

    d->customToolBox = NULL;

    //Connect Message Controller:
    connect(this,SIGNAL(showError(QObject*,const QString&,unsigned int)),
            medMessageController::instance(),SLOT(showError(QObject*,const QString&,unsigned int)));
    connect(this,SIGNAL(showInfo(QObject*,const QString&,unsigned int)),
            medMessageController::instance(),SLOT(showInfo(QObject*,const QString&,unsigned int)));
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

void medToolBoxRegistration::setFuseView(dtkAbstractView *view)
{
    if (!view)
        return;
    
    d->fuseView = view;
    d->fuseView->enableInteractor("v3dViewFuseInteractor");
    dtkAbstractViewInteractor *interactor = d->fuseView->interactor("v3dViewFuseInteractor");

    connect(d->layoutFuseSlider, SIGNAL(valueChanged(int)), interactor,
            SLOT(onBlendAlphaValueSet(int)));
    connect(d->layoutFuseSlider, SIGNAL(valueChanged(int)), interactor,
            SLOT(onCheckerboardDivisionCountValueSet(int)));


}

void medToolBoxRegistration::clear(void)
{
    d->processDropSiteFixed->clear();
    d->processDropSiteMoving->clear();
    //maybe clear the customtoolbox?
    if (d->customToolBox)
        d->customToolBox->clear();
}

dtkAbstractProcess * medToolBoxRegistration::process(void)
{
    return d->process;
}

void medToolBoxRegistration::setProcess(dtkAbstractProcess* proc)
{
    d->process = proc;
}

void medToolBoxRegistration::onSaveImage()
{
    if ( !d->movingData)
    {
        emit showError(this, tr  ("Please Select a moving image before saving"),3000);
        return;
    }
    if (!d->process )
    {
        emit showError(this, tr  ("Please run the registration before saving"),3000);
        return;
    }
    QFileDialog dialog(this, tr("Save Image"),
                               QDir::homePath(),
                               tr("MetaFile (*.mha *.mhd);;Nifty (*.nii);;Analyse (*.hdr);;Nrrd (*.nrrd);;VTK (*.vtk);;All supported files (*.mha *.mhd *.nii *.hdr *.nrrd)"));
    dialog.setDefaultSuffix("mha");
    dialog.setAcceptMode(QFileDialog::AcceptSave);
    QStringList fileName;
    if (dialog.exec())
        fileName = dialog.selectedFiles();

    qDebug() << fileName;

    if (!fileName.isEmpty())
    {
        //qDebug()<< (void *) d->movingData;
        //qDebug()<<  d->movingView->data();
        if (d->process->write(fileName[0]))
        {
            emit(showInfo(this, tr  ("Registered Image Saved"),3000));
        }
        else
        {
            emit(showError(this, tr  ("Image saving failed, no suitable writer found"),3000));
        }
    }

}

void medToolBoxRegistration::onSaveTrans()
{
    if (!d->movingData)
    {
        emit showError(this, tr  ("Please Select a moving image before saving"),3000);
        return;
    }
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save Transformation"),
                               QDir::homePath(),
                               tr("Transformation (*.txt)"));
    qDebug() << fileName;
}

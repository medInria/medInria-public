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

#include "medToolBoxRegistration.h"

#include <dtkCore/dtkAbstractDataFactory.h>
#include <dtkCore/dtkAbstractData.h>
#include <dtkCore/dtkAbstractProcessFactory.h>
#include <dtkCore/dtkAbstractProcess.h>
#include <dtkCore/dtkAbstractViewInteractor.h>

#include <medDataManager.h>
#include <medViewManager.h>
#include <medMessageController.h>
#include <medAbstractView.h>

#include <medAbstractDataImage.h>

#include <medToolBoxTab.h>
#include <medToolBoxFactory.h>

#include "medToolBoxRegistrationCustom.h"


#include <QtGui>

class medToolBoxRegistrationPrivate
{
public:
//    medDropSite *processDropSiteFixed;
//    medDropSite *processDropSiteMoving;

//    QRadioButton *blendRadio;
//    QRadioButton *checkerboardRadio;
//    QSlider *layoutFuseSlider;

    QPushButton * saveImageButton;
    QPushButton * saveTransButton;
    QComboBox *toolboxes;
    medAbstractView *fixedView;
    medAbstractView *movingView;
    medAbstractView *fuseView;

    medAbstractDataImage *fixedData;
    medAbstractDataImage *movingData;

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

    // Process section
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


    // Layout section

    QPushButton *layoutButtonCompare = new QPushButton("Compare", this);
    layoutButtonCompare->setCheckable(true);
    layoutButtonCompare->setChecked(true);

    QPushButton *layoutButtonFuse = new QPushButton("Fuse", this);
    layoutButtonFuse->setCheckable(true);
    layoutButtonFuse->setChecked(false);

    QButtonGroup *layoutButtonGroup = new QButtonGroup(this);
    layoutButtonGroup->addButton(layoutButtonCompare);
    layoutButtonGroup->addButton(layoutButtonFuse);
    layoutButtonGroup->setExclusive(true);

//    d->layoutFuseSlider = new QSlider(Qt::Horizontal, this);
//    d->layoutFuseSlider->setRange(1, 100);
//    d->layoutFuseSlider->setValue(50);

//    d->blendRadio = new QRadioButton("Blend", this);
//    d->checkerboardRadio = new QRadioButton("Checkerboard", this);
//    d->blendRadio->setChecked(true);

//    QButtonGroup *radioGroup = new QButtonGroup(this);
//    radioGroup->addButton(d->blendRadio);
//    radioGroup->addButton(d->checkerboardRadio);
//    radioGroup->setExclusive(true);

//    QHBoxLayout *radioGroupLayout = new QHBoxLayout;
//    radioGroupLayout->addWidget(d->blendRadio);
//    radioGroupLayout->addWidget(d->checkerboardRadio);

    QHBoxLayout *layoutButtonLayout = new QHBoxLayout;
    layoutButtonLayout->addWidget(layoutButtonCompare);
    layoutButtonLayout->addWidget(layoutButtonFuse);

    QVBoxLayout *layoutLayout = new QVBoxLayout;
    layoutLayout->addLayout(layoutButtonLayout);
//    layoutLayout->addLayout(radioGroupLayout);
//    layoutLayout->addWidget(d->layoutFuseSlider);
    QWidget * layoutSection = new QWidget(this);
    layoutSection->setLayout(layoutLayout);

    connect(layoutButtonCompare, SIGNAL(clicked()), this, SIGNAL(setupLayoutCompare()));
    connect(layoutButtonFuse, SIGNAL(clicked()), this, SIGNAL(setupLayoutFuse()));

//    connect(d->blendRadio, SIGNAL(toggled(bool)), this, SLOT(onBlendModeSet(bool)));
//    connect(d->checkerboardRadio, SIGNAL(toggled(bool)), this, SLOT(onCheckerboardModeSet(bool)));


    // /////////////////////////////////////////////////////////////////
    // Setup
    // /////////////////////////////////////////////////////////////////


    // ---
    addWidget(layoutSection);
    addWidget(d->toolboxes);
    addWidget(d->saveImageButton);
    addWidget(d->saveTransButton);


    this->setTitle(tr("Registration"));
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

medAbstractDataImage *medToolBoxRegistration::fixedData(void)
{
    return d->fixedData;
}

medAbstractDataImage *medToolBoxRegistration::movingData(void)
{
    return d->movingData;
}

//void medToolBoxRegistration::onBlendModeSet(bool value)
//{
//    if (value)
//        if (d->fuseView)
//            if (dtkAbstractViewInteractor *interactor = d->fuseView->interactor("v3dViewFuseInteractor")) {
//                interactor->setProperty("FusionStyle", "blend");
//                d->fuseView->update();
//            }
//}

//void medToolBoxRegistration::onCheckerboardModeSet(bool value)
//{
//    if (value)
//        if (d->fuseView)
//            if (dtkAbstractViewInteractor *interactor = d->fuseView->interactor("v3dViewFuseInteractor")) {
//                interactor->setProperty("FusionStyle", "checkerboard");
//                d->fuseView->update();
//            }
//}

void medToolBoxRegistration::onFixedImageDropped (const medDataIndex& index)
{

    if (!index.isValid())
        return;

    d->fixedData = dynamic_cast<medAbstractDataImage*>( medDataManager::instance()->data(index).data() );

    if (!d->fixedData)
        return;

    d->fixedView = dynamic_cast<medAbstractView*>
                   (medViewManager::instance()->views(index).first());

    if(!d->fixedView) {
        qDebug() << "Unable to retrieve fixed view";
	return;
    }


    if (d->fuseView)
    {
        if (d->movingView && d->fuseView->layerCount()==1)
        {
            //only the moving view has been set: shift it to layer 1
            d->fuseView->setData(d->fixedData,0);
            d->fuseView->setData(d->movingData,1);
        }
        else
        {
            //either both views are set, or only the fixed view
            d->fuseView->setData(d->fixedData,0);
        }
        d->fuseView->reset();
        d->fuseView->update();
    }
}

void medToolBoxRegistration::onMovingImageDropped (const medDataIndex& index)
{

    if (!index.isValid())
        return;

    d->movingData = dynamic_cast<medAbstractDataImage*>(medDataManager::instance()->data(index).data());

    if (!d->movingData)
        return;

    d->movingView = dynamic_cast<medAbstractView*>
                    (medViewManager::instance()->views
                     (index).first());

    if(!d->movingView) {
        qDebug() << "Unable to retrieve moving view";
	return;
    }

    if (d->fixedView) {
        d->movingView->update();
    }

//    if (d->fuseView) {
//        if (dtkAbstractViewInteractor *interactor = d->fuseView->interactor("v3dViewFuseInteractor")) {
//            if (d->fixedData) {
//                interactor->setData(d->fixedData,   0);
//                interactor->setData(d->movingData,  1);
//                d->fuseView->reset();
//                d->fuseView->update();
//            }
//        }
//    }
    if (d->fixedView)
    {
        //already one layer present
        d->fuseView->setData(d->movingData,1);
    }
    else
    {
        //only the moving view is set
        d->fuseView->setData(d->movingData,0);
    }
    //d->fuseView->reset();
    d->fuseView->update();
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
    toolbox->show();
    emit addToolBox(toolbox);

    connect (toolbox, SIGNAL (success()), this, SLOT (onSuccess()));
    connect (toolbox, SIGNAL (failure()), this, SIGNAL (failure()));
}

void medToolBoxRegistration::setFuseView(dtkAbstractView *view)
{
    if (!view)
        return;

    d->fuseView = dynamic_cast <medAbstractView*> (view);
//    d->fuseView->enableInteractor("v3dViewFuseInteractor");
//    dtkAbstractViewInteractor *interactor = d->fuseView->interactor("v3dViewFuseInteractor");

//    connect(d->layoutFuseSlider, SIGNAL(valueChanged(int)), interactor,
//            SLOT(onBlendAlphaValueSet(int)));
//    connect(d->layoutFuseSlider, SIGNAL(valueChanged(int)), interactor,
//            SLOT(onCheckerboardDivisionCountValueSet(int)));
}

void medToolBoxRegistration::clear(void)
{

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
    if (!d->process )
    {
        emit showError(this, tr  ("Please run the registration before saving"),3000);
        return;
    }
    QFileDialog dialog(this, tr("Save Transformation"),
                               QDir::homePath(),
                               tr("Transformation (*.txt);;MetaFile (*.mha *.mhd);;Nifty (*.nii);;Analyse (*.hdr);;Nrrd (*.nrrd);;VTK (*.vtk);;All supported files (*.mha *.mhd *.nii *.hdr *.nrrd)"));

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
        QStringList transformFileName;
        transformFileName << "" << fileName[0];
        if (d->process->write(transformFileName))
        {
            emit(showInfo(this, tr  ("Registered Image Saved"),3000));
        }
        else
        {
            emit(showError(this, tr  ("Image saving failed, no suitable writer found"),3000));
        }
    }
}


void medToolBoxRegistration::onSuccess()
{
    dtkAbstractData *output = d->process->output();

	if(output)
    {
	    d->movingView->setData(output,0);
	    d->fixedView->unlink (d->movingView);
	    d->fixedView->link (d->movingView);
	    d->movingView->update();
	    d->fuseView->setData(output,1);
        d->fuseView->update();

    }
}

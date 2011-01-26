/* medToolBoxDiffusion.cpp ---
 *
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Fri Feb 19 09:06:02 2010 (+0100)
 * Version: $Id$
 * Last-Updated: Wed Nov 10 11:02:58 2010 (+0100)
 *           By: Julien Wintz
 *     Update #: 126
 */

/* Commentary:
 *
 */

/* Change log:
 *
 */

#include <dtkCore/dtkAbstractData.h>
#include <dtkCore/dtkAbstractDataFactory.h>
#include <dtkCore/dtkAbstractProcess.h>
#include <dtkCore/dtkAbstractProcessFactory.h>

#include <medCore/medPluginManager.h>
#include <medCore/medDataManager.h>
#include <medCore/medViewManager.h>

#include <medSql/medDatabaseController.h>

#include <medGui/medDropSite.h>
#include <medGui/medToolBoxTab.h>
#include <medGui/medProgressionStack.h>
#include <medGui/medToolBoxFactory.h>

#include "medToolBoxDiffusion.h"
#include "medToolBoxDiffusionCustom.h"

class medToolBoxDiffusionPrivate
{
public:
    medDropSite         *tractographyDropSite;
    medProgressionStack *progression_stack;

    QComboBox   *coefficientsCombo;
    QPushButton *runButton;

    QHash<medDataIndex, dtkAbstractProcess *> activeMethods;

    medToolBoxDiffusionCustom * customToolBox;
};

medToolBoxDiffusion::medToolBoxDiffusion(QWidget *parent) : medToolBox(parent), d(new medToolBoxDiffusionPrivate)
{
    d->customToolBox = 0;
    
    // /////////////////////////////////////////////////////////////////
    // Tractography page
    // /////////////////////////////////////////////////////////////////
    QWidget   *tractographyPage        = new QWidget(this);
    QLabel    *tractographyMethodLabel = new QLabel("Algorithm:", tractographyPage);
    QComboBox *tractographyMethodCombo = new QComboBox(tractographyPage);

    d->tractographyDropSite = new medDropSite(tractographyPage);

    tractographyMethodCombo->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    d->progression_stack = new medProgressionStack(tractographyPage);
    QHBoxLayout *progressStackLayout = new QHBoxLayout;
    progressStackLayout->addWidget(d->progression_stack);

    QHBoxLayout *tractographyMethodLayout = new QHBoxLayout;
    tractographyMethodLayout->addWidget(tractographyMethodLabel);
    tractographyMethodLayout->addWidget(tractographyMethodCombo);

    QLabel *coefficientsLabel = new QLabel("Output:", tractographyPage);
    d->coefficientsCombo = new QComboBox(tractographyPage);
    d->coefficientsCombo->addItem("Apparent Diffusion Coefficient");
    d->coefficientsCombo->addItem("Fractional Anisotropy");
    d->coefficientsCombo->addItem("Relative Anisotropy");
    d->coefficientsCombo->addItem("Volume Ratio");
    d->coefficientsCombo->addItem("Linear Coefficient");
    d->coefficientsCombo->addItem("Planar Coefficient");
    d->coefficientsCombo->addItem("Spherical Coefficient");
    d->coefficientsCombo->addItem("Lambda 1");
    d->coefficientsCombo->addItem("Lambda 2");
    d->coefficientsCombo->addItem("Lambda 3");
    d->coefficientsCombo->addItem("Fibers");

    QHBoxLayout *coefficientsLayout = new QHBoxLayout;
    coefficientsLayout->addWidget(coefficientsLabel);
    coefficientsLayout->addWidget(d->coefficientsCombo);

    d->runButton = new QPushButton(tr("Run"), tractographyPage);
    
    QVBoxLayout *tractographyLayout = new QVBoxLayout(tractographyPage);
    tractographyLayout->addWidget(d->tractographyDropSite);
    tractographyLayout->addLayout(tractographyMethodLayout);
    //tractographyLayout->addWidget(tractographyRunButton);
    tractographyLayout->addLayout (coefficientsLayout);
    //tractographyLayout->addWidget(coefficientsButton);
    tractographyLayout->addWidget (d->runButton);
    tractographyLayout->addLayout (progressStackLayout);
    tractographyLayout->setAlignment(d->tractographyDropSite, Qt::AlignHCenter);

    // /////////////////////////////////////////////////////////////////
    // Setup
    // /////////////////////////////////////////////////////////////////
    // medToolBoxTab *tab = new medToolBoxTab(this);
    // tab->addTab(tractographyPage, "Tractography");

    // --- Setting up custom toolboxes list ---

    tractographyMethodCombo->addItem("Choose...");

    foreach(QString handler, medPluginManager::instance()->handlers("tractography"))
    {
        qDebug()<<handler;
        if (medToolBoxFactory::instance()->diffusionToolBoxes().contains(handler))
        tractographyMethodCombo->addItem(handler);
    }

    foreach(QString toolbox, medToolBoxFactory::instance()->diffusionToolBoxes())
        tractographyMethodCombo->addItem(toolbox, toolbox);

    connect(tractographyMethodCombo, SIGNAL(activated(const QString&)), this, SLOT(onToolBoxChosen(const QString&)));

    // ---

    connect (d->tractographyDropSite,  SIGNAL(objectDropped()),          this, SLOT (onObjectDropped()));
    connect (d->coefficientsCombo,     SIGNAL(activated(int)),           this, SLOT (onCoefficientsChanged(int)));
    connect (d->runButton,             SIGNAL(clicked()),                this, SLOT(run()));
    
    this->setTitle("Tractography");
    this->addWidget(tractographyPage);
}

medToolBoxDiffusion::~medToolBoxDiffusion(void)
{
    delete d;

    d = NULL;
}

void medToolBoxDiffusion::onObjectDropped()
{
    medDataIndex index = d->tractographyDropSite->index();

    if (!index.isValid())
        return;
    
    dtkAbstractData *data = medDataManager::instance()->data (index);
    if (!data) {
        data = medDatabaseController::instance()->read(index);
        if (data)
            medDataManager::instance()->insert(index, data);
    }

    if (!data)
        return;
    
    if (!data->hasMetaData ("DiffusionGradientList")) {
        QMessageBox msgBox;
        msgBox.setText("No diffusion gradient attached to the image.");
        msgBox.setInformativeText("Do you want to specify them manually?");
        msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
        msgBox.setDefaultButton(QMessageBox::Ok);
        int ret = msgBox.exec();

        switch (ret) {
        case QMessageBox::Ok:
        {
            QString fileName = QFileDialog::getOpenFileName(this, tr("Gradient File"), "", tr("Gradient files (*.*)"));

            if (fileName.isEmpty())
                return;

            dtkAbstractData *gradients=dtkAbstractDataFactory::instance()->create ("itkDataDiffusionGradientList");
            if (!gradients){
                // error
                return;
            }
            if (!gradients->read(fileName)) {
                // error
                return;
            }

            double *grad = (double *)(gradients->data(0));
            int i=0;
            QStringList gradientList;
            while (double *grad=(double *)(gradients->data(i))){
                QString s_gx, s_gy, s_gz;
                s_gx = QString::number (grad[0], 'g', 10);
                s_gy = QString::number (grad[1], 'g', 10);
                s_gz = QString::number (grad[2], 'g', 10);
                gradientList.append (s_gx);
                gradientList.append (s_gy);
                gradientList.append (s_gz);
                i++;
            }
            data->addMetaData ("DiffusionGradientList", gradientList);

        }

        break;
        case QMessageBox::Cancel:
            return;
        default:
            return;
        }
    }

    this->createProcessForIndex ( index );
}

void medToolBoxDiffusion::onCoefficientsChanged (int ind)
{
    foreach (dtkAbstractProcess *activeMethod, d->activeMethods) {
        this->setRequiredOutputForProcess (activeMethod, ind);
    }
}

void medToolBoxDiffusion::onToolBoxChosen(const QString & id)
{
    medToolBoxDiffusionCustom *toolbox = medToolBoxFactory::instance()->createCustomDiffusionToolBox(id);
    if(!toolbox) {
        //qDebug() << "Unable to instanciate" << id << "toolbox";
        return;
    }
    
    toolbox->setDiffusionToolBox(this);

    //get rid of old toolBox
    if (d->customToolBox) {
        emit removeToolBox(d->customToolBox);
        d->customToolBox->deleteLater();
    }
    d->customToolBox = toolbox;

    emit addToolBox(toolbox);

    medDataIndex index = d->tractographyDropSite->index();
    this->createProcessForIndex ( index );
}

void medToolBoxDiffusion::run()
{
    medDataIndex index = d->tractographyDropSite->index();

    if (!index.isValid())
        return;

    dtkAbstractProcess *activeMethod = 0;
    if (d->activeMethods.contains (index))
        activeMethod = d->activeMethods[index];

    if (!activeMethod) {
        return;
    }

    d->progression_stack->setLabel(activeMethod, "Progress:");

    if (activeMethod->run()==0)
        emit success();
    else
        emit failure();
}

dtkAbstractData *medToolBoxDiffusion::output(void)
{
    medDataIndex index = d->tractographyDropSite->index();
    
    dtkAbstractProcess *activeMethod = 0;
    if (d->activeMethods.contains (index))
        activeMethod = d->activeMethods[index];
    
    if (activeMethod)
        return activeMethod->output();

    return NULL;
}

void medToolBoxDiffusion::clear(void)
{
    d->tractographyDropSite->clear();
}

void medToolBoxDiffusion::createProcessForIndex (const medDataIndex &index)
{
    if (!index.isValid())
        return;
  
    if (!d->customToolBox)
        return;
  
    if (d->activeMethods.contains (index)) { // we should keep a copy of process / algorithm / index
        d->customToolBox->setProcess (d->activeMethods[index]);
        return;
        /*if (d->activeMethods[index])
	    delete d->activeMethods[index];
	d->activeMethods.remove (index);
	*/
    }
    
    if (dtkAbstractProcess *proc = d->customToolBox->create()) {
        d->activeMethods[index] = proc;
	connect (proc, SIGNAL (progressed (int)), d->progression_stack,  SLOT (setProgress (int)));
	connect (proc, SIGNAL (success()), d->progression_stack,  SLOT (onSuccess ()));
	connect (proc, SIGNAL (failure()), d->progression_stack,  SLOT (onFailure ()));

	dtkAbstractData *data = medDataManager::instance()->data (index);
	if (!data) {
	    data = medDatabaseController::instance()->read(index);
	    if (data)
	        medDataManager::instance()->insert(index, data);
	}
      
	if (data)
	    proc->setInput (data);

	this->setRequiredOutputForProcess (proc, d->coefficientsCombo->currentIndex());
    }
}

void medToolBoxDiffusion::setRequiredOutputForProcess (dtkAbstractProcess *proc, int type)
{
    if (!proc)
        return;

    if (type==0)
        proc->setProperty ("RequiredOutput", "ADC");
    else if (type==1)
        proc->setProperty ("RequiredOutput", "FA");
    else if (type==2)
        proc->setProperty ("RequiredOutput", "RA");
    else if (type==3)
        proc->setProperty ("RequiredOutput", "VR");
    else if (type==4)
        proc->setProperty ("RequiredOutput", "Cl");
    else if (type==5)
        proc->setProperty ("RequiredOutput", "Cp");
    else if (type==6)
        proc->setProperty ("RequiredOutput", "Cs");
    else if (type==7)
        proc->setProperty ("RequiredOutput", "Lambda1");
    else if (type==8)
        proc->setProperty ("RequiredOutput", "Lambda2");
    else if (type==9)
        proc->setProperty ("RequiredOutput", "Lambda3");
    else if (type==10)
        proc->setProperty ("RequiredOutput", "Fibers");
}

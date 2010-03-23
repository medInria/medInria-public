/* medToolBoxDiffusion.cpp --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Fri Feb 19 09:06:02 2010 (+0100)
 * Version: $Id$
 * Last-Updated: Mon Feb 22 21:35:18 2010 (+0100)
 *           By: Julien Wintz
 *     Update #: 105
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#include <medCore/medPluginManager.h>

#include "medDropSite.h"
#include "medToolBoxDiffusion.h"

#include "dtkCore/dtkAbstractData.h"
#include "dtkCore/dtkAbstractDataFactory.h"
#include "dtkCore/dtkAbstractView.h"
#include "dtkCore/dtkAbstractViewInteractor.h"
#include "dtkCore/dtkAbstractProcess.h"
#include "dtkCore/dtkAbstractProcessFactory.h"
#include <medSql/medDatabaseController.h>
#include <medCore/medDataManager.h>
#include <medCore/medViewManager.h>

#include <medGui/medProgressionStack.h>


class medToolBoxDiffusionPrivate
{
public:
  medDropSite         *tractographyDropSite;
  medProgressionStack *progression_stack;

  QComboBox *colorCombo;
  QCheckBox *displayCheckBox;
  QRadioButton *displayRadioPolylines;
  QRadioButton *displayRadioRibbons;
  QRadioButton *displayRadioTubes;
  QSlider      *radiusSlider;

  QListWidget *bundlingList;
  QPushButton *bundlingButtonVdt;
  QPushButton *bundlingButtonTag;
  QPushButton *bundlingButtonRst;

  QComboBox *coefficientsCombo;
  
  QList<dtkAbstractProcess *> methods;

  QHash<medDataIndex, dtkAbstractProcess*> activeMethods;

  dtkAbstractView *view;
};

medToolBoxDiffusion::medToolBoxDiffusion(QWidget *parent) : medToolBox(parent), d(new medToolBoxDiffusionPrivate)
{
    d->view = 0;

    
    // /////////////////////////////////////////////////////////////////
    // Display page
    // /////////////////////////////////////////////////////////////////
    QWidget *displayPage = new QWidget(this);

    QLabel *colorLabel = new QLabel("Color fibers by:", displayPage);

    d->colorCombo = new QComboBox(displayPage);
    d->colorCombo->addItem("Local orientation");
    d->colorCombo->addItem("Global orientation");
    d->colorCombo->addItem("Fractional anisotropy");

    QHBoxLayout *colorLayout = new QHBoxLayout;
    colorLayout->addWidget(colorLabel);
    colorLayout->addWidget(d->colorCombo);

    d->displayCheckBox = new QCheckBox("Use hardware acceleration", displayPage);

    d->displayRadioPolylines = new QRadioButton("Display fibers as polylines", displayPage);
    d->displayRadioRibbons = new QRadioButton("Display fibers as ribbons", displayPage);
    d->displayRadioTubes = new QRadioButton("Display fibers as tubes", displayPage);

    QButtonGroup *displayRadioGroup = new QButtonGroup(this);
    displayRadioGroup->addButton(d->displayRadioPolylines);
    displayRadioGroup->addButton(d->displayRadioRibbons);
    displayRadioGroup->addButton(d->displayRadioTubes);
    displayRadioGroup->setExclusive(true);

    QVBoxLayout *displayGroupLayout = new QVBoxLayout;
    displayGroupLayout->addWidget(d->displayRadioPolylines);
    displayGroupLayout->addWidget(d->displayRadioRibbons);
    displayGroupLayout->addWidget(d->displayRadioTubes);
    displayGroupLayout->addStretch(1);

    d->displayRadioPolylines->setChecked(true);

    QLabel *radiusLabel = new QLabel("Fibers radius:", displayPage);

    d->radiusSlider = new QSlider(Qt::Horizontal, displayPage);
    d->radiusSlider->setMinimum(1);
    d->radiusSlider->setMaximum(10);

    QHBoxLayout *radiusLayout = new QHBoxLayout;
    radiusLayout->addWidget(radiusLabel);
    radiusLayout->addWidget(d->radiusSlider);

    QVBoxLayout *displayLayout = new QVBoxLayout(displayPage);
    displayLayout->addLayout(colorLayout);
    displayLayout->addWidget(d->displayCheckBox);
    displayLayout->addLayout(displayGroupLayout);
    displayLayout->addLayout(radiusLayout);

    
    // /////////////////////////////////////////////////////////////////
    // Bundling page
    // /////////////////////////////////////////////////////////////////
    QWidget *bundlingPage = new QWidget(this);

    QCheckBox *bundleBoxCheckBox = new QCheckBox("Activate bundling box", bundlingPage);
    
    d->bundlingButtonTag = new QPushButton("Tag", bundlingPage);
    QPushButton *bundlingButtonAdd = new QPushButton("Add", bundlingPage);
    d->bundlingButtonRst = new QPushButton("Reset", bundlingPage);
    d->bundlingButtonVdt = new QPushButton("Validate", bundlingPage);

    QHBoxLayout *bundlingButtonsLayout = new QHBoxLayout;
    bundlingButtonsLayout->addWidget(d->bundlingButtonTag);
    bundlingButtonsLayout->addWidget(bundlingButtonAdd);
    bundlingButtonsLayout->addWidget(d->bundlingButtonRst);
    bundlingButtonsLayout->addWidget(d->bundlingButtonVdt);

    d->bundlingList = new QListWidget(bundlingPage);
    d->bundlingList->setAlternatingRowColors(true);

    QCheckBox *bundlingShowCheckBox = new QCheckBox("Show all bundles", bundlingPage);

    QVBoxLayout *bundlingLayout = new QVBoxLayout(bundlingPage);
    bundlingLayout->addWidget(bundleBoxCheckBox);
    bundlingLayout->addLayout(bundlingButtonsLayout);
    bundlingLayout->addWidget(d->bundlingList);
    bundlingLayout->addWidget(bundlingShowCheckBox);

    
    // /////////////////////////////////////////////////////////////////
    // Tractography page
    // /////////////////////////////////////////////////////////////////
    QWidget   *tractographyPage        = new QWidget(this);
    QLabel    *tractographyMethodLabel = new QLabel("Use:", tractographyPage);
    QComboBox *tractographyMethodCombo = new QComboBox(tractographyPage);

    d->tractographyDropSite = new medDropSite(tractographyPage);

    foreach(QString handler, medPluginManager::instance()->handlers("tractography"))
        tractographyMethodCombo->addItem(handler);
    tractographyMethodCombo->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    d->progression_stack = new medProgressionStack(tractographyPage);
    QHBoxLayout *progressStackLayout = new QHBoxLayout;
    progressStackLayout->addWidget(d->progression_stack);
    //QLabel *progressLabel = new QLabel("Progress:", tractographyPage);
    //progressStackLayout->addWidget(progressLabel);

    QHBoxLayout *tractographyMethodLayout = new QHBoxLayout;
    tractographyMethodLayout->addWidget(tractographyMethodLabel);
    tractographyMethodLayout->addWidget(tractographyMethodCombo);
    
    QPushButton *tractographyRunButton = new QPushButton("Run Tractography", tractographyPage);


    QLabel *coefficientsLabel = new QLabel("Coefficient:", tractographyPage);
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

    QHBoxLayout *coefficientsLayout = new QHBoxLayout;
    coefficientsLayout->addWidget(coefficientsLabel);
    coefficientsLayout->addWidget(d->coefficientsCombo);
    //QPushButton *coefficientsButton = new QPushButton("Calculate Coefficient", tractographyPage);

    
    /*
    QPushButton *faButton  = new QPushButton ("FA", tractographyPage);
    QPushButton *lfaButton = new QPushButton ("LFA", tractographyPage);
    QPushButton *raButton  = new QPushButton ("RA", tractographyPage);
    QPushButton *adcButton = new QPushButton ("ADC", tractographyPage);
    QPushButton *l1Button  = new QPushButton ("L1", tractographyPage);
    QPushButton *l2Button  = new QPushButton ("L2", tractographyPage);
    QPushButton *l3Button  = new QPushButton ("L3", tractographyPage);
    QPushButton *clButton  = new QPushButton ("Cl", tractographyPage);
    QPushButton *cpButton  = new QPushButton ("Cp", tractographyPage);
    QPushButton *csButton  = new QPushButton ("Cs", tractographyPage);
    QPushButton *vrButton  = new QPushButton ("VR", tractographyPage);

    QHBoxLayout *coefficientsLayout1 = new QHBoxLayout;
    coefficientsLayout1->addWidget (faButton);
    coefficientsLayout1->addWidget (lfaButton);
    coefficientsLayout1->addWidget (raButton);
    coefficientsLayout1->addWidget (adcButton);

    QHBoxLayout *coefficientsLayout2 = new QHBoxLayout;
    coefficientsLayout2->addWidget (vrButton);
    coefficientsLayout2->addWidget (l1Button);
    coefficientsLayout2->addWidget (l2Button);
    coefficientsLayout2->addWidget (l3Button);

    QHBoxLayout *coefficientsLayout3 = new QHBoxLayout;
    coefficientsLayout3->addWidget (clButton);
    coefficientsLayout3->addWidget (cpButton);
    coefficientsLayout3->addWidget (csButton);

    QVBoxLayout *coefficientsLayout = new QVBoxLayout;
    coefficientsLayout->addLayout (coefficientsLayout1);
    coefficientsLayout->addLayout (coefficientsLayout2);
    coefficientsLayout->addLayout (coefficientsLayout3);
    */
    
    QVBoxLayout *tractographyLayout = new QVBoxLayout(tractographyPage);
    tractographyLayout->addWidget(d->tractographyDropSite);
    tractographyLayout->addLayout(tractographyMethodLayout);
    tractographyLayout->addWidget(tractographyRunButton);
    tractographyLayout->addLayout (coefficientsLayout);
    //tractographyLayout->addWidget(coefficientsButton);
    tractographyLayout->addLayout(progressStackLayout);
    tractographyLayout->setAlignment(d->tractographyDropSite, Qt::AlignHCenter);




    // /////////////////////////////////////////////////////////////////
    // Setup
    // /////////////////////////////////////////////////////////////////
    medToolBoxTab *tab = new medToolBoxTab(this);
    tab->addTab(displayPage, "Display");
    tab->addTab(bundlingPage, "Bundling");
    tab->addTab(tractographyPage, "Tractography"); 

    if (dtkAbstractProcess *proc = dtkAbstractProcessFactory::instance()->create ("itkProcessTensorDTITrackPipeline")) {
      tractographyMethodCombo->addItem( proc->description() );
      d->methods.append ( proc );
    }

    connect (d->tractographyDropSite,  SIGNAL(objectDropped()),          this, SLOT (onObjectDropped()));
    connect (tractographyRunButton,    SIGNAL(clicked()),                this, SLOT (run()) );
    connect (d->colorCombo,            SIGNAL(currentIndexChanged(int)), this, SLOT (onColorModeChanged (int)));
    connect (d->displayCheckBox,       SIGNAL(stateChanged(int)),        this, SLOT (onGPUActivated (int)));
    connect (d->displayRadioPolylines, SIGNAL(toggled(bool)),            this, SLOT (onLinesRenderingModeSelected (bool)));
    connect (d->displayRadioRibbons,   SIGNAL(toggled(bool)),            this, SLOT (onRibbonsRenderingModeSelected (bool)));
    connect (d->displayRadioTubes,     SIGNAL(toggled(bool)),            this, SLOT (onTubesRenderingModeSelected (bool)));
    connect (d->coefficientsCombo,     SIGNAL(activated(int)),           this, SLOT(onCoefficientsChanged(int)));
    
    connect (bundleBoxCheckBox, SIGNAL (stateChanged(int)), this, SLOT (onBundlingBoxActivated (int)));

    this->setTitle("Diffusion");
    this->setWidget(tab);
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

    if (!medViewManager::instance()->views(index).isEmpty() ) {
        dtkAbstractView *view = medViewManager::instance()->views(index).first();
	if (view && view!=d->view) {
	    if (d->view)
	        if (dtkAbstractViewInteractor *interactor = d->view->interactor ("v3dViewFiberInteractor")) {
		  //disconnect (d->radiusSlider,      SIGNAL(valueChanged(int)),           interactor, SLOT (onRadiusSet(int)));
		  disconnect (d->bundlingButtonVdt, SIGNAL (clicked(void)),              interactor, SLOT (onSelectionValidated (void)));
		  disconnect (d->bundlingButtonTag, SIGNAL(clicked(void)),               interactor, SLOT (onSelectionTagged(void)));
		  disconnect (d->bundlingButtonRst, SIGNAL(clicked(void)),               interactor, SLOT (onSelectionReset(void)));
		  //disconnect (interactor,           SIGNAL(selectionValidated(QString)), this,       SLOT (onBundleValidated(QString)));
		}
	    
	    view->enableInteractor ("v3dViewFiberInteractor");
	    dtkAbstractViewInteractor *interactor = view->interactor ("v3dViewFiberInteractor");
	    
	    if (!interactor) {
	        qDebug() << "Cannot enable interactor: v3dViewFiberInteractor";
	    }
	    
	    d->view = view;
	    
	    connect (d->radiusSlider,      SIGNAL(valueChanged(int)),           interactor, SLOT (onRadiusSet(int)));
	    connect (d->bundlingButtonVdt, SIGNAL (clicked(void)),              interactor, SLOT (onSelectionValidated (void)));
	    connect (d->bundlingButtonTag, SIGNAL(clicked(void)),               interactor, SLOT (onSelectionTagged(void)));
	    connect (d->bundlingButtonRst, SIGNAL(clicked(void)),               interactor, SLOT (onSelectionReset(void)));
	    connect (interactor,           SIGNAL(selectionValidated(QString)), this,       SLOT (onBundleValidated(QString)));
	}
    }
    
    if (!d->activeMethods.contains(index)) {
        dtkAbstractProcess *proc = dtkAbstractProcessFactory::instance()->create ("itkProcessTensorDTITrackPipeline");
	proc->setInput (data);
	connect (proc, SIGNAL (progressed (int)), d->progression_stack,  SLOT (setProgress (int)));
        d->activeMethods.insert(index, proc);
    }
}

void medToolBoxDiffusion::run (void)
{
    medDataIndex index = d->tractographyDropSite->index();

    dtkAbstractProcess *activeMethod = 0;
    if (d->activeMethods.contains (index))
        activeMethod = d->activeMethods[index];

    if (!activeMethod)
        return;

    activeMethod->setProperty ("RequiredOutput", "Fibers");

    d->progression_stack->setLabel(activeMethod, "Progress:");
    
    if (activeMethod->run()==0 )
      if (d->view)
	if (dtkAbstractViewInteractor *interactor = d->view->interactor ("v3dViewFiberInteractor")) {
	  d->view->setData ( activeMethod->output() );
	  d->view->update();
	}
}


void medToolBoxDiffusion::update (dtkAbstractView *view)
{
  /*
  if (!view || view==d->view)
    return;

  if (d->view)
    if (dtkAbstractViewInteractor *interactor = d->view->interactor ("v3dViewFiberInteractor")) {
      disconnect (d->radiusSlider,      SIGNAL(valueChanged(int)),           interactor, SLOT (onRadiusSet(int)));
      disconnect (d->bundlingButtonVdt, SIGNAL (clicked(void)),              interactor, SLOT (onSelectionValidated (void)));
      disconnect (d->bundlingButtonTag, SIGNAL(clicked(void)),               interactor, SLOT (onSelectionTagged(void)));
      disconnect (d->bundlingButtonRst, SIGNAL(clicked(void)),               interactor, SLOT (onSelectionReset(void)));
      disconnect (interactor,           SIGNAL(selectionValidated(QString)), this,       SLOT (onBundleValidated(QString)));
    }

  view->enableInteractor ("v3dViewFiberInteractor");
  dtkAbstractViewInteractor *interactor = view->interactor ("v3dViewFiberInteractor");

  if (!interactor) {
    qDebug() << "Cannot enable interactor: v3dViewFiberInteractor";
    return;
  }
  
  d->view = view;

  connect (d->radiusSlider,      SIGNAL(valueChanged(int)),           interactor, SLOT (onRadiusSet(int)));
  connect (d->bundlingButtonVdt, SIGNAL (clicked(void)),              interactor, SLOT (onSelectionValidated (void)));
  connect (d->bundlingButtonTag, SIGNAL(clicked(void)),               interactor, SLOT (onSelectionTagged(void)));
  connect (d->bundlingButtonRst, SIGNAL(clicked(void)),               interactor, SLOT (onSelectionReset(void)));
  connect (interactor,           SIGNAL(selectionValidated(QString)), this,       SLOT (onBundleValidated(QString)));
  */
}

void medToolBoxDiffusion::onColorModeChanged (int index)
{
  if (!d->view)
    return;

  if (dtkAbstractViewInteractor *interactor = d->view->interactor ("v3dViewFiberInteractor")) {
    if (index==0)
      interactor->setProperty("ColorMode","local");
    if (index==1)
      interactor->setProperty("ColorMode","global");
    if (index==2)
      interactor->setProperty("ColorMode","fa");

    d->view->update();
  }
}

void medToolBoxDiffusion::onGPUActivated (int value)
{
  if (!d->view)
    return;

  if (dtkAbstractViewInteractor *interactor = d->view->interactor ("v3dViewFiberInteractor")) {
    if (value)
      interactor->setProperty ("GPUMode", "true");
    else
      interactor->setProperty ("GPUMode", "false");

    d->view->update();
  }
}

void medToolBoxDiffusion::onLinesRenderingModeSelected (bool value)
{
  if (!d->view)
    return;
  
  if (value)
    if(dtkAbstractViewInteractor *interactor = d->view->interactor ("v3dViewFiberInteractor")) {
      interactor->setProperty ("RenderingMode", "lines");
      
      d->view->update();
    }
}

void medToolBoxDiffusion::onRibbonsRenderingModeSelected (bool value)
{
    if (!d->view)
    return;
  
    if (value)
      if(dtkAbstractViewInteractor *interactor = d->view->interactor ("v3dViewFiberInteractor")) {
	interactor->setProperty ("RenderingMode", "ribbons");
	
	d->view->update();
      }
}

void medToolBoxDiffusion::onTubesRenderingModeSelected (bool value)
{
  if (!d->view)
    return;
  
  if (value)
    if(dtkAbstractViewInteractor *interactor = d->view->interactor ("v3dViewFiberInteractor")) {
      interactor->setProperty ("RenderingMode", "tubes");

      d->view->update();
    }
}

void medToolBoxDiffusion::onBundlingBoxActivated (int value)
{
  if (!d->view)
    return;

  if(dtkAbstractViewInteractor *interactor = d->view->interactor ("v3dViewFiberInteractor")) {
    if (value)
      interactor->setProperty ("BoxVisibility", "true");
    else
      interactor->setProperty ("BoxVisibility", "false");

      d->view->update();
    }
}

void medToolBoxDiffusion::onBundleValidated (QString name)
{
    d->bundlingList->addItem (name);
}

void medToolBoxDiffusion::onCoefficientsChanged (int ind)
{
    medDataIndex index = d->tractographyDropSite->index();

    dtkAbstractProcess *activeMethod = 0;
    if (d->activeMethods.contains (index))
        activeMethod = d->activeMethods[index];

    if (!activeMethod)
        return;

    if (ind==0)
        activeMethod->setProperty ("RequiredOutput", "ADC");
    else if (ind==1)
        activeMethod->setProperty ("RequiredOutput", "FA");
    else if (ind==2)
        activeMethod->setProperty ("RequiredOutput", "RA");
    else if (ind==3)
        activeMethod->setProperty ("RequiredOutput", "VR");
    else if (ind==4)
        activeMethod->setProperty ("RequiredOutput", "Cl");
    else if (ind==5)
        activeMethod->setProperty ("RequiredOutput", "Cp");
    else if (ind==6)
        activeMethod->setProperty ("RequiredOutput", "Cs");
    else if (ind==7)
        activeMethod->setProperty ("RequiredOutput", "Lambda1");
    else if (ind==8)
        activeMethod->setProperty ("RequiredOutput", "Lambda2");
    else if (ind==9)
        activeMethod->setProperty ("RequiredOutput", "Lambda3");

    d->progression_stack->setLabel(activeMethod, "Progress:");
    
    if (activeMethod->run()==0 )
      if (d->view) {
	  d->view->setData ( activeMethod->output() );
	  d->view->reset();
	  d->view->update();
      }
}

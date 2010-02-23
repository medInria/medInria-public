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
#include "dtkCore/dtkAbstractProcess.h"
#include "dtkCore/dtkAbstractProcessFactory.h"
#include <medSql/medDatabaseController.h>
#include <medCore/medDataManager.h>

class medToolBoxDiffusionPrivate
{
public:
  medDropSite *tractographyDropSite;
  
  QList<dtkAbstractProcess *> methods;
  dtkAbstractProcess *activeMethod;
};

medToolBoxDiffusion::medToolBoxDiffusion(QWidget *parent) : medToolBox(parent), d(new medToolBoxDiffusionPrivate)
{
    d->activeMethod = 0;
  
    // /////////////////////////////////////////////////////////////////
    // Display page
    // /////////////////////////////////////////////////////////////////

    QWidget *displayPage = new QWidget(this);

    QLabel *colorLabel = new QLabel("Color fibers by:", displayPage);

    QComboBox *colorCombo = new QComboBox(displayPage);
    colorCombo->addItem("Local orientation");
    colorCombo->addItem("Global orientation");
    colorCombo->addItem("Fractional anisotropy");

    QHBoxLayout *colorLayout = new QHBoxLayout;
    colorLayout->addWidget(colorLabel);
    colorLayout->addWidget(colorCombo);

    QCheckBox *displayCheckBox = new QCheckBox("Use hardware acceleration", displayPage);

    QRadioButton *displayRadioPolylines = new QRadioButton("Display fibers as polylines", displayPage);
    QRadioButton *displayRadioRibbons = new QRadioButton("Display fibers as ribbons", displayPage);
    QRadioButton *displayRadioTubes = new QRadioButton("Display fibers as tubes", displayPage);

    QButtonGroup *displayRadioGroup = new QButtonGroup(this);
    displayRadioGroup->addButton(displayRadioPolylines);
    displayRadioGroup->addButton(displayRadioRibbons);
    displayRadioGroup->addButton(displayRadioTubes);
    displayRadioGroup->setExclusive(true);

    QVBoxLayout *displayGroupLayout = new QVBoxLayout;
    displayGroupLayout->addWidget(displayRadioPolylines);
    displayGroupLayout->addWidget(displayRadioRibbons);
    displayGroupLayout->addWidget(displayRadioTubes);
    displayGroupLayout->addStretch(1);

    displayRadioPolylines->setChecked(true);

    QLabel *radiusLabel = new QLabel("Fibers radius:", displayPage);

    QSlider *radiusSlider = new QSlider(Qt::Horizontal, displayPage);
    radiusSlider->setMinimum(0);
    radiusSlider->setMaximum(100);

    QHBoxLayout *radiusLayout = new QHBoxLayout;
    radiusLayout->addWidget(radiusLabel);
    radiusLayout->addWidget(radiusSlider);

    QVBoxLayout *displayLayout = new QVBoxLayout(displayPage);
    displayLayout->addLayout(colorLayout);
    displayLayout->addWidget(displayCheckBox);
    displayLayout->addLayout(displayGroupLayout);
    displayLayout->addLayout(radiusLayout);

    // /////////////////////////////////////////////////////////////////
    // Bundling page
    // /////////////////////////////////////////////////////////////////

    QWidget *bundlingPage = new QWidget(this);

    QPushButton *bundlingButtonTag = new QPushButton("Tag", bundlingPage);
    QPushButton *bundlingButtonAdd = new QPushButton("Add", bundlingPage);
    QPushButton *bundlingButtonRst = new QPushButton("Reset", bundlingPage);
    QPushButton *bundlingButtonVdt = new QPushButton("Validate", bundlingPage);

    QHBoxLayout *bundlingButtonsLayout = new QHBoxLayout;
    bundlingButtonsLayout->addWidget(bundlingButtonTag);
    bundlingButtonsLayout->addWidget(bundlingButtonAdd);
    bundlingButtonsLayout->addWidget(bundlingButtonRst);
    bundlingButtonsLayout->addWidget(bundlingButtonVdt);

    QListWidget *bundlingList = new QListWidget(bundlingPage);
    bundlingList->setAlternatingRowColors(true);
    bundlingList->addItem("Bundle 1");
    bundlingList->addItem("Bundle 2");
    bundlingList->addItem("Bundle 3");

    QCheckBox *bundlingShowCheckBox = new QCheckBox("Show all bundles", bundlingPage);

    QVBoxLayout *bundlingLayout = new QVBoxLayout(bundlingPage);
    bundlingLayout->addLayout(bundlingButtonsLayout);
    bundlingLayout->addWidget(bundlingList);
    bundlingLayout->addWidget(bundlingShowCheckBox);

    // /////////////////////////////////////////////////////////////////
    // Tractography page
    // /////////////////////////////////////////////////////////////////

    QWidget *tractographyPage = new QWidget(this);

    QLabel *tractographyMethodLabel = new QLabel("Use:", tractographyPage);

    QComboBox *tractographyMethodCombo = new QComboBox(tractographyPage);

    foreach(QString handler, medPluginManager::instance()->handlers("tractography"))
        tractographyMethodCombo->addItem(handler);

    QHBoxLayout *tractographyMethodLayout = new QHBoxLayout;
    tractographyMethodLayout->addWidget(tractographyMethodLabel);
    tractographyMethodLayout->addWidget(tractographyMethodCombo);

    // QTextEdit *tractographyMethodEdit = new QTextEdit(tractographyPage);
    // tractographyMethodEdit->setReadOnly(true);
    // tractographyMethodEdit->setMaximumHeight(100);

    d->tractographyDropSite = new medDropSite(tractographyPage);

    QPushButton *tractographyRunButton = new QPushButton("Run", tractographyPage);

    QVBoxLayout *tractographyLayout = new QVBoxLayout(tractographyPage);
    tractographyLayout->addLayout(tractographyMethodLayout);
    // tractographyLayout->addWidget(tractographyMethodEdit);
    tractographyLayout->addWidget(d->tractographyDropSite);
    tractographyLayout->addWidget(tractographyRunButton);
    tractographyLayout->setAlignment(d->tractographyDropSite, Qt::AlignHCenter);

    // /////////////////////////////////////////////////////////////////
    // Setup
    // /////////////////////////////////////////////////////////////////

    medToolBoxTab *tab = new medToolBoxTab(this);
    tab->addTab(displayPage, "Display");
    tab->addTab(bundlingPage, "Bundling");
    tab->addTab(tractographyPage, "Tractography");


    connect (tractographyRunButton, SIGNAL (clicked()), this, SLOT (run()) );
    

    if (dtkAbstractProcess *proc = dtkAbstractProcessFactory::instance()->create ("itkProcessTensorDTITrackPipeline")) {
      tractographyMethodCombo->addItem( proc->description() );
      d->methods.append ( proc );
      d->activeMethod = proc;
      connect (d->activeMethod, SIGNAL (progressed (int)), this,  SLOT (printProgress (int)));
    }
    
    

    this->setTitle("Diffusion");
    this->setWidget(tab);
}

medToolBoxDiffusion::~medToolBoxDiffusion(void)
{
    delete d;

    d = NULL;
}


void medToolBoxDiffusion::run (void)
{

  medDataIndex index = d->tractographyDropSite->index();
  
  if (!index.isValid())
      return;
  
  if (d->activeMethod) {
    
    dtkAbstractData *data = medDataManager::instance()->data (index);
    
    if (!data) {
        data = medDatabaseController::instance()->read(index);
	if (data)
	    medDataManager::instance()->insert(index, data);
    }
    
    if (data) {

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
		  QString fileName = QFileDialog::getOpenFileName(this,
								  tr("Gradient File"), "", tr("Gradient files (*.*)"));

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
	
        d->activeMethod->setInput (data);
	d->activeMethod->run();
    }
  }
}


void medToolBoxDiffusion::update (dtkAbstractView *view)
{
  if (!view)
    return;
  
  dtkAbstractViewInteractor *interactor = view->interactor ("v3dViewFiberInteractor");
  if (interactor)
    qDebug() << "Yiarghhh!";

  if (d->activeMethod) {

    medDataIndex index (1,1,1,-1);
    if(!index.isValid())
        return;

    dtkAbstractData *data = medDataManager::instance()->data (index);
    if (data)
      qDebug() << "no need to read!";
    
    if (!data) {
        data = medDatabaseController::instance()->read(index);
	if (data)
	  medDataManager::instance()->insert(index, data);
    }
    
    if (data) {        
        d->activeMethod->setInput (data);
        qDebug() << "sboooon!";
    }
  }
}


void medToolBoxDiffusion::printProgress (int value)
{
  qDebug() << value;
}

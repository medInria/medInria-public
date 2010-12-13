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
#include <dtkCore/dtkAbstractView.h>
#include <dtkCore/dtkAbstractViewInteractor.h>
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

    // bundler widgets
    QComboBox    *colorCombo;
    QCheckBox    *displayCheckBox;
    QRadioButton *displayRadioPolylines;
    QRadioButton *displayRadioRibbons;
    QRadioButton *displayRadioTubes;
    QSlider      *radiusSlider;
    QListWidget  *bundlingList;
    QPushButton  *bundlingButtonVdt;
    QPushButton  *bundlingButtonTag;
    QPushButton  *bundlingButtonRst;

    QComboBox *coefficientsCombo;

    QList<dtkAbstractProcess *>              methods;
    QHash<medDataIndex, dtkAbstractProcess*> activeMethods;

    medDataIndex currentIndex;

    medToolBoxDiffusionCustom * customToolBox;
};

medToolBoxDiffusion::medToolBoxDiffusion(QWidget *parent) : medToolBox(parent), d(new medToolBoxDiffusionPrivate)
{
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


    tractographyMethodCombo->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    d->progression_stack = new medProgressionStack(tractographyPage);
    QHBoxLayout *progressStackLayout = new QHBoxLayout;
    progressStackLayout->addWidget(d->progression_stack);
    //QLabel *progressLabel = new QLabel("Progress:", tractographyPage);
    //progressStackLayout->addWidget(progressLabel);

    QHBoxLayout *tractographyMethodLayout = new QHBoxLayout;
    tractographyMethodLayout->addWidget(tractographyMethodLabel);
    tractographyMethodLayout->addWidget(tractographyMethodCombo);

    //QPushButton *tractographyRunButton = new QPushButton("Run Tractography", tractographyPage);


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
    d->coefficientsCombo->addItem("Fibers");

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
    //tractographyLayout->addWidget(tractographyRunButton);
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

    // --- Setting up custom toolboxes list ---

    tractographyMethodCombo->addItem("Choose algorithm");

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
    connect (d->bundlingButtonVdt,     SIGNAL(clicked(void)),            this, SLOT (onBundlingButtonVdtClicked (void)));
	
    connect (bundlingShowCheckBox,     SIGNAL(toggled(bool)),            this, SIGNAL (showBundles (bool)));
    connect (d->colorCombo,            SIGNAL(currentIndexChanged(int)), this, SIGNAL (fiberColorModeChanged (int)));
    connect (d->displayCheckBox,       SIGNAL(toggled(bool)),            this, SIGNAL (GPUActivated (bool)));
    connect (d->displayRadioPolylines, SIGNAL(toggled(bool)),            this, SIGNAL (lineModeSelected (bool)));
    connect (d->displayRadioRibbons,   SIGNAL(toggled(bool)),            this, SIGNAL (ribbonModeSelected (bool)));
    connect (d->displayRadioTubes,     SIGNAL(toggled(bool)),            this, SIGNAL (tubeModeSelected (bool)));
    connect (bundleBoxCheckBox,        SIGNAL(toggled(bool)),            this, SIGNAL (bundlingBoxActivated (bool)));
    connect (d->radiusSlider,          SIGNAL(valueChanged(int)),        this, SIGNAL (fiberRadiusSet(int)));
    connect (d->bundlingButtonTag,     SIGNAL(clicked(void)),            this, SIGNAL (fiberSelectionTagged(void)));
    connect (d->bundlingButtonRst,     SIGNAL(clicked(void)),            this, SIGNAL (fiberSelectionReset(void)));

    d->customToolBox = 0;
    
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
    
    d->currentIndex = index;

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

    if (d->activeMethods.contains(index)) {
        // TODO: clear bundle list and fill it with output of appropriate tracto method
    }
    else {
        dtkAbstractProcess *proc = dtkAbstractProcessFactory::instance()->create ("itkProcessTensorDTITrackPipeline");
        proc->setInput (data);
        connect (proc, SIGNAL (progressed (int)), d->progression_stack,  SLOT (setProgress (int)));
        d->activeMethods.insert(index, proc);
    }
}

void medToolBoxDiffusion::addBundle (QString name)
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
    else if (ind==10)
        activeMethod->setProperty ("RequiredOutput", "Fibers");

    // d->progression_stack->setLabel(activeMethod, "Progress:");

    /*
    if (activeMethod->run()==0 )
        if (d->view) {
            d->view->setData ( activeMethod->output() );
            d->view->reset();
            d->view->update();
        }
     */
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
    if (d->customToolBox)
    {
        emit removeToolBox(d->customToolBox);
        delete d->customToolBox;
    }
    d->customToolBox = toolbox;

    emit addToolBox(toolbox);
    //TODO refactor methods and active methods (here take care of existing methods...)
    if (dtkAbstractProcess *proc = dtkAbstractProcessFactory::instance()->create (id)) {
        proc->setProperty ("RequiredOutput", "ADC");
        d->methods.append( proc );
    }
}

void medToolBoxDiffusion::run()
{
    medDataIndex index = d->tractographyDropSite->index();

    dtkAbstractProcess *activeMethod = 0;
    if (d->activeMethods.contains (index))
        activeMethod = d->activeMethods[index];

    if (!activeMethod) {
        return;
    }

    // activeMethod->setProperty ("RequiredOutput", "Fibers");

    d->progression_stack->setLabel(activeMethod, "Progress:");

    if (activeMethod->run()==0) {
        emit success();
    }
    else {
        emit failure();
    }

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
    d->bundlingList->clear();
    d->currentIndex = medDataIndex();
}

void medToolBoxDiffusion::onBundlingButtonVdtClicked (void)
{
    bool ok;
    QString text = QInputDialog::getText(this, tr("Enter bundle name"),
                                        tr(""), QLineEdit::Normal, tr(""), &ok);

    if (ok && !text.isEmpty()) {
      // if (!d->bundlingList->contains (name)) // should popup a warning
      // d->bundlingList->addItem (text);
	addBundle (text);
	emit fiberSelectionValidated (text);
    }
}

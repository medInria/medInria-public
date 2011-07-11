#include "medToolBoxDiffusionFiberBundling.h"

#include <dtkCore/dtkAbstractData.h>
#include <dtkCore/dtkAbstractDataFactory.h>
#include <dtkCore/dtkAbstractView.h>
#include <dtkCore/dtkAbstractViewInteractor.h>

#include <medDataManager.h>
#include <medAbstractDbController.h>
#include <medDbControllerFactory.h>
#include <medAbstractViewFiberInteractor.h>

#include <medDatabaseNonPersistentController.h>

class medToolBoxDiffusionFiberBundlingPrivate
{
public:
    //QListWidget  *bundlingList;
    QTreeView  *bundlingList;
    QStandardItemModel *bundlingModel;
    QPushButton  *bundlingButtonVdt;
    QPushButton  *bundlingButtonTag;
    QPushButton  *bundlingButtonRst;
    QCheckBox    *bundlingShowCheckBox;
    QCheckBox    *bundleBoxCheckBox;
    QPushButton  *bundlingButtonAdd;
    QComboBox    *roiComboBox;
    QRadioButton *andButton;
    QRadioButton *notButton;
    QRadioButton *nullButton;
    
    dtkAbstractView *view;
    dtkAbstractData *data;
};

medToolBoxDiffusionFiberBundling::medToolBoxDiffusionFiberBundling(QWidget *parent) : medToolBox(parent), d(new medToolBoxDiffusionFiberBundlingPrivate)
{
    d->view = 0;
    d->data = 0;
    
    QWidget *bundlingPage = new QWidget(this);

    QPushButton *openRoiButton = new QPushButton("Open ROI", bundlingPage);
    QPushButton *clearRoiButton = new QPushButton("Clear ROI", bundlingPage);
    d->roiComboBox = new QComboBox(bundlingPage);
    for (int i=0; i<255; i++)
        d->roiComboBox->addItem(tr("ROI ")+QString::number(i+1));
    d->roiComboBox->setCurrentIndex(0);

    QGroupBox *boolGroup = new QGroupBox(bundlingPage);
    boolGroup->setStyleSheet("border:0;");
    boolGroup->setContentsMargins(0, 0, 0, 0);
    boolGroup->setAlignment(Qt::AlignHCenter);

    d->andButton  = new QRadioButton(tr("AND"), bundlingPage);
    d->andButton->setChecked(true);
    d->notButton  = new QRadioButton(tr("NOT"), bundlingPage);
    d->nullButton = new QRadioButton(tr("NULL"), bundlingPage);

    QHBoxLayout *boolLayout = new QHBoxLayout;
    boolLayout->setContentsMargins(0, 0, 0, 0);
    boolLayout->addWidget(d->andButton);
    boolLayout->addWidget(d->notButton);
    boolLayout->addWidget(d->nullButton);

    boolGroup->setLayout(boolLayout);

    QHBoxLayout *roiLayout = new QHBoxLayout;
    roiLayout->addWidget(d->roiComboBox);
    roiLayout->addWidget(boolGroup);
    
    d->bundlingButtonTag = new QPushButton("Tag", bundlingPage);
    d->bundlingButtonAdd = new QPushButton("Add", bundlingPage);
    d->bundlingButtonRst = new QPushButton("Reset", bundlingPage);
    d->bundlingButtonVdt = new QPushButton("Validate", bundlingPage);
    d->bundlingButtonAdd->setCheckable(true);
    
    QHBoxLayout *bundlingButtonsLayout = new QHBoxLayout;
    bundlingButtonsLayout->addWidget(d->bundlingButtonTag);
    bundlingButtonsLayout->addWidget(d->bundlingButtonAdd);
    bundlingButtonsLayout->addWidget(d->bundlingButtonRst);
    bundlingButtonsLayout->addWidget(d->bundlingButtonVdt);
    
    // d->bundlingList = new QListWidget(bundlingPage);
    
    d->bundlingModel = new QStandardItemModel(0, 1, bundlingPage);
    d->bundlingModel->setHeaderData(0, Qt::Horizontal, tr("Fiber bundles"));
            
    // QItemSelectionModel *selectionModel = new QItemSelectionModel(d->bundlingModel);
    
    d->bundlingList = new QTreeView(bundlingPage);
    d->bundlingList->setAlternatingRowColors(true);
    d->bundlingList->setMinimumHeight(150);
    d->bundlingList->setModel (d->bundlingModel);
    // d->bundlingList->setSelectionModel(selectionModel);
    
    d->bundlingShowCheckBox = new QCheckBox("Show all bundles", bundlingPage);
    d->bundlingShowCheckBox->setChecked(true);
    d->bundleBoxCheckBox = new QCheckBox("Activate bundling box", bundlingPage);

    QHBoxLayout *roiButtonLayout = new QHBoxLayout;
    roiButtonLayout->addWidget (openRoiButton);
    roiButtonLayout->addWidget (clearRoiButton);

    QVBoxLayout *bundlingLayout = new QVBoxLayout(bundlingPage);
    bundlingLayout->addLayout(roiButtonLayout);
    bundlingLayout->addLayout(roiLayout);
    bundlingLayout->addWidget(d->bundleBoxCheckBox);
    bundlingLayout->addLayout(bundlingButtonsLayout);
    bundlingLayout->addWidget(d->bundlingList);
    bundlingLayout->addWidget(d->bundlingShowCheckBox);

    connect (d->bundlingButtonVdt,     SIGNAL(clicked(void)),            this, SLOT (onBundlingButtonVdtClicked (void)));
    connect (d->bundleBoxCheckBox,     SIGNAL(toggled(bool)),            this, SLOT (onBundleBoxCheckBoxToggled (bool)));
    connect (d->bundlingButtonAdd,     SIGNAL(toggled(bool)),            this, SLOT (onBundlingButtonAndToggled(bool)));
	
    connect (d->bundlingShowCheckBox,  SIGNAL(toggled(bool)),            this, SLOT (onBundlingShowCheckBoxToggled (bool)));
    connect (d->bundlingButtonTag,     SIGNAL(clicked(void)),            this, SIGNAL (fiberSelectionTagged(void)));
    connect (d->bundlingButtonRst,     SIGNAL(clicked(void)),            this, SIGNAL (fiberSelectionReset(void)));

    connect (d->bundlingModel, SIGNAL(itemChanged(QStandardItem*)), this, SLOT(onBundlingItemChanged(QStandardItem*)));

    connect (openRoiButton,  SIGNAL(clicked()),                this, SLOT(onOpenRoiButtonClicked()));
    connect (clearRoiButton, SIGNAL(clicked()),                this, SLOT(onClearRoiButtonClicked()));
    connect (d->roiComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onRoiComboIndexChanged(int)));
    connect (d->andButton,   SIGNAL(toggled(bool)),            this, SLOT(onAddButtonToggled(bool)));
    connect (d->notButton,   SIGNAL(toggled(bool)),            this, SLOT(onNotButtonToggled(bool)));
    connect (d->nullButton,  SIGNAL(toggled(bool)),            this, SLOT(onNullButtonToggled(bool)));
 
    this->setTitle("Fiber Bundling");
    this->addWidget(bundlingPage);
}

medToolBoxDiffusionFiberBundling::~medToolBoxDiffusionFiberBundling()
{
    delete d;
    d = NULL;
}

void medToolBoxDiffusionFiberBundling::setData(dtkAbstractData *data)
{
    if (!data)
        return;
    
    if (data->description()!="v3dDataFibers") {
        return;
    }
    
    if (d->data==data)
        return;
    
    d->data = data;
    
    //d->bundlingList->clear();
    //d->bundlingModel->clear();
    d->bundlingModel->removeRows(0, d->bundlingModel->rowCount(QModelIndex()), QModelIndex());
        
    if (data->hasMetaData("BundleList") && data->hasMetaData("BundleColorList")) {
        
        QStringList bundleNames  = data->metaDataValues("BundleList");
        QStringList bundleColors = data->metaDataValues("BundleColorList");
        if (bundleNames.count()==bundleColors.count()) {
            for (int i=0; i<bundleNames.count(); i++)
                this->addBundle(bundleNames[i], QColor(bundleColors[i]));
        }
    }
}

void medToolBoxDiffusionFiberBundling::onBundlingButtonVdtClicked (void)
{
    if (!d->data)
        return;

    // bool ok;
    //QString text = QInputDialog::getText(this, tr("Enter bundle name"),
      //                                   tr(""), QLineEdit::Normal, tr(""), &ok);
    QString text = tr("Fiber bundle #") + QString::number(d->bundlingModel->rowCount()+1);
    
    //if (ok && !text.isEmpty()) {
        // if (!d->bundlingList->contains (name)) // should popup a warning
        QColor color = QColor::fromHsv(qrand()%360, 255, 210);
        
        emit fiberSelectionValidated (text, color);
        this->addBundle (text, color);
    //}
}

void medToolBoxDiffusionFiberBundling::onBundleBoxCheckBoxToggled (bool value)
{
    if (!d->view)
        return;

    if (dtkAbstractViewInteractor *interactor = d->view->interactor ("v3dViewFiberInteractor")) {
        if (value)
	    interactor->setProperty ("BoxVisibility", "true");
        else
            interactor->setProperty ("BoxVisibility", "false");
                
        d->view->update();
    } 
}

void medToolBoxDiffusionFiberBundling::addBundle (const QString &name, const QColor &color)
{/*
    if(!d->data)
        return;
  */
    //d->bundlingList->addItem (name);
    
    //d->bundlingModel->removeRows(0, d->bundlingModel->rowCount(QModelIndex()), QModelIndex());
    
    int row = d->bundlingModel->rowCount();
    QStandardItem *item = new QStandardItem (name);
    item->setCheckable(true);
    item->setTristate(false);
    item->setEditable(false); // for now

    double meanFA = 0.0;
    double minFA  = 0.0;
    double maxFA  = 0.0;
    double varFA  = 0.0;

    double meanADC = 0.0;
    double minADC  = 0.0;
    double maxADC  = 0.0;
    double varADC  = 0.0;

    double meanLength = 0.0;
    double minLength  = 0.0;
    double maxLength  = 0.0;
    double varLength  = 0.0;

    if (d->view) {
        if (medAbstractViewFiberInteractor *interactor = dynamic_cast<medAbstractViewFiberInteractor*>(d->view->interactor ("v3dViewFiberInteractor"))) {
            interactor->bundleFAStatistics(name, meanFA, minFA, maxFA, varFA);
            interactor->bundleADCStatistics(name, meanADC, minADC, maxADC, varADC);
            interactor->bundleLengthStatistics(name, meanLength, minLength, maxLength, varLength);
        }
    }

    QStandardItem *childItem1 = new QStandardItem (tr("FA: ") + QString::number(meanFA));
    childItem1->setEditable(false);
    childItem1->appendRow(new QStandardItem (tr("mean: ")     + QString::number(meanFA)));
    childItem1->appendRow(new QStandardItem (tr("variance: ") + QString::number(varFA)));
    childItem1->appendRow(new QStandardItem (tr("min: ")      + QString::number(minFA)));
    childItem1->appendRow(new QStandardItem (tr("max: ")      + QString::number(maxFA)));

    QStandardItem *childItem2 = new QStandardItem (tr("ADC: ") + QString::number(meanADC));
    childItem2->setEditable(false);
    childItem2->appendRow(new QStandardItem (tr("mean: ")     + QString::number(meanADC)));
    childItem2->appendRow(new QStandardItem (tr("variance: ") + QString::number(varADC)));
    childItem2->appendRow(new QStandardItem (tr("min: ")      + QString::number(minADC)));
    childItem2->appendRow(new QStandardItem (tr("max: ")      + QString::number(maxADC)));

    QStandardItem *childItem3 = new QStandardItem (tr("Length: ") + QString::number(meanLength));
    childItem3->setEditable(false);
    childItem3->appendRow(new QStandardItem (tr("mean: ")     + QString::number(meanLength)));
    childItem3->appendRow(new QStandardItem (tr("variance: ") + QString::number(varLength)));
    childItem3->appendRow(new QStandardItem (tr("min: ")      + QString::number(minLength)));
    childItem3->appendRow(new QStandardItem (tr("max: ")      + QString::number(maxLength)));

    item->appendRow(childItem1);
    item->appendRow(childItem2);
    item->appendRow(childItem3);

    d->bundlingModel->setItem(row, item);
    
    //d->bundlingModel->addItem (name);
    
//    d->bundlingModel->setData(d->bundlingModel->index(row, 0, QModelIndex()),
  //                            name);
    
    d->bundlingModel->setData(d->bundlingModel->index(row, 0, QModelIndex()),
                              true, Qt::CheckStateRole);

    d->bundlingModel->setData(d->bundlingModel->index(row, 0, QModelIndex()),
                              color, Qt::DecorationRole);


    //d->bundlingList->update();
}

void medToolBoxDiffusionFiberBundling::onOpenRoiButtonClicked(void)
{
    if (!d->view)
        return;

    QString roiFileName = QFileDialog::getOpenFileName(this, tr("Open ROI"), "", tr("Image file (*.*)"));

    if (roiFileName.isEmpty())
        return;

    medAbstractDbController *nonPersDbController  = medDbControllerFactory::instance()->createDbController("NonPersistentDbController");
    if (!nonPersDbController) {
        qWarning() << "No nonPersistentDbController registered!";
        return;
    }
    
    medDataIndex index = nonPersDbController->import(roiFileName);

    dtkAbstractData *data = medDataManager::instance()->data(index).data();

    if (!data)
        return;

    if (medAbstractViewFiberInteractor *interactor = dynamic_cast<medAbstractViewFiberInteractor*>(d->view->interactor ("v3dViewFiberInteractor"))) {
        interactor->setROI(data);
        d->view->update();
    }

}

void medToolBoxDiffusionFiberBundling::onClearRoiButtonClicked(void)
{
    if (!d->view)
        return;

    // create dummy mask image
    dtkAbstractData *data = dtkAbstractDataFactory::instance()->create("itkDataImageUChar3");
    if (medAbstractViewFiberInteractor *interactor = dynamic_cast<medAbstractViewFiberInteractor*>(d->view->interactor ("v3dViewFiberInteractor"))) {
        interactor->setROI(data);
        d->view->update();
    }
    data->deleteLater();
}

void medToolBoxDiffusionFiberBundling::onRoiComboIndexChanged (int value)
{
    if (!d->view)
        return;

    if (medAbstractViewFiberInteractor *interactor = dynamic_cast<medAbstractViewFiberInteractor*>(d->view->interactor ("v3dViewFiberInteractor"))) {
        int boolean = interactor->roiBoolean (value);
        switch (boolean) {
        case 2:
            d->andButton->blockSignals (true);
            d->andButton->setChecked   (true);
            d->andButton->blockSignals (false);
            break;

        case 1:
            d->notButton->blockSignals (true);
            d->notButton->setChecked   (true);
            d->notButton->blockSignals (false);
            break;

        case 0:
        default:
            d->nullButton->blockSignals (true);
            d->nullButton->setChecked   (true);
            d->nullButton->blockSignals (false);
            break;
        }
    }

    d->view->update();
}

void medToolBoxDiffusionFiberBundling::onAddButtonToggled (bool value)
{
    if (!d->view)
        return;

    if (medAbstractViewFiberInteractor *interactor = dynamic_cast<medAbstractViewFiberInteractor*>(d->view->interactor ("v3dViewFiberInteractor"))) {
        int roi = d->roiComboBox->currentIndex();
        if (value)
            interactor->setRoiBoolean(roi+1, 2);
    }

    d->view->update();
}

void medToolBoxDiffusionFiberBundling::onNotButtonToggled (bool value)
{
    if (!d->view)
        return;

    if (medAbstractViewFiberInteractor *interactor = dynamic_cast<medAbstractViewFiberInteractor*>(d->view->interactor ("v3dViewFiberInteractor"))) {
        int roi = d->roiComboBox->currentIndex();
        if (value)
            interactor->setRoiBoolean(roi+1, 1);
    }

    d->view->update();
}

void medToolBoxDiffusionFiberBundling::onNullButtonToggled (bool value)
{
    if (!d->view)
        return;

    if (medAbstractViewFiberInteractor *interactor = dynamic_cast<medAbstractViewFiberInteractor*>(d->view->interactor ("v3dViewFiberInteractor"))) {
        int roi = d->roiComboBox->currentIndex();
        if (value)
            interactor->setRoiBoolean(roi+1, 0);
    }

    d->view->update();
}

void medToolBoxDiffusionFiberBundling::clear(void)
{
    d->bundlingModel->removeRows(0, d->bundlingModel->rowCount(QModelIndex()), QModelIndex());

    d->view = 0;
    d->data = 0;
}

void medToolBoxDiffusionFiberBundling::update(dtkAbstractView *view)
{
    if (d->view==view) {
        if (view)
            if (dtkAbstractViewInteractor *interactor = view->interactor ("v3dViewFiberInteractor"))
                this->setData (interactor->data()); // data may have changed
        return;
    }
    
    d->bundlingModel->removeRows(0, d->bundlingModel->rowCount(QModelIndex()), QModelIndex());
    
    if (d->view) {
        if (dtkAbstractViewInteractor *interactor = d->view->interactor ("v3dViewFiberInteractor")) {
            disconnect (this, SIGNAL(fiberSelectionValidated(const QString&, const QColor&)), 
                        interactor, SLOT(onSelectionValidated(const QString&, const QColor&)));
            disconnect (this, SIGNAL(fiberSelectionTagged()),    interactor, SLOT(onSelectionTagged()));
            disconnect (this, SIGNAL(fiberSelectionReset()),     interactor, SLOT(onSelectionReset()));
            disconnect (this, SIGNAL(bundlingBoxBooleanOperatorChanged(int)),
                        interactor, SLOT(onBundlingBoxBooleanOperatorChanged(int)));
        }
    }
    
    if (!view) {
        d->view = 0;
        d->data = 0;
        return;
    }
    
    if (view->property ("Orientation")!="3D") { // only interaction with 3D views is authorized
        d->view = 0;
        d->data = 0;
        return;
    }
    
    d->view = view;
    
    if (dtkAbstractViewInteractor *interactor = view->interactor ("v3dViewFiberInteractor")) {
        connect (this, SIGNAL(fiberSelectionValidated(const QString&, const QColor&)), 
                 interactor, SLOT(onSelectionValidated(const QString&, const QColor&)));
        connect (this, SIGNAL(fiberSelectionTagged()),           interactor, SLOT(onSelectionTagged()));
        connect (this, SIGNAL(fiberSelectionReset()),            interactor, SLOT(onSelectionReset()));
        connect (this, SIGNAL(bundlingBoxBooleanOperatorChanged(int)),
                 interactor, SLOT(onBundlingBoxBooleanOperatorChanged(int)));

        d->bundleBoxCheckBox->blockSignals (true);
        d->bundleBoxCheckBox->setChecked( interactor->property("BoxVisibility")=="true" );
        d->bundleBoxCheckBox->blockSignals (false);
        
        this->setData (interactor->data());
    }
}

void medToolBoxDiffusionFiberBundling::onBundlingItemChanged(QStandardItem *item)
{
    if (d->view) {
        if (medAbstractViewFiberInteractor *interactor = dynamic_cast<medAbstractViewFiberInteractor*>(d->view->interactor ("v3dViewFiberInteractor")))
            interactor->setBundleVisibility(item->text(), item->checkState());
        d->view->update();
    }
}

void medToolBoxDiffusionFiberBundling::onBundlingShowCheckBoxToggled(bool value)
{
    if (d->view) {
        if (medAbstractViewFiberInteractor *interactor = dynamic_cast<medAbstractViewFiberInteractor*>(d->view->interactor ("v3dViewFiberInteractor")))
            interactor->setBundleVisibility(value);
        d->view->update();
    }
}

void medToolBoxDiffusionFiberBundling::onBundlingButtonAndToggled(bool value)
{
    if (value)
        emit bundlingBoxBooleanOperatorChanged (0);
    else
        emit bundlingBoxBooleanOperatorChanged (1);
    if (d->view)
        d->view->update();
}

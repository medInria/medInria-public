#include "medToolBoxDiffusionFiberBundling.h"

#include <dtkCore/dtkAbstractData.h>
#include <dtkCore/dtkAbstractDataFactory.h>
#include <dtkCore/dtkAbstractView.h>
#include <dtkCore/dtkAbstractViewInteractor.h>

#include <medCore/medDataManager.h>
#include <medCore/medAbstractDbController.h>
#include <medCore/medDbControllerFactory.h>
#include <medCore/medAbstractViewFiberInteractor.h>

#include <medSql/medDatabaseNonPersistentController.h>

class medToolBoxDiffusionFiberBundlingPrivate
{
public:
    //QListWidget  *bundlingList;
    QListView  *bundlingList;
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
    d->roiComboBox = new QComboBox(bundlingPage);
    for (int i=0; i<255; i++)
        d->roiComboBox->addItem(tr("ROI ")+QString::number(i+1));
    d->roiComboBox->setCurrentIndex(0);

    QGroupBox *boolGroup = new QGroupBox(bundlingPage);
    boolGroup->setStyleSheet("border:0;");
    boolGroup->setContentsMargins(0, 0, 0, 0);
    boolGroup->setAlignment(Qt::AlignHCenter);

    d->andButton  = new QRadioButton(tr("AND"), bundlingPage);
    d->notButton  = new QRadioButton(tr("NOT"), bundlingPage);
    d->nullButton = new QRadioButton(tr("NULL"), bundlingPage);

    QHBoxLayout *boolLayout = new QHBoxLayout;
    boolLayout->addWidget(d->andButton);
    boolLayout->addWidget(d->notButton);
    boolLayout->addWidget(d->nullButton);
    d->andButton->setChecked(true);

    boolGroup->setLayout(boolLayout);

    QHBoxLayout *roiLayout = new QHBoxLayout;
    roiLayout->addWidget(d->roiComboBox);
    roiLayout->addWidget(boolGroup);
    
    d->bundlingButtonTag = new QPushButton("Tag", bundlingPage);
    d->bundlingButtonAdd = new QPushButton("Add", bundlingPage);
    d->bundlingButtonRst = new QPushButton("Reset", bundlingPage);
    d->bundlingButtonVdt = new QPushButton("Validate", bundlingPage);
    
    QHBoxLayout *bundlingButtonsLayout = new QHBoxLayout;
    bundlingButtonsLayout->addWidget(d->bundlingButtonTag);
    bundlingButtonsLayout->addWidget(d->bundlingButtonAdd);
    bundlingButtonsLayout->addWidget(d->bundlingButtonRst);
    bundlingButtonsLayout->addWidget(d->bundlingButtonVdt);
    
    // d->bundlingList = new QListWidget(bundlingPage);
    
    d->bundlingModel = new QStandardItemModel(0, 1, bundlingPage);
    d->bundlingModel->setHeaderData(0, Qt::Horizontal, tr("Fiber bundles"));
            
    QItemSelectionModel *selectionModel = new QItemSelectionModel(d->bundlingModel);
    
    d->bundlingList = new QListView(bundlingPage);
    d->bundlingList->setAlternatingRowColors(true);
    d->bundlingList->setModel (d->bundlingModel);
    d->bundlingList->setSelectionModel(selectionModel);
    
    d->bundlingShowCheckBox = new QCheckBox("Show all bundles", bundlingPage);
    d->bundleBoxCheckBox = new QCheckBox("Activate bundling box", bundlingPage);
        
    QVBoxLayout *bundlingLayout = new QVBoxLayout(bundlingPage);
    bundlingLayout->addWidget(openRoiButton);
    bundlingLayout->addLayout(roiLayout);
    bundlingLayout->addWidget(d->bundleBoxCheckBox);
    bundlingLayout->addLayout(bundlingButtonsLayout);
    bundlingLayout->addWidget(d->bundlingList);
    bundlingLayout->addWidget(d->bundlingShowCheckBox);

    connect (d->bundlingButtonVdt,     SIGNAL(clicked(void)),            this, SLOT (onBundlingButtonVdtClicked (void)));
    connect (d->bundleBoxCheckBox,     SIGNAL(toggled(bool)),            this, SLOT (onBundleBoxCheckBoxToggled (bool)));
	
    connect (d->bundlingShowCheckBox,  SIGNAL(toggled(bool)),            this, SIGNAL (showBundles (bool)));
    connect (d->bundlingButtonTag,     SIGNAL(clicked(void)),            this, SIGNAL (fiberSelectionTagged(void)));
    connect (d->bundlingButtonRst,     SIGNAL(clicked(void)),            this, SIGNAL (fiberSelectionReset(void)));

    connect (openRoiButton,  SIGNAL(clicked()),                this, SLOT(onOpenRoiButtonClicked()));
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
    bool ok;
    QString text = QInputDialog::getText(this, tr("Enter bundle name"),
                                         tr(""), QLineEdit::Normal, tr(""), &ok);
    
    if (ok && !text.isEmpty()) {
        // if (!d->bundlingList->contains (name)) // should popup a warning
        QColor color = QColor::fromHsv(qrand()%360, 255, 210);
        
        emit fiberSelectionValidated (text, color);
        this->addBundle (text, color);
    }
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
    QStandardItem *item = new QStandardItem;
    item->setCheckable(true);
    d->bundlingModel->insertRow(row, item);
    
    //d->bundlingModel->addItem (name);
    
    d->bundlingModel->setData(d->bundlingModel->index(row, 0, QModelIndex()),
                              name);
    
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

    dtkAbstractData *data = medDataManager::instance()->data(index);

    if (!data)
        return;

    if (medAbstractViewFiberInteractor *interactor = dynamic_cast<medAbstractViewFiberInteractor*>(d->view->interactor ("v3dViewFiberInteractor"))) {
        interactor->setROI(data);
        d->view->update();
    }

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
    //d->bundlingList->clear();
    //d->bundlingModel->clear();
    d->bundlingModel->removeRows(0, d->bundlingModel->rowCount(QModelIndex()), QModelIndex());

    //this->update (0);
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

        d->bundleBoxCheckBox->blockSignals (true);
        d->bundleBoxCheckBox->setChecked( interactor->property("BoxVisibility")=="true" );
        d->bundleBoxCheckBox->blockSignals (false);
        
        this->setData (interactor->data());
    }
}

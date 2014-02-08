/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include "medFiberBundlingToolBox.h"

#include <dtkCore/dtkAbstractData.h>
#include <dtkCore/dtkAbstractDataFactory.h>
#include <dtkCore/dtkAbstractView.h>
#include <dtkCore/dtkAbstractViewInteractor.h>
#include <dtkCore/dtkSmartPointer.h>

#include <medDataManager.h>
#include <medAbstractDbController.h>
#include <medDbControllerFactory.h>
#include <v3dViewFiberInteractor.h>
#include <medMessageController.h>
#include <medDropSite.h>
#include <medMetaDataKeys.h>
#include <medImageFileLoader.h>

#include <medToolBoxFactory.h>
#include <v3dView.h>

class medFiberBundlingToolBoxPrivate
{
public:
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
    medDropSite *dropOrOpenRoi;
    QPushButton *saveBundlesButton;
    
    v3dView * view;
    v3dViewFiberInteractor * interactor;
    dtkSmartPointer<dtkAbstractData> data;
};

medFiberBundlingToolBox::medFiberBundlingToolBox(QWidget *parent) : medToolBox(parent), d(new medFiberBundlingToolBoxPrivate)
{
    QWidget *bundlingPage = new QWidget(this);

    d->dropOrOpenRoi = new medDropSite(bundlingPage);
    d->dropOrOpenRoi->setToolTip(tr("Once you are viewing fibers, click here to open a ROI or drag-and-drop one from the database."));
    d->dropOrOpenRoi->setText(tr("Once you are viewing fibers\nclick here to open a ROI\nor drag-and-drop one\nfrom the database."));
    d->dropOrOpenRoi->setCanAutomaticallyChangeAppereance(false);

    QPushButton *clearRoiButton = new QPushButton("Clear ROI", bundlingPage);
    clearRoiButton->setToolTip(tr("Clear previously loaded ROIs."));
    d->roiComboBox = new QComboBox(bundlingPage);
    for (int i=0; i<255; i++)
        d->roiComboBox->addItem(tr("ROI ")+QString::number(i+1));
    d->roiComboBox->setCurrentIndex(0);
    d->roiComboBox->setToolTip(tr("Select a ROI to modify how its interaction with the fibers affects whether they are displayed."));

    QGroupBox *boolGroup = new QGroupBox(bundlingPage);
    boolGroup->setStyleSheet("border:0;");
    boolGroup->setContentsMargins(0, 0, 0, 0);
    boolGroup->setAlignment(Qt::AlignHCenter);

    d->andButton  = new QRadioButton(tr("AND"), bundlingPage);
    d->andButton->setToolTip(tr("If \"AND\" is selected fibers will need to overlap with this ROI to be displayed."));
    d->andButton->setChecked(true);
    d->notButton  = new QRadioButton(tr("NOT"), bundlingPage);
    d->notButton->setToolTip(tr("If \"NOT\" is selected fibers overlapping with this ROI will not be displayed."));
    d->nullButton = new QRadioButton(tr("NULL"), bundlingPage);
    d->nullButton->setToolTip(tr("If \"NULL\" is selected this ROI won't be taken into account."));

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
    d->bundlingButtonTag->setToolTip(tr("Tag the currently shown bundle to let medInria memorize it and another, new bundling box, will appear.\nThis new box will also isolate fibers but will also consider the previously \"tagged\" fibers."));
    d->bundlingButtonAdd = new QPushButton("Add", bundlingPage);
    d->bundlingButtonAdd->setToolTip(tr("Select to either include the fibers that overlap with the bundling box, or to include the ones that do not overlap."));
    d->bundlingButtonRst = new QPushButton("Reset", bundlingPage);
    d->bundlingButtonRst->setToolTip(tr("Reset all previously tagged bundling boxes."));
    d->bundlingButtonVdt = new QPushButton("Validate", bundlingPage);
    d->bundlingButtonVdt->setToolTip(tr("Save the current shown bundle and show useful information about it."));
    d->bundlingButtonAdd->setCheckable(true);
    d->saveBundlesButton = new QPushButton("Save bundles",bundlingPage);
    d->saveBundlesButton->setToolTip(tr("Save all bundles to database"));

    QHBoxLayout *bundlingButtonsLayout = new QHBoxLayout;
    bundlingButtonsLayout->addWidget(d->bundlingButtonTag);
    bundlingButtonsLayout->addWidget(d->bundlingButtonAdd);
    bundlingButtonsLayout->addWidget(d->bundlingButtonRst);
    bundlingButtonsLayout->addWidget(d->bundlingButtonVdt);

    d->bundlingModel = new QStandardItemModel(0, 1, bundlingPage);
    d->bundlingModel->setHeaderData(0, Qt::Horizontal, tr("Fiber bundles"));

    d->bundlingList = new QTreeView(bundlingPage);
    d->bundlingList->setAlternatingRowColors(true);
    d->bundlingList->setMinimumHeight(150);
    d->bundlingList->setModel (d->bundlingModel);
    d->bundlingList->setEditTriggers(QAbstractItemView::SelectedClicked);

    d->bundlingShowCheckBox = new QCheckBox("Show all bundles", bundlingPage);
    d->bundlingShowCheckBox->setChecked(true);
    d->bundlingShowCheckBox->setToolTip(tr("Uncheck if you do not want the previously validated bundles to be displayed."));
    d->bundleBoxCheckBox = new QCheckBox("Activate bundling box", bundlingPage);
    d->bundleBoxCheckBox->setToolTip(tr("Select to activate and show the fiber bundling box on the screen."));

    QVBoxLayout *roiButtonLayout = new QVBoxLayout;
    roiButtonLayout->addWidget(d->dropOrOpenRoi);
    roiButtonLayout->addWidget (clearRoiButton);
    roiButtonLayout->setAlignment(Qt::AlignCenter);

    QVBoxLayout *bundlingLayout = new QVBoxLayout(bundlingPage);
    bundlingLayout->addLayout(roiButtonLayout);
    bundlingLayout->addLayout(roiLayout);
    bundlingLayout->addWidget(d->bundleBoxCheckBox);
    bundlingLayout->addLayout(bundlingButtonsLayout);
    bundlingLayout->addWidget(d->saveBundlesButton);
    bundlingLayout->addWidget(d->bundlingList);
    bundlingLayout->addWidget(d->bundlingShowCheckBox);

    connect (d->bundlingButtonVdt,     SIGNAL(clicked(void)),            this, SLOT (validateBundling()));
    connect (d->bundleBoxCheckBox,     SIGNAL(toggled(bool)),            this, SLOT (showBundlingBox (bool)));
    connect (d->bundlingButtonAdd,     SIGNAL(toggled(bool)),            this, SLOT (setBoxBooleanOperation(bool)));

    connect (d->bundlingShowCheckBox,  SIGNAL(toggled(bool)),            this, SLOT (showBundling (bool)));
    connect (d->bundlingButtonTag,     SIGNAL(clicked(void)),            this, SIGNAL (fiberSelectionTagged(void)));
    connect (d->bundlingButtonRst,     SIGNAL(clicked(void)),            this, SIGNAL (fiberSelectionReset(void)));

    connect (d->saveBundlesButton, SIGNAL(clicked()), this, SLOT(saveBundles()));
    
    connect (d->bundlingModel, SIGNAL(itemChanged(QStandardItem*)), this, SLOT(changeBundlingItem(QStandardItem*)));

    connect (d->dropOrOpenRoi, SIGNAL(objectDropped(const medDataIndex&)), this, SLOT(importROI(const medDataIndex&)));
    connect (d->dropOrOpenRoi, SIGNAL(clicked()),                          this, SLOT(onDropSiteClicked()));
    connect (clearRoiButton,   SIGNAL(clicked()),                          this, SLOT(clearRoi()));
    connect (d->roiComboBox,   SIGNAL(currentIndexChanged(int)),           this, SLOT(selectRoi(int)));
    connect (d->andButton,     SIGNAL(toggled(bool)),                      this, SLOT(setRoiAddOperation(bool)));
    connect (d->notButton,     SIGNAL(toggled(bool)),                      this, SLOT(setRoiNotOperation(bool)));
    connect (d->nullButton,    SIGNAL(toggled(bool)),                      this, SLOT(setRoiNullOperation(bool)));

    this->setTitle("Fiber Bundling");
    this->addWidget(bundlingPage);

    this->setValidDataTypes(QStringList() << "v3dDataFibers");

    this->hide();
}

medFiberBundlingToolBox::~medFiberBundlingToolBox()
{
    delete d;
    d = NULL;
}

bool medFiberBundlingToolBox::registered()
{
    return medToolBoxFactory::instance()->registerToolBox<medFiberBundlingToolBox>("medFiberBundlingToolBox",
                                                                                   "medFiberBundlingToolBox",
                                                                                   "Fiber bundling toolbox",
                                                                                   QStringList()<<"fiber"<<"bundling");
}

void medFiberBundlingToolBox::setData(dtkAbstractData *data)
{
    if (!data)
        return;

    if (data->identifier()!="v3dDataFibers") {
        return;
    }

    if (d->data==data)
        return;

    d->data = data;

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

void medFiberBundlingToolBox::validateBundling()
{
    if (!d->data)
        return;

    QString text = tr("Fiber bundle #") + QString::number(d->bundlingModel->rowCount()+1);

    QColor color = QColor::fromHsv(qrand()%360, 255, 210);

    emit fiberSelectionValidated (text, color);
    this->addBundle (text, color);
}

void medFiberBundlingToolBox::showBundlingBox (bool show)
{
    if (d->view && d->interactor)
        d->interactor->setBoxVisibility(show);
}

void medFiberBundlingToolBox::addBundle (const QString &name, const QColor &color)
{
    int row = d->bundlingModel->rowCount();

    QStandardItem *item = new QStandardItem (name);
    item->setCheckable(true);
    item->setTristate(false);
    item->setEditable(true);

    QMap <QString, double> meanData;
    QMap <QString, double> minData;
    QMap <QString, double> maxData;
    QMap <QString, double> varData;

    double meanLength = 0.0;
    double minLength  = 0.0;
    double maxLength  = 0.0;
    double varLength  = 0.0;

    if (d->view && d->interactor)
    {
        d->interactor->bundleImageStatistics(name, meanData, minData, maxData, varData);
        d->interactor->bundleLengthStatistics(name, meanLength, minLength, maxLength, varLength);
    }

    foreach (QString key, meanData.keys())
    {
        QStandardItem *childItem1 = new QStandardItem (key + ": " + QString::number(meanData[key]));
        childItem1->setEditable(false);
        childItem1->appendRow(new QStandardItem (tr("mean: ")     + QString::number(meanData[key])));
        childItem1->appendRow(new QStandardItem (tr("variance: ") + QString::number(varData[key])));
        childItem1->appendRow(new QStandardItem (tr("min: ")      + QString::number(minData[key])));
        childItem1->appendRow(new QStandardItem (tr("max: ")      + QString::number(maxData[key])));
        
        item->appendRow(childItem1);
    }

    QStandardItem *childItem2 = new QStandardItem (tr("Length: ") + QString::number(meanLength));
    childItem2->setEditable(false);
    childItem2->appendRow(new QStandardItem (tr("mean: ")     + QString::number(meanLength)));
    childItem2->appendRow(new QStandardItem (tr("variance: ") + QString::number(varLength)));
    childItem2->appendRow(new QStandardItem (tr("min: ")      + QString::number(minLength)));
    childItem2->appendRow(new QStandardItem (tr("max: ")      + QString::number(maxLength)));

    item->appendRow(childItem2);
    item->setData(name,Qt::UserRole+1);
    
    d->bundlingModel->setItem(row, item);

    d->bundlingModel->setData(d->bundlingModel->index(row, 0, QModelIndex()),
                              true, Qt::CheckStateRole);

    d->bundlingModel->setData(d->bundlingModel->index(row, 0, QModelIndex()),
                              color, Qt::DecorationRole);

}

void medFiberBundlingToolBox::saveBundles()
{
    if (d->view)
    {
        if (d->interactor)
        {
            d->interactor->saveBundles();
        }
    }
}

void medFiberBundlingToolBox::importROI(const medDataIndex& index)
{
    dtkSmartPointer<dtkAbstractData> data = medDataManager::instance()->data(index);

    // we accept only ROIs (itkDataImageUChar3)
    if (!data || !d->view || data->identifier() != "itkDataImageUChar3")
    {
        return;
    }

    // put the thumbnail in the medDropSite as well
    // (code copied from @medDatabasePreviewItem)
    medAbstractDbController* dbc = medDataManager::instance()->controllerForDataSource(index.dataSourceId());
    QString thumbpath = dbc->metaData(index, medMetaDataKeys::ThumbnailPath);

    bool shouldSkipLoading = false;
    if ( thumbpath.isEmpty() )
    {
        // first try to get it from controller
        QImage thumbImage = dbc->thumbnail(index);
        if (!thumbImage.isNull())
        {
            d->dropOrOpenRoi->setPixmap(QPixmap::fromImage(thumbImage));
            shouldSkipLoading = true;
        }
    }

    if (!shouldSkipLoading) {
        medImageFileLoader *loader = new medImageFileLoader(thumbpath);
        connect(loader, SIGNAL(completed(const QImage&)), this, SLOT(setImage(const QImage&)));
        QThreadPool::globalInstance()->start(loader);
    }

    if (d->interactor)
    {
        d->interactor->setROI(data);
    }
}

void medFiberBundlingToolBox::clearRoi(void)
{
    if (!d->view)
        return;

    // create dummy mask image
    dtkAbstractData *data = dtkAbstractDataFactory::instance()->create("itkDataImageUChar3");
    if (d->interactor) {
        d->interactor->setROI(data);
    }
    data->deleteLater();

    // clear medDropSite and put text again
    d->dropOrOpenRoi->clear();
    d->dropOrOpenRoi->setText(tr("Click to open a ROI\nfrom your hard drive\nor drag-and-drop one\nfrom the database."));
}

void medFiberBundlingToolBox::selectRoi(int value)
{
    if (!d->view)
        return;

    if (d->interactor) {
        int boolean = d->interactor->roiBoolean (value);
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
}

void medFiberBundlingToolBox::setRoiAddOperation (bool value)
{
    if (!d->view)
        return;

    if (d->interactor) {
        int roi = d->roiComboBox->currentIndex();
        if (value)
            d->interactor->setRoiBoolean(roi+1, 2);
    }
}

void medFiberBundlingToolBox::setRoiNotOperation (bool value)
{
    if (!d->view)
        return;

    if (d->interactor) {
        int roi = d->roiComboBox->currentIndex();
        if (value)
            d->interactor->setRoiBoolean(roi+1, 1);
    }
}

void medFiberBundlingToolBox::setRoiNullOperation (bool value)
{
    if (!d->view)
        return;

    if (d->interactor) {
        int roi = d->roiComboBox->currentIndex();
        if (value)
            d->interactor->setRoiBoolean(roi+1, 0);
    }
}

void medFiberBundlingToolBox::clear(void)
{
    // clear ROIs and related GUI elements
    clearRoi();

    d->bundlingModel->removeRows(0, d->bundlingModel->rowCount(QModelIndex()), QModelIndex());

    d->view = 0;
    d->data = 0;
}

void medFiberBundlingToolBox::update(dtkAbstractView *view)
{
    medToolBox::update(view);

    if (!view) {
        d->view = 0;
        d->data = 0;
        return;
    }

    d->interactor = qobject_cast<v3dViewFiberInteractor*>(view->dtkAbstractView::interactor("v3dViewFiberInteractor"));
    if (d->view == view) {
        if (d->interactor)
            this->setData (d->interactor->data()); // data may have changed
        return;
    }

    d->bundlingModel->removeRows(0, d->bundlingModel->rowCount(QModelIndex()), QModelIndex());

    if (d->interactor) {
        disconnect (this, SIGNAL(fiberSelectionValidated(const QString&, const QColor&)),
                    d->interactor, SLOT(validateSelection(QString,QColor)));
        disconnect (this, SIGNAL(fiberSelectionTagged()),
                    d->interactor, SLOT(tagSelection()));
        disconnect (this, SIGNAL(fiberSelectionReset()),
                    d->interactor, SLOT(resetSelection()));
    }

    d->view = qobject_cast<v3dView*>(view);

    if (d->interactor) {
        connect (this, SIGNAL(fiberSelectionValidated(const QString&, const QColor&)),
                 d->interactor, SLOT(validateSelection(const QString&, const QColor&)));
        connect (this, SIGNAL(fiberSelectionTagged()),
                 d->interactor, SLOT(tagSelection()));
        connect (this, SIGNAL(fiberSelectionReset()),
                 d->interactor, SLOT(resetSelection()));

        d->bundleBoxCheckBox->blockSignals (true);
        d->bundleBoxCheckBox->setChecked(d->interactor->property("BoxVisibility")=="true" );
        d->bundleBoxCheckBox->blockSignals (false);

        this->setData (d->interactor->data());
    }
}

void medFiberBundlingToolBox::changeBundlingItem(QStandardItem *item)
{
    if (!d->view)
        return;
    
    if (!d->interactor)
        return;
    
    QString itemOldName = item->data(Qt::UserRole+1).toString();
    
    if (itemOldName != item->text())
    {
        d->interactor->changeBundleName(itemOldName,item->text());
        item->setData(item->text(),Qt::UserRole+1);
    }
    
    d->interactor->setBundleVisibility(item->text(), item->checkState());
}

void medFiberBundlingToolBox::showBundling(bool show)
{
    if (d->view) {
        if (d->interactor)
            d->interactor->setAllBundlesVisibility(show);
    }
}

void medFiberBundlingToolBox::setBoxBooleanOperation(bool value)
{
    if (value)
        d->interactor->setBoxBooleanOperation(v3dViewFiberInteractor::Plus);
    else
        d->interactor->setBoxBooleanOperation(v3dViewFiberInteractor::Minus);
}

void medFiberBundlingToolBox::onDropSiteClicked()
{
    if (!d->view)
        return;

    QString roiFileName = QFileDialog::getOpenFileName(this, tr("Open ROI"), "", tr("Image file (*.*)"));

    if (roiFileName.isEmpty())
        return;

    medDataManager* mdm = medDataManager::instance();
    connect(mdm, SIGNAL(dataAdded(const medDataIndex &)), this, SLOT(importROI(const medDataIndex &)));
    mdm->importNonPersistent(roiFileName);
}

void medFiberBundlingToolBox::setImage(const QImage& thumbnail)
{
    d->dropOrOpenRoi->setPixmap(QPixmap::fromImage(thumbnail));
}


void medFiberBundlingToolBox::setInput(dtkAbstractData *data)
{
    this->setData(data);
}

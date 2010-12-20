#include "medToolBoxFiberBundling.h"

#include <dtkCore/dtkAbstractData.h>
#include <dtkCore/dtkAbstractDataFactory.h>
#include <dtkCore/dtkAbstractView.h>
#include <dtkCore/dtkAbstractViewInteractor.h>

#include <medGui/medDropSite.h>
#include <medCore/medDataManager.h>
#include <medSql/medDatabaseController.h>

class medToolBoxFiberBundlingPrivate
{
public:
    QListWidget  *bundlingList;
    QPushButton  *bundlingButtonVdt;
    QPushButton  *bundlingButtonTag;
    QPushButton  *bundlingButtonRst;
    QCheckBox    *bundlingShowCheckBox;
    QCheckBox    *bundleBoxCheckBox;
    QPushButton  *bundlingButtonAdd;
    
    medDropSite  *bundlingDropSite;
    
    dtkAbstractView *view;
    dtkAbstractData *data;
};

medToolBoxFiberBundling::medToolBoxFiberBundling(QWidget *parent) : medToolBox(parent), d(new medToolBoxFiberBundlingPrivate)
{
    d->view = 0;
    d->data = 0;
    
    QWidget *bundlingPage = new QWidget(this);
    
    d->bundlingDropSite = new medDropSite(bundlingPage);
    
    d->bundlingButtonTag = new QPushButton("Tag", bundlingPage);
    d->bundlingButtonAdd = new QPushButton("Add", bundlingPage);
    d->bundlingButtonRst = new QPushButton("Reset", bundlingPage);
    d->bundlingButtonVdt = new QPushButton("Validate", bundlingPage);
    
    QHBoxLayout *bundlingButtonsLayout = new QHBoxLayout;
    bundlingButtonsLayout->addWidget(d->bundlingButtonTag);
    bundlingButtonsLayout->addWidget(d->bundlingButtonAdd);
    bundlingButtonsLayout->addWidget(d->bundlingButtonRst);
    bundlingButtonsLayout->addWidget(d->bundlingButtonVdt);
    
    d->bundlingList = new QListWidget(bundlingPage);
    d->bundlingList->setAlternatingRowColors(true);
    
    d->bundlingShowCheckBox = new QCheckBox("Show all bundles", bundlingPage);
    d->bundleBoxCheckBox = new QCheckBox("Activate bundling box", bundlingPage);
        
    QVBoxLayout *bundlingLayout = new QVBoxLayout(bundlingPage);
    bundlingLayout->addWidget(d->bundlingDropSite);
    bundlingLayout->addWidget(d->bundleBoxCheckBox);
    bundlingLayout->addLayout(bundlingButtonsLayout);
    bundlingLayout->addWidget(d->bundlingList);
    bundlingLayout->addWidget(d->bundlingShowCheckBox);
    bundlingLayout->setAlignment(d->bundlingDropSite, Qt::AlignHCenter);

    connect (d->bundlingDropSite,      SIGNAL(objectDropped()),          this, SLOT (onObjectDropped()));
    connect (d->bundlingButtonVdt,     SIGNAL(clicked(void)),            this, SLOT (onBundlingButtonVdtClicked (void)));
    connect (d->bundlingShowCheckBox,  SIGNAL(toggled(bool)),            this, SIGNAL (showBundles (bool)));
    connect (d->bundleBoxCheckBox,     SIGNAL(toggled(bool)),            this, SIGNAL (bundlingBoxActivated (bool)));
    connect (d->bundlingButtonTag,     SIGNAL(clicked(void)),            this, SIGNAL (fiberSelectionTagged(void)));
    connect (d->bundlingButtonRst,     SIGNAL(clicked(void)),            this, SIGNAL (fiberSelectionReset(void)));
 
    this->setTitle("Fiber Bundling");
    this->setWidget(bundlingPage);
}

medToolBoxFiberBundling::~medToolBoxFiberBundling()
{
    delete d;
    d = NULL;
}

void medToolBoxFiberBundling::setInput(dtkAbstractData *data)
{
    qDebug() << __func__;
    
    if (!data)
        return;
    
    if (data->description()!="v3dDataFibers") {
        d->bundlingDropSite->clear();
        return;
    }
    
    if (d->data==data)
        return;
    
    d->data = data;
    
    d->bundlingDropSite->setPixmap( QPixmap::fromImage(data->thumbnail()) );
    
    d->bundlingList->clear();
    if (data->hasMetaData("BundleList")) {
        QStringList bundleNames = data->metaDataValues("BundleList");
        foreach(QString name, bundleNames)
            this->addBundle(name);
    }
}

void medToolBoxFiberBundling::onObjectDropped(void)
{
    qDebug() << __func__;
    
    medDataIndex index = d->bundlingDropSite->index();
    
    if (!index.isValid())
        return;
    
    dtkAbstractData *data = medDataManager::instance()->data (index);
    if (!data) {
        data = medDatabaseController::instance()->read(index);
        if (data)
            medDataManager::instance()->insert(index, data);
    }
    
    this->setInput(data);
}

void medToolBoxFiberBundling::onBundlingButtonVdtClicked (void)
{
    bool ok;
    QString text = QInputDialog::getText(this, tr("Enter bundle name"),
                                         tr(""), QLineEdit::Normal, tr(""), &ok);
    
    if (ok && !text.isEmpty()) {
        // if (!d->bundlingList->contains (name)) // should popup a warning
        // d->bundlingList->addItem (text);
        this->addBundle (text);
        emit fiberSelectionValidated (text);
    }
}

void medToolBoxFiberBundling::addBundle (QString name)
{
    d->bundlingList->addItem (name);
}

void medToolBoxFiberBundling::clear(void)
{
    d->bundlingDropSite->clear();
    d->bundlingList->clear();
}

void medToolBoxFiberBundling::update(dtkAbstractView *view)
{
    if (d->view==view)
        return;
    
    if (d->view) {
        if (dtkAbstractViewInteractor *interactor = d->view->interactor ("v3dViewFiberInteractor")) {
            disconnect (this, SIGNAL(fiberSelectionValidated(QString)), interactor, SLOT(onSelectionValidated(QString)));
            disconnect (this, SIGNAL(fiberSelectionTagged()),    interactor, SLOT(onSelectionTagged()));
            disconnect (this, SIGNAL(fiberSelectionReset()),     interactor, SLOT(onSelectionReset()));
        }
    }
    
    if (!view)
        return;
    
    if (view->property ("Orientation")!="3D") // only interaction with 3D views is authorized
        return;
    
    d->view = view;
    
    if (dtkAbstractViewInteractor *interactor = view->interactor ("v3dViewFiberInteractor")) {
        connect (this, SIGNAL(fiberSelectionValidated(QString)), interactor, SLOT(onSelectionValidated(QString)));
        connect (this, SIGNAL(fiberSelectionTagged()),           interactor, SLOT(onSelectionTagged()));
        connect (this, SIGNAL(fiberSelectionReset()),            interactor, SLOT(onSelectionReset()));
    }
    
}
#include "medToolBoxFunctional.h"
#include "medToolBoxFunctionalCustom.h"
#include <dtkCore/dtkAbstractView.h>
#include <dtkCore/dtkAbstractDataImage.h>
#include <dtkCore/dtkAbstractData.h>
#include <medCore/medDataManager.h>
#include <medCore/medPluginManager.h>
#include <medGui/medToolBoxFactory.h>
#include <medGui/medToolBoxHeader.h>

class medToolBoxFunctionalPrivate
{
public:
    dtkAbstractView* view;

    QHash<QString, medToolBoxFunctionalCustom*> toolBoxes;

    medToolBoxFunctionalCustom *currentToolBox;

    QComboBox *functionalMethodCombo;

    QVBoxLayout *functionalLayout;
};

medToolBoxFunctional::medToolBoxFunctional(QWidget *parent) : medToolBox(parent), d(new medToolBoxFunctionalPrivate)
{
    d->currentToolBox = 0;

    QWidget* functionalPage = new QWidget(this);
    QLabel* methodLabel = new QLabel("Algorithm:", functionalPage);
    d->functionalMethodCombo = new QComboBox(functionalPage);

    QHBoxLayout *methodLayout = new QHBoxLayout;
    methodLayout->addWidget(methodLabel);
    methodLayout->addWidget(d->functionalMethodCombo);

    d->functionalLayout = new QVBoxLayout(functionalPage);
    d->functionalLayout->addLayout(methodLayout);

    // --- Setting up custom toolboxes list ---
    d->functionalMethodCombo->addItem("Choose...");

    foreach(QString handler, medPluginManager::instance()->handlers("functional")) {
        if (medToolBoxFactory::instance()->functionalToolBoxes().contains(handler))
            d->functionalMethodCombo->addItem(handler);
    }

    foreach(QString toolbox, medToolBoxFactory::instance()->functionalToolBoxes())
        d->functionalMethodCombo->addItem(toolbox, toolbox);

    connect(d->functionalMethodCombo, SIGNAL(activated(const QString&)), this, SLOT(onToolBoxChosen(const QString&)));

    this->setTitle("Functional");
    this->addWidget(functionalPage);
}

medToolBoxFunctional::~medToolBoxFunctional(void)
{
    delete d;
    d = NULL;
}

void medToolBoxFunctional::onToolBoxChosen(const QString& id)
{
    medToolBoxFunctionalCustom *toolbox = NULL;

    if (d->toolBoxes.contains (id))
        toolbox = d->toolBoxes[id];
    else {
        toolbox = medToolBoxFactory::instance()->createCustomFunctionalToolBox(id, this);
        if (toolbox) {
            toolbox->setStyleSheet("medToolBoxBody {border:none}");
            toolbox->header()->hide();

            connect (toolbox, SIGNAL (success()), this, SIGNAL (success()));
            connect (toolbox, SIGNAL (failure()), this, SIGNAL (failure()));

            d->toolBoxes[id] = toolbox;
        }
    }

    if(!toolbox) {
        if (d->currentToolBox) {
            d->currentToolBox->hide();
            d->functionalLayout->removeWidget ( d->currentToolBox );
            //emit removeToolBox(d->customToolBox);
            d->currentToolBox = 0;
        }
        return;
    }

    toolbox->setFunctionalToolBox(this);

    //get rid of old toolBox
    if (d->currentToolBox) {
        d->currentToolBox->hide();
        d->functionalLayout->removeWidget ( d->currentToolBox );
        d->currentToolBox = 0;
    }

    toolbox->show();
    d->functionalLayout->addWidget ( toolbox );

    d->currentToolBox = toolbox;
}

dtkAbstractData *medToolBoxFunctional::output(void) const
{
    if (d->currentToolBox)
        return d->currentToolBox->output();

    return NULL;
}


void medToolBoxFunctional::clear(void)
{
    foreach (medToolBoxFunctionalCustom *tb, d->toolBoxes)
        tb->deleteLater();

    d->toolBoxes.clear();

    d->currentToolBox = 0;

    d->functionalMethodCombo->blockSignals (true);
    d->functionalMethodCombo->setCurrentIndex (0);
    d->functionalMethodCombo->blockSignals (false);
}

void medToolBoxFunctional::onViewAdded(dtkAbstractView *view)
{
    dtkAbstractData *data = static_cast<dtkAbstractData*>(view->data());
    d->currentToolBox->setData(data);
}

void medToolBoxFunctional::update (dtkAbstractView *view)
{
}

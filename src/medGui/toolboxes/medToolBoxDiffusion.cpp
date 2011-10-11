/* medToolBoxDiffusion.cpp ---
 *
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Fri Feb 19 09:06:02 2010 (+0100)
 * Version: $Id$
 * Last-Updated: Wed Mar  2 16:57:02 2011 (+0100)
 *           By: Jaime Garcia_Guevara
 *     Update #: 133
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

#include <medPluginManager.h>
#include <medViewManager.h>

//#include <medDatabaseController.h>

#include <medDropSite.h>
#include <medToolBoxTab.h>
#include <medProgressionStack.h>
#include <medToolBoxFactory.h>
#include <medToolBoxHeader.h>

#include "medToolBoxDiffusion.h"
#include "medToolBoxDiffusionCustom.h"

class medToolBoxDiffusionPrivate
{
public:

    QHash<QString, medToolBoxDiffusionCustom*> toolBoxes;

    medToolBoxDiffusionCustom *currentToolBox;

    QComboBox *tractographyMethodCombo;

    QVBoxLayout *tractographyLayout;
};

medToolBoxDiffusion::medToolBoxDiffusion(QWidget *parent) : medToolBox(parent), d(new medToolBoxDiffusionPrivate)
{
    d->currentToolBox = 0;

    // /////////////////////////////////////////////////////////////////
    // Tractography page
    // /////////////////////////////////////////////////////////////////
    QWidget *tractographyPage        = new QWidget(this);
    QLabel  *tractographyMethodLabel = new QLabel( tr("Algorithm:"), tractographyPage );
    d->tractographyMethodCombo = new QComboBox(tractographyPage);

    QHBoxLayout *tractographyMethodLayout = new QHBoxLayout;
    tractographyMethodLayout->addWidget(tractographyMethodLabel);
    tractographyMethodLayout->addWidget(d->tractographyMethodCombo);

    d->tractographyLayout = new QVBoxLayout(tractographyPage);
    d->tractographyLayout->addLayout(tractographyMethodLayout);

    // --- Setting up custom toolboxes list ---
    d->tractographyMethodCombo->addItem( tr( "Choose..." ) );
    medToolBoxFactory* tbFactory = medToolBoxFactory::instance();
    foreach(QString toolbox, tbFactory->diffusionToolBoxes())
    {
        QPair<const QString&, const QString&> pair = tbFactory->diffusionToolBoxDetailsFromId(toolbox);
        d->tractographyMethodCombo->addItem(pair.first, toolbox);
    }

    connect(d->tractographyMethodCombo, SIGNAL(activated(int)), this, SLOT(onToolBoxChosen(int)));

    this->setTitle( tr("Tractography") );
    this->addWidget(tractographyPage);
}

medToolBoxDiffusion::~medToolBoxDiffusion(void)
{
    delete d;
    d = NULL;
}

void medToolBoxDiffusion::onToolBoxChosen(int id)
{
    medToolBoxDiffusionCustom *toolbox = NULL;
    //get identifier for toolbox.
    QString identifier = d->tractographyMethodCombo->itemData(id).toString();
    if (d->toolBoxes.contains (identifier))
        toolbox = d->toolBoxes[identifier];
    else {
        toolbox = medToolBoxFactory::instance()->createCustomDiffusionToolBox(identifier, this);
        if (toolbox) {
            toolbox->setStyleSheet("medToolBoxBody {border:none}");
            toolbox->header()->hide();

            connect (toolbox, SIGNAL (success()), this, SIGNAL (success()));
            connect (toolbox, SIGNAL (failure()), this, SIGNAL (failure()));

            d->toolBoxes[identifier] = toolbox;
        }
    }

    if(!toolbox) {
        if (d->currentToolBox) {
            d->currentToolBox->hide();
            d->tractographyLayout->removeWidget ( d->currentToolBox );
            //emit removeToolBox(d->customToolBox);
            d->currentToolBox = 0;
            this->setAboutPluginVisibility(false);
        }
        return;
    }

    toolbox->setDiffusionToolBox(this);

    //get rid of old toolBox
    if (d->currentToolBox) {
        d->currentToolBox->hide();
        d->tractographyLayout->removeWidget ( d->currentToolBox );
        d->currentToolBox = 0;
    }

    //set the aboutPlugin button.
    dtkPlugin* plugin = toolbox->plugin();
    this->setAboutPluginButton(plugin);
    this->setAboutPluginVisibility(true);

    toolbox->show();
    d->tractographyLayout->addWidget ( toolbox );

    d->currentToolBox = toolbox;
}

dtkAbstractData *medToolBoxDiffusion::output(void) const
{
    if (d->currentToolBox)
        return d->currentToolBox->output();

    return NULL;
}


void medToolBoxDiffusion::clear(void)
{
    foreach (medToolBoxDiffusionCustom *tb, d->toolBoxes)
        tb->deleteLater();

    d->toolBoxes.clear();

    d->currentToolBox = 0;

    d->tractographyMethodCombo->blockSignals (true);
    d->tractographyMethodCombo->setCurrentIndex (0);
    d->tractographyMethodCombo->blockSignals (false);
    this->setAboutPluginVisibility(false);
}


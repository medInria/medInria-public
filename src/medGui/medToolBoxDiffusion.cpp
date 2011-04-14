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

#include <medCore/medPluginManager.h>
#include <medCore/medViewManager.h>

//#include <medSql/medDatabaseController.h>

#include <medGui/medDropSite.h>
#include <medGui/medToolBoxTab.h>
#include <medGui/medProgressionStack.h>
#include <medGui/medToolBoxFactory.h>
#include <medGui/medToolBoxHeader.h>

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
    QWidget   *tractographyPage        = new QWidget(this);
    QLabel    *tractographyMethodLabel = new QLabel("Algorithm:", tractographyPage);
    d->tractographyMethodCombo = new QComboBox(tractographyPage);
    
    QHBoxLayout *tractographyMethodLayout = new QHBoxLayout;
    tractographyMethodLayout->addWidget(tractographyMethodLabel);
    tractographyMethodLayout->addWidget(d->tractographyMethodCombo);
    
    d->tractographyLayout = new QVBoxLayout(tractographyPage);
    d->tractographyLayout->addLayout(tractographyMethodLayout);
    
    // --- Setting up custom toolboxes list ---
    d->tractographyMethodCombo->addItem("Choose...");
    
    foreach(QString handler, medPluginManager::instance()->handlers("tractography")) {
        if (medToolBoxFactory::instance()->diffusionToolBoxes().contains(handler))
            d->tractographyMethodCombo->addItem(handler);
    }
    
    foreach(QString toolbox, medToolBoxFactory::instance()->diffusionToolBoxes())
        d->tractographyMethodCombo->addItem(toolbox, toolbox);
    
    connect(d->tractographyMethodCombo, SIGNAL(activated(const QString&)), this, SLOT(onToolBoxChosen(const QString&)));
    
    this->setTitle("Tractography");
    this->addWidget(tractographyPage);
}

medToolBoxDiffusion::~medToolBoxDiffusion(void)
{
    delete d;
    d = NULL;
}

void medToolBoxDiffusion::onToolBoxChosen(const QString & id)
{
    medToolBoxDiffusionCustom *toolbox = NULL;
    
    if (d->toolBoxes.contains (id))
        toolbox = d->toolBoxes[id];
    else {
        toolbox = medToolBoxFactory::instance()->createCustomDiffusionToolBox(id, this);
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
            d->tractographyLayout->removeWidget ( d->currentToolBox );
            //emit removeToolBox(d->customToolBox);
            d->currentToolBox = 0;
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
}


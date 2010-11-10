/* medViewerToolBoxConfiguration.cpp --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Fri Feb 19 09:06:02 2010 (+0100)
 * Version: $Id$
 * Last-Updated: Wed Nov 10 10:58:26 2010 (+0100)
 *           By: Julien Wintz
 *     Update #: 15
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#include "medViewerToolBoxConfiguration.h"

class medViewerToolBoxConfigurationPrivate
{
public:
    QComboBox *combo;
};

medViewerToolBoxConfiguration::medViewerToolBoxConfiguration(QWidget *parent) : medToolBox(parent), d(new medViewerToolBoxConfigurationPrivate)
{
    QWidget *central = new QWidget(this);

    d->combo = new QComboBox(this);
    d->combo->setFocusPolicy(Qt::NoFocus);

    QHBoxLayout *layout = new QHBoxLayout(central);
    layout->addWidget(d->combo);

    this->setTitle("Configuration");
    this->setWidget(central);

    connect(d->combo, SIGNAL(currentIndexChanged(QString)), this, SIGNAL(configurationChanged(QString)));
}

medViewerToolBoxConfiguration::~medViewerToolBoxConfiguration(void)
{
    delete d;

    d = NULL;
}

void medViewerToolBoxConfiguration::addConfiguration(QString name)
{
    d->combo->addItem(name);
}

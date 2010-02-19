/* medToolBoxLayout.cpp --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Fri Feb 19 09:06:02 2010 (+0100)
 * Version: $Id$
 * Last-Updated: Fri Feb 19 10:07:06 2010 (+0100)
 *           By: Julien Wintz
 *     Update #: 22
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#include "medLayoutChooser.h"
#include "medToolBoxLayout.h"

class medToolBoxLayoutPrivate
{
public:
};

medToolBoxLayout::medToolBoxLayout(QWidget *parent) : medToolBox(parent), d(new medToolBoxLayoutPrivate)
{
    medLayoutChooser *layoutChooser = new medLayoutChooser(this);

    QWidget *layoutToolBoxChooserPage = new QWidget(this);

    QHBoxLayout *layoutToolBoxChooserPageLayout = new QHBoxLayout(layoutToolBoxChooserPage);
    layoutToolBoxChooserPageLayout->setContentsMargins(0, 3, 0, 0);
    layoutToolBoxChooserPageLayout->setAlignment(Qt::AlignHCenter | Qt::AlignCenter);
    layoutToolBoxChooserPageLayout->addWidget(layoutChooser);

    medToolBoxTab *layoutToolBoxTab = new medToolBoxTab(this);
    layoutToolBoxTab->addTab(new QWidget, "Single");
    layoutToolBoxTab->addTab(new QWidget, "Multi");
    layoutToolBoxTab->addTab(layoutToolBoxChooserPage, "Custom");

    this->setTitle("Layout");
    this->setWidget(layoutToolBoxTab);

    connect(layoutToolBoxTab, SIGNAL(currentChanged(int)), this, SIGNAL(modeChanged(int)));
    connect(layoutChooser, SIGNAL(selected(int,int)), this, SIGNAL(split(int,int)));
}

medToolBoxLayout::~medToolBoxLayout(void)
{
    delete d;

    d = NULL;
}

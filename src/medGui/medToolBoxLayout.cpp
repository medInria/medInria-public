/* medToolBoxLayout.cpp --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Fri Feb 19 09:06:02 2010 (+0100)
 * Version: $Id$
 * Last-Updated: Fri May 21 15:26:02 2010 (+0200)
 *           By: Julien Wintz
 *     Update #: 55
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
    QWidget *customPage = new QWidget(this);

    QPushButton *buttonA = new QPushButton(customPage);
    buttonA->setFlat(true);
    buttonA->setIcon(QIcon(":/icons/medViewContainerCustomA.png"));
    buttonA->setIconSize(QSize(32, 32));
    connect(buttonA, SIGNAL(clicked()), this, SLOT(onButtonAClicked()));

    QPushButton *buttonB = new QPushButton(customPage);
    buttonB->setFlat(true);
    buttonB->setIcon(QIcon(":/icons/medViewContainerCustomB.png"));
    buttonB->setIconSize(QSize(32, 32));
    connect(buttonB, SIGNAL(clicked()), this, SLOT(onButtonBClicked()));

    QPushButton *buttonC = new QPushButton(customPage);
    buttonC->setFlat(true);
    buttonC->setIcon(QIcon(":/icons/medViewContainerCustomC.png"));
    buttonC->setIconSize(QSize(32, 32));
    connect(buttonC, SIGNAL(clicked()), this, SLOT(onButtonCClicked()));

    QPushButton *buttonD = new QPushButton(customPage);
    buttonD->setFlat(true);
    buttonD->setIcon(QIcon(":/icons/medViewContainerCustomD.png"));
    buttonD->setIconSize(QSize(32, 32));
    connect(buttonD, SIGNAL(clicked()), this, SLOT(onButtonDClicked()));

    QPushButton *buttonE = new QPushButton(customPage);
    buttonE->setFlat(true);
    buttonE->setIcon(QIcon(":/icons/medViewContainerCustomE.png"));
    buttonE->setIconSize(QSize(32, 32));
    connect(buttonE, SIGNAL(clicked()), this, SLOT(onButtonEClicked()));

    medLayoutChooser *layoutChooser = new medLayoutChooser(this);

    QWidgetAction *action = new QWidgetAction(this);
    action->setDefaultWidget(layoutChooser);

    QMenu *menu = new QMenu(this);
    menu->addAction(action);

    QPushButton *buttonF = new QPushButton(customPage);
    buttonF->setFlat(true);
    buttonF->setIcon(QIcon(":/icons/medViewContainerCustomF.png"));
    buttonF->setIconSize(QSize(32, 32));
    buttonF->setMenu(menu);

    QGridLayout *customPageLayout = new QGridLayout(customPage);
    customPageLayout->setAlignment(Qt::AlignHCenter | Qt::AlignCenter);
    customPageLayout->addWidget(buttonA, 0, 0);
    customPageLayout->addWidget(buttonB, 0, 1);
    customPageLayout->addWidget(buttonC, 0, 2);
    customPageLayout->addWidget(buttonD, 1, 0);
    customPageLayout->addWidget(buttonE, 1, 1);
    customPageLayout->addWidget(buttonF, 1, 2);

    medToolBoxTab *layoutToolBoxTab = new medToolBoxTab(this);
    layoutToolBoxTab->addTab(new QWidget, "Single");
    layoutToolBoxTab->addTab(new QWidget, "Multi");
    layoutToolBoxTab->addTab(customPage, "Custom");

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

void medToolBoxLayout::onButtonAClicked(void)
{
    emit presetClicked(1);
}

void medToolBoxLayout::onButtonBClicked(void)
{
    emit presetClicked(2);
}

void medToolBoxLayout::onButtonCClicked(void)
{
    emit presetClicked(3);
}

void medToolBoxLayout::onButtonDClicked(void)
{
    emit presetClicked(4);
}

void medToolBoxLayout::onButtonEClicked(void)
{
    emit presetClicked(5);
}

/* medViewerToolBoxLayout.cpp ---
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Fri Feb 19 09:06:02 2010 (+0100)
 * Version: $Id$
 * Last-Updated: Wed Nov 10 10:28:27 2010 (+0100)
 *           By: Julien Wintz
 *     Update #: 56
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */


#include "medToolBoxTab.h"
#include "medLayoutChooser.h"
#include "medViewerToolBoxLayout.h"

class medViewerToolBoxLayoutPrivate
{
public:
    medToolBoxTab *layoutToolBoxTab;
};

medViewerToolBoxLayout::medViewerToolBoxLayout(QWidget *parent) : medToolBox(parent), d(new medViewerToolBoxLayoutPrivate)
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

    d->layoutToolBoxTab = new medToolBoxTab(this);
    d->layoutToolBoxTab->addTab(new QWidget, "Single");
    d->layoutToolBoxTab->addTab(new QWidget, "Multi");
    d->layoutToolBoxTab->addTab(customPage, "Custom");
	
	d->layoutToolBoxTab->setTabToolTip(0,"Single display");
	d->layoutToolBoxTab->setTabToolTip(1,"Multi-display (divide screen when an image is added to the view)");
	d->layoutToolBoxTab->setTabToolTip(2,"Custom display setup");

    d->layoutToolBoxTab->setCurrentIndex(1);

    this->setTitle("Layout");
    this->setTabWidget(d->layoutToolBoxTab);

    connect(d->layoutToolBoxTab, SIGNAL(currentChanged(int)), this, SLOT(modeChanged(int)));
    connect(layoutChooser, SIGNAL(selected(int,int)), this, SIGNAL(split(int,int)));
}

medViewerToolBoxLayout::~medViewerToolBoxLayout(void)
{
    delete d;

    d = NULL;
}

void medViewerToolBoxLayout::onButtonAClicked(void)
{
    emit presetClicked(1);
}

void medViewerToolBoxLayout::onButtonBClicked(void)
{
    emit presetClicked(2);
}

void medViewerToolBoxLayout::onButtonCClicked(void)
{
    emit presetClicked(3);
}

void medViewerToolBoxLayout::onButtonDClicked(void)
{
    emit presetClicked(4);
}

void medViewerToolBoxLayout::onButtonEClicked(void)
{
    emit presetClicked(5);
}

void medViewerToolBoxLayout::modeChanged(int tabIndex)
{
    switch (tabIndex){
    case 1: 
        emit (modeChanged("Multi"));
        break;
    case 2:
        emit (modeChanged("Custom"));
        break;
    case 0:
    default:
        emit (modeChanged("Single"));
    }            
}

void medViewerToolBoxLayout::clear()
{
    blockSignals(true);
    d->layoutToolBoxTab->setCurrentIndex(0);
    blockSignals(false);
}

void medViewerToolBoxLayout::setTab(const QString &name)
{
    if (name == "Single")
        d->layoutToolBoxTab->setCurrentIndex(0);
    else if (name == "Custom")
        d->layoutToolBoxTab->setCurrentIndex(2);
    else if (name == "Multi")
        d->layoutToolBoxTab->setCurrentIndex(1);
}

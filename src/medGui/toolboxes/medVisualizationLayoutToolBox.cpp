/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include "medToolBoxTab.h"
#include "medLayoutChooser.h"
#include "medVisualizationLayoutToolBox.h"

class medVisualizationLayoutToolBoxPrivate
{
public:
    medToolBoxTab *layoutToolBoxTab;
};

medVisualizationLayoutToolBox::medVisualizationLayoutToolBox(QWidget *parent) : medToolBox(parent), d(new medVisualizationLayoutToolBoxPrivate)
{
    QWidget *customPage = new QWidget(this);

    QPushButton *buttonA = new QPushButton(customPage);
    buttonA->setFlat(true);
    buttonA->setIcon(QIcon(":/icons/medCustomViewContainerA.png"));
    buttonA->setIconSize(QSize(32, 32));
    connect(buttonA, SIGNAL(clicked()), this, SLOT(onButtonAClicked()));

    QPushButton *buttonB = new QPushButton(customPage);
    buttonB->setFlat(true);
    buttonB->setIcon(QIcon(":/icons/medCustomViewContainerB.png"));
    buttonB->setIconSize(QSize(32, 32));
    connect(buttonB, SIGNAL(clicked()), this, SLOT(onButtonBClicked()));

    QPushButton *buttonC = new QPushButton(customPage);
    buttonC->setFlat(true);
    buttonC->setIcon(QIcon(":/icons/medCustomViewContainerC.png"));
    buttonC->setIconSize(QSize(32, 32));
    connect(buttonC, SIGNAL(clicked()), this, SLOT(onButtonCClicked()));

    QPushButton *buttonD = new QPushButton(customPage);
    buttonD->setFlat(true);
    buttonD->setIcon(QIcon(":/icons/medCustomViewContainerD.png"));
    buttonD->setIconSize(QSize(32, 32));
    connect(buttonD, SIGNAL(clicked()), this, SLOT(onButtonDClicked()));

    QPushButton *buttonE = new QPushButton(customPage);
    buttonE->setFlat(true);
    buttonE->setIcon(QIcon(":/icons/medCustomViewContainerE.png"));
    buttonE->setIconSize(QSize(32, 32));
    connect(buttonE, SIGNAL(clicked()), this, SLOT(onButtonEClicked()));

    medLayoutChooser *layoutChooser = new medLayoutChooser(this);

    QWidgetAction *action = new QWidgetAction(this);
    action->setDefaultWidget(layoutChooser);

    QMenu *menu = new QMenu(this);
    menu->addAction(action);

    QPushButton *buttonF = new QPushButton(customPage);
    buttonF->setFlat(true);
    buttonF->setIcon(QIcon(":/icons/medCustomViewContainerF.png"));
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
    d->layoutToolBoxTab->addTab(new QWidget(d->layoutToolBoxTab), tr("Single"));
    d->layoutToolBoxTab->addTab(new QWidget(d->layoutToolBoxTab), tr("Multi"));
    d->layoutToolBoxTab->addTab(customPage, tr("Custom"));

    d->layoutToolBoxTab->setTabToolTip(0,tr("Display data in the same view (use overlay)"));
    d->layoutToolBoxTab->setTabToolTip(1,tr("Display data in multiple views (one data = one view)"));
    d->layoutToolBoxTab->setTabToolTip(2,tr("Use a custom layout for multiple views"));

    d->layoutToolBoxTab->setCurrentIndex(1);

    this->setTitle(tr("Layout"));
    this->setTabWidget(d->layoutToolBoxTab);

    connect(d->layoutToolBoxTab, SIGNAL(currentChanged(int)), this, SLOT(modeChanged(int)));
    connect(layoutChooser, SIGNAL(selected(int,int)), this, SIGNAL(split(int,int)));
}

medVisualizationLayoutToolBox::~medVisualizationLayoutToolBox(void)
{
    delete d;

    d = NULL;
}

void medVisualizationLayoutToolBox::onButtonAClicked(void)
{
    emit presetClicked(1);
}

void medVisualizationLayoutToolBox::onButtonBClicked(void)
{
    emit presetClicked(2);
}

void medVisualizationLayoutToolBox::onButtonCClicked(void)
{
    emit presetClicked(3);
}

void medVisualizationLayoutToolBox::onButtonDClicked(void)
{
    emit presetClicked(4);
}

void medVisualizationLayoutToolBox::onButtonEClicked(void)
{
    emit presetClicked(5);
}

void medVisualizationLayoutToolBox::modeChanged(int tabIndex)
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

void medVisualizationLayoutToolBox::clear()
{
    blockSignals(true);
    d->layoutToolBoxTab->setCurrentIndex(0);
    blockSignals(false);
}

void medVisualizationLayoutToolBox::setTab(const QString &name)
{
    if (name == "Single")
        d->layoutToolBoxTab->setCurrentIndex(0);
    else if (name == "Custom")
        d->layoutToolBoxTab->setCurrentIndex(2);
    else if (name == "Multi")
        d->layoutToolBoxTab->setCurrentIndex(1);
}

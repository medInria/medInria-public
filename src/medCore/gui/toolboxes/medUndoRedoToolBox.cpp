/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medUndoRedoToolBox.h>

#include <medAbstractUndoRedoProcess.h>
#include <medToolBoxFactory.h>


class medUndoRedoToolBoxPrivate
{
public:
    QPushButton * undoButton;
    QPushButton * redoButton;
    QPushButton * resetButton;
    QListWidget * commandStack;
    QIcon arrowCurrentStep;
    int currentStep;

    medAbstractUndoRedoProcess * undoRedoProcess;
};

medUndoRedoToolBox::medUndoRedoToolBox(QWidget *parent) : medToolBox(parent), d(new medUndoRedoToolBoxPrivate)
{
    // Undo/redo Buttons
    d->undoButton = new QPushButton(QIcon(":undoRedoRegistration/icons/ArrowDown.png"),tr("Undo"),this);
    d->redoButton = new QPushButton(QIcon(":undoRedoRegistration/icons/ArrowUp.png"),tr("Redo"),this);
    d->undoButton->setEnabled(false);
    d->redoButton->setEnabled(false);
    d->resetButton = new QPushButton(tr("Reset"),this);
    d->resetButton->setEnabled(false);

    connect(d->undoButton,SIGNAL(clicked()),this,SLOT(onUndo()));
    connect(d->redoButton,SIGNAL(clicked()),this,SLOT(onRedo()));
    connect(d->resetButton,SIGNAL(clicked()),this,SLOT(onReset()));
    //connect(d->resetButton,SIGNAL(clicked()),registrationFactory::instance(),SLOT(reset()));

    d->arrowCurrentStep = QIcon(":undoRedoRegistration/icons/BlueArrowRight.png");
    d->currentStep = -1;

    // Transformation Stack
    d->commandStack = new QListWidget(this);

    QVBoxLayout *layoutButtonUndoRedo = new QVBoxLayout;
    layoutButtonUndoRedo->addWidget(d->redoButton);
    layoutButtonUndoRedo->addWidget(d->undoButton);
    layoutButtonUndoRedo->addWidget(d->resetButton);
    QHBoxLayout *layoutButtonsStack = new QHBoxLayout;
    layoutButtonsStack->addLayout(layoutButtonUndoRedo);
    layoutButtonsStack->addWidget(d->commandStack);
    d->commandStack->setFixedSize(180,160);

    QWidget * layoutSection = new QWidget(this);
    layoutSection->setLayout(layoutButtonsStack);

    addWidget(layoutSection);

    this->setTitle(tr("Undo/Redo stack"));

    //connect(registrationFactory::instance(),SIGNAL(transformationAdded(int,QString)),this,SLOT(addTransformationIntoList(int, QString)));
    //connect(registrationFactory::instance(),SIGNAL(transformationStackReset()),this,SLOT(onTransformationStackReset()));

}

medUndoRedoToolBox::~medUndoRedoToolBox(void)
{
    delete d;

    d = NULL;
}

bool medUndoRedoToolBox::registered(void)
{
    return medToolBoxFactory::instance()-> registerToolBox<medUndoRedoToolBox>();
}

void medUndoRedoToolBox::onUndo()
{
    if ((d->currentStep >= 0) && (d->currentStep < d->commandStack->count()))
    {
        updatePositionArrow(d->currentStep + 1);
        emit undoRequested();
        d->redoButton->setEnabled(true);
    }

    if (d->currentStep>=d->commandStack->count()){
        d->undoButton->setEnabled(false);
        d->resetButton->setEnabled(false);
    }
}

void medUndoRedoToolBox::onRedo()
{
    if (d->currentStep > 0)
    {
        updatePositionArrow(d->currentStep - 1);
        emit redoRequested();
        d->undoButton->setEnabled(true);
        d->resetButton->setEnabled(true);
    }

    if (d->currentStep <= 0)
        d->redoButton->setEnabled(false);
}

void medUndoRedoToolBox::onReset(void)
{
    d->currentStep=-1;
    while (d->commandStack->count() != 0)
    {
        QListWidgetItem * tmp = d->commandStack->takeItem(0);
        delete tmp;
    }
    d->undoButton->setEnabled(false);
    d->redoButton->setEnabled(false);
    d->resetButton->setEnabled(false);

    //registrationFactory::instance()->getItkRegistrationFactory()->Modified();
    //d->m_UndoRedo->generateOutput();
    //this->parentToolBox()->handleOutput(medRegistrationSelectorToolBox::reset);
}

void medUndoRedoToolBox::addCommand(QString methodParameters)
{
    if ((d->currentStep >= 0) && (d->currentStep < d->commandStack->count()))
        d->commandStack->item(d->currentStep)->setIcon(QIcon());
    for(int k = d->currentStep-1;k>=0;k--)
    {
        QListWidgetItem * tmp = d->commandStack->takeItem(k);
        delete tmp;
    }
    d->currentStep = 0;

    //TODO GPR: to check
    //        d->transformationStack->insertItem(d->currentStep,QString::number(d->transformationStack->count()+1)+ ". " +
    //                                           d->m_UndoRedo->currentProcess()->name().remove(" "));
    d->commandStack->item(d->currentStep)->setToolTip(methodParameters);
    d->commandStack->item(d->currentStep)->setIcon(d->arrowCurrentStep);
    d->commandStack->item(d->currentStep)->setForeground(QColor(0,200,0));

    d->undoButton->setEnabled(true);
    d->resetButton->setEnabled(true);
    d->redoButton->setEnabled(false);
}

void medUndoRedoToolBox::updatePositionArrow(int newStep)
{
    if ((d->commandStack->count() != 0) && (newStep >= 0))
    {
        if (!(d->commandStack->count()==d->currentStep)){
            d->commandStack->item(d->currentStep)->setIcon(QIcon());
        }
        d->currentStep = newStep;
        if (!(d->commandStack->count()==d->currentStep)){
            d->commandStack->item(d->currentStep)->setIcon(d->arrowCurrentStep);
            d->commandStack->item(d->currentStep)->setForeground(QColor(0,200,0));
        }
    }
    for(int k = d->currentStep-1;k>=0;k--){
        d->commandStack->item(k)->setForeground(QColor(255,255,255));
    }
}

void medUndoRedoToolBox::onSuccess()
{
    qDebug() << "void medUndoRedoToolBox::onSuccess()";
}

//void medUndoRedoToolBox::setRegistrationToolBox(medRegistrationSelectorToolBox *toolbox)
//{
//    medRegistrationAbstractToolBox::setRegistrationToolBox(toolbox);
////    toolbox->setUndoRedoProcess(d->m_UndoRedo);
////    connect(this->parentToolBox(),SIGNAL(success()),this,SLOT(onRegistrationSuccess()));
//}



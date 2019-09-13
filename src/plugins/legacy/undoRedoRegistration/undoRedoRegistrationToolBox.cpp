/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2019. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <dtkCoreSupport/dtkAbstractProcess.h>
#include <dtkCoreSupport/dtkSmartPointer.h>

#include <QtGui>

#include <medAbstractDataFactory.h>
#include <medAbstractData.h>
#include <medAbstractView.h>
#include <medAbstractImageData.h>
#include <medToolBoxFactory.h>
#include <medRegistrationSelectorToolBox.h>

#include <registrationFactory.h>

#include <undoRedoRegistration.h>
#include <undoRedoRegistrationToolBox.h>

class undoRedoRegistrationToolBoxPrivate
{
public:
    QPushButton * undoButton;
    QPushButton * redoButton;
    QPushButton * resetButton;
    QListWidget * transformationStack;
    QIcon arrowCurrentStep;
    int currentStep;
    dtkSmartPointer<undoRedoRegistration> m_UndoRedo;
};

undoRedoRegistrationToolBox::undoRedoRegistrationToolBox(QWidget *parent) : medRegistrationAbstractToolBox(parent), d(new undoRedoRegistrationToolBoxPrivate)
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
    connect(d->resetButton,SIGNAL(clicked()),registrationFactory::instance(),SLOT(reset()));

    d->arrowCurrentStep = QIcon(":undoRedoRegistration/icons/BlueArrowRight.png");
    d->currentStep = -1;

    d->m_UndoRedo = new undoRedoRegistration();
    // Transformation Stack
    d->transformationStack = new QListWidget(this);

    QVBoxLayout *layoutButtonUndoRedo = new QVBoxLayout;
    layoutButtonUndoRedo->addWidget(d->redoButton);
    layoutButtonUndoRedo->addWidget(d->undoButton);
    layoutButtonUndoRedo->addWidget(d->resetButton);
    QHBoxLayout *layoutButtonsStack = new QHBoxLayout;
    layoutButtonsStack->addLayout(layoutButtonUndoRedo);
    layoutButtonsStack->addWidget(d->transformationStack);
    d->transformationStack->setFixedSize(180,160);

    QWidget * layoutSection = new QWidget(this);
    layoutSection->setLayout(layoutButtonsStack);

    addWidget(layoutSection);

    connect(registrationFactory::instance(),SIGNAL(transformationAdded(int,QString)),this,SLOT(addTransformationIntoList(int, QString)));
    connect(registrationFactory::instance(),SIGNAL(transformationStackReset()),this,SLOT(onTransformationStackReset()));
}

undoRedoRegistrationToolBox::~undoRedoRegistrationToolBox(void)
{
    delete d;

    d = nullptr;
}

bool undoRedoRegistrationToolBox::registered(void)
{
    return medToolBoxFactory::instance()-> registerToolBox<undoRedoRegistrationToolBox>();
}

void undoRedoRegistrationToolBox::onUndo()
{
    if(!this->parentToolBox())
        return;

    if ((d->currentStep >= 0) && (d->currentStep < d->transformationStack->count()))
    {
        updatePositionArrow(d->currentStep+1);
        d->m_UndoRedo->undo();
        this->parentToolBox()->handleOutput(medRegistrationSelectorToolBox::undo);
        d->redoButton->setEnabled(true);
    }

    if (d->currentStep>=d->transformationStack->count()){
        d->undoButton->setEnabled(false);
        d->resetButton->setEnabled(false);
    }
}

void undoRedoRegistrationToolBox::onRedo()
{
    if(!this->parentToolBox())
        return;
    if (d->currentStep>0)
    {
        updatePositionArrow(d->currentStep-1);
        d->m_UndoRedo->redo();
        this->parentToolBox()->handleOutput(medRegistrationSelectorToolBox::redo,d->transformationStack->item(d->currentStep)->text().remove(" "));
        d->undoButton->setEnabled(true);
        d->resetButton->setEnabled(true);
    }

    if (d->currentStep<=0)
        d->redoButton->setEnabled(false);
}

void undoRedoRegistrationToolBox::onTransformationStackReset(void)
{
    d->currentStep=-1;
    while (d->transformationStack->count()!=0)
    {
        QListWidgetItem * tmp = d->transformationStack->takeItem(0);
        delete tmp;
    }
    d->undoButton->setEnabled(false);
    d->redoButton->setEnabled(false);
    d->resetButton->setEnabled(false);

    registrationFactory::instance()->getItkRegistrationFactory()->Modified();
    d->m_UndoRedo->generateOutput();
    this->parentToolBox()->handleOutput(medRegistrationSelectorToolBox::reset);
}

void undoRedoRegistrationToolBox::addTransformationIntoList(int i, QString methodParameters){
    if (i!=-1){
        if ((d->currentStep >= 0) && (d->currentStep < d->transformationStack->count()))
            d->transformationStack->item(d->currentStep)->setIcon(QIcon());
        for(int k = d->currentStep-1;k>=0;k--)
        {
            QListWidgetItem * tmp = d->transformationStack->takeItem(k);
            delete tmp;
        }
        d->currentStep = 0;

        d->transformationStack->insertItem(d->currentStep,QString::number(d->transformationStack->count()+1)+ ". " + this->parentToolBox()->getNameOfCurrentAlgorithm().remove(" "));
        d->transformationStack->item(d->currentStep)->setToolTip(methodParameters);
        d->transformationStack->item(d->currentStep)->setIcon(d->arrowCurrentStep);
        d->transformationStack->item(d->currentStep)->setForeground(QColor(0,200,0));
    }
    d->undoButton->setEnabled(true);
    d->resetButton->setEnabled(true);
    d->redoButton->setEnabled(false);
}

void undoRedoRegistrationToolBox::updatePositionArrow(int newStep){

    if ((d->transformationStack->count() != 0) && (newStep >= 0))
    {
        if (!(d->transformationStack->count()==d->currentStep)){
            d->transformationStack->item(d->currentStep)->setIcon(QIcon());
        }
        d->currentStep = newStep;
        if (!(d->transformationStack->count()==d->currentStep)){
            d->transformationStack->item(d->currentStep)->setIcon(d->arrowCurrentStep);
            d->transformationStack->item(d->currentStep)->setForeground(QColor(0,200,0));
        }
    }
    for(int k = d->currentStep-1;k>=0;k--){
        d->transformationStack->item(k)->setForeground(QColor(255,255,255));
    }
}

void undoRedoRegistrationToolBox::setRegistrationToolBox(medRegistrationSelectorToolBox *toolbox)
{
    medRegistrationAbstractToolBox::setRegistrationToolBox(toolbox);
    toolbox->setUndoRedoProcess(d->m_UndoRedo);
    connect(this->parentToolBox(),SIGNAL(success()),this,SLOT(onRegistrationSuccess()));
}

void undoRedoRegistrationToolBox::onRegistrationSuccess()
{
    registrationFactory::instance()->addTransformation(static_cast<itkProcessRegistration*>(this->parentToolBox()->process())->getTransform(),static_cast<itkProcessRegistration*>(this->parentToolBox()->process())->getTitleAndParameters());
    registrationFactory::instance()->getItkRegistrationFactory()->Modified();
    d->m_UndoRedo->generateOutput(true,this->parentToolBox()->process());
    this->parentToolBox()->handleOutput();
}

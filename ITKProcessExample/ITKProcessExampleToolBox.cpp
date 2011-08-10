
#include "ITKProcessExample.h"
#include "ITKProcessExampleToolBox.h"

#include <dtkCore/dtkAbstractDataFactory.h>
#include <dtkCore/dtkAbstractData.h>

#include <medCore/medAbstractDataImage.h>

#include <dtkCore/dtkAbstractProcessFactory.h>
#include <dtkCore/dtkAbstractProcess.h>
#include <dtkCore/dtkAbstractViewFactory.h>
#include <dtkCore/dtkAbstractView.h>
#include <dtkCore/dtkAbstractViewInteractor.h>

#include <medCore/medRunnableProcess.h>
#include <medCore/medJobManager.h>

#include <medGui/medToolBoxFactory.h>
#include <medGui/medToolBoxFiltering.h>
#include <medGui/medToolBoxFilteringCustom.h>
#include <medGui/medProgressionStack.h>

#include <QtGui>

class ITKProcessExampleToolBoxPrivate
{
public:
        QLineEdit *variance;
        dtkAbstractProcess *process;
        medProgressionStack * progression_stack;
};

ITKProcessExampleToolBox::ITKProcessExampleToolBox(QWidget *parent) : medToolBoxFilteringCustom(parent), d(new ITKProcessExampleToolBoxPrivate)
{
      // Parameters:
      
      QLabel *varianceLabel = new QLabel("variance : ");
      d->variance = new QLineEdit("1.0");

      QHBoxLayout *varianceLayout = new QHBoxLayout();
      varianceLayout->addWidget(varianceLabel);
      varianceLayout->addWidget(d->variance);

      // Run button:

      QPushButton *runButton = new QPushButton(tr("Run"));

      // Principal layout:

      QWidget *widget = new QWidget(this);

      d->progression_stack = new medProgressionStack(widget);

      QVBoxLayout *layprinc = new QVBoxLayout();
      layprinc->addLayout(varianceLayout);
      layprinc->addWidget(runButton);
      layprinc->addWidget(d->progression_stack);

      widget->setLayout(layprinc);

      // Main toolbox:
      this->setTitle("ITK Gaussian Smoothing");
      this->addWidget(widget);

      connect(runButton, SIGNAL(clicked()), this, SLOT(run()));

}

ITKProcessExampleToolBox::~ITKProcessExampleToolBox(void)
{
    delete d;
    
    d = NULL;
}

bool ITKProcessExampleToolBox::registered(void)
{
    return medToolBoxFactory::instance()->registerCustomFilteringToolBox("ITK GaussianBlur Filter Example",
                                                                           createITKProcessExampleToolBox);
}


dtkAbstractData* ITKProcessExampleToolBox::processOutput(void)
{
        if(!d->process)
            return NULL;

        return d->process->output();
}


void ITKProcessExampleToolBox::run(void)
{
    if(!this->parent())
        return;

    d->process = dtkAbstractProcessFactory::instance()->create("ITKProcessExampleGaussianBlur");

    if(!this->parent()->data())
        return;

    d->process->setInput(this->parent()->data());

    d->process->setParameter(d->variance->text().toDouble(),0);

    medRunnableProcess *runProcess = new medRunnableProcess;
    runProcess->setProcess (d->process);

    d->progression_stack->addJobItem(runProcess, "Progress:");

    connect (runProcess, SIGNAL (success  (QObject*)),  this, SIGNAL (success ()));
    connect (runProcess, SIGNAL (failure  (QObject*)),  this, SIGNAL (failure ()));

    medJobManager::instance()->registerJobItem(runProcess);
    QThreadPool::globalInstance()->start(dynamic_cast<QRunnable*>(runProcess));

//    if(d->process->update()==0)
//        emit success();
//    else
//        emit failure();

}

medToolBoxFilteringCustom *createITKProcessExampleToolBox(QWidget *parent)
{
    return new ITKProcessExampleToolBox(parent);
}

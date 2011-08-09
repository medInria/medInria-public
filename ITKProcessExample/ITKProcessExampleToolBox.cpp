
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
        QLineEdit *sigma;
        QLineEdit *maxKernelWidth;
        dtkAbstractProcess *process;
        medProgressionStack * progression_stack;
};

ITKProcessExampleToolBox::ITKProcessExampleToolBox(QWidget *parent) : medToolBoxFilteringCustom(parent), d(new ITKProcessExampleToolBoxPrivate)
{
      // Parameters:
      
      QLabel *sigmaLabel = new QLabel("sigma : ");
      d->sigma = new QLineEdit("0");

      QHBoxLayout *sigmaBox = new QHBoxLayout();
      sigmaBox->addWidget(sigmaLabel);
      sigmaBox->addWidget(d->sigma);

      QLabel *maxKernelWidthLabel = new QLabel("max kernel width : ");
      d->maxKernelWidth = new QLineEdit("0");

      QHBoxLayout *betaBox = new QHBoxLayout();
      betaBox->addWidget(maxKernelWidthLabel);
      betaBox->addWidget(d->maxKernelWidth);

      QVBoxLayout *parametersLayout = new QVBoxLayout();
      parametersLayout->addLayout(sigmaBox);
      parametersLayout->addLayout(betaBox);      

      // Run button:

      QPushButton *runButton = new QPushButton(tr("Run"));

      // Principal layout:

      QWidget *widget = new QWidget(this);

      d->progression_stack = new medProgressionStack(widget);

      QVBoxLayout *layprinc = new QVBoxLayout();
      layprinc->addLayout(parametersLayout);
      layprinc->addWidget(runButton);
      layprinc->addWidget(d->progression_stack);

      widget->setLayout(layprinc);

      // Main toolbox:
      this->setTitle("ITK GaussianBlur - Example of filter");
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

    d->process->setParameter(d->sigma->text().toDouble(),0);
    d->process->setParameter(d->maxKernelWidth->text().toDouble(),1);

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

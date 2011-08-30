
#include "itkAddConstantToImage.h"
#include "itkAddConstantToImageFilterToolBox.h"

#include <dtkCore/dtkAbstractDataFactory.h>
#include <dtkCore/dtkAbstractData.h>

#include <medCore/medAbstractDataImage.h>

#include <dtkCore/dtkAbstractProcessFactory.h>
#include <dtkCore/dtkAbstractProcess.h>
#include <dtkCore/dtkAbstractViewFactory.h>
#include <dtkCore/dtkAbstractView.h>
#include <dtkCore/dtkAbstractViewInteractor.h>

#include <medRunnableProcess.h>
#include <medJobManager.h>

#include <medToolBoxFactory.h>
#include <medToolBoxFiltering.h>
#include <medToolBoxFilteringCustom.h>
#include <medProgressionStack.h>

#include <QtGui>

class itkAddConstantToImageFilterToolBoxPrivate
{
public:
        QDoubleSpinBox *constant;
        dtkAbstractProcess *process;
        medProgressionStack * progression_stack;
};

itkAddConstantToImageFilterToolBox::itkAddConstantToImageFilterToolBox(QWidget *parent) : medToolBoxFilteringCustom(parent), d(new itkAddConstantToImageFilterToolBoxPrivate)
{
      // Parameters:

      QLabel * constantLabel = new QLabel("Constant : ");
      d->constant = new QDoubleSpinBox;
      d->constant->setValue(100.0);
      d->constant->setMaximum(10000);
      d->constant->setMinimum(-10000);

      QHBoxLayout *constantLayout = new QHBoxLayout();
      constantLayout->addWidget(constantLabel);
      constantLayout->addWidget(d->constant);

      // Run button:
      QPushButton *runButton = new QPushButton(tr("Run"));

      // Principal layout:
      QWidget *widget = new QWidget(this);

      d->progression_stack = new medProgressionStack(widget);

      QVBoxLayout *layout = new QVBoxLayout();
      layout->addLayout(constantLayout);
      layout->addWidget(runButton);
      layout->addWidget(d->progression_stack);

      widget->setLayout(layout);

      // Main toolbox:
      this->setTitle("ITK Add constant to image");
      this->addWidget(widget);

      connect(runButton, SIGNAL(clicked()), this, SLOT(run()));

}

itkAddConstantToImageFilterToolBox::~itkAddConstantToImageFilterToolBox(void)
{
    delete d;

    d = NULL;
}

bool itkAddConstantToImageFilterToolBox::registered(void)
{
    return medToolBoxFactory::instance()->registerCustomFilteringToolBox("ITK Add Constant to Image Filter",
                                                                           createitkAddConstantToImageFilterToolBox);
}


dtkAbstractData* itkAddConstantToImageFilterToolBox::processOutput(void)
{
        if(!d->process)
            return NULL;

        return d->process->output();
}


void itkAddConstantToImageFilterToolBox::run(void)
{
    if(!this->parent())
        return;

    d->process = dtkAbstractProcessFactory::instance()->create("itkAddConstantToImage");

    if(!this->parent()->data())
        return;

    d->process->setInput(this->parent()->data());

    d->process->setParameter(d->constant->value(),0);

    medRunnableProcess *runProcess = new medRunnableProcess;
    runProcess->setProcess (d->process);

    d->progression_stack->addJobItem(runProcess, tr("Progress:"));

    connect (runProcess, SIGNAL (success  (QObject*)),  this, SIGNAL (success ()));
    connect (runProcess, SIGNAL (failure  (QObject*)),  this, SIGNAL (failure ()));

    medJobManager::instance()->registerJobItem(runProcess);
    QThreadPool::globalInstance()->start(dynamic_cast<QRunnable*>(runProcess));

}

medToolBoxFilteringCustom *createitkAddConstantToImageFilterToolBox(QWidget *parent)
{
    return new itkAddConstantToImageFilterToolBox(parent);
}

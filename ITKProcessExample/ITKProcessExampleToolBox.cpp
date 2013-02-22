
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

#include <medRunnableProcess.h>
#include <medJobManager.h>
#include <medPluginManager.h>

#include <medToolBoxFactory.h>
#include <medFilteringSelectorToolBox.h>
#include <medFilteringAbstractToolBox.h>
#include <medProgressionStack.h>

#include <QtGui>

class ITKProcessExampleToolBoxPrivate
{
public:
    QLineEdit *variance;
    dtkAbstractProcess *process;
    medProgressionStack * progression_stack;
};

ITKProcessExampleToolBox::ITKProcessExampleToolBox(QWidget *parent) : medFilteringAbstractToolBox(parent), d(new ITKProcessExampleToolBoxPrivate)
{
    // Parameters:

    QLabel *varianceLabel = new QLabel("Sigma : ");
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

    //enable about plugin. Construtor called after the plugin has been registered, go ahead call it.
    medPluginManager* pm = medPluginManager::instance();
    dtkPlugin* plugin = pm->plugin(
                "ITKProcessExamplePlugin");
    setAboutPluginButton(plugin);
    setAboutPluginVisibility(true);

    connect(runButton, SIGNAL(clicked()), this, SLOT(run()));

}

ITKProcessExampleToolBox::~ITKProcessExampleToolBox(void)
{
    delete d;

    d = NULL;
}

bool ITKProcessExampleToolBox::registered(void)
{
    return medToolBoxFactory::instance()->registerToolBox
            <ITKProcessExampleToolBox>(
                "itkGaussianBlurExample",
                "ITK Gaussian Blur Filter Example",
                "Applis Gaussian Blur to a view",
                QStringList()<<"filtering");
}


dtkAbstractData* ITKProcessExampleToolBox::processOutput(void)
{
    if(!d->process)
        return NULL;

    return d->process->output();
}


void ITKProcessExampleToolBox::run(void)
{
    if(!this->parentToolBox())
        return;

    d->process = dtkAbstractProcessFactory::instance()->create("ITKProcessExampleGaussianBlur");

    if(!this->parentToolBox()->data())
        return;

    d->process->setInput(this->parentToolBox()->data());

    d->process->setParameter(d->variance->text().toDouble(),0);

    medRunnableProcess *runProcess = new medRunnableProcess;
    runProcess->setProcess (d->process);

    d->progression_stack->addJobItem(runProcess, tr("Progress:"));

    connect (runProcess, SIGNAL (success  (QObject*)),  this, SIGNAL (success ()));
    connect (runProcess, SIGNAL (failure  (QObject*)),  this, SIGNAL (failure ()));

    medJobManager::instance()->registerJobItem(runProcess);
    QThreadPool::globalInstance()->start(dynamic_cast<QRunnable*>(runProcess));

}


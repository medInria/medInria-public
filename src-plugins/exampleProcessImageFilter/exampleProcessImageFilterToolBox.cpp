/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <exampleProcessImageFilter.h>
#include <exampleProcessImageFilterToolBox.h>

#include <v3dView.h>
#include <vtkImageView2D.h>


#include <dtkCore/dtkAbstractProcessFactory.h>

#include <dtkCore/dtkGlobal.h>

#include <medDataIndex.h>
#include <medDataManager.h>
#include <medViewManager.h>
#include <medAbstractView.h>

#include <medDropSite.h>
#include <medDiffusionSelectorToolBox.h>

#include <medProgressionStack.h>
#include <medRunnableProcess.h>
#include <medJobManager.h>
#include <medPluginManager.h>

class exampleProcessImageFilterToolBoxPrivate
{
public:
    QWidget *widget;

    //QDoubleSpinBox *x;
    QSpinBox *x;
    QSpinBox *y;
    QSpinBox *z;
    QSpinBox *a;

    QCheckBox *option_A;
    QCheckBox *option_B;
    QCheckBox *option_C;

    QPushButton *ButtonRun;

public:
    medDropSite *site;
    medDropSite *siteB;
    dtkAbstractProcess *process;

    medProgressionStack *progression_stack;
};

exampleProcessImageFilterToolBox::exampleProcessImageFilterToolBox(QWidget *parent): medDiffusionAbstractToolBox(parent), d(new exampleProcessImageFilterToolBoxPrivate)
{
    d->process= 0;

    d->widget = new QWidget(this);

    d->site = new medDropSite(d->widget);
    d->siteB = new medDropSite(d->widget);
    QHBoxLayout *dropSites_layout = new QHBoxLayout;
    dropSites_layout->addWidget(d->site);
    dropSites_layout->addWidget(d->siteB);

    d->x = new QSpinBox(this); d->x->setMaximum(500); d->x->setSingleStep(10);//QDoubleSpinBox(this);
    d->y = new QSpinBox(this); d->y->setMaximum(500); d->y->setSingleStep(10);
    d->z = new QSpinBox(this); d->z->setMaximum(500); d->z->setSingleStep(10);
    d->a = new QSpinBox(this); d->a->setMaximum(4000); d->a->setSingleStep(100);

    QHBoxLayout *coord_layout = new QHBoxLayout;
    coord_layout->addWidget(d->x);
    coord_layout->addWidget(d->y);
    coord_layout->addWidget(d->z);

    d->option_A = new QCheckBox("Mask", this);
    d->option_B = new QCheckBox("Add", this);
    d->option_C = new QCheckBox("Connected threshold", this);

    QVBoxLayout *options_layout = new QVBoxLayout;
    options_layout->addWidget(d->option_A);
    options_layout->addWidget(d->option_B);
    options_layout->addWidget(d->option_C);

    QButtonGroup *group = new QButtonGroup(this);
    group->addButton(d->option_A);
    group->addButton(d->option_B);
    group->addButton(d->option_C);

    d->ButtonRun = new QPushButton("Run", this);
    d->ButtonRun->setCheckable(true);
    d->ButtonRun->setChecked(false);

//#ifdef Progression stack
    d->progression_stack = new medProgressionStack(d->widget);
    QHBoxLayout *progressStackLayout = new QHBoxLayout;
    progressStackLayout->addWidget(d->progression_stack);
//#endif /* Progression stack */

    QVBoxLayout *layout = new QVBoxLayout(d->widget);
    layout->addLayout(dropSites_layout);
    //layout->addWidget(d->siteB);
    layout->addLayout(coord_layout);
    layout->addWidget(d->a);
    layout->addLayout(options_layout);
    layout->addWidget(d->ButtonRun);

//#ifdef Progression stack
    layout->addLayout(progressStackLayout);
    layout->setAlignment(progressStackLayout, Qt::AlignHCenter);
//#endif /* Progression stack */

    layout->setAlignment(d->ButtonRun, Qt::AlignHCenter);
    layout->setAlignment(dropSites_layout, Qt::AlignHCenter);
    //layout->setAlignment(d->siteB, Qt::AlignHCenter);

    this->setTitle("Example Process Plugin Image Filtering");
    this->addWidget(d->widget);

    connect(d->site, SIGNAL(objectDropped()), this, SLOT(onObjectDropped()));
    connect(d->siteB, SIGNAL(objectDropped()), this, SLOT(onObjectDropped()));
    connect(d->x, SIGNAL(valueChanged(int)), this, SLOT(onXValueChanged(int)));
    connect(d->y, SIGNAL(valueChanged(int)), this, SLOT(onYValueChanged(int)));
    connect(d->z, SIGNAL(valueChanged(int)), this, SLOT(onZValueChanged(int)));
    connect(d->a, SIGNAL(valueChanged(int)), this, SLOT(onAValueChanged(int)));
    connect(group, SIGNAL(buttonClicked(QAbstractButton *)), this, SLOT(onOptionClicked(QAbstractButton *)));
    connect(d->ButtonRun, SIGNAL(clicked()), this, SLOT(run()));
}

exampleProcessImageFilterToolBox::~exampleProcessImageFilterToolBox()
{
    delete d;

    d = NULL;
}

QString exampleProcessImageFilterToolBox::description() const
{
    return QString("exampleProcessImageFilter");
}

dtkAbstractProcess *exampleProcessImageFilterToolBox::create()
{
    return new exampleProcessImageFilter;
}

void exampleProcessImageFilterToolBox::onXValueChanged(int value)
{
    Q_UNUSED(value);

    if(exampleProcessImageFilter *process = dynamic_cast<exampleProcessImageFilter *>(d->process))
        process->setPoint(d->x->value(), d->y->value(), d->z->value());
}

void exampleProcessImageFilterToolBox::onYValueChanged(int value)
{
    Q_UNUSED(value);

    if(exampleProcessImageFilter *process = dynamic_cast<exampleProcessImageFilter *>(d->process))
        process->setPoint(d->x->value(), d->y->value(), d->z->value());
}

void exampleProcessImageFilterToolBox::onZValueChanged(int value)
{
    Q_UNUSED(value);

    if(exampleProcessImageFilter *process = dynamic_cast<exampleProcessImageFilter *>(d->process))
        process->setPoint(d->x->value(), d->y->value(), d->z->value());
}

void exampleProcessImageFilterToolBox::onAValueChanged(int value)
{
    if(exampleProcessImageFilter *process = dynamic_cast<exampleProcessImageFilter *>(d->process))
        process->setLowerThreshold(value);
}

void exampleProcessImageFilterToolBox::onOptionClicked(QAbstractButton *button)
{
    exampleProcessImageFilter *process = dynamic_cast<exampleProcessImageFilter *>(d->process);

    if(!process)
        return;

    if(button == d->option_A)
        process->setOption(exampleProcessImageFilter::optionA);

    if(button == d->option_B)
        process->setOption(exampleProcessImageFilter::optionB);

    if(button == d->option_C)
        process->setOption(exampleProcessImageFilter::optionC);

}

void exampleProcessImageFilterToolBox::onObjectDropped()
{
    dtkAbstractData *in_A = NULL;
    dtkAbstractData *in_B = NULL;
    medAbstractView * viewed;

//    QSharedPointer<dtkAbstractData> data =  data.data(); // return dtkAbstractData *

//    medDataIndex in_A_index = parent()->dataIndex();//method added in medDiffusionSelectorToolBox MEdINria/src/medGUI 4 this to work, index from medDiffusionSelectorToolBox dropsite
    medDataIndex in_A_index = d->site->index();
    medDataIndex in_B_index = d->siteB->index();

    d->process= dtkAbstractProcessFactory::instance()->create("exampleProcessImageFilter");

    if(in_A_index.isValid()) {
        in_A = (medDataManager::instance()->data(in_A_index)).data();
        viewed = medViewManager::instance()->views(in_A_index).first();
        QVector3D pos = viewed->position();
        qDebug() << pos;

//        v3dView *v3dviewed = dynamic_cast<v3dView*>(viewed);
//        view2D =v3dviewed->view2d();
//        double * point=view2D->GetCurrentPoint();

//   qDebug() << point;

     }




    if(in_B_index.isValid())
        in_B = (medDataManager::instance()->data(in_B_index)).data();

    if(!in_A || !in_B)
        return;

    //exampleProcessImageFilter *process = dynamic_cast<exampleProcessImageFilter *>(this->process());

    if (d->process) {
       d->process->setInput(in_A, 0);
       d->process->setInput(in_B, 1);
    }
}


medDiffusionAbstractToolBox *createExampleProcessImageFilterToolBox(QWidget * w)
{
    return new exampleProcessImageFilterToolBox(w);
}

void exampleProcessImageFilterToolBox::run()
{
    exampleProcessImageFilter *process = dynamic_cast<exampleProcessImageFilter *>(d->process);

//#ifdef Progression stack
    if (!process)
        return;

    medRunnableProcess *runProcess = new medRunnableProcess;
    runProcess->setProcess (process);

    d->progression_stack->addJobItem(runProcess, "Progress:");

    connect (runProcess, SIGNAL (success  (QObject*)),  this, SIGNAL (success ()));
    connect (runProcess, SIGNAL (failure  (QObject*)),  this, SIGNAL (failure ()));
    connect (runProcess, SIGNAL (cancelled (QObject*)), this, SIGNAL (failure ()));

    medJobManager::instance()->registerJobItem(runProcess);
//#endif /* Progression stack */
   //  process->update();
}

dtkAbstractData* exampleProcessImageFilterToolBox::output() const
{
    if (d->process && d->process->output()) {
        // fill out Pateint/Study/Series for proper inclusion in the database
//        if (d->data) {
//	    if (d->data->hasMetaData (medMetaDataKeys::PatientName.key()))
//	        d->process->output()->setMetaData (medMetaDataKeys::PatientName.key(), d2->data->metaDataValues (medMetaDataKeys::PatientName.key())[0]);
//	    if (d->data->hasMetaData (medMetaDataKeys::StudyDescription.key()))
//	        d->process->output()->setMetaData (medMetaDataKeys::StudyDescription.key(), d2->data->metaDataValues (medMetaDataKeys::StudyDescription.key())[0]);
//	    d->process->output()->setMetaData (medMetaDataKeys::SeriesDescription.key(), d2->process->property ("RequiredOutput"));
//	}
        return d->process->output();
    }
    return NULL;
}

dtkPlugin* exampleProcessImageFilterToolBox::plugin() const {
    medPluginManager* pm = medPluginManager::instance();
    return pm->plugin("exampleProcessImageFilterPlugin");
}

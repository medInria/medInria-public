/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include "medTimeLineToolBox.h"

#include <iomanip>
#include <cmath>
#include <sstream>

#include <QListWidget>
#include <QList>
#include <QMouseEvent>

#include <dtkCore/dtkAbstractView.h>
#include <dtkLog/dtkLog.h>
#include <dtkCore/dtkSmartPointer.h>

#include <med4DAbstractViewInteractor.h>
#include <medToolBoxTab.h>
#include <medButton.h>




class medTimeLineToolBoxPrivate
{
public:
    QSlider     *timeSlider;
    medButton *playSequencesButton;
    medButton *nextFrameButton;
    medButton *previousFrameButton;
    medButton *stopButton;

    QList <QAction*> actionlist;
    QList< dtkSmartPointer<dtkAbstractView> > views;
    QList<med4DAbstractViewInteractor*> interactors;
    QTimeLine *timeLine;
    QSpinBox *spinBox;
    QLabel *labelmin;
    QLabel *labelmax;
    QLabel *labelcurr;
    QLabel *labelspeed;
    double minTime;
    double minTimeStep;
    double maxTime;

    QPixmap playIcon;
};

medTimeLineToolBox::medTimeLineToolBox(QWidget *parent) : medToolBox(parent), d(new medTimeLineToolBoxPrivate)
{
    QWidget *box = new QWidget (this);
    d->labelmin = new QLabel(this);
    d->labelmax = new QLabel(this);
    d->labelcurr = new QLabel(this);
    d->labelspeed = new QLabel(this);
    d->labelspeed->setText("Speed %: ");
    d->timeSlider = new QSlider (Qt::Horizontal, this);
    d->timeSlider->setRange (0, 100);
    d->timeSlider->setValue (0);
    d->timeSlider->setTracking( false );
    d->timeSlider->setToolTip(tr("Follow The Sequence"));

    QStringList validDataTypes;
    validDataTypes << "itkDataImageChar4"
            << "itkDataImageUChar4"
            << "itkDataImageShort4"
            << "itkDataImageUShort4"
            << "itkDataImageInt4"
            << "itkDataImageUInt4"
            << "itkDataImageLong4"
            << "itkDataImageULong4"
            << "itkDataImageFloat4"
            << "itkDataImageDouble4"
               << "vtkDataMesh4D";

    setValidDataTypes(validDataTypes);

    d->playIcon = QPixmap(":/icons/play.png");

    d->playSequencesButton = new medButton(this,d->playIcon,
                                           tr("Play Sequence"));

    d->nextFrameButton = new medButton(this,":/icons/forward.png",
                                       tr("Next Frame"));

    d->previousFrameButton = new medButton(this,":/icons/backward.png",
                                               tr("Previous Frame"));

    d->stopButton = new medButton(this,":/icons/stop.png",
                                  tr("Stop Sequence"));

    d->timeLine = new QTimeLine(1000, this);
    d->timeLine->setLoopCount(0);
    d->timeLine->setCurveShape (QTimeLine::LinearCurve);

    d->spinBox = new QSpinBox(this);
    d->spinBox->setRange(1,5000);
    d->spinBox->setSingleStep(10);
    d->spinBox->setValue(100);
    d->spinBox->setToolTip(tr("Control the display speed"));

    connect(d->timeLine, SIGNAL(frameChanged(int)), d->timeSlider, SLOT(setValue(int)));

    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->addWidget( d->previousFrameButton,1,Qt::AlignHCenter);
    buttonLayout->addWidget (d->playSequencesButton,1,Qt::AlignHCenter);
    buttonLayout->addWidget( d->nextFrameButton,1,Qt::AlignHCenter);


    buttonLayout->addWidget( d->stopButton,1,Qt::AlignHCenter);

    QHBoxLayout *labelLayout = new QHBoxLayout();
    labelLayout->addWidget( d->labelmin);
    labelLayout->addStretch();
    labelLayout->addWidget( d->labelcurr);
    labelLayout->addStretch();
    labelLayout->addWidget( d->labelmax);

    QHBoxLayout *topLayout = new QHBoxLayout();

    topLayout->addStretch();
    topLayout->addWidget(d->labelspeed);
    topLayout->addWidget(d->spinBox);

    QVBoxLayout* boxlayout = new QVBoxLayout ();
    boxlayout->addLayout(topLayout);
    boxlayout->addLayout( buttonLayout );
    boxlayout->addWidget (d->timeSlider);
    boxlayout->addLayout(labelLayout);

    d->actionlist.insert(0,new QAction("1",this));
    d->actionlist.insert(1,new QAction("5",this));
    d->actionlist.insert(2,new QAction("10",this));
    d->actionlist.insert(3,new QAction("25",this));
    d->actionlist.insert(4,new QAction("50",this));

    connect(d->timeSlider, SIGNAL(sliderMoved(int)), this, SLOT(onTimeChanged(int)));
    connect(d->timeSlider, SIGNAL(valueChanged(int)), this, SLOT(onTimeChanged(int)));
    connect(d->playSequencesButton, SIGNAL(triggered()), this, SLOT(onPlaySequences()));

    connect(d->nextFrameButton, SIGNAL(triggered()), this, SLOT(onNextFrame()));
    connect(d->previousFrameButton, SIGNAL(triggered()), this, SLOT(onPreviousFrame()));
    connect(d->spinBox, SIGNAL(valueChanged(int)),this, SLOT(onSpinBoxChanged(int)));
    connect(d->stopButton, SIGNAL(triggered()),this, SLOT(onStopButton()));

    this->setTitle(tr("Time Management"));
    box->setLayout (boxlayout);
    this->addWidget (box);

    d->minTime = 0.0;
    d->minTimeStep = 1.0;
    d->maxTime = 0.0;

    this->isViewAdded = false;

    this->hide();
}

medTimeLineToolBox::~medTimeLineToolBox(void)
{
    delete d;

    d = NULL;
}


void medTimeLineToolBox::onViewAdded (dtkAbstractView *view)
{
    if (!view)
        return;

    if (med4DAbstractViewInteractor *interactor = dynamic_cast<med4DAbstractViewInteractor*>(view->interactor ("v3dView4DInteractor")))
    {
        if (!d->views.contains (view))
            d->views.append (view);
        if (!d->interactors.contains (interactor))
            d->interactors.append (interactor);
        connect (view, SIGNAL ( dataAdded(dtkAbstractData*)),   this, SLOT (onDataAdded (dtkAbstractData*)));
    }

    this->updateRange();
    this->isViewAdded = true;
}

void medTimeLineToolBox::onDataAdded (dtkAbstractData *data)
{
    if (!data)
        return;

    this->updateRange();
}

void medTimeLineToolBox::onViewRemoved (dtkAbstractView *view)
{
    d->timeLine->stop();
    d->timeSlider->setValue(0);

    if (!view)
        return;

    if (!d->views.contains(view))
        return;

    if (med4DAbstractViewInteractor *interactor = dynamic_cast<med4DAbstractViewInteractor*>(view->interactor ("v3dView4DInteractor")))
    {
        d->views.removeOne (view);
        d->interactors.removeOne (interactor);
    }

    this->updateRange();
    this->isViewAdded = false;
}

void medTimeLineToolBox::AddInteractor (med4DAbstractViewInteractor* interactor)
{
    d->interactors.removeOne (interactor);
    d->interactors.append (interactor);
}
void medTimeLineToolBox::RemoveInteractor (med4DAbstractViewInteractor* interactor)
{
    d->interactors.removeOne (interactor);
}

void medTimeLineToolBox::update(dtkAbstractView *view)
{
    //JGG qDebug()<<"updating time tb";
    medToolBox::update(view);
}


void medTimeLineToolBox::onPlaySequences ()
{
    if ( this->isViewAdded)
    {
        this->updateRange();
        d->timeLine->setDuration((d->maxTime + d->minTimeStep)*(1000/(d->spinBox->value()/100.0)));
        if(d->timeLine->state() == QTimeLine::NotRunning)
        {
            d->timeLine->start();
            d->playSequencesButton->setIcon (QPixmap(":/icons/pause.png"));
            d->playSequencesButton->setToolTip( tr("Pause Sequence"));
        }
        else if(d->timeLine->state() == QTimeLine::Paused )
        {
            d->timeLine->resume();
            d->playSequencesButton->setIcon (QPixmap(":/icons/pause.png"));
            d->playSequencesButton->setToolTip( tr("Pause Sequence"));
        }
        else if(d->timeLine->state() == QTimeLine::Running)
        {
            d->timeLine->setPaused(true);
            d->playSequencesButton->setIcon (QPixmap(":/icons/play.png"));
            d->playSequencesButton->setToolTip( tr("Play Sequence"));
        }
    }
}

void medTimeLineToolBox::onStopButton ()
{
    if (d->timeLine->state() == QTimeLine::Running)
    {
        d->playSequencesButton->setIcon (QPixmap(":/icons/play.png"));
        d->playSequencesButton->setToolTip( tr("Play Sequence"));
    }
    d->timeLine->stop();
    d->timeSlider->setValue(0);
}

void medTimeLineToolBox::onNextFrame ()
{
    if ( this->isViewAdded)
	d->timeSlider->setValue(d->timeSlider->value()+1);
}
void medTimeLineToolBox::onPreviousFrame ()
{
    if ( this->isViewAdded)
	d->timeSlider->setValue(d->timeSlider->value()-1);
}

void medTimeLineToolBox::onTimeChanged (int val)
{
    double time = this->getTimeFromSliderValue (val);
    for (int i=0; i<d->interactors.size(); i++)
    {
        d->interactors[i]->setCurrentTime (time);
    }

    d->labelcurr->setText( DoubleToQString(( time ) / (d->spinBox->value()/100.0)) + QString(" sec") );
}

void medTimeLineToolBox::onSpinBoxChanged(int time)
{
    this->updateRange();

    d->timeLine->setDuration((d->maxTime + d->minTimeStep)*(1000/(time/100.0)));
}

double medTimeLineToolBox::getTimeFromSliderValue (int s)
{
    double value = d->minTime + (double)(s) * (d->minTimeStep);
    return value;
}

unsigned int medTimeLineToolBox::getSliderValueFromTime (double t)
{
    unsigned int value = std::ceil ((t - d->minTime) / (d->minTimeStep));
    return value;
}

void medTimeLineToolBox::updateRange()
{
    if (!d->interactors.size())
        return;
    double mintime = 3000;
    double maxtime = -3000;
    double mintimestep = 3000;

    for (int i=0; i<d->interactors.size(); i++)
    {
        double range[2]={0,0};
        d->interactors[i]->sequencesRange (range);
        mintimestep = std::min (mintimestep, d->interactors[i]->sequencesMinTimeStep ());
        mintime = std::min (mintime, range[0]);
        maxtime = std::max (maxtime, range[1]);
    }

    unsigned int numberofsteps = std::ceil ((maxtime - mintime) / (mintimestep) + 1.0);

    d->timeSlider->setRange (0, numberofsteps - 1);
    d->timeLine->setFrameRange(d->timeSlider->minimum(), d->timeSlider->maximum() );

    d->minTime = mintime;
    d->minTimeStep = mintimestep;
    d->maxTime = maxtime;

    d->timeLine->setDuration((maxtime+mintimestep)*1000);

    d->labelmin->setText( DoubleToQString(( mintime ) / (d->spinBox->value()/100.0)) + QString(" sec"));
    d->labelmax->setText( DoubleToQString(( maxtime ) / (d->spinBox->value()/100.0)) + QString(" sec"));
}

QString medTimeLineToolBox::DoubleToQString (double val)
{
    std::ostringstream strs;
    strs <<  std::fixed << std::setprecision(2)<< val;
    std::string str = strs.str();
    return QString(str.c_str());
}


void medTimeLineToolBox::mouseReleaseEvent ( QMouseEvent *  mouseEvent)
{
    if(mouseEvent->button() == Qt::RightButton)
    {
        QMenu *menu = new QMenu(this);

        QAction *actionNotify = new QAction(tr("Set Speed Increment : "),
                                            this);
        actionNotify->setDisabled(true);

        menu->addAction(actionNotify);
        for (int i = 0; i < d->actionlist.size() ; i++)
        {
            connect(d->actionlist[i], SIGNAL(triggered()), this, SLOT(onStepIncreased()));
            menu->addAction(d->actionlist[i]);

        }

        menu->exec(mouseEvent->globalPos());
    }
}
void medTimeLineToolBox::onStepIncreased()
{
    if (QObject::sender() == d->actionlist[0])
        d->spinBox->setSingleStep(1);
    else if (QObject::sender() == d->actionlist[1])
        d->spinBox->setSingleStep(5);
    else if (QObject::sender() == d->actionlist[2])
        d->spinBox->setSingleStep(10);
    else if (QObject::sender() == d->actionlist[3])
        d->spinBox->setSingleStep(25);
    else if (QObject::sender() == d->actionlist[4])
        d->spinBox->setSingleStep(50);
}

void medTimeLineToolBox::clear()
{
    d->minTime = 0.0;
    d->minTimeStep = 1.0;
    d->maxTime = 0.0;

    d->interactors.clear();
    d->views.clear();
}

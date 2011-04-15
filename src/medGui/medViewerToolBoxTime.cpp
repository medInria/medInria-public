/* medViewerToolBoxTime.h ---
 * 
 * Author: Fatih Arslan and Nicolas Toussaint

 * Change log:
 * 
 */

#include "medViewerToolBoxTime.h"
#include <qdebug.h>
#include <iomanip>
#include <QListWidget>
#include <dtkCore/dtkAbstractView.h>
#include <dtkCore/dtkLog.h>
#include <medGui/medToolBoxTab.h>
#include <medCore/med4DAbstractViewInteractor.h>
#include <QList>
#include <string>
#include <sstream>
#include <QMouseEvent>
#include <cmath>



class medViewerToolBoxTimePrivate
{
public:
    QSlider     *timeSlider;
    QPushButton *playSequencesPushButton;
    QPushButton *nextFramePushButton;
    QPushButton *previousFramePushButton;
    QPushButton *stopPushButton;

    QList <QAction*> actionlist;
    QList<dtkAbstractView*> views;
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


};

medViewerToolBoxTime::medViewerToolBoxTime(QWidget *parent) : medToolBox(parent), d(new medViewerToolBoxTimePrivate)
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

    d->playSequencesPushButton = new QPushButton("", this);
    d->playSequencesPushButton->setIcon (QIcon(":/icons/play.png"));
    d->playSequencesPushButton->setCheckable (true);
    d->playSequencesPushButton->setMinimumWidth ( 20 );
    d->playSequencesPushButton->setToolTip( tr("Play Sequence"));

    d->nextFramePushButton = new QPushButton("", this);
    d->nextFramePushButton->setIcon (QIcon(":/icons/forward.png"));
    //d->nextFramePushButton->setCheckable (true);
    d->nextFramePushButton->setMinimumWidth ( 20 );
    d->nextFramePushButton->setToolTip( tr("Next Frame"));

    d->previousFramePushButton = new QPushButton("", this);
    d->previousFramePushButton->setIcon (QIcon(":/icons/backward.png"));
    //d->previousFramePushButton->setCheckable (true);
    d->previousFramePushButton->setMinimumWidth ( 20 );
    d->previousFramePushButton->setToolTip( tr("Previous Frame"));

    d->stopPushButton = new QPushButton("", this);
    d->stopPushButton->setIcon (QIcon(":/icons/stop.png"));
    //d->stopPushButton->setCheckable (true);
    d->stopPushButton->setMinimumWidth ( 20 );
    d->stopPushButton->setToolTip( tr("Stop Sequence"));

    d->timeLine = new QTimeLine();
    d->timeLine->setLoopCount(0);
    d->timeLine->setCurveShape (QTimeLine::LinearCurve);

    d->spinBox = new QSpinBox();
    d->spinBox->setRange(1,5000);
    d->spinBox->setSingleStep(10);
    d->spinBox->setValue(100);
    d->spinBox->setToolTip(tr("Control the display speed"));

    connect(d->timeLine, SIGNAL(frameChanged(int)), d->timeSlider, SLOT(setValue(int)));

    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->addWidget( d->previousFramePushButton);
    buttonLayout->addWidget (d->playSequencesPushButton);
    buttonLayout->addWidget( d->nextFramePushButton);


    buttonLayout->addWidget( d->stopPushButton);

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
    connect(d->playSequencesPushButton, SIGNAL(toggled(bool)), this, SLOT(onPlaySequences()));

    connect(d->nextFramePushButton, SIGNAL(clicked(bool)), this, SLOT(onNextFrame(bool)));
    connect(d->previousFramePushButton, SIGNAL(clicked(bool)), this, SLOT(onPreviousFrame(bool)));
    connect(d->spinBox, SIGNAL(valueChanged(int)),this, SLOT(onSpinBoxChanged(int)));
    connect(d->stopPushButton, SIGNAL(clicked(bool)),this, SLOT(onStopButton()));

    this->setTitle("Time Management");
    box->setLayout (boxlayout);
    this->addWidget (box);

    d->minTime = 0.0;
    d->minTimeStep = 1.0;
    d->maxTime = 0.0;

    this->isViewAdded = false;

}

medViewerToolBoxTime::~medViewerToolBoxTime(void)
{
    delete d;

    d = NULL;
}


void medViewerToolBoxTime::onViewAdded (dtkAbstractView *view)
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

void medViewerToolBoxTime::onDataAdded (dtkAbstractData *data)
{
    if (!data)
        return;

    this->updateRange();
}

void medViewerToolBoxTime::onViewRemoved (dtkAbstractView *view)
{
    d->timeLine->stop();
    d->timeSlider->setValue(0);

    if (!view)
        return;

    if (med4DAbstractViewInteractor *interactor = dynamic_cast<med4DAbstractViewInteractor*>(view->interactor ("v3dView4DInteractor")))
    {
        d->views.removeOne (view);
        d->interactors.removeOne (interactor);
    }

    this->updateRange();
    this->isViewAdded = false;
}

void medViewerToolBoxTime::AddInteractor (med4DAbstractViewInteractor* interactor)
{
    d->interactors.removeOne (interactor);
    d->interactors.append (interactor);
}
void medViewerToolBoxTime::RemoveInteractor (med4DAbstractViewInteractor* interactor)
{
    d->interactors.removeOne (interactor);
}

void medViewerToolBoxTime::update(dtkAbstractView *view)
{

}


void medViewerToolBoxTime::onPlaySequences ()
{
    if ( this->isViewAdded)
    {
        this->updateRange();
        d->timeLine->setDuration((d->maxTime + d->minTimeStep)*(1000/(d->spinBox->value()/100.0)));
        if(d->timeLine->state() == QTimeLine::NotRunning)
        {
            d->timeLine->start();
            d->playSequencesPushButton->setIcon (QIcon(":/icons/pause.png"));
            d->playSequencesPushButton->setToolTip( tr("Pause Sequence"));
        }
        else if(d->timeLine->state() == QTimeLine::Paused )
        {
            d->timeLine->resume();
            d->playSequencesPushButton->setIcon (QIcon(":/icons/pause.png"));
            d->playSequencesPushButton->setToolTip( tr("Pause Sequence"));
        }
        else if(d->timeLine->state() == QTimeLine::Running)
        {
            d->timeLine->setPaused(true);
            d->playSequencesPushButton->setIcon (QIcon(":/icons/play.png"));
            d->playSequencesPushButton->setToolTip( tr("Play Sequence"));
        }
    }
}

void medViewerToolBoxTime::onStopButton ()
{
    d->timeLine->stop();
    d->timeSlider->setValue(0);
}

void medViewerToolBoxTime::onNextFrame (bool val)
{
    if ( this->isViewAdded)
	d->timeSlider->setValue(d->timeSlider->value()+1);
}
void medViewerToolBoxTime::onPreviousFrame (bool val)
{   
    if ( this->isViewAdded)
	d->timeSlider->setValue(d->timeSlider->value()-1);
}

void medViewerToolBoxTime::onTimeChanged (int val)
{
	double time = this->getTimeFromSliderValue (val);
	for (int i=0; i<d->interactors.size(); i++)
	{
		d->interactors[i]->setCurrentTime (time);
	}

    d->labelcurr->setText( DoubleToQString(( time ) / (d->spinBox->value()/100.0)) + QString(" sec") );
}

void medViewerToolBoxTime::onSpinBoxChanged(int time)
{
    this->updateRange();

    d->timeLine->setDuration((d->maxTime + d->minTimeStep)*(1000/(time/100.0)));
}

double medViewerToolBoxTime::getTimeFromSliderValue (int s)
{
    double value = d->minTime + (double)(s) * (d->minTimeStep);
    return value;
}

unsigned int medViewerToolBoxTime::getSliderValueFromTime (double t)
{
    unsigned int value = std::ceil ((t - d->minTime) / (d->minTimeStep));
    return value;
}

void medViewerToolBoxTime::updateRange (void)
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

QString medViewerToolBoxTime::DoubleToQString (double val) 
{
    std::ostringstream strs;
    strs <<  std::fixed << std::setprecision(2)<< val;
    std::string str = strs.str();
    return QString(str.c_str());
}


void medViewerToolBoxTime::mouseReleaseEvent ( QMouseEvent *  mouseEvent)
{
    if(mouseEvent->button() == Qt::RightButton)
    {
        QMenu *menu = new QMenu(this);

        QAction *actionNotify = new QAction("Set Speed Increment : ",this);
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
void medViewerToolBoxTime::onStepIncreased()
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

void medViewerToolBoxTime::clear()
{
    d->minTime = 0.0;
    d->minTimeStep = 1.0;
    d->maxTime = 0.0;

    d->interactors.clear();
    d->views.clear();
}

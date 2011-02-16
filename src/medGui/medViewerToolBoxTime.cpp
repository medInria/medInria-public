#include "medViewerToolBoxTime.h"


#include <dtkCore/dtkAbstractView.h>
#include <dtkCore/dtkLog.h>

#include <medGui/medToolBoxTab.h>
#include <medCore/med4DAbstractViewInteractor.h>
#include <QList>

#include <cmath>

class medViewerToolBoxTimePrivate
{
public:
  QSlider     *timeSlider;
  QPushButton *playSequencesPushButton;
  QList<dtkAbstractView*> views;
  QList<med4DAbstractViewInteractor*> interactors;
  QTimeLine *timeLine;

  double minTime;
  double minTimeStep;
  
};

medViewerToolBoxTime::medViewerToolBoxTime(QWidget *parent) : medToolBox(parent), d(new medViewerToolBoxTimePrivate)
{
  QWidget *box = new QWidget (this);
    
  d->timeSlider = new QSlider (Qt::Horizontal, this);
  d->timeSlider->setRange (0, 100);
  d->timeSlider->setValue (100);
  d->timeSlider->setTracking( false );
  
  d->playSequencesPushButton = new QPushButton("", this);
  d->playSequencesPushButton->setIcon (QIcon (":/icons/wlww.tiff"));
  d->playSequencesPushButton->setCheckable (true);
  d->playSequencesPushButton->setMinimumWidth ( 20 );

  d->timeLine = new QTimeLine(3000, this);
  d->timeLine->setLoopCount(0);
  d->timeLine->setCurveShape (QTimeLine::LinearCurve);
  
  connect(d->timeLine, SIGNAL(frameChanged(int)), d->timeSlider, SLOT(setValue(int)));
  
  QVBoxLayout* boxlayout = new QVBoxLayout (this);
  boxlayout->addWidget (d->playSequencesPushButton);
  boxlayout->addWidget (d->timeSlider);

  connect(d->timeSlider, SIGNAL(sliderMoved(int)), this, SLOT(onTimeChanged(int)));
  connect(d->timeSlider, SIGNAL(valueChanged(int)), this, SLOT(onTimeChanged(int)));
  connect(d->playSequencesPushButton, SIGNAL(toggled(bool)), this, SLOT(onPlaySequences(bool)));
  
  this->setTitle("Time Management");
  box->setLayout (boxlayout);
  this->addWidget (box);

  d->minTime = 0.0;
  d->minTimeStep = 1.0;
  
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
  dtkWarning() << "medViewerToolBoxTime::onViewAdded" ;
  

  if (med4DAbstractViewInteractor *interactor = dynamic_cast<med4DAbstractViewInteractor*>(view->interactor ("v3dView4DInteractor")))
  {
    if (!d->views.contains (view))
      d->views.append (view);
    if (!d->interactors.contains (interactor))
      d->interactors.append (interactor);

    connect (view, SIGNAL ( dataAdded(dtkAbstractData*)),   this, SLOT (onDataAdded (dtkAbstractData*)));
  }

  this->updateRange();
}

void medViewerToolBoxTime::onDataAdded (dtkAbstractData *data)
{
  dtkWarning() << "medViewerToolBoxTime::onDataAdded" ;

  if (!data)
    return;

  this->updateRange();
}

void medViewerToolBoxTime::onViewRemoved (dtkAbstractView *view)
{
  if (!view)
    return;

  if (med4DAbstractViewInteractor *interactor = dynamic_cast<med4DAbstractViewInteractor*>(view->interactor ("v3dView4DInteractor")))
  {
    d->views.removeOne (view);
    d->interactors.removeOne (interactor);
  }  

  this->updateRange();
  
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
  // // find all interactors
  // QList<dtkAbstractView *> views = currentStack()->current()->views();
  // for (unsigned int i=0; i<views.size(); i++)
  //   this->onViewAdded (views[i]);
  
  // //this->AddInteractor(...)
}

void medViewerToolBoxTime::onPlaySequences (bool val)
{
  this->updateRange();
  
  if(val)
    d->timeLine->start();
  else
    d->timeLine->stop();
}


void medViewerToolBoxTime::onTimeChanged (int val)
{
  double time = this->getTimeFromSliderValue (val);
  
  //  dtkWarning() << "time is "<< val;  
  for (unsigned int i=0; i<d->interactors.size(); i++)
  {
    d->interactors[i]->setCurrentTime (time);
  }
}

double medViewerToolBoxTime::getTimeFromSliderValue (unsigned int s)
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
  
  for (unsigned int i=0; i<d->interactors.size(); i++)
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
}


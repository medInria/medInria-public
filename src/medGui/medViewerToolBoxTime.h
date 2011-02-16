#ifndef MEDVIEWERTOOLBOXTIME_H
#define MEDVIEWERTOOLBOXTIME_H

#include <medGui/medToolBox.h>

class dtkAbstractView;
class dtkAbstractData;
class medViewerToolBoxTimePrivate;
class med4DAbstractViewInteractor;

class medViewerToolBoxTime : public medToolBox
{
    Q_OBJECT

public:
     medViewerToolBoxTime(QWidget *parent = 0);
    ~medViewerToolBoxTime(void);

    void update(dtkAbstractView *view);

public slots:
    
    void onPlaySequences(bool);
    void onTimeChanged(int);
    void onViewAdded   (dtkAbstractView *view);
    void onViewRemoved (dtkAbstractView *view);
    void onDataAdded (dtkAbstractData* data);

 protected:

    void AddInteractor (med4DAbstractViewInteractor* interactor);
    void RemoveInteractor (med4DAbstractViewInteractor* interactor);

    void updateRange (void);

    double getTimeFromSliderValue (unsigned int);
    unsigned int getSliderValueFromTime (double);
    
private:
    medViewerToolBoxTimePrivate *d;
    
};

#endif

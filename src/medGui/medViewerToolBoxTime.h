#ifndef MEDVIEWERTOOLBOXTIME_H
#define MEDVIEWERTOOLBOXTIME_H

#include <medGui/medToolBox.h>


class dtkAbstractView;
class dtkAbstractData;
class medViewerToolBoxTimePrivate;
/* medViewerToolBoxTime.h ---
 * 
 * Author: Fatih Arslan and Nicolas Toussaint

/* Commentary: 
 * Class Declaration for 4D Image Support ToolBox
 */

/* Change log:
 * 
 */

class med4DAbstractViewInteractor;

class MEDGUI_EXPORT medViewerToolBoxTime : public medToolBox
{
    Q_OBJECT

public:
     medViewerToolBoxTime(QWidget *parent = 0);
    ~medViewerToolBoxTime(void);

    void update(dtkAbstractView *view);
	
public slots:
 /*
 * Slot when the sequence is in play, icons will be changed according to the state
 */
    void onPlaySequences();
/*
 * Slot for next and previous frame
 */
	void onNextFrame(bool);
	void onPreviousFrame(bool);
/*
 * parameters updated when time is changed
 */
    void onTimeChanged(int);
    void onViewAdded   (dtkAbstractView *view);
    void onViewRemoved (dtkAbstractView *view);
    void onDataAdded (dtkAbstractData* data);
/*
 * parameters updated when the spinbox is changed
 */
	void onSpinBoxChanged(int);
	void onStopButton();
	void onStepIncreased( );

 protected:

    void AddInteractor (med4DAbstractViewInteractor* interactor);
    void RemoveInteractor (med4DAbstractViewInteractor* interactor);
/*
 * update range and associated objects like labels
 */
    void updateRange (void);

    double getTimeFromSliderValue (unsigned int);
    unsigned int getSliderValueFromTime (double);
/*
 * for popup menu mouseRelaseEvent is overriden
 */
	void mouseReleaseEvent ( QMouseEvent *  );
    
private:
    medViewerToolBoxTimePrivate *d;
	bool isViewAdded;
	QString DoubleToQString(double); 
    
    
};

#endif

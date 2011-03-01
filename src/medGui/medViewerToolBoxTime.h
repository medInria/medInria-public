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
 * Class Declaration for 4D Image Support
 */

/* Change log:
 * 
 */

class med4DAbstractViewInteractor;

class medViewerToolBoxTime : public medToolBox
{
    Q_OBJECT

public:
     medViewerToolBoxTime(QWidget *parent = 0);
    ~medViewerToolBoxTime(void);

    void update(dtkAbstractView *view);
	
public slots:
    
    void onPlaySequences();
	void onNextFrame(bool);
	void onPreviousFrame(bool);
    void onTimeChanged(int);
    void onViewAdded   (dtkAbstractView *view);
    void onViewRemoved (dtkAbstractView *view);
    void onDataAdded (dtkAbstractData* data);
	void onSpinBoxChanged(int);
	void onStopButton();
	void onStepIncreased( );
	

 protected:

    void AddInteractor (med4DAbstractViewInteractor* interactor);
    void RemoveInteractor (med4DAbstractViewInteractor* interactor);

    void updateRange (void);

    double getTimeFromSliderValue (unsigned int);
    unsigned int getSliderValueFromTime (double);
	void mouseReleaseEvent ( QMouseEvent *  );
    
private:
    medViewerToolBoxTimePrivate *d;
	bool isViewAdded;
	QString DoubleToQString(double); 
    
    
};

#endif

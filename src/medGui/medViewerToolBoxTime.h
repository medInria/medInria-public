#ifndef MEDVIEWERTOOLBOXTIME_H
#define MEDVIEWERTOOLBOXTIME_H

#include <medGui/medToolBox.h>

class dtkAbstractView;
class dtkAbstractData;
class medViewerToolBoxTimePrivate;

/* \class medViewerToolBoxTime
 * 
 * \author: Fatih Arslan and Nicolas Toussaint
 * 
 * Class Declaration for 4D Image Support ToolBox
 * The time toolbox is dedicated to the handling of a 4th
 * dimension of objects that are visualized in the set of 
 * views currently loaded.
 *
 * If any object contains a 4th dimension (supposely time)
 * then the toolbox is activated and user can display the
 * different available time points.
 *
 * When several loaded objects contain a time dimension, 
 * then the toolbox automatically re-arranges its range
 * to adapt to the set of time objects ( updateRange() ).
 * The smallest time resolution is taken for the slider.
 *
 * The toolbox has a list of med4DAbstractViewInteractor
 * that do the main part of the work.
 * 
 * \see med4DAbstractViewInteractor
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

    /** Slot when the sequence is in played, icons will be changed according to the state */
    void onPlaySequences();

    /** Slot for next and previous frame */
    void onNextFrame(bool);
    void onPreviousFrame(bool);

    /** parameters updated when time is changed */
    void onTimeChanged(int);

    /** parameters updated when either views or data environments have changed */
    void onViewAdded   (dtkAbstractView *view);
    void onViewRemoved (dtkAbstractView *view);
    void onDataAdded (dtkAbstractData* data);
    
    /** parameters updated when the spinbox is changed */
    void onSpinBoxChanged(int);
    void onStopButton();
    void onStepIncreased( );
    
 protected:

    /** Add/Remove an interactor from the list */
    void AddInteractor (med4DAbstractViewInteractor* interactor);
    void RemoveInteractor (med4DAbstractViewInteractor* interactor);

    /**
       Update the range and associated objects like labels
       The function will question the interactors concerning
       time ranges and then will adapt the slider accordingly.
    */
    void updateRange (void);

    /** recover time information from slider (integer) value and vice-versa */
    double getTimeFromSliderValue (int);
    unsigned int getSliderValueFromTime (double);

    /** override the mouse release function for the slider */
    void mouseReleaseEvent ( QMouseEvent *  );
    
 private:
    
    medViewerToolBoxTimePrivate *d;
    bool isViewAdded;
    QString DoubleToQString(double);
};

#endif

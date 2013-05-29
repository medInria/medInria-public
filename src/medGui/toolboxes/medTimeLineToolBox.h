/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medToolBox.h>

class dtkAbstractView;
class dtkAbstractData;
class medTimeLineToolBoxPrivate;

/**
 * \class medTimeLineToolBox
 *
 * \author Fatih Arslan and Nicolas Toussaint
 *
 * \brief Class Declaration for 4D Image/Mesh Support ToolBox
 *
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
 *
 */

class med4DAbstractViewInteractor;

class MEDGUI_EXPORT medTimeLineToolBox : public medToolBox
{
    Q_OBJECT

public:
     medTimeLineToolBox(QWidget *parent = 0);
    ~medTimeLineToolBox();

    void update(dtkAbstractView *view);

public slots:

    /** Slot when the sequence is in played, icons will be changed according to the state */
    void onPlaySequences();

    /** Slot for next and previous frame */
    void onNextFrame();
    void onPreviousFrame();

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

    void clear();
    void setTime(int);

 protected:

    /** Add/Remove an interactor from the list */
    void AddInteractor (med4DAbstractViewInteractor* interactor);
    void RemoveInteractor (med4DAbstractViewInteractor* interactor);

    /**
       Update the range and associated objects like labels
       The function will question the interactors concerning
       time ranges and then will adapt the slider accordingly.
    */
    void updateRange();

    /** recover time information from slider (integer) value and vice-versa */
    double getTimeFromSliderValue (int);
    unsigned int getSliderValueFromTime (double);

    /** override the mouse release function for the slider */
    void mouseReleaseEvent ( QMouseEvent *  );

 private:

    medTimeLineToolBoxPrivate *d;
    bool isViewAdded;
    QString DoubleToQString(double);
};



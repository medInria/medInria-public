#ifndef MEDABSTRACTVIEWFIBERINTERACTOR_H
#define MEDABSTRACTVIEWFIBERINTERACTOR_H

#include <dtkCore/dtkAbstractViewInteractor.h>

#include "medCoreExport.h"

class dtkAbstractData;

/**
 * \class medAbstractViewFiberInteractor
 * \author Pierre Fillard
 * \brief To write
*/
class MEDCORE_EXPORT medAbstractViewFiberInteractor : public dtkAbstractViewInteractor
{
    Q_OBJECT

public:
    medAbstractViewFiberInteractor();
    ~medAbstractViewFiberInteractor();

    /**
      Set a region of interest.
    */
    virtual void setROI (dtkAbstractData *data);

    /**
      Set the boolean meaning for a ROI:
       - 0: NULL
       - 1: NOT
       - 2: AND
      Default is 2 (AND) for all ROIs.
    */
    virtual void setRoiBoolean(int roi, int meaning);
    virtual int  roiBoolean (int roi);
};

#endif

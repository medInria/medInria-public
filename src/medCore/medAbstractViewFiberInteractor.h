#ifndef MEDABSTRACTVIEWFIBERINTERACTOR_H
#define MEDABSTRACTVIEWFIBERINTERACTOR_H

#include <dtkCore/dtkAbstractViewInteractor.h>

#include "medCoreExport.h"

class dtkAbstractData;
class medAbstractViewFiberInteractorPrivate;

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

    void bundleFAStatistics (const QString &name,
                             double &mean,
                             double &min,
                             double &max,
                             double &var);

    void bundleADCStatistics (const QString &name,
                              double &mean,
                              double &min,
                              double &max,
                              double &var);

    void bundleLengthStatistics (const QString &name,
                                 double &mean,
                                 double &min,
                                 double &max,
                                 double &var);

    virtual void setBundleVisibility(const QString &name, bool visibility);
    virtual bool bundleVisibility(const QString &name) const;

    virtual void setBundleVisibility(bool visibility);

protected:
    virtual void computeBundleFAStatistics (const QString &name,
                                            double &mean,
                                            double &min,
                                            double &max,
                                            double &var);

    virtual void computeBundleADCStatistics (const QString &name,
                                             double &mean,
                                             double &min,
                                             double &max,
                                             double &var);

    virtual void computeBundleLengthStatistics (const QString &name,
                                                double &mean,
                                                double &min,
                                                double &max,
                                                double &var);

    void clearStatistics (void);

private:
    medAbstractViewFiberInteractorPrivate *d2;
};

#endif

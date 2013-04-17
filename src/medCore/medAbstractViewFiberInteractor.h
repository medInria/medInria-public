/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <dtkCore/dtkAbstractViewInteractor.h>

#include "medCoreExport.h"

class dtkAbstractData;
class medAbstractViewFiberInteractorPrivate;

/**
 * @class medAbstractViewFiberInteractor
 * @author Pierre Fillard
 * @brief Abstract class for view interactor to display fibers.
 * Fiber visualization can be controlled with Regions of Interest (ROI), which
 * are images of unsigned char defining regions of voxels where fibers should
 * (or shouldn't) pass. The meaning of each ROI can be controlled with the
 * setRoiBoolean() method: 0 is NULL (ROI is deactivated), 1 (fibers should not pass
 * through the ROI), and 2 (fibers should pass through the ROI).
 * Moreover, this class offers an interface to fiber bundle statistics: given a
 * fiber bundle (identified by a name), subclass should implement calculation of
 * FA, ADC, etc. statistics of the entire bundle.
 * Fiber bundle visibility can also be controlled.
 */
class MEDCORE_EXPORT medAbstractViewFiberInteractor : public dtkAbstractViewInteractor
{
    Q_OBJECT

public:
    medAbstractViewFiberInteractor();
    ~medAbstractViewFiberInteractor();

    /**
     * Set a Region Of Interest (ROI). A ROI is an unsigned char image containing
     * 1s, 2s, etc., each value defining a region.
     * @param a dtkAbstractData encapsulating a ROI. Subclass should cast the contained
     * pointer to a compatible ROI type.
     * @return void
     */
    virtual void setROI (dtkAbstractData *data);

    /**
     * Set the boolean meaning for a region of a ROI:
     *  - 0: NULL (region has no effect)
     *  - 1: NOT  (fibers should not pass through the region)
     *  - 2: AND  (fibers should pass through the region)
     * Default is 2 (AND) for all ROIs.
     * @param roi the value of the region
     * @param meaning the region meaning
     * @return void
     */

    virtual void setRoiBoolean(int roi, int meaning);
    /**
      * Access method to the roi meaning (see setRoi()).
      * @param roi the value of the region
      * @return the boolean meaning of the region
      */
    virtual int  roiBoolean (int roi);

    /**
      * Triggers the computation of the FA statistics of a fiber bundle.
      * An internal call to computeBundleFAStatistics() is made. Values
      * are cached for faster subsequent access to FA statistics.
      * @param name identifies the fiber bundle
      * @param mean mean bundle FA value
      * @param min  minimum bundle FA value
      * @param max  maximum bundle FA value
      * @param var  variance of bundle FA
      */
    void bundleFAStatistics (const QString &name,
                             double &mean,
                             double &min,
                             double &max,
                             double &var);

    /**
      * Triggers the computation of the ADC statistics of a fiber bundle.
      * An internal call to computeBundleADCStatistics() is made. Values
      * are cached for faster subsequent access to ADC statistics.
      * @param name identifies the fiber bundle
      * @param mean mean bundle ADC value
      * @param min  minimum bundle ADC value
      * @param max  maximum bundle ADC value
      * @param var  variance of bundle ADC
      */
    void bundleADCStatistics (const QString &name,
                              double &mean,
                              double &min,
                              double &max,
                              double &var);

    /**
      * Triggers the computation of the length statistics of a fiber bundle.
      * An internal call to computeBundleLengthtatistics() is made. Values
      * are cached for faster subsequent access to Length statistics.
      * @param name identifies the fiber bundle
      * @param mean mean bundle Length value
      * @param min  minimum bundle Length value
      * @param max  maximum bundle Length value
      * @param var  variance of bundle Length
      */
    void bundleLengthStatistics (const QString &name,
                                 double &mean,
                                 double &min,
                                 double &max,
                                 double &var);

    /**
      * Set the visibility of a fiber bundle.
      * @param name identifies the fiber bundle
      * @param visibility controls the visibility of the bundle
      */
    virtual void setBundleVisibility(const QString &name, bool visibility);

    /**
      * Access method to the visibility of a fiber bundle.
      * @param name fiber bundle identified by name
      * @return visibility of the fiber bundle.
      */
    virtual bool bundleVisibility(const QString &name) const;

    /**
      * Set the visibility of all fiber bundles.
      * @param visibility controls the visibility of all bundles
      */
    virtual void setBundleVisibility(bool visibility);

protected:
    /**
      * Internal method to actually compute the FA statistics of a fiber
      * bundle. Should be reimplemented in subclasses.
      * @param name identifies the fiber bundle
      * @param mean mean bundle FA value
      * @param min  minimum bundle FA value
      * @param max  maximum bundle FA value
      * @param var  variance of bundle FA
      */
    virtual void computeBundleFAStatistics (const QString &name,
                                            double &mean,
                                            double &min,
                                            double &max,
                                            double &var);

    /**
      * Internal method to actually compute the ADC statistics of a fiber
      * bundle. Should be reimplemented in subclasses.
      * @param name identifies the fiber bundle
      * @param mean mean bundle ADC value
      * @param min  minimum bundle ADC value
      * @param max  maximum bundle ADC value
      * @param var  variance of bundle ADC
      */
    virtual void computeBundleADCStatistics (const QString &name,
                                             double &mean,
                                             double &min,
                                             double &max,
                                             double &var);

    /**
      * Internal method to actually compute the Length statistics of a fiber
      * bundle. Should be reimplemented in subclasses.
      * @param name identifies the fiber bundle
      * @param mean mean bundle Length value
      * @param min  minimum bundle Length value
      * @param max  maximum bundle Length value
      * @param var  variance of bundle Length
      */
    virtual void computeBundleLengthStatistics (const QString &name,
                                                double &mean,
                                                double &min,
                                                double &max,
                                                double &var);

    /**
      * Clear the statistics (removes pre-computed from the cache).
      */
    void clearStatistics();

private:
    medAbstractViewFiberInteractorPrivate *d2;
};



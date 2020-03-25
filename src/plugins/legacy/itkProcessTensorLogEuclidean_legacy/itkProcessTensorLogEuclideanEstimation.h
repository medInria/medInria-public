/* itkProcessTensorEstimation.h --- 
 * 
 * Author: Pierre Fillard
 * Copyright (C) 2008 - Pierre Fillard, Inria.
 * Created: Thu May 28 20:23:16 2009 (+0200)
 * Version: $Id$
 * Last-Updated: Mon Jun 15 19:07:01 2009 (+0200)
 *           By: Julien Wintz
 *     Update #: 7
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#pragma once

#include <medAbstractDiffusionProcess.h>

#include "itkProcessTensorLogEuclideanPluginExport.h"

class itkProcessTensorLogEuclideanEstimationPrivate;

class ITKPROCESSTENSORLOGEUCLIDEANPLUGIN_EXPORT itkProcessTensorLogEuclideanEstimation : public medAbstractDiffusionProcess
{
    Q_OBJECT
      
public:
  
             itkProcessTensorLogEuclideanEstimation(void);
    virtual ~itkProcessTensorLogEuclideanEstimation(void);

    virtual QString identifier(void) const;
    virtual QString description(void) const;

    static bool registered(void);

    //void setBackgroundRemovalThreshold ( double value);
    
public slots:
    virtual int  update(void);

    void onCanceled (void);
    
    virtual medAbstractData* output (void);
    
    virtual void setInputImage(medAbstractData *data);

    /**
     * Set the background removal threshold to @param value.
     */
    virtual void setParameter(double value);

    /**
     * Equivalent to setParameter(value) (channel is unused).
     */
    virtual void setParameter(double value,  int channel);

    /**
     * Set the x, y,z gradient coordinates for DWI number @param channel.
     * @param count should be always equal to 3.
     */
    virtual void setParameter(double *data, int count, int channel);

public slots:    
    void onBSTValueChanged (double value);

private:
    itkProcessTensorLogEuclideanEstimationPrivate *d;
};

dtkAbstractProcess *createItkProcessTensorLogEuclideanEstimation(void);

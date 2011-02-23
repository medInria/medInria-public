/* medToolBoxDiffusion.h ---
 *
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Fri Feb 19 09:02:09 2010 (+0100)
 * Version: $Id$
 * Last-Updated: Wed Nov 10 11:03:19 2010 (+0100)
 *           By: Julien Wintz
 *     Update #: 14
 */

/* Commentary:
 *
 */

/* Change log:
 *
 */

#ifndef MEDTOOLBOXDIFFUSION_H
#define MEDTOOLBOXDIFFUSION_H

#include <medGui/medToolBox.h>

class dtkAbstractView;
class dtkAbstractData;
class dtkAbstractProcess;
class medToolBoxDiffusionPrivate;
class medDataIndex;

class medToolBoxDiffusion : public medToolBox
{
    Q_OBJECT
public:
     medToolBoxDiffusion(QWidget *parent = 0);
    ~medToolBoxDiffusion(void);

    medDataIndex dataIndex(void);

public slots:
    void run (void);
    void clear(void);
    
    void onObjectDropped (void);

    void onCoefficientsChanged  (int);

    void onToolBoxChosen(const QString&);
    
    /*
      void onComputeLFA (void);
      void onComputeRA  (void);
      void onComputeADC (void);
      void onComputeL1  (void);
      void onComputeL2  (void);
      void onComputeL3  (void);
      void onComputeCl  (void);
      void onComputeCp  (void);
      void onComputeCs  (void);
      void onComputeVR  (void);
    */
    
    dtkAbstractData *output(void);

protected:
    virtual void createProcessForIndex (const medDataIndex &index);

    void setRequiredOutputForProcess (dtkAbstractProcess *proc, int type);

private:
    medToolBoxDiffusionPrivate *d;

};

#endif // MEDTOOLBOXDIFFUSION_H

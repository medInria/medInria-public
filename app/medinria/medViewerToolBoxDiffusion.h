/* medViewerToolBoxDiffusion.h --- 
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

#ifndef MEDVIEWERTOOLBOXDIFFUSION_H
#define MEDVIEWERTOOLBOXDIFFUSION_H

#include <medGui/medToolBox.h>

class dtkAbstractView;
class medViewerToolBoxDiffusionPrivate;

class MEDGUI_EXPORT medViewerToolBoxDiffusion : public medToolBox
{
    Q_OBJECT

public:
     medViewerToolBoxDiffusion(QWidget *parent = 0);
    ~medViewerToolBoxDiffusion(void);

    void update(dtkAbstractView *view);
    
public slots:
    void run (void);

    void onColorModeChanged      (int index);
    void onGPUActivated          (int value);
    void onLinesRenderingModeSelected   (bool value);
    void onRibbonsRenderingModeSelected (bool value);
    void onTubesRenderingModeSelected   (bool value);
    void onBundlingBoxActivated         (int value);

    void onBundleValidated (QString name);

    void onObjectDropped (void);

    void onCoefficientsChanged  (int);

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
    
private:
    medViewerToolBoxDiffusionPrivate *d;
};

#endif

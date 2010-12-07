/* medViewerToolBoxView.h ---
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Fri Feb 19 09:02:09 2010 (+0100)
 * Version: $Id$
 * Last-Updated: Fri Feb 19 10:24:17 2010 (+0100)
 *           By: Julien Wintz
 *     Update #: 7
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#ifndef MEDVIEWERTOOLBOXVIEW_H
#define MEDVIEWERTOOLBOXVIEW_H

#include <medGui/medToolBox.h>

class dtkAbstractView;
class medViewerToolBoxViewPrivate;

class medViewerToolBoxView : public medToolBox
{
    Q_OBJECT

public:
     medViewerToolBoxView(QWidget *parent = 0);
    ~medViewerToolBoxView(void);

    void update(dtkAbstractView *view);

public slots:
    void onForegroundLookupTableChanged(QString);
    //void backgroundLookupTableChanged(QString);
    void onLutPresetChanged(QString);
    void onModeChanged(QString);
    void onVRModeChanged(QString);
    void onLodChanged(int);
    void onWindowingChanged(bool);
    void onZoomingChanged(bool);
    void onSlicingChanged(bool);
    void onMeasuringChanged(bool);
    void onCroppingChanged(bool);
    void onScalarBarVisibilityChanged(bool);
    void onAxisVisibilityChanged(bool);
    void onRulerVisibilityChanged(bool);
    void onAnnotationsVisibilityChanged(bool);

private:
    medViewerToolBoxViewPrivate *d;
};

#endif

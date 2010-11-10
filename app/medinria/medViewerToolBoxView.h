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

class MEDGUI_EXPORT medViewerToolBoxView : public medToolBox
{
    Q_OBJECT

public:
     medViewerToolBoxView(QWidget *parent = 0);
    ~medViewerToolBoxView(void);

    void update(dtkAbstractView *view);

signals:
    void foregroundLookupTableChanged(QString);
    void backgroundLookupTableChanged(QString);
    void lutPresetChanged(QString);
    void tdModeChanged(QString);
    void tdVRModeChanged(QString);
    void tdLodChanged(int);
    void windowingChanged(bool);
    void zoomingChanged(bool);
    void slicingChanged(bool);
    void measuringChanged(bool);
    void croppingChanged(bool);
    void scalarBarVisibilityChanged(bool);
    void axisVisibilityChanged(bool);
    void rulerVisibilityChanged(bool);
    void annotationsVisibilityChanged(bool);

private:
    medViewerToolBoxViewPrivate *d;
};

#endif

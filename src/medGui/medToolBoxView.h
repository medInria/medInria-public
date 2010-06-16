/* medToolBoxView.h --- 
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

#ifndef MEDTOOLBOXVIEW_H
#define MEDTOOLBOXVIEW_H

#include "medToolBox.h"

class dtkAbstractView;
class medToolBoxViewPrivate;

class MEDGUI_EXPORT medToolBoxView : public medToolBox
{
    Q_OBJECT

public:
     medToolBoxView(QWidget *parent = 0);
    ~medToolBoxView(void);

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

private:
    medToolBoxViewPrivate *d;
};

#endif

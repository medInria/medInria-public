/* medDatabasePreview.h --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Tue Dec 15 09:42:06 2009 (+0100)
 * Version: $Id$
 * Last-Updated: Wed Apr 21 09:35:09 2010 (+0200)
 *           By: Julien Wintz
 *     Update #: 5
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#ifndef MEDDATABASEPREVIEW_H
#define MEDDATABASEPREVIEW_H

#include "medGuiExport.h"

#include <QtCore>
#include <QtGui>

class medDatabasePreviewItem;
class medDatabasePreviewPrivate;
class medDataIndex;

class MEDGUI_EXPORT medDatabasePreview : public QFrame
{
    Q_OBJECT

public:
     medDatabasePreview(QWidget *parent = 0);
    ~medDatabasePreview();

    void init();
    void reset();

signals:
    void seriesClicked(const medDataIndex& id);
    void  imageClicked(const medDataIndex& id);

public slots:
    void onPatientClicked(const medDataIndex& id);
    void  onSeriesClicked(const medDataIndex& id);

protected slots:
    void onSlideUp();
    void onSlideDw();
    void onMoveRt();
    void onMoveLt();
    void onMoveUp();
    void onMoveDw();
    void onMoveBg();
    void onHovered(medDatabasePreviewItem *item);
    
protected:
    void moveToItem(medDatabasePreviewItem *target);

private:
    medDatabasePreviewPrivate *d;
};

#endif // MEDDATABASEPREVIEW_H

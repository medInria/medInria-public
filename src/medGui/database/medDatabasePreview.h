/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medGuiExport.h>

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
    void onStudyClicked(const medDataIndex& id);
    void onSeriesClicked(const medDataIndex& id);

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



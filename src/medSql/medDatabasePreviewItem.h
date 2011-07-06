/* medDatabasePreviewItem.h --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Tue Dec 15 09:42:56 2009 (+0100)
 * Version: $Id$
 * Last-Updated: Tue Dec 15 09:42:57 2009 (+0100)
 *           By: Julien Wintz
 *     Update #: 1
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#ifndef MEDDATABASEPREVIEWITEM_H
#define MEDDATABASEPREVIEWITEM_H

#include <QtGui/QGraphicsPixmapItem>
#include "medCore/medDataIndex.h"

class medDatabasePreviewItemPrivate;

class medDatabasePreviewItem : public QObject, public QGraphicsPixmapItem
{
    Q_OBJECT

public:
     medDatabasePreviewItem(const medDataIndex &index, QGraphicsItem *parent = 0);
    ~medDatabasePreviewItem(void);

    medDatabasePreviewItem *clone(void);

    medDataIndex dataIndex(void) const;

signals:
    void patientClicked(int id);
    void   studyClicked(int id);
    void  seriesClicked(int id);
    void   imageClicked(int id);

protected slots:
    void setImage(const QImage& image);

private:
    medDatabasePreviewItemPrivate *d;
};

#endif // MEDDATABASEPREVIEWITEM_H

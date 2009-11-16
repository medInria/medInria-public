/* medStudyPreview.h --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Thu Oct  8 19:45:13 2009 (+0200)
 * Version: $Id$
 * Last-Updated: Wed Oct 21 15:23:25 2009 (+0200)
 *           By: Julien Wintz
 *     Update #: 9
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#ifndef MEDSTUDYPREVIEW_H
#define MEDSTUDYPREVIEW_H

#include "medGuiExport.h"

#include <QtGui>

class medStudyPreviewPrivate;

class MEDGUI_EXPORT medStudyPreview : public QFrame
{
    Q_OBJECT

public:
     medStudyPreview(QWidget *parent = 0);
    ~medStudyPreview(void);

    QSize sizeHint(void) const;

    void setup(int studyId);

private:
    medStudyPreviewPrivate *d;
};

#endif

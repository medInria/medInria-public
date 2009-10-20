/* medBrowserArea.h --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Fri Oct  9 08:28:09 2009 (+0200)
 * Version: $Id$
 * Last-Updated: Fri Oct 16 22:53:10 2009 (+0200)
 *           By: Julien Wintz
 *     Update #: 4
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#ifndef MEDBROWSERAREA_H
#define MEDBROWSERAREA_H

#include <QWidget>

// /////////////////////////////////////////////////////////////////
// medBrowserAreaPreview
// /////////////////////////////////////////////////////////////////

class medBrowserAreaPreviewPrivate;
class medDatabaseView;
class medDatabaseModel;

class medBrowserAreaPreview : public QWidget
{
    Q_OBJECT

public:
     medBrowserAreaPreview(QWidget *parent = 0);
    ~medBrowserAreaPreview(void);

    QSize sizeHint(void) const;

public slots:
    void onPatientClicked(int id);
    void onStudyClicked(int id);
    void onSeriesClicked(int id);
    void onImageClicked(int id);

private:
    medBrowserAreaPreviewPrivate *d;
};

// /////////////////////////////////////////////////////////////////
// medBrowserArea
// /////////////////////////////////////////////////////////////////

class medBrowserAreaPrivate;

class medBrowserArea : public QWidget
{
    Q_OBJECT

public:
     medBrowserArea(QWidget *parent = 0);
    ~medBrowserArea(void);

    medDatabaseView *view(void);
    medDatabaseModel *model(void);

private:
    medBrowserAreaPrivate *d;
};

#endif // MEDBROWSERAREA_H

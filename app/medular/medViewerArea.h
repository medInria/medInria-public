/* medViewerArea.h --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Fri Sep 18 12:42:58 2009 (+0200)
 * Version: $Id$
 * Last-Updated: Thu May 13 16:26:10 2010 (+0200)
 *           By: Julien Wintz
 *     Update #: 78
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#ifndef MEDVIEWERAREA_H
#define MEDVIEWERAREA_H

#include <QtGui>

class dtkAbstractView;

class medDataIndex;
class medViewerAreaPrivate;

class medViewerArea : public QWidget
{
    Q_OBJECT

public:
     medViewerArea(QWidget *parent = 0);
    ~medViewerArea(void);

    void setup(QStatusBar *status);
    void setdw(QStatusBar *status);

    void setPatientIndex(int id);
    void   setStudyIndex(int id);
    void  setSeriesIndex(int id);
    void   setImageIndex(int id);

public slots:
    void setup(void);
    void split(int rows, int cols);

    void open(const medDataIndex& index);
    void open(const QString& file);

    void onPatientIndexChanged(int index);
    void   onStudyIndexChanged(int index);
    void  onSeriesIndexChanged(int index);
    void   onImageIndexChanged(int index);

    void onViewFocused(dtkAbstractView *view);

public slots: // layout settings
    void setStackIndex(int index);

protected slots: // view settings
    void setupForegroundLookupTable(QString table);
    void setupBackgroundLookupTable(QString table);
    void setupAxisVisibility(bool visible);
    void setupScalarBarVisibility(bool visible);
    void setup3DMode (QString table);
    void setupLUTPreset (QString table);
    void setup3DLOD (int value);
    void setupWindowing (bool checked);
    void setupZooming (bool checked);
    void setupSlicing (bool checked);
    void setupMeasuring (bool checked);
    void setupCropping (bool checked);

protected slots: // registration settings
    void setupLayoutCompare(void);
    void setupLayoutFuse(void);

private:
    medViewerAreaPrivate *d;
};

#endif // MEDVIEWERAREA_H

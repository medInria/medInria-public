/* medViewerArea.h --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Fri Sep 18 12:42:58 2009 (+0200)
 * Version: $Id$
 * Last-Updated: Fri Oct 16 12:58:11 2009 (+0200)
 *           By: Julien Wintz
 *     Update #: 7
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#ifndef MEDVIEWERAREA_H
#define MEDVIEWERAREA_H

#include <QtGui/QWidget>

class medViewerAreaPrivate;
class medViewerAreaViewContainer;

class medViewerArea : public QWidget
{
    Q_OBJECT

public:
     medViewerArea(QWidget *parent = 0);
    ~medViewerArea(void);

    void setPatientIndex (int index);
    void setStudyIndex (int index);
    void setSeriesIndex (int index);
    void setImageIndex (int index);

    void addWidget (QWidget* widget);
        
signals:
    void patientSelected (int index);
    void studySelected (int index);
    void seriesSelected (int index);
    void imageSelected (int index);
        

public slots:
    void setup(void);
    void split(int rows, int cols);

    void onPatientIndexChanged(int index);
    void onStudyIndexChanged(int index);
    void onSeriesIndexChanged(int index);
    void onImageIndexChanged(int index);

private:
    medViewerAreaPrivate *d;
};

#endif // MEDVIEWERAREA_H

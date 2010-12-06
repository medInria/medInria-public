/* medViewerToolBoxPatient.h ---
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Fri Feb 19 09:02:09 2010 (+0100)
 * Version: $Id$
 * Last-Updated: Sun Jun 27 18:16:49 2010 (+0200)
 *           By: Julien Wintz
 *     Update #: 28
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#ifndef MEDVIEWERTOOLBOXPATIENT_H
#define MEDVIEWERTOOLBOXPATIENT_H

#include <medGui/medToolBox.h>

class medDataIndex;
class medViewerToolBoxPatientPrivate;

class medViewerToolBoxPatient : public medToolBox
{
    Q_OBJECT

public:
     medViewerToolBoxPatient(QWidget *parent = 0);
    ~medViewerToolBoxPatient(void);

    void addItem(const QString& item);
    void addItem(const QString& item, const QVariant& data);

    void clear(void);

    int patientIndex(void);
    int patientIndex(QString patient);

signals:
    void patientIndexChanged(int);

public slots:
    void setPatientIndex(int patientId);
    void setPatientIndex(const medDataIndex& index);
    void setupDatabase(void);

protected slots:
    void onCurrentIndexChanged(int);

private:
    medViewerToolBoxPatientPrivate *d;
};

#endif

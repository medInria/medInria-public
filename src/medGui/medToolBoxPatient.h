/* medToolBoxPatient.h --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Fri Feb 19 09:02:09 2010 (+0100)
 * Version: $Id$
 * Last-Updated: Mon Jun 14 16:33:07 2010 (+0200)
 *           By: Julien Wintz
 *     Update #: 21
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#ifndef MEDTOOLBOXPATIENT_H
#define MEDTOOLBOXPATIENT_H

#include "medToolBox.h"

class medDataIndex;
class medToolBoxPatientPrivate;

class MEDGUI_EXPORT medToolBoxPatient : public medToolBox
{
    Q_OBJECT

public:
     medToolBoxPatient(QWidget *parent = 0);
    ~medToolBoxPatient(void);

    void addItem(const QString& item);
    void addItem(const QString& item, const QVariant& data);

    void clear(void);

    int patientIndex(void);
    int patientIndex(QString patient);
    int patientIndex(const medDataIndex& index);

signals:
    void patientIndexChanged(int);

public slots:
    void setPatientIndex(int);

private:
    medToolBoxPatientPrivate *d;
};

#endif

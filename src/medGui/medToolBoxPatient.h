/* medToolBoxPatient.h --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Fri Feb 19 09:02:09 2010 (+0100)
 * Version: $Id$
 * Last-Updated: Fri Feb 19 09:46:15 2010 (+0100)
 *           By: Julien Wintz
 *     Update #: 18
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

signals:
    void patientIndexChanged(int);
    void quickOpenImage(void);

public slots:
    void setPatientIndex(int);

private:
    medToolBoxPatientPrivate *d;
};

#endif

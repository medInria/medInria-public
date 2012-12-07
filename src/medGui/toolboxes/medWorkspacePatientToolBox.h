/* medWorkspacePatientToolBox.h ---
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

#include "medGuiExport.h"
#include "medToolBox.h"

class medDataIndex;
class medWorkspacePatientToolBoxPrivate;

class MEDGUI_EXPORT medWorkspacePatientToolBox : public medToolBox
{
    Q_OBJECT

public:
     medWorkspacePatientToolBox(QWidget *parent = 0);
    ~medWorkspacePatientToolBox(void);


    QSet<medDataIndex> patientIndex(void) const;
    QSet<medDataIndex> patientIndex(QString patient) const;
    QSet<medDataIndex> patientIndex(int itemId) const;

signals:
    /** Emitted when the selected patient changes.
     *  \param Patients currently selected */
    void patientIndexChanged(const medDataIndex&);

public slots:
    void setPatientIndex(const medDataIndex& index);
    void setupDatabase(void);

protected slots:
    void onCurrentIndexChanged(int);
    //! Attaches new connections to the registered controller, and updates if needed.
    void onDbControllerRegistered(const QString&);
protected:
    int addChooseItem();
    void removeChooseItem();

    int addItem(const QString& item, const medDataIndex& data);

    void clear(void);

private:
    medWorkspacePatientToolBoxPrivate *d;
};

#endif

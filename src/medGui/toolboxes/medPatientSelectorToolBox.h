/*=========================================================================

 MedInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include "medGuiExport.h"
#include "medToolBox.h"

class medDataIndex;
class medPatientSelectorToolBoxPrivate;

class MEDGUI_EXPORT medPatientSelectorToolBox : public medToolBox
{
    Q_OBJECT

public:
     medPatientSelectorToolBox(QWidget *parent = 0);
    ~medPatientSelectorToolBox();


    QSet<medDataIndex> patientIndex() const;
    QSet<medDataIndex> patientIndex(QString patient) const;
    QSet<medDataIndex> patientIndex(int itemId) const;

signals:
    /** Emitted when the selected patient changes.
     *  \param Patients currently selected */
    void patientIndexChanged(const medDataIndex&);

public slots:
    void setPatientIndex(const medDataIndex& index);
    void setupDatabase();

protected slots:
    void onCurrentIndexChanged(int);
    //! Attaches new connections to the registered controller, and updates if needed.
    void onDbControllerRegistered(const QString&);
protected:
    int addChooseItem();
    void removeChooseItem();

    int addItem(const QString& item, const medDataIndex& data);

    void clear();

private:
    medPatientSelectorToolBoxPrivate *d;
};



/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medDataIndex.h>
#include <medToolBox.h>
#include <medCoreExport.h>

class medActionsToolBoxPrivate;

class MEDCORE_EXPORT medActionsToolBox : public medToolBox
{
    Q_OBJECT
public:
    medActionsToolBox(QWidget *parent = 0, bool ok = false); // second parameter used to differentiate if we
    ~medActionsToolBox();                                    // are looking into db or File System

signals:

    /** Emitted when the 'Remove' button is clicked. */
    void removeClicked();

    /** Emitted when the 'Interactor' button is clicked. */
    void viewClicked();

    /** Emitted when the 'Load' button is clicked. */
    void loadClicked();

    /** Emitted when the 'Import' button is clicked. */
    void importClicked();

    /** Emitted when the 'Index' button is clicked. */
    void indexClicked();

    /** Emitted when the 'Bookmark' button is clicked. */
    void bookmarkClicked();

    /** Emitted when the 'Export' button is clicked. */
    void exportClicked();

    /** Emitted when the 'Save' button is clicked. */
    void saveClicked();
    
    /** Emitted when the 'new Patient' button is clicked. */   
    void newPatientClicked();
    
    /** Emitted when the 'new Study' button is clicked. */   
    void newStudyClicked();
    
    /** Emitted when the 'Edit' button is clicked. */   
    void editClicked();

public slots:

    void patientSelected(const medDataIndex& index);
    void seriesSelected(const medDataIndex& index);
    void noPatientOrSeriesSelected();
    void selectedPathsChanged(const QStringList& paths);
    void multipleEntriesSelected(const QVector<medDataIndex>& selectedIndexes);

protected:

    void updateButtons(QString item);

private:

    void initializeItemToActionsMap();

    medActionsToolBoxPrivate* d;
};

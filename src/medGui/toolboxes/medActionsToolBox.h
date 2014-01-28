/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medDataIndex.h>
#include <medToolBox.h>
#include "medGuiExport.h"

class medActionsToolBoxPrivate;

class MEDGUI_EXPORT medActionsToolBox : public medToolBox
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

    /**
    * Slot to call when an item representing a patient has been selected.
    * The appropriate buttons will appear in the toolbox.
    * @param index – the @medDataIndex of the db item
    **/
    void patientSelected(const medDataIndex& index);

    /**
    * Slot to call when an item representing a series has been selected.
    * The appropriate buttons will appear in the toolbox.
    * @param index – the @medDataIndex of the db item
    **/
    void seriesSelected(const medDataIndex& index);

    /**
    * Slot to call when the items selected in the db browser change
    * and no item is selected. Hence no buttons are displayed.
    **/
    void noPatientOrSeriesSelected();

    /**
    * Slot to call when items representing files and/or folders have been selected.
    * The appropriate buttons will appear in the toolbox.
    * @param paths – a @QStringList with all the selected paths
    **/
    void selectedPathsChanged(const QStringList& paths);

protected:

    void updateButtons(QString item);

private:

    void initializeItemToActionsMap();

    medActionsToolBoxPrivate* d;
};

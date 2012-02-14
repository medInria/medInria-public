#ifndef MEDTOOLBOXACTIONS_H
#define MEDTOOLBOXACTIONS_H

#include <medDataIndex.h>
#include <medToolBox.h>
#include "medGuiExport.h"

class medToolBoxActionsPrivate;

class MEDGUI_EXPORT medToolBoxActions : public medToolBox
{
    Q_OBJECT

public:
    medToolBoxActions(QWidget *parent = 0);
    ~medToolBoxActions();

signals:

    /** Emitted when the 'Remove' button is clicked. */
    void removeClicked();

    /** Emitted when the 'View' button is clicked. */
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
    * Slot to call when items representing files and/or folders have been selected.
    * The appropriate buttons will appear in the toolbox.
    * @param paths – a @QStringList with all the selected paths
    **/
    void selectedPathsChanged(const QStringList& paths);

protected:

    void updateButtons(QString item);

private:
    medToolBoxActionsPrivate* d;
};

#endif // MEDTOOLBOXACTIONS_H

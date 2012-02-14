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

    void removeClicked();
    void viewClicked();
    void loadClicked();
    void importClicked();
    void indexClicked();
    void bookmarkClicked();
    void exportClicked();
    void saveClicked();

public slots:

    void patientSelected(const medDataIndex& index);
    void seriesSelected(const medDataIndex& index);
    void selectedPathsChanged(const QStringList& paths);

protected:

    void updateButtons(QString item);

private:
    medToolBoxActionsPrivate* d;
};

#endif // MEDTOOLBOXACTIONS_H

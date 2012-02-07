#ifndef MEDTOOLBOXACTIONS_H
#define MEDTOOLBOXACTIONS_H

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

public slots:

    void patientSelected();
    void seriesSelected();
    void pathSelected(QString path);

protected:

    void updateButtons(QString item);

private:
    medToolBoxActionsPrivate* d;
};

#endif // MEDTOOLBOXACTIONS_H

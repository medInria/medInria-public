#ifndef MEDMULTIPLEIMAGESELECTIONWIDGET_H
#define MEDMULTIPLEIMAGESELECTIONWIDGET_H

#include <QtGui>

#include "medGuiExport.h"
#include <medDataIndex.h>

class medMultipleImageSelectionWidgetPrivate;

/**
* @class medMultipleImageSelectionWidget
* @brief A widget that allows the user to select multiple images from different patients by dragging-and-dropping.
* The @medMultipleImageSelectionWidget is a modal window that allows the user to drag-and-drop series from one panel
* to another panel. Images belonging to different patients can be selected at the same time. The @getSelectedIndexes()
* function returns a list of the selected @medDataIndex.
*
* Sample use:
*
* medMultipleImageSelectionWidget* misw = new medMultipleImageSelectionWidget( previouslySelectedIndexes, parent );
* int returnCode = misw->exec();
*
* if ( returnCode == QDialog::Accepted )
*     selectedIndexes = misw->getSelectedIndexes();
* else if ( returnCode == QDialog::Rejected )
*     // user pressed the cancel button
*
**/
class MEDGUI_EXPORT medMultipleImageSelectionWidget : public QDialog
{
    Q_OBJECT

public:
    medMultipleImageSelectionWidget(QList<medDataIndex> previouslySelectedIndexes, QWidget *parent = 0);
    ~medMultipleImageSelectionWidget(void);

    QSize sizeHint() const;

    /**
    * Returns a list of the selected images.
    * @return a @QList of @medDataIndex of each selected image.
    **/
    QList<medDataIndex> getSelectedIndexes();

    /**
    * Sets the database model
    * @param a @QAbstractItemModel of the database
    * (probably a medDatabaseModel or medDatabaseProxyModel)
    **/
    void setModel(QAbstractItemModel* model);

public slots:
    void clear();

private slots:
    void onPatientSelected(const medDataIndex&);

protected slots:
    void onCancelButtonClicked();
    void onOkButtonClicked();

protected:
    void paintEvent(QPaintEvent* paintEvent);
    void resizeEvent(QResizeEvent* resizeEvent);

private:
    medMultipleImageSelectionWidgetPrivate *d;
};

#endif

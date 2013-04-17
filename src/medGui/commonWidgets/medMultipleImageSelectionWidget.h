/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

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
    ~medMultipleImageSelectionWidget();

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

private slots:

    /**
     * This slot is called when the user selects a patient
     * in the database view and it populates the preview panel
     * with thumbnails for the patient's series.
     */
    void onPatientSelected(const medDataIndex&);

protected slots:

    /**
     * Slot called when the user presses the cancel button.
     * The return code of the @QDialog becomes QDialog::Rejected.
     */
    void onCancelButtonClicked();

    /**
     * Slot called when the user presses the Ok button.
     * The return code of the @QDialog becomes QDialog::Accepted.
     */
    void onOkButtonClicked();

private:
    medMultipleImageSelectionWidgetPrivate *d;
};



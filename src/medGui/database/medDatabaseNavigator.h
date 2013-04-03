/* medDatabaseNavigator.h ---
 *
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Tue Dec 15 09:38:25 2009 (+0100)
 * Version: $Id$
 * Last-Updated: Thu May 13 20:07:04 2010 (+0200)
 *           By: Julien Wintz
 *     Update #: 4
 */

/* Commentary:
 *
 */

/* Change log:
 *
 */

#ifndef MEDDATABASENAVIGATOR_H
#define MEDDATABASENAVIGATOR_H

#include <QtCore>
#include <QtGui>

#include "medGuiExport.h"

class medDatabaseNavigatorItem;
class medDatabaseNavigatorPrivate;
class medDataIndex;

class MEDGUI_EXPORT medDatabaseNavigator : public QFrame
{
    Q_OBJECT

public:
     medDatabaseNavigator(QWidget *parent = 0);
    ~medDatabaseNavigator();

    void reset();

    Qt::Orientation orientation() const;

signals:
    void itemClicked(const medDataIndex& id);

public slots:
    /**
     * @brief Updates the navigator when an item has been clicked
     *
     * For the moment, the only item considered calls this method
     * with a valid patient ID. There are some stubs for reacting to studies and series items.
     *
     * Switches the navigator to the list of study thumbnails linked to the valid paitent Id.
     * @param index a medDataIndex valid for patient. Will always switch to displaying a valid patient.
     */

    void onItemClicked(const medDataIndex& id);
    /**
     * @brief Updates the navigator when there is a change to the item currently displayed.
     *
     * For the moment, the only item considered calls this method
     * with a valid patient ID. There are some stubs for reacting to studies and series items.
     * This method updates the navigator only if the patient Id is the same as the one currently displayed. Typical case: a study has been removed from the currently selected patient.
     * @param index a medDataIndex valid for patient. Does nothing if it doesn't
     * match the currently displayed patient.
     */
    void updateNavigator(const medDataIndex& index);
    void   setOrientation(Qt::Orientation orientation);

protected:
    void onPatientClicked(const medDataIndex& id);
    void   onStudyClicked(const medDataIndex& id);
    void  onSeriesClicked(const medDataIndex& id);
    void   onImageClicked(const medDataIndex& id);

private:
    medDatabaseNavigatorPrivate *d;
};

#endif // MEDDATABASENAVIGATOR_H

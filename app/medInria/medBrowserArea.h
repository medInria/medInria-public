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

// /////////////////////////////////////////////////////////////////
// medBrowserArea
// /////////////////////////////////////////////////////////////////

class medDataIndex;
class medDatabaseView;
class medDatabaseModel;
class medBrowserAreaPrivate;
class medAbstractDataSource;
class dtkAbstractData;
class medToolBox;
class medJobItem;

class medBrowserArea : public QWidget
{
    Q_OBJECT

public:
     medBrowserArea(QWidget *parent = 0);
    ~medBrowserArea();

public slots:
    void onSourceIndexChanged(int index);
    void displayJobItem(medJobItem *importer, QString infoBaseName);

    /**
     * @brief Adds a medToolBox to the medToolBoxContainer.
     *
     * @param toolbox
    */
    void addToolBox(medToolBox *toolbox);
    /**
     * @brief Removes a medToolBox from the medToolBoxContainer.
     *
     * @param toolbox
    */
    void removeToolBox(medToolBox *toolbox);
    
private slots:

    /** This function is called when the import/index
     * process detects a partial import attempt. That is when the user
     * tried to import, in 2 separate steps, images belonging
     * to the same volume.  */
    void onPartialImportAttempted(const QString& message);

protected:
    void setToolBoxesVisible(int index, bool visible);
    void addDataSource(medAbstractDataSource* dataSource);

private:
    medBrowserAreaPrivate *d;
};



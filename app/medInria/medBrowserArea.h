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

    void setup(QStatusBar *status);
    void setdw(QStatusBar *status);

signals:

    /**
    * Signal emitted when the user chooses to open an image
    * from the database browser.
    * @param index - the @medDataIndex of the image
    **/
    void open(const medDataIndex& index);

    /**
    * Signal emitted when the user chooses to open an image
    * from the database browser (double click on a thumbnail)
    * @param index - the @medDataIndex of the image
    * @param slice - the slice number corresponding to the image
    **/
    void open(const medDataIndex& index, int slice);

    /**
    * Signal emitted when the user chooses to open a file or
    * directory in the file browser.
    * @param path - the path of the image or directory
    **/
    void open(const QString& path);

    /**
    * Signal emitted when the user chooses to load a file or
    * directory in the file browser.
    * @param path - the path of the image or directory
    **/
    void load(const QString& path);

    void showError (const QString&,unsigned int timeout);

public slots:
    void onSourceIndexChanged(int index);
    void onFileImport(QString);
    void onFileIndex(QString);
    void onDataImport(dtkAbstractData *data);
    void onDataReceivingFailed(QString fileName);
    void displayJobItem(medJobItem *importer, QString infoBaseName);

    /*
     * Function to call when (after emitting opening signal)
     * the db item fail to open. This will make changes in the GUI
     * to let the user know.
     */
    void onOpeningFailed(const medDataIndex& index);

    /**
     * @brief Export data from a selected medDataIndex in a data source.
     *
     * Opens a file dialog and tries writers based 
     * on the file extension given by the user.
     * @param index
    */
    void onExportData(const medDataIndex &index);

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
    
    /** Called when data has been removed from a data source.*/
    void onDataRemoved(const medDataIndex &index);

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



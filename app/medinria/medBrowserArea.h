/* medBrowserArea.h --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Fri Oct  9 08:28:09 2009 (+0200)
 * Version: $Id$
 * Last-Updated: Wed Oct  6 15:51:28 2010 (+0200)
 *           By: Julien Wintz
 *     Update #: 20
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#ifndef MEDBROWSERAREA_H
#define MEDBROWSERAREA_H

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

class medBrowserArea : public QWidget
{
    Q_OBJECT

public:
     medBrowserArea(QWidget *parent = 0);
    ~medBrowserArea(void);

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

    void showError (QObject*,const QString&,unsigned int timeout);

public slots:
    void onFileImported(void);
    void onSourceIndexChanged(int index);
    void onFileImport(QString);
    void onDataImport(dtkAbstractData *data);
    void onDataReceivingFailed(QString fileName);
    
    /**
     * @brief Export data from a selected medDataIndex in a data source.
     *
     * Opens a file dialog and tries writers based 
     * on the file extension given by the user.
     * @param index
    */
    void onExportData(const medDataIndex &index);

protected:
    void setToolBoxesVisible(int index, bool visible);
    void addDataSource(medAbstractDataSource* dataSource);

private:
    medBrowserAreaPrivate *d;
};

#endif // MEDBROWSERAREA_H

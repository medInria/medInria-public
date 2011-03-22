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


class medBrowserArea : public QWidget
{
    Q_OBJECT

public:
     medBrowserArea(QWidget *parent = 0);
    ~medBrowserArea(void);

    void setup(QStatusBar *status);
    void setdw(QStatusBar *status);

signals:
    void open(const medDataIndex& index);
    void open(const QString& file);
    void showError (QObject*,const QString&,unsigned int timeout);

public slots:
    void onFileImported(void);
    void onSourceIndexChanged(int index);
    void onFileImport(QString);
    void onDataReceivingFailed(QString fileName);

protected:
    void setToolBoxesVisible(int index, bool visible);
    void addDataSource(medAbstractDataSource* dataSource);

private:
    medBrowserAreaPrivate *d;
};

#endif // MEDBROWSERAREA_H

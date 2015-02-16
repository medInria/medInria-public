/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <QDialog>

class medDataSourceDialogPrivate;
class medAbstractDataSource;
class medDataSourceDialog: public QDialog
{
    Q_OBJECT

public:
    medDataSourceDialog(QWidget * parent = NULL, Qt::WindowFlags flag = 0);
    virtual ~medDataSourceDialog();

    static QString getFilenameFromFileSystem(QWidget *parent = NULL);
    static QStringList getFilenamesFromFileSystem(QWidget *parent = NULL);

protected:
    void addDataSource(medAbstractDataSource* dataSource);

protected:
    virtual void closeEvent(QCloseEvent *event);

private:
    medDataSourceDialogPrivate *d;
};

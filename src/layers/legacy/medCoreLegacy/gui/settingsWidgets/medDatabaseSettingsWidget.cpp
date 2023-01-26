/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2018. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medDatabaseSettingsWidget.h>
#include <QtWidgets>
#include <QtGui>
#include <QtCore>

#include <medStorage.h>
#include <medSettingsManager.h>

medDatabaseSettingsWidget::medDatabaseSettingsWidget(QWidget *parent) :
    QDialog(parent)
{
    setWindowTitle("Database");
    setMinimumWidth(500);

    dbPath = new QLineEdit(this);

    btChooseDir = new QPushButton(tr("Select dir..."), this);
    connect(btChooseDir, SIGNAL(clicked()), this, SLOT(selectDbDirectory())); 

    QWidget* databaseLocation = new QWidget(this);

    QHBoxLayout* dbLayout = new QHBoxLayout;
    dbLayout->addWidget(dbPath);
    dbLayout->addWidget(btChooseDir);
    databaseLocation->setLayout(dbLayout);

    QFormLayout* formLayout = new QFormLayout(this);
    formLayout->addRow(tr("Database location:"), databaseLocation);

    // Display the current database location
    read();

    setLayout(formLayout);
}

void medDatabaseSettingsWidget::selectDbDirectory()
{
    QFileDialog dialog(this);
    dialog.setFileMode(QFileDialog::DirectoryOnly);

    QString path;
    if (dialog.exec())
    {
        QString path = dialog.selectedFiles().first();
        if (!QDir(path).entryInfoList(QDir::NoDotAndDotDot | QDir::Dirs | QDir::Files).isEmpty() && QDir(path).entryInfoList(QStringList("db")).isEmpty())
        {
            QMessageBox::information(this, tr("Database location not valid"), tr("The directory selected is not valid:\nEither it doesn't point to an existing medInria database directory \nOR your new directory is not empty. \nPlease choose another one."));
        }
        else
        {
            dbPath->setText(path);

            // Save the new database location
            write();
        }
    }
}

void medDatabaseSettingsWidget::read()
{
    // We always show the data location here,
    //the medStorage class takes care of retrieving the correct one
    dbPath->setText(medStorage::dataLocation());
}

void medDatabaseSettingsWidget::write()
{
    medSettingsManager * mnger = medSettingsManager::instance();
    mnger->setValue("medDatabaseSettingsWidget","new_database_location", dbPath->text());
}

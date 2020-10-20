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

class medDatabaseSettingsWidgetPrivate {

public:
  QLineEdit* dbPath;
  QPushButton* btChooseDir;
};

medDatabaseSettingsWidget::medDatabaseSettingsWidget(QWidget *parent) :
        medSettingsWidget(parent),
        d(new medDatabaseSettingsWidgetPrivate())
{
    setTabName("Database");

    d->dbPath = new QLineEdit(this);
    d->btChooseDir = new QPushButton(tr("Select directory..."), this);

    QWidget* databaseLocation = new QWidget(this);
    QHBoxLayout* dbLayout = new QHBoxLayout;
    dbLayout->addWidget(d->dbPath);
    dbLayout->addWidget(d->btChooseDir);
    databaseLocation->setLayout(dbLayout);

    connect(d->btChooseDir, SIGNAL(clicked()), this, SLOT(selectDbDirectory()));

    databaseLocation->setContentsMargins(0,-8,0,0);

    QFormLayout* formLayout = new QFormLayout(this);
    formLayout->addRow(tr("Database location:"), databaseLocation);
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
             d->dbPath->setText(path);
         }
     }
}

/**
 * @brief Performs the validation of each control inside the widget.
 *
 * @param void
 * @return true is the validation is successful, false otherwise.
*/
bool medDatabaseSettingsWidget::validate()
{
    return true;
}

void medDatabaseSettingsWidget::read()
{
    // we always show the datalocation here, the medStorage class takes care of retrieving the correct one
    d->dbPath->setText(medStorage::dataLocation());
}

bool medDatabaseSettingsWidget::write()
{
    medSettingsManager * mnger = medSettingsManager::instance();
    mnger->setValue(this->identifier(),"new_database_location", d->dbPath->text());
    return true;
}

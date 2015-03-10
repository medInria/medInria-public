/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medDatabaseSettingsWidget.h>
#include <QWidget>
#include <QtGui>
#include <QtCore>

#include <medStorage.h>
#include <medSettingsManager.h>

class medDatabaseSettingsWidgetPrivate {

public:
  QLineEdit* dbPath;
  QPushButton* btChooseDir;
  QCheckBox* anonymousDb;
};

medDatabaseSettingsWidget::medDatabaseSettingsWidget(QWidget *parent) :
        medSettingsWidget(parent),
        d(new medDatabaseSettingsWidgetPrivate())
{
    setTabName("Database");

    d->dbPath = new QLineEdit(this);
    d->btChooseDir = new QPushButton(tr("Select directory..."), this);
    d->anonymousDb = new QCheckBox(this);
    d->anonymousDb->setToolTip(tr("Hide patient name in GUI"));

    QWidget* databaseLocation = new QWidget(this);
    QHBoxLayout* dbLayout = new QHBoxLayout;
    dbLayout->addWidget(d->dbPath);
    dbLayout->addWidget(d->btChooseDir);
    databaseLocation->setLayout(dbLayout);

    connect(d->btChooseDir, SIGNAL(clicked()), this, SLOT(selectDbDirectory()));

    databaseLocation->setContentsMargins(0,-8,0,0);

    QFormLayout* formLayout = new QFormLayout(this);
    formLayout->addRow(tr("Database location:"), databaseLocation);
    formLayout->addRow(tr("Anonymous database:"),d->anonymousDb);
}

void medDatabaseSettingsWidget::selectDbDirectory()
{
    QFileDialog dialog(this);
    dialog.setFileMode(QFileDialog::DirectoryOnly);

     QString path;
     if (dialog.exec())
     {
         QString path = dialog.selectedFiles().first();
         if (!QDir(path).entryInfoList(QDir::NoDotAndDotDot | QDir::Dirs | QDir::Files).isEmpty())
         {
             QMessageBox::information( this, tr("Directory not empty"), tr("The archive directory needs to be empty! \nPlease choose another one."));
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
    medSettingsManager * mnger = medSettingsManager::instance();
    d->anonymousDb->setChecked(mnger->value("database","anonymous").toBool());
}

bool medDatabaseSettingsWidget::write()
{
    medSettingsManager * mnger = medSettingsManager::instance();
    mnger->setValue(this->identifier(),"new_database_location", d->dbPath->text());
    mnger->setValue("database","anonymous",d->anonymousDb->isChecked());
    return true;
}

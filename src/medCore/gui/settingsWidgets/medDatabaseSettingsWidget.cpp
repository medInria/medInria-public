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
  QPushButton *selectDirectoryButton;
  QCheckBox* anonymousDb;
};

medDatabaseSettingsWidget::medDatabaseSettingsWidget(QWidget *parent) :
        medSettingsWidget(parent),
        d(new medDatabaseSettingsWidgetPrivate())
{
    setTabName("Database");
    QString anonymousTooltip = tr("Hide patient name in the software, data stays untouched");

    d->dbPath = new QLineEdit(this);
    d->selectDirectoryButton = new QPushButton(tr("Select directory..."), this);
    d->selectDirectoryButton->setToolTip("Change the database directory or create a new database by selecting an empty directory");
    d->anonymousDb = new QCheckBox(this);
    d->anonymousDb->setToolTip(anonymousTooltip);

    QWidget* databaseLocation = new QWidget(this);
    QHBoxLayout* dbLayout = new QHBoxLayout;
    dbLayout->addWidget(d->dbPath);
    dbLayout->addWidget(d->selectDirectoryButton);
    databaseLocation->setLayout(dbLayout);

    connect(d->selectDirectoryButton, SIGNAL(clicked()), this, SLOT(selectDirectory()));

    databaseLocation->setContentsMargins(0,-8,0,0);

    QLabel* anonymousLabel = new QLabel(tr("Anonymous database:"));
    anonymousLabel->setToolTip(anonymousTooltip);

    QFormLayout* formLayout = new QFormLayout(this);
    formLayout->addRow(tr("Database location:"), databaseLocation);
    formLayout->addRow(anonymousLabel,d->anonymousDb);
    formLayout->addRow(new QLabel("(The changes will only be effective after saving and restarting the application.)"));
}

void medDatabaseSettingsWidget::selectDirectory()
{
    QDir defaultPath = QDir(medStorage::dataLocation());
    defaultPath.cdUp();

    QString path = QFileDialog::getExistingDirectory(this, tr("Select the database directory"),
                                                     QDir(defaultPath).absolutePath(),
                                                     QFileDialog::ShowDirsOnly
                                                     | QFileDialog::DontResolveSymlinks);

    if(path != "") //the user hasn't clicked on Cancel
    {
        if (!QDir(path).entryInfoList(QDir::NoDotAndDotDot | QDir::Dirs | QDir::Files).isEmpty() && QDir(path).entryInfoList(QStringList("db")).isEmpty())
        {
            QMessageBox::information( this, tr("Database location not valid"),
                                      tr("The directory selected is not valid:\nEither it doesn't point to an existing MUSIC database directory \nOR your new directory is not empty. \nPlease choose another one."));
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
    medSettingsManager * manager = medSettingsManager::instance();
    d->anonymousDb->setChecked(manager->value("database","anonymous").toBool());
}

bool medDatabaseSettingsWidget::write()
{
    medSettingsManager * manager = medSettingsManager::instance();
    manager->setValue(this->identifier(),"new_database_location", d->dbPath->text());
    manager->setValue("database","anonymous",d->anonymousDb->isChecked());
    return true;
}

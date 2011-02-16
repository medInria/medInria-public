#include "medDatabaseSettingsWidget.h"
#include <QWidget>
#include <QtGui>
#include <QtCore>

#include <medCore/medSettingsManager.h>

class medDatabaseSettingsWidgetPrivate {

public:
  QWidget* parent;
  QLineEdit* dbPath;
  QPushButton* btChooseDir;

  medDatabaseSettingsWidgetPrivate();
  ~medDatabaseSettingsWidgetPrivate();
};

medDatabaseSettingsWidgetPrivate::medDatabaseSettingsWidgetPrivate()
{
}

medDatabaseSettingsWidgetPrivate::~medDatabaseSettingsWidgetPrivate()
{
}


medDatabaseSettingsWidget::medDatabaseSettingsWidget(QWidget *parent) :
        medSettingsWidget(parent),
        d(new medDatabaseSettingsWidgetPrivate())
{
    setSection("Database");

    d->dbPath = new QLineEdit(this);
    d->btChooseDir = new QPushButton(tr("Select directory..."), this);

    QWidget* databaseLocation = new QWidget(this);
    QHBoxLayout* dbLayout = new QHBoxLayout;
    dbLayout->addWidget(d->dbPath);
    dbLayout->addWidget(d->btChooseDir);
    databaseLocation->setLayout(dbLayout);

    connect(d->btChooseDir, SIGNAL(clicked()), this, SLOT(selectDbDirectory()));

    QFormLayout* layout = new QFormLayout;
    layout->addRow(tr("Database location:"), databaseLocation);
    this->setLayout(layout);
}

void medDatabaseSettingsWidget::selectDbDirectory()
{
	QFileDialog dialog(this);
	dialog.setFileMode(QFileDialog::DirectoryOnly);

	 QString path;
	 if (dialog.exec())
		 d->dbPath->setText(dialog.selectedFiles().first());
}


medSettingsWidget* createDatabaseSettingsWidget(QWidget *parent){
    return new medDatabaseSettingsWidget(parent);
}

bool medDatabaseSettingsWidget::validate()
{
    return true;
}

void medDatabaseSettingsWidget::read()
{
    qDebug()<<"reading QSettings";
    medSettingsManager * mnger = medSettingsManager::instance();

    // Database location

    // actual_database_location setting is never null as it is filled during
    // the initial database routine
    QString dbLoc = mnger->value("database", "actual_database_location").toString();
    d->dbPath->setText(dbLoc);
}

bool medDatabaseSettingsWidget::write()
{
    medSettingsManager * mnger = medSettingsManager::instance();
    mnger->setValue("database","new_database_location", d->dbPath->text());
    return true;
}

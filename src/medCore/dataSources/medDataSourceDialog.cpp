/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medDataSourceDialog.h>

#include <QTabWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QCloseEvent>
#include <QFileDialog>

#include <medAbstractDataSource.h>
#include <medDataSourceManager.h>
#include <medSettingsManager.h>

class medDataSourceDialogPrivate
{
public:
    QTabWidget *tabDataSourceWidget;

    QPushButton *okButton;
    QPushButton *cancelButton;

    QList <medAbstractDataSource *> dataSources;
};

medDataSourceDialog::medDataSourceDialog(QWidget* parent, Qt::WindowFlags flags):
    QDialog(parent, flags), d(new medDataSourceDialogPrivate)
{
    d->tabDataSourceWidget = new QTabWidget;
    QVBoxLayout *vLayout = new QVBoxLayout;
    vLayout->addWidget(d->tabDataSourceWidget);

    //dataSources
    foreach (medAbstractDataSource *dataSource, medDataSourceManager::instance()->dataSources())
        addDataSource(dataSource);

    d->okButton = new QPushButton(tr("Ok"));
    connect(d->okButton, SIGNAL(clicked()), this, SLOT(accept()));
    d->cancelButton = new QPushButton(tr("Cancel"));
    connect(d->cancelButton, SIGNAL(clicked()), this, SLOT(reject()));
    QHBoxLayout *hLayout = new QHBoxLayout;
    hLayout->addStretch(1);
    hLayout->addWidget(d->okButton, 0, Qt::AlignRight);
    hLayout->addWidget(d->cancelButton, 0, Qt::AlignRight);

    vLayout->addLayout(hLayout);
    vLayout->setContentsMargins(1,1,1,1);
    this->setLayout(vLayout);

    this->restoreGeometry(medSettingsManager::instance()->value("medDataSourceDialog", "geometry").toByteArray());
}

medDataSourceDialog::~medDataSourceDialog()
{
    delete d;
}


void medDataSourceDialog::addDataSource( medAbstractDataSource* dataSource )
{
    if(!dataSource->dialogWidget())
        return;

    d->dataSources << dataSource;
    d->tabDataSourceWidget->addTab(dataSource->dialogWidget(), dataSource->tabName());
}

void medDataSourceDialog::closeEvent(QCloseEvent *event)
{
    medSettingsManager::instance()->setValue("medDataSourceDialog", "geometry", this->saveGeometry());
}


QString medDataSourceDialog::getFilenameFromFileSystem(QWidget *parent)
{
    //  get last directory opened in settings.
    QString path;
    QFileDialog dialog(parent);

    medSettingsManager::instance()->setValue("getFilenameFromFileSystem", "state", dialog.saveState());
    medSettingsManager::instance()->setValue("getFilenameFromFileSystem", "geometry", dialog.saveGeometry());

    dialog.setFileMode(QFileDialog::ExistingFile);
    dialog.setViewMode(QFileDialog::Detail);
    dialog.restoreState(medSettingsManager::instance()->value("getFilenameFromFileSystem", "state").toByteArray());
    dialog.restoreGeometry(medSettingsManager::instance()->value("getFilenameFromFileSystem", "geometry").toByteArray());
    if(dialog.exec())
        path = dialog.selectedFiles().first();

    return path;
}

QStringList medDataSourceDialog::getFilenamesFromFileSystem(QWidget *parent)
{
    //  get last directory opened in settings.
    QStringList paths;
    QFileDialog dialog(parent);

    medSettingsManager::instance()->setValue("getFilenameFromFileSystem", "state", dialog.saveState());
    medSettingsManager::instance()->setValue("getFilenameFromFileSystem", "geometry", dialog.saveGeometry());

    dialog.setFileMode(QFileDialog::ExistingFile);
    dialog.setViewMode(QFileDialog::Detail);
    dialog.restoreState(medSettingsManager::instance()->value("getFilenameFromFileSystem", "state").toByteArray());
    dialog.restoreGeometry(medSettingsManager::instance()->value("getFilenameFromFileSystem", "geometry").toByteArray());
    if(dialog.exec())
        paths = dialog.selectedFiles();

    return paths;
}

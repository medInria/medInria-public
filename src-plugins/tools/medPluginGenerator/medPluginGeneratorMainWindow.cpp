/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include    "medPluginGenerator.h"
#include    "medPluginGeneratorMainWindow.h"
#include "ui_medPluginGeneratorMainWindow.h"

class medPluginGeneratorMainWindowPrivate
{
public:
    Ui::medPluginGeneratorMainWindow ui;

    QString output;
    QString type;
    QString name;
    QString description;
    QString license;
};

medPluginGeneratorMainWindow::medPluginGeneratorMainWindow(QWidget *parent) : QMainWindow(parent)
{
    this->d = new medPluginGeneratorMainWindowPrivate;
    this->d->ui.setupUi(this);
    QSettings settings;

    QDir plugins_dir = qApp->applicationDirPath() + ".";

    d->output = settings.value("path", plugins_dir.absolutePath()).toString();
    d->ui.pathLineEdit->setText(d->output);

    d->ui.descriptionTextEdit->setAcceptRichText(false);

    connect(d->ui.pathToolButton, SIGNAL(clicked()), this, SLOT(onOutputPathClicked()));
    connect(d->ui.pathLineEdit, SIGNAL(textEdited(const QString&)), this, SLOT(onOutputPathChanged()));
    connect(d->ui.pluginTypeCombo, SIGNAL(currentIndexChanged(const QString&)), this, SLOT(onPluginTypeChanged()));
    connect(d->ui.nameLineEdit, SIGNAL(textEdited(const QString&)), this, SLOT(onNameChanged()));
    connect(d->ui.descriptionTextEdit, SIGNAL(textChanged()), this, SLOT(onDescriptionChanged()));
    connect(d->ui.pluginLicenseCombo, SIGNAL(currentIndexChanged(const QString&)), this, SLOT(onPluginLicenseChanged()));
    connect(d->ui.FamilyCombo, SIGNAL(currentIndexChanged(const int&)), this, SLOT(onFamilyChanged(int)));
    connect(d->ui.generateAction,    SIGNAL(triggered()), this, SLOT(generate()));
    connect(d->ui.quitAction,        SIGNAL(triggered()), qApp, SLOT(quit()));
    connect(d->ui.aboutAction,       SIGNAL(triggered()), this, SLOT(about()));
    connect(d->ui.actionDefault_Path,SIGNAL(triggered()), this,
            SLOT(onSetDefaultPath()));

    connect(d->ui.quitButton,        SIGNAL(clicked()), qApp, SLOT(quit()));
    connect(d->ui.generateButton,    SIGNAL(clicked()), this, SLOT(generate()));

    Q_UNUSED(statusBar());
}

medPluginGeneratorMainWindow::~medPluginGeneratorMainWindow()
{
    delete this->d;
    this->d = NULL;
}

void medPluginGeneratorMainWindow::about()
{
    QMessageBox::about(this, "medPluginGenerator v0.1.0",
                       "<b>medPluginGenerator</b> aims at helping developer "
                       "generate a template to be used as a starting point for "
                       "the development of new plugins.");
}

void medPluginGeneratorMainWindow::onOutputPathClicked()
{
    d->output = QFileDialog::getExistingDirectory(this, tr("Choose Directory"),
    d->ui.pathLineEdit->text(),
                                                      QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

    d->ui.pathLineEdit->setText(d->output);

    update();
}

void medPluginGeneratorMainWindow::onOutputPathChanged()
{
    d->output = d->ui.pathLineEdit->text();

    update();
}

void medPluginGeneratorMainWindow::onPluginTypeChanged()
{
    if(d->ui.pluginTypeCombo->currentIndex() > 0) {
        d->type = d->ui.pluginTypeCombo->currentText();
        update();
    }
}

void medPluginGeneratorMainWindow::onNameChanged()
{
    d->name = d->ui.nameLineEdit->text();

    update();
}

void medPluginGeneratorMainWindow::onDescriptionChanged()
{
    d->description = d->ui.descriptionTextEdit->toPlainText();
}

void medPluginGeneratorMainWindow::onPluginLicenseChanged()
{
    if(d->ui.pluginLicenseCombo->currentIndex() > 0)
        d->license = d->ui.pluginLicenseCombo->currentText();
}

void medPluginGeneratorMainWindow::update()
{
    d->ui.outputNameLabel->setText(
        QString("%1")
        .arg(QString(d->name).replace(0, 1, QString(d->name).left(1).toLower()))
    );
}

void medPluginGeneratorMainWindow::generate()
{
    QDir outputParentDirectory(d->output);

    if(d->output.isEmpty() || !outputParentDirectory.exists()) {
        QMessageBox::warning(this, "Plugin generation", "Specify a valid output directory.");
        return;
    }

    if(d->type.isEmpty()) {
        QMessageBox::warning(this, "Plugin generation", "Specify a plugin type.");
        return;
    }
    
    if(d->name.isEmpty()) {
        QMessageBox::warning(this, "Plugin generation", "Specify a name.");
        return;
    }
    
    medPluginGenerator generator;
    generator.setPluginFamily(static_cast<medPluginGenerator::PluginFamily>(d->ui.FamilyCombo->currentIndex()));
    generator.setOutputDirectory(d->output);
    generator.setName(d->name);
    generator.setType(d->type);
    generator.setDescription(d->description);
    generator.setLicense(d->license);

    if(generator.run())
        statusBar()->showMessage("Generation succeeded", 2000);
    else
        QMessageBox::warning(this, "Plugin generation", "Generation failed.");
}

void medPluginGeneratorMainWindow::onFamilyChanged(int index)
{
    if (index !=0)
    {
        d->ui.pluginTypeCombo->setCurrentIndex(2);
        d->ui.pluginTypeCombo->setEnabled(false);
    }
    else
    {
        d->ui.pluginTypeCombo->setCurrentIndex(0);
        d->ui.pluginTypeCombo->setEnabled(true);
    }
}

void medPluginGeneratorMainWindow::onSetDefaultPath()
{
    d->output = QFileDialog::getExistingDirectory(this, tr("Choose default Directory"),
    d->output,
                                                      QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    QSettings settings;
    settings.setValue("path",d->output);
    d->ui.pathLineEdit->setText(d->output);
}

/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include "medPathParameter.h"

#include <QComboBox>
#include <QDir>
#include <QPushButton>
#include <QLineEdit>
#include <QFileDialog>
#include <QHBoxLayout>

#include <medSettingsManager.h>

class medPathParameterPrivate
{
public:
    QPushButton* pushButton;
    QIcon icon;
    QSize iconSize;
    QString text;
    QLineEdit* lineEdit;
    QWidget* widget;
    QString currentValue;
    QString exploratorCaption;
    QString exploratorFilter;
    bool explorFile;

    ~medPathParameterPrivate()
    {
        delete pushButton;
        delete lineEdit;
        delete widget;
    }
};

medPathParameter::medPathParameter(QString name, QObject *parent):
    medAbstractStringParameter(name, parent),
    d(new medPathParameterPrivate)
{
    d->lineEdit = NULL;
    d->pushButton = NULL;
    d->widget = NULL;
    d->text = "...";
    d->icon = QIcon();
    d->iconSize = QSize(15,15);
    d->explorFile = true;
    d->exploratorFilter = tr("All (*)");
}

medPathParameter::~medPathParameter()
{
    delete d;
}


void medPathParameter::clear()
{
    if(d->lineEdit)
        d->lineEdit->clear();;
}

void medPathParameter::setButtonIcon(QIcon &icon)
{
    d->icon = icon;
    if(d->pushButton)
        d->pushButton->setIcon(icon);
}

void medPathParameter::setButtonIconSize(QSize& size)
{
    d->iconSize = size;
    if(d->pushButton)
    {
        d->pushButton->setIconSize(size);
        d->pushButton->setMinimumSize(size);
    }
}

void medPathParameter::setButtonText(QString& value)
{
    d->text = value;

    if(d->pushButton)
        d->pushButton->setText(value);
}


QWidget* medPathParameter::getWidget()
{
    if(!d->widget)
    {
        d->widget = new QWidget;
        QHBoxLayout *layout = new QHBoxLayout(d->widget);
        d->lineEdit = new QLineEdit;
        d->lineEdit->setText(m_value);
        d->pushButton = new QPushButton;
        d->pushButton->setToolTip(tr("Browse your file system..."));
        layout->addWidget(d->lineEdit);
        layout->addWidget(d->pushButton);
        d->pushButton->setText(d->text);
        if(!d->icon.isNull())
        {
            d->pushButton->setIcon(d->icon);
            d->pushButton->setIconSize(d->iconSize);
            d->pushButton->setMinimumSize(d->iconSize);
        }
        this->addToInternWidgets(d->widget);
        connect(d->lineEdit, SIGNAL(textChanged(QString)), this, SLOT(setValue(QString)));
        connect(d->widget, SIGNAL(destroyed()), this, SLOT(removeInternWidget()));
    }
    return d->widget;
}

void medPathParameter::setExploratorCaption(QString &caption)
{
    d->exploratorCaption = caption;
}

void medPathParameter::setExploratorFilter(QString &filter)
{
    d->exploratorFilter = filter;
}

void medPathParameter::setExplorDirectory(bool explorDirectory)
{
    d->explorFile = !explorDirectory;
}

void medPathParameter::setExplorFile(bool explorFile)
{
    d->explorFile = explorFile;
}

void medPathParameter::openExplorator()
{
    //  get last directory opened in settings.
    QString path = medSettingsManager::instance()->value("path", this->name(), QDir::homePath()).toString();
    QString caption = d->exploratorCaption;
    if(caption == QString())
    {
        QString fileOrDirectory = tr("file");
        if(!d->explorFile)
            fileOrDirectory = tr("directory");

        caption = tr("Select a ") + fileOrDirectory + ".";
    }
    if(d->explorFile)
        path = QFileDialog::getOpenFileName(0, caption, path, d->exploratorFilter);
    else
        path = QFileDialog::getExistingDirectory(0, caption, path, QFileDialog::ShowDirsOnly);

    if (path.isEmpty())
        return;

    //  save last directory opened in settings.
    medSettingsManager::instance()->setValue("path", this->name(), path);

    this->setValue(path);
}

void medPathParameter::updateInternWigets()
{
    if (d->widget)
        d->lineEdit->setText(m_value);
}

void medPathParameter::removeInternWidget()
{
    this->removeFromInternWidgets(d->widget);
    d->widget = NULL;
    d->pushButton = NULL;
    d->lineEdit = NULL;
}

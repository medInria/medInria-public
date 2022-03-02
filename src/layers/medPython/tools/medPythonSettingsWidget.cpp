#include "medPythonSettingsWidget.h"

#include <QDir>
#include <QFileDialog>
#include <QHBoxLayout>
#include <QLabel>
#include <QListWidget>
#include <QMessageBox>
#include <QPushButton>
#include <QStandardPaths>

#include <medPython.h>

namespace med::python
{

class PythonSettingsWidgetPrivate
{
public:
    QListWidget* pathsList;
    QPushButton* removePathButton;
};

PythonSettingsWidget::PythonSettingsWidget(QWidget *parent) :
    medSettingsWidget(parent), d(new PythonSettingsWidgetPrivate())
{
    setTabName("Python");

    QVBoxLayout* mainLayout = new QVBoxLayout(this);

    d->pathsList = new QListWidget(this);
    mainLayout->addWidget(new QLabel("External module paths:"));
    mainLayout->addWidget(d->pathsList);

    QWidget* pathButtonsWidget = new QWidget;
    QHBoxLayout* pathButtonsLayout = new QHBoxLayout(pathButtonsWidget);
    mainLayout->addWidget(pathButtonsWidget);

    QPushButton* addPathButton = new QPushButton("Add");
    pathButtonsLayout->addWidget(addPathButton);
    connect(addPathButton, &QPushButton::clicked, this, &PythonSettingsWidget::addPath);

    d->removePathButton = new QPushButton("Remove");
    pathButtonsLayout->addWidget(d->removePathButton);
    d->removePathButton->setEnabled(false);

    connect(d->pathsList, &QListWidget::itemSelectionChanged, [=]() {
        d->removePathButton->setEnabled(!d->pathsList->selectedItems().isEmpty());
    });

    connect(d->removePathButton, &QPushButton::clicked, this, [=]() {
        foreach (QListWidgetItem* item, d->pathsList->selectedItems())
        {
            delete item;
        }
    });
}

bool PythonSettingsWidget::write()
{
    QStringList paths;

    for (int row = 0; row < d->pathsList->count(); row++)
    {
        QListWidgetItem* item = d->pathsList->item(row);
        paths << QDir::fromNativeSeparators(item->text());
    }

    setStartupPythonPaths(paths);

    return true;
}

void PythonSettingsWidget::read()
{
    d->pathsList->clear();

    foreach (QString path, getStartupPythonPaths())
    {
        new QListWidgetItem(QDir::toNativeSeparators(path), d->pathsList);
    }
}

bool PythonSettingsWidget::validate()
{
    return true;
}

void PythonSettingsWidget::addPath()
{
    QString defaultPath = QStandardPaths::writableLocation(QStandardPaths::HomeLocation);
    QString path = QFileDialog::getExistingDirectory(this,
                                                     tr("Select a path to add to Python's path variable"),
                                                     QDir::toNativeSeparators(defaultPath),
                                                     QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

    if(!path.isEmpty())
    {
        path = QDir::toNativeSeparators(path);

        if (d->pathsList->findItems(path, Qt::MatchFixedString).isEmpty())
        {
            new QListWidgetItem(path, d->pathsList);
        }
    }
}

} // namespace med::python

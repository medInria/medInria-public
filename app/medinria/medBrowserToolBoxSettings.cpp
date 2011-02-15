#include "medBrowserToolBoxSettings.h"

#include <QtGui>
#include <QMessageBox>
#include <medGui/medSettingsEditor.h>
#include <medGui/medToolBox.h>
#include <medGui/medSystemSettingsWidget.h>

class medBrowserToolBoxSettingsPrivate
{
public:
    medSettingsEditor* editor;

    medBrowserToolBoxSettingsPrivate();
    ~medBrowserToolBoxSettingsPrivate();
};

medBrowserToolBoxSettingsPrivate::medBrowserToolBoxSettingsPrivate()
{
    editor= 0;
}

medBrowserToolBoxSettingsPrivate::~medBrowserToolBoxSettingsPrivate(){
    delete editor;
}


medBrowserToolBoxSettings::medBrowserToolBoxSettings(QWidget *parent) : medToolBox(parent), d(new medBrowserToolBoxSettingsPrivate)
{
    QWidget *container = new QWidget(this);

    QVBoxLayout *layout = new QVBoxLayout(container);
    QPushButton *openButton = new QPushButton("Open", container);
    layout->addWidget(openButton);

    connect (openButton,  SIGNAL(clicked()),          this, SLOT (onClick()));

    this->setTitle("Settings Launcher");
    this->addWidget(container);
}

medBrowserToolBoxSettings::~medBrowserToolBoxSettings(void)
{
    delete d;

    d = NULL;
}

void medBrowserToolBoxSettings::onClick()
{

    if (d->editor)
    {
        d->editor->show();
        return;
    }

    d->editor = new medSettingsEditor(this);
    d->editor->setGeometry(100,100, 500, 500);
    d->editor->setWindowFlags(Qt::Tool);

    d->editor->show();

}

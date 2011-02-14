#include "medBrowserToolBoxSettings.h"

#include <QtGui>
#include <QMessageBox>
#include <medGui/medSettingsEditor.h>
#include <medGui/medToolBox.h>
#include <medGui/medSystemSettingsWidget.h>

class medBrowserToolBoxSettingsPrivate
{
public:

};

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
//	 QMessageBox msgBox;
//	 msgBox.setText("Cool.");
//	 msgBox.exec();

	QFrame* popup = new QFrame(this, Qt::Popup | Qt::Window );
	popup->resize(500,500);

	medSettingsEditor* editor = new medSettingsEditor(popup);

	popup->show();

}

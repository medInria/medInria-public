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
{//popup->resize(700,700);
    delete d;

    d = NULL;
}

void medBrowserToolBoxSettings::onClick()
{
//	 QMessageBox msgBox;
//	 msgBox.setText("Cool.");
//	 msgBox.exec();

	QFrame* popup = new QFrame(this, Qt::Popup | Qt::Window );
	medSettingsEditor* editor = new medSettingsEditor(popup);
	editor->setGeometry(100,100, 500, 500);
	//editor->setFocus();

	popup->show();

}

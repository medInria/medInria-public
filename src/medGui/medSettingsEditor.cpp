#include "medSettingsEditor.h"


#include <QtGui>
#include <QWidget>

#include <medCore/medMessageController.h>
#include <medGui/medSystemSettingsWidget.h>
#include <medGui/medSettingsWidgetFactory.h>

class medSettingsEditorPrivate {

public:
  QWidget* parent;
  QTabWidget* tabWidget;
  QHash<QString,medSettingsWidget*> settingsWidgets;
  //medSettingsWidget * systemSettingsWidget;

  medSettingsEditorPrivate();
  ~medSettingsEditorPrivate();
};

medSettingsEditorPrivate::medSettingsEditorPrivate()
{
}

medSettingsEditorPrivate::~medSettingsEditorPrivate()
{
    foreach(medSettingsWidget* widget, settingsWidgets)
    {
        delete widget;
    }
}

medSettingsEditor::medSettingsEditor(QWidget *parent) :
    QWidget(parent),d(new medSettingsEditorPrivate())
{
    QVBoxLayout * vLayout = new QVBoxLayout(this);
    setLayout(vLayout);
    this->setFixedSize(QSize(500,500));
    d->tabWidget = new QTabWidget (this);
    d->tabWidget->setTabPosition(QTabWidget::West);
    d->tabWidget->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);

    medSettingsWidgetFactory * settingsFactory =
            medSettingsWidgetFactory::instance();
    //System widget
    medSettingsWidget * systemSettingsWidget = settingsFactory->createSettingsWidget("System",d->tabWidget);

    d->tabWidget->addTab(systemSettingsWidget,
                         systemSettingsWidget->section());
    d->settingsWidgets.insert("System",systemSettingsWidget);

    //Dicom widget

    //Startup widget

    medSettingsWidget * startupSettingsWidget = settingsFactory->createSettingsWidget ("Startup",
                                                                                      d->tabWidget);

    d->tabWidget->addTab(startupSettingsWidget,
                         startupSettingsWidget->section());
    d->settingsWidgets.insert("Startup",
                              startupSettingsWidget);


    //Logging widget

    //Process manager widget
    medSettingsWidget * setWid;
    foreach (QString widgetStyle, settingsFactory->settingsWidgets())
    {
        if (!d->settingsWidgets.contains(widgetStyle))
        {
            setWid = settingsFactory->createSettingsWidget(widgetStyle,d->tabWidget);

            d->tabWidget->addTab(startupSettingsWidget,
                             startupSettingsWidget->section());
            d->settingsWidgets.insert(widgetStyle, setWid);
        }
    }

    this->layout()->addWidget(d->tabWidget);

    //save button
    QPushButton * save = new QPushButton (tr("Save"),this);
    connect(save,SIGNAL(clicked()),
            this,SLOT(onSaveClicked()));

    //cancel button
    QPushButton * cancel = new QPushButton (tr("Cancel"),this);
    connect(cancel,SIGNAL(clicked()),
            this,SLOT(onCancelClicked()));
    //reset button
    QPushButton * reset = new QPushButton (tr("Reset"), this);
    connect(reset,SIGNAL(clicked()),
            this,SLOT(onResetClicked()));

    QHBoxLayout * buttonLayout = new QHBoxLayout(this);
    buttonLayout->addWidget(reset);
    buttonLayout->addWidget(cancel);
    buttonLayout->addWidget(save);
    vLayout->addLayout(buttonLayout);

    connect(this,SIGNAL(showError(QObject*,const        QString&,unsigned int)),
            medMessageController::instance(),SLOT(showError (QObject*,const QString&,unsigned int)));

    foreach(medSettingsWidget * setting, d->settingsWidgets)
    {
        setting->read();
    }
}


void medSettingsEditor::onSaveClicked(){



    foreach(medSettingsWidget * setting, d->settingsWidgets)
    {
        if (!setting->validate())
        {
            qDebug()<<"validation of section"<< setting->section() <<
                    "failed";
            QString error = "Error in validation of section";
            error.append(setting->section());
            emit (showError(this,error,3));
        }
        else
        {
            //do the saving
            setting->write();
            qDebug()<<"validation of section"<< setting->section() <<
                    "successful";
        }
    }
}

void medSettingsEditor::onCancelClicked(){
    this->close();
}

void medSettingsEditor::onResetClicked(){
    foreach(medSettingsWidget * setting, d->settingsWidgets)
    {
        setting->read();
    }
}

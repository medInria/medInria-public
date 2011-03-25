#include "medSettingsEditor.h"


#include <QtGui>
#include <QWidget>

#include <dtkGui/dtkSettingsEditor.h>

#include <medCore/medMessageController.h>
#include <medGui/medSystemSettingsWidget.h>
#include <medGui/medSettingsWidgetFactory.h>

class medSettingsEditorPrivate {

public:

  // methods
  void read();

  // member
  QWidget* parent;
  QStackedWidget * stack;
  QTabWidget* tabWidget;
  dtkSettingsEditor *editor;
  QPushButton* advanced;
  QPushButton* save;
  QPushButton* reset;
  QPushButton* cancel;
  QHash<QString,medSettingsWidget*> settingsWidgets;
  bool isInitialized;  
};


medSettingsEditor::medSettingsEditor(QWidget *parent) :
    QWidget(parent),d(new medSettingsEditorPrivate())
{
    d->isInitialized = false;


}

void medSettingsEditorPrivate::read()
{
    foreach(medSettingsWidget * setting, settingsWidgets)
    {
        setting->read();
    }
}

void medSettingsEditor::onSaveClicked()
{
    if(this->save())
        this->close();
}

void medSettingsEditor::onCancelClicked()
{
    this->close();
}

void medSettingsEditor::onResetClicked()
{
    d->read();
}

void medSettingsEditor::onAdvancedClicked()
{
    if (d->stack->currentIndex()){
        //index is 1 ->advanced->default
        d->read();
        d->cancel->show();
        d->reset->show();
        d->advanced->setText(tr("Advanced"));
        d->stack->setCurrentIndex(0);
    }
    else{
        d->advanced->setText(tr("Basic"));
        d->cancel->hide();
        d->reset->hide();
        QSettings settings;
        d->editor->setSettings(settings.organizationName(), settings.applicationName());
        d->stack->setCurrentIndex(1);
    }

}

void medSettingsEditor::initialize()
{
    if (d->isInitialized)
        return;

    QVBoxLayout * vLayout = new QVBoxLayout(this);
    setLayout(vLayout);
    d->stack = new QStackedWidget(this);
    d->tabWidget = new QTabWidget (this);
    d->tabWidget->setTabPosition(QTabWidget::West);
    d->tabWidget->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);

    //use stackedWidget to display the advanced or normal editor
    d->stack->addWidget(d->tabWidget);

    d->editor = new dtkSettingsEditor(this);
    d->stack->addWidget(d->editor);

    this->layout()->addWidget(d->stack);

    int buttonWidth = 100;

    //advanced button
    d->advanced = new QPushButton (tr("Advanced"),this);
    d->advanced->setMaximumWidth(buttonWidth);
    connect(d->advanced,SIGNAL(clicked()),
        this,SLOT(onAdvancedClicked()));

    //save button
    d->save = new QPushButton (tr("Save"),this);
    d->save->setMaximumWidth(buttonWidth);
    connect(d->save,SIGNAL(clicked()),
        this,SLOT(onSaveClicked()));

    //cancel button
    d->cancel = new QPushButton (tr("Cancel"),this);
    d->cancel->setMaximumWidth(buttonWidth);
    connect(d->cancel,SIGNAL(clicked()),
        this,SLOT(onCancelClicked()));

    //reset button
    d->reset = new QPushButton (tr("Reset"), this);
    d->reset->setMaximumWidth(buttonWidth);
    connect(d->reset,SIGNAL(clicked()),
        this,SLOT(onResetClicked()));

    QHBoxLayout * buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch(1);
    buttonLayout->addWidget(d->advanced,1);
    buttonLayout->addWidget(d->reset,1);
    buttonLayout->addWidget(d->cancel,1);
    buttonLayout->addWidget(d->save,1);
    vLayout->addLayout(buttonLayout);

    connect(this,SIGNAL(showError(QObject*,const        QString&,unsigned int)),
        medMessageController::instance(),SLOT(showError (QObject*,const QString&,unsigned int)));
    connect(this,SIGNAL(showInfo(QObject*,const        QString&,unsigned int)),
        medMessageController::instance(),SLOT(showInfo (QObject*,const QString&,unsigned int)));

    d->isInitialized = true;
}

void medSettingsEditor::queryWidgets()
{
    medSettingsWidgetFactory * settingsFactory =
        medSettingsWidgetFactory::instance();

    //Process manager widget
    medSettingsWidget * setWid;
    QScrollArea * scroll;
    foreach (QString widgetStyle, settingsFactory->settingsWidgets())
    {
        if (!d->settingsWidgets.contains(widgetStyle))
        {
            setWid = settingsFactory->createSettingsWidget(widgetStyle,d->tabWidget);
            scroll = new QScrollArea(this);

            scroll->setWidget(setWid);
            d->tabWidget->addTab(scroll, setWid->tabName());
            d->settingsWidgets.insert(widgetStyle, setWid);
            // read the settings for this widget
            setWid->read();
        }
    }
}

bool medSettingsEditor::save()
{
    bool success = true;
    if (!d->stack->currentIndex())
    {
        foreach(medSettingsWidget * setting,
            d->settingsWidgets)
        {
            if (!setting->validate())
            {
                qDebug()<<"validation of tabname"<< setting->tabName() <<
                    "failed";
                QString error = tr("Error in validation of tabname ");
                error.append(setting->tabName());
                emit (showError(this,error,3000));
                success = false;
            }
            else
            {
                //do the saving
                setting->write();
            }
        }
    }
    if (success)
    {
        emit (showInfo(this,
            tr("Settings successfully saved"),
            3000));
    }

    return success;
}

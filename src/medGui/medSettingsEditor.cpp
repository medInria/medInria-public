#include "medSettingsEditor.h"


#include <QtGui>
#include <QWidget>

#include <dtkGui/dtkSettingsEditor.h>

#include <medCore/medMessageController.h>
#include <medGui/medSystemSettingsWidget.h>
#include <medGui/medSettingsWidgetFactory.h>

class medSettingsEditorPrivate {

public:
  QWidget* parent;
  QStackedWidget * stack;
  QTabWidget* tabWidget;
  dtkSettingsEditor *editor;
  QPushButton* advanced;
  QPushButton* reset;
  QPushButton* cancel;
  QHash<QString,medSettingsWidget*> settingsWidgets;
  //medSettingsWidget * systemSettingsWidget;

  medSettingsEditorPrivate();
  ~medSettingsEditorPrivate();

  void read();
};

medSettingsEditorPrivate::medSettingsEditorPrivate()
{

}

medSettingsEditorPrivate::~medSettingsEditorPrivate()
{

}

medSettingsEditor::medSettingsEditor(QWidget *parent) :
    QWidget(parent),d(new medSettingsEditorPrivate())
{
    QVBoxLayout * vLayout = new QVBoxLayout(this);
    setLayout(vLayout);
    this->setFixedSize(QSize(500,500));
    d->stack = new QStackedWidget(this);
    d->tabWidget = new QTabWidget (this);
    d->tabWidget->setTabPosition(QTabWidget::West);
    d->tabWidget->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);

    //use stackedWidget to display the advanced or normal editor
    d->stack->addWidget(d->tabWidget);

    d->editor = new dtkSettingsEditor(this);
    d->stack->addWidget(d->editor);


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
            d->tabWidget->addTab(scroll,
                             setWid->section());
            d->settingsWidgets.insert(widgetStyle, setWid);
        }
    }

    //fill in widgets with the settings
    d->read();

    this->layout()->addWidget(d->stack);

    //advanced button
    d->advanced = new QPushButton (tr("Advanced"),this);
    connect(d->advanced,SIGNAL(clicked()),
            this,SLOT(onAdvancedClicked()));
    //save button
    QPushButton * save = new QPushButton (tr("Save"),this);
    connect(save,SIGNAL(clicked()),
            this,SLOT(onSaveClicked()));

    //cancel button
    d->cancel = new QPushButton (tr("Cancel"),this);
    connect(d->cancel,SIGNAL(clicked()),
            this,SLOT(onCancelClicked()));
    //reset button
    d->reset = new QPushButton (tr("Reset"), this);
    connect(d->reset,SIGNAL(clicked()),
            this,SLOT(onResetClicked()));

    QHBoxLayout * buttonLayout = new QHBoxLayout(this);
    buttonLayout->addWidget(d->advanced);
    buttonLayout->addWidget(d->reset);
    buttonLayout->addWidget(d->cancel);
    buttonLayout->addWidget(save);
    vLayout->addLayout(buttonLayout);

    connect(this,SIGNAL(showError(QObject*,const        QString&,unsigned int)),
            medMessageController::instance(),SLOT(showError (QObject*,const QString&,unsigned int)));
    connect(this,SIGNAL(showInfo(QObject*,const        QString&,unsigned int)),
            medMessageController::instance(),SLOT(showInfo (QObject*,const QString&,unsigned int)));


}

void medSettingsEditorPrivate::read()
{
    foreach(medSettingsWidget * setting, settingsWidgets)
    {
        setting->read();
    }
}

void medSettingsEditor::onSaveClicked(){

    bool success = true;
    if (!d->stack->currentIndex())
    {
        foreach(medSettingsWidget * setting,
                d->settingsWidgets)
        {
            if (!setting->validate())
            {
                qDebug()<<"validation of section"<< setting->section() <<
                        "failed";
                QString error = tr("Error in validation of section ");
                error.append(setting->section());
                emit (showError(this,error,3000));
                success = false;
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
    if (success)
    {
         emit (showInfo(this,
                           tr("Settings successfully saved"),
                           3000));
        this->close();
    }
}

void medSettingsEditor::onCancelClicked(){
    this->close();
}

void medSettingsEditor::onResetClicked(){
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
        d->advanced->setText(tr("Default"));
        d->cancel->hide();
        d->reset->hide();
        d->editor->setSettings("inria", "medinria");
        d->stack->setCurrentIndex(1);
    }

}

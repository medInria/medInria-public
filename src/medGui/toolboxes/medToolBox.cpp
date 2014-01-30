/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medAbstractData.h>

#include <medAbstractView.h>

#include <medToolBox.h>
#include <medToolBoxHeader.h>
#include <medToolBoxBody.h>
#include <medToolBoxTab.h>
#include <medButton.h>

#include <dtkCore/dtkGlobal.h>
#include <dtkCore/dtkPlugin>
#include <dtkGui/dtkAboutPlugin.h>

class medToolBoxPrivate
{
public:
    medToolBoxHeader *header;
    medToolBoxBody *body;
    bool isMinimized;

    QStringList validDataTypes;
    bool isContextVisible;
    bool aboutPluginVisibility;
    dtkPlugin* plugin;

public:
    QBoxLayout *layout;
};

medToolBox::medToolBox(QWidget *parent) : QWidget(parent), d(new medToolBoxPrivate)
{
    //d->view = 0;

    d->header = new medToolBoxHeader(this);
    d->body = new medToolBoxBody(this);
    d->isContextVisible = false;
    d->aboutPluginVisibility = false;
    d->plugin= NULL;


    d->layout = new QBoxLayout(QBoxLayout::TopToBottom, this);
    d->layout->setContentsMargins(0, 0, 0, 0);
    d->layout->setSpacing(0);
    d->layout->addWidget(d->header);
    d->layout->addWidget(d->body);
    d->isMinimized = false;
    connect(d->header,SIGNAL(triggered()),this,SLOT(switchMinimize()));

    this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
}

medToolBox::~medToolBox(void)
{
    delete d;

    d = NULL;
}

void medToolBox::setTabWidget(medToolBoxTab *tab)
{
    d->body->setTabWidget(tab);
}

void medToolBox::addWidget(QWidget *widget)
{
    d->body->addWidget(widget);
}

void medToolBox::setTitle(const QString &title)
{
    d->header->setTitle(title);
}

void medToolBox::setTitleOffset(const QPoint & titleOffset)
{
    d->header->setTitleOffset(titleOffset);
}

medToolBoxHeader *medToolBox::header(void) const
{
    return d->header;
}

medToolBoxBody *medToolBox::body(void) const
{
    return d->body;
}

void medToolBox::update(dtkAbstractView *view)
{
//    medAbstractView* medView = dynamic_cast<medAbstractView*>(view);
//    if (medView)
//        setContextVisibility(medView->dataTypes());
//    else
//    {
//        setContextVisibility(QHash<QString, unsigned int> ());
//    }
//    //DTK_DEFAULT_IMPLEMENTATION;
//    //DTK_UNUSED(view);
}

void medToolBox::clear(void)
{
    DTK_DEFAULT_IMPLEMENTATION;
}

void medToolBox::setOrientation(Qt::Orientation orientation)
{
    switch(orientation) {
    case Qt::Vertical:
        if (d->layout->direction() == QBoxLayout::TopToBottom) {
            return;
        } else {
            d->body->setOrientation(Qt::Vertical);
        }
        break;
    case Qt::Horizontal:
        if (d->layout->direction() == QBoxLayout::LeftToRight) {
            return;
        } else {
            d->body->setOrientation(Qt::Horizontal);
        }
        break;
   }
}

Qt::Orientation medToolBox::orientation() const
{
    if(d->layout->direction() == QBoxLayout::LeftToRight)
        return Qt::Horizontal;
    else
        return Qt::Vertical;
}

void medToolBox::switchMinimize()
{
    //isMinimized before switch == wanted body visibility
    d->body->setVisible(d->isMinimized);
    d->isMinimized = !d->isMinimized;
}

bool medToolBox::ContextVisible()
{
 return d->isContextVisible;
}

void medToolBox::show()
{
    //JGG qDebug()<<"show TB:" << header()->title();
    if(d->validDataTypes.isEmpty() || d->isContextVisible)
    {
        //JGG qDebug()<<"actually showing in TB";
        QWidget::show();
    }
}

void medToolBox::setValidDataTypes(const QStringList & dataTypes)
{
    d->validDataTypes = QStringList(dataTypes);

}

const QStringList medToolBox::ValidDataTypes()
{
   return  d->validDataTypes;

}

void medToolBox::addValidDataType(const QString & dataType)
{
    if (!d->validDataTypes.contains(dataType))
    {
        d->validDataTypes.append(dataType);
    }
}

void medToolBox::setContextVisibility(
        const QHash<QString, unsigned int> & viewDataTypes )
{
    //JGG qDebug()<< "setContextVisibility";
    if (d->validDataTypes.isEmpty())
    {
        //JGG qDebug()<< "no datatypes";
        d->isContextVisible = true;
    }
    else
    {
        //JGG qDebug()<<"View datatypes"<<viewDataTypes.keys()<< "values" << viewDataTypes.values();
        d->isContextVisible = false;
        foreach(QString validDataType, d->validDataTypes)
        {
            //JGG qDebug()<<"datatype"<< validDataType ;
            if(viewDataTypes.contains(validDataType))
            {
                //JGG qDebug()<<"viewDataTypes: "<< viewDataTypes.value(validDataType);
                if(viewDataTypes.value(validDataType)!=0)
                {
                    d->isContextVisible = true;
                    break;
                }
            }
        }
    }
    //JGG qDebug()<<"visibility" << d->isContextVisible ;
    this->setVisible(d->isContextVisible);
}

void medToolBox::setAboutPluginVisibility(bool enable)
{
    d->aboutPluginVisibility = enable;
    d->header->setAboutButtonVisibility(enable);
}

bool medToolBox::aboutPluginVisibility()
{
    return d->aboutPluginVisibility;
}

void medToolBox::setAboutPluginButton(dtkPlugin *plugin)
{
    medButton* aboutButton = d->header->aboutButton();
    if (aboutButton)
    {
        aboutButton->disconnect();
        connect(aboutButton,SIGNAL(triggered()),this,SLOT(onAboutButtonClicked()));
        d->plugin = plugin;
    }
    else
    {
        qWarning() << "no aboutButton found for toolbox" << d->header->title();
    }
}

void medToolBox::onAboutButtonClicked()
{
    if(d->plugin)
    {
        qDebug() << "about plugin" << d->plugin->name();

        QDialog * dial = new QDialog(this);
        QString windowTitle = tr("medInria: about ");
        windowTitle += d->plugin->name();
        dial->setWindowTitle(windowTitle);
        dtkAboutPlugin * apWidget = new dtkAboutPlugin(d->plugin,dial);

        QVBoxLayout * layout = new QVBoxLayout(dial);

        QPushButton * okBut = new QPushButton(dial);
        okBut->setText("Ok");
        okBut->setFocusPolicy(Qt::NoFocus);
        QObject::connect(okBut, SIGNAL(clicked()), dial, SLOT(close()));

        QHBoxLayout * butLayout = new QHBoxLayout;
        butLayout->addStretch();
        butLayout->addWidget(okBut);
        butLayout->addStretch();

        layout->addWidget(apWidget);
        layout->addLayout(butLayout);

        dial->setLayout(layout);
        dial->exec();
    }
    else
    {
        qWarning() << "No plugin set for toolbox" << d->header->title();
    }
}

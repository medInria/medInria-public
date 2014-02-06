#include "medParameterPoolManagerToolBox.h"

#include <medParameterPoolManager.h>
#include <medAbstractParameter.h>
#include <medParameterPool.h>

class medParameterPoolManagerToolBoxPrivate
{
public:
    medParameterPoolManager *mngr;
    QWidget *linkAllWidget;
    QListWidget *parameters;
    QListWidget *pools;

    QString currentParam;

    QHash<medAbstractParameter*, QComboBox*> comboHash;
    QHash<medAbstractParameter*, QPushButton*> buttonHash;
    QHash<medAbstractParameter*, QListWidgetItem*> itemHash;

    QHash<int, QListWidgetItem*> poolHash;
    QHash<int, QPushButton*> poolIdButtonHash;

};


medParameterPoolManagerToolBox::medParameterPoolManagerToolBox() : d(new medParameterPoolManagerToolBoxPrivate)
{
    this->setTitle("Parameters Pool Managment");
    d->mngr = medParameterPoolManager::instance();

    d->mngr->setToolBox(this);

    d->linkAllWidget = new QWidget;
    QHBoxLayout* layout = new QHBoxLayout(d->linkAllWidget);
    layout->setContentsMargins(0,0,10,0);
    QLabel *linkAllWidgetLabel = new QLabel("Link all parameters?");
    QPushButton *okButton = new QPushButton;
    okButton->setIcon(QIcon(":/icons/link.svg"));
    layout->addWidget(linkAllWidgetLabel);
    layout->addStretch();
    layout->addWidget(okButton);

    d->parameters = new QListWidget(this);
    d->parameters->setContentsMargins(0,0,0,0);

    d->pools = new QListWidget(this);

    this->addWidget(d->linkAllWidget);
    this->addWidget(d->parameters);
    this->addWidget(new QLabel("Pools:"));
    this->addWidget(d->pools);

    connect(d->mngr, SIGNAL(poolCreated(medParameterPool*,int)), this, SLOT(addPool(medParameterPool*,int)));
    connect(d->mngr, SIGNAL(poolRemoved(int)), this, SLOT(removePoolFromWidget(int)));
    connect(d->mngr, SIGNAL(multipleViewsSelected()), this, SLOT(showLinkAllWidget()));
    connect(d->mngr, SIGNAL(singleViewSelected()), this, SLOT(hideLinkAllWidget()));
    connect(okButton, SIGNAL(clicked()), this, SLOT(linkAll()));

}

void medParameterPoolManagerToolBox::addParameter(medAbstractParameter* parameter)
{
    if(d->currentParam != parameter->name())
    {
        d->currentParam = parameter->name();

        QWidget *paramWidget = new QWidget(d->parameters);
        QHBoxLayout* layout = new QHBoxLayout(paramWidget);
        layout->setContentsMargins(0,0,10,0);

        QLabel *paramLabel = new QLabel("Keep "+parameter->name()+" link?", paramWidget);

        QComboBox* comboBox = new QComboBox(paramWidget);
        comboBox->addItem("New", -1);

        QList<int> poolsId = d->mngr->poolsId(parameter->name());

        foreach(int poolId, poolsId)
            comboBox->addItem(parameter->name() + QString::number(poolId), poolId);

        QPushButton *okButton = new QPushButton;
        okButton->setIcon(QIcon(":/icons/link.svg"));

        QPushButton *removeButton = new QPushButton;
        removeButton->setIcon(QIcon(":/icons/cross.svg"));

        layout->addWidget(paramLabel);
        layout->addWidget(comboBox);
        layout->addWidget(okButton);
        layout->addWidget(removeButton);

        QListWidgetItem *item = new QListWidgetItem(d->parameters);
        d->parameters->addItem(item);
        d->parameters->setItemWidget(item, paramWidget);

        connect(okButton, SIGNAL(clicked()), this, SLOT(addToPool()));
        //connect(removeButton, SIGNAL(clicked()), this, SLOT(removeParamItemWidget()));

        d->comboHash.insert(parameter, comboBox);
        d->buttonHash.insert(parameter, okButton);
        d->itemHash.insert(parameter, item);

    }
}


QIcon medParameterPoolManagerToolBox::createIcon(QString &colorName) const
{
    QPixmap iconPixmap(32,32);
    iconPixmap.fill(QColor(colorName));
    QIcon itemIcon(iconPixmap);
    return itemIcon;
}

void medParameterPoolManagerToolBox::addToPool()
{
    QPushButton *okButton = (QPushButton *)this->sender();
    medAbstractParameter *param = d->buttonHash.key(okButton);
    QComboBox *combo = d->comboHash.value(param);
    QListWidgetItem *item = d->itemHash.value(param);

    QVariant poolNumber = combo->itemData(combo->currentIndex());

    emit poolSelected(param, poolNumber.toInt());

    d->parameters->model()->removeRow( d->parameters->row(item) );
    d->currentParam = "";
}

void medParameterPoolManagerToolBox::addPool(medParameterPool *selectedPool,int id)
{
    QWidget *poolWidget = new QWidget(d->pools);

    QHBoxLayout* layout = new QHBoxLayout(poolWidget);
    layout->setContentsMargins(0,0,10,0);

    QPixmap iconPixmap(14,14);
    iconPixmap.fill(selectedPool->color());

    QLabel *colorLabel = new QLabel;
    colorLabel->setPixmap(iconPixmap);

    QLabel *poolLabel = new QLabel("Link " + selectedPool->name() + QString::number(id), poolWidget);

    QPushButton *removeButton = new QPushButton;
    removeButton->setIcon(QIcon(":/icons/cross.svg"));

    layout->addWidget(colorLabel);
    layout->addWidget(poolLabel);
    layout->addStretch();
    layout->addWidget(removeButton);

    QListWidgetItem *item = new QListWidgetItem(d->pools);
    d->pools->addItem(item);
    d->pools->setItemWidget(item, poolWidget);

    d->poolHash.insert(id, item);
    d->poolIdButtonHash.insert(id, removeButton);

    connect(removeButton, SIGNAL(clicked()), this, SLOT(removePool()));
}

void medParameterPoolManagerToolBox::removePool()
{
    QPushButton *removeButton = (QPushButton *)this->sender();
    int poolId = d->poolIdButtonHash.key(removeButton);

    emit poolDeletionRequested(poolId);

}

void medParameterPoolManagerToolBox::removePoolFromWidget(int poolId)
{
     QListWidgetItem *item = d->poolHash.value(poolId);
     d->pools->model()->removeRow( d->pools->row(item) );
}

void medParameterPoolManagerToolBox::showLinkAllWidget()
{
    d->linkAllWidget->show();
}

void medParameterPoolManagerToolBox::hideLinkAllWidget()
{
    d->linkAllWidget->hide();
}

void medParameterPoolManagerToolBox::linkAll()
{
    emit linkAllRequested();

    hideLinkAllWidget();
}

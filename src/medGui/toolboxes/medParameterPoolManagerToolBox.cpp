#include "medParameterPoolManagerToolBox.h"

#include <medParameterPoolManager.h>
#include <medAbstractParameter.h>
#include <medParameterPool.h>
#include <medViewManager.h>
#include <medAbstractView.h>

class medParameterPoolManagerToolBoxPrivate
{
public:
    medParameterPoolManager *mngr;

    QListWidget *pools;
    QListWidget *parameters;

    QComboBox *poolsSelector;
    QPushButton *okButton;
    QWidget *poolsSelectorWidget;

    QHash<medAbstractParameter*, QListWidgetItem*> itemHash;

    QHash<int, QListWidgetItem*> poolItemHash;
    QHash<int, medParameterPool*> poolHash;
    QHash<int, QPushButton*> poolIdButtonHash;

};


medParameterPoolManagerToolBox::medParameterPoolManagerToolBox() : d(new medParameterPoolManagerToolBoxPrivate)
{
    this->setTitle("Parameters Pool Managment");
    d->mngr = medParameterPoolManager::instance();

    d->pools = new QListWidget(this);

    d->parameters = new QListWidget(this);
    d->parameters->setSelectionMode(QAbstractItemView::NoSelection);
    connect(d->parameters, SIGNAL(itemChanged(QListWidgetItem*)), this, SLOT(buildPool(QListWidgetItem*)));

    d->poolsSelector = new QComboBox;
    d->okButton = new QPushButton(tr("OK"));
    d->poolsSelectorWidget = new QWidget;
    QHBoxLayout *poolsSelectorLayout = new QHBoxLayout(d->poolsSelectorWidget);
    poolsSelectorLayout->addWidget(new QLabel(tr("Add to pool: ")));
    poolsSelectorLayout->addWidget(d->poolsSelector);
    poolsSelectorLayout->addWidget(d->okButton);

    this->addWidget(new QLabel("Pools:"));
    this->addWidget(d->pools);
    this->addWidget(new QLabel("Parameters linked:"));
    this->addWidget(d->parameters);
    this->addWidget(d->poolsSelectorWidget);

    connect(d->pools, SIGNAL(itemSelectionChanged()), this, SLOT(displayPoolParameters()));

    connect(d->okButton, SIGNAL(clicked()), this, SLOT(addViewParamsToPool()));

    d->parameters->hide();
    d->poolsSelectorWidget->hide();
}


QIcon medParameterPoolManagerToolBox::createIcon(QString &colorName) const
{
    QPixmap iconPixmap(32,32);
    iconPixmap.fill(QColor(colorName));
    QIcon itemIcon(iconPixmap);
    return itemIcon;
}

QIcon medParameterPoolManagerToolBox::createIcon(QColor color) const
{
    QPixmap iconPixmap(32,32);
    iconPixmap.fill(color);
    QIcon itemIcon(iconPixmap);
    return itemIcon;
}


void medParameterPoolManagerToolBox::addPool(medParameterPool *newPool, int id)
{
    QWidget *poolWidget = new QWidget(d->pools);

    QHBoxLayout* layout = new QHBoxLayout(poolWidget);
    layout->setContentsMargins(0,0,10,0);

    QPixmap iconPixmap(14,14);
    iconPixmap.fill(newPool->color());

    QLabel *colorLabel = new QLabel;
    colorLabel->setPixmap(iconPixmap);

    QLabel *poolLabel = new QLabel(newPool->name(), poolWidget);

    QPushButton *removeButton = new QPushButton;
    removeButton->setIcon(QIcon(":/icons/cross.svg"));

    layout->addWidget(colorLabel);
    layout->addWidget(poolLabel);
    layout->addStretch();
    layout->addWidget(removeButton);

    QListWidgetItem *item = new QListWidgetItem(d->pools);
    d->pools->addItem(item);
    d->pools->setItemWidget(item, poolWidget);

    d->poolHash.insert(id, newPool);
    d->poolItemHash.insert(id, item);
    d->poolIdButtonHash.insert(id, removeButton);

    connect(removeButton, SIGNAL(clicked()), this, SLOT(removePool()));

    d->poolsSelector->addItem(createIcon(newPool->color()), newPool->name(), id);
}

void medParameterPoolManagerToolBox::removePool()
{
    QPushButton *removeButton = (QPushButton *)this->sender();
    int poolId = d->poolIdButtonHash.key(removeButton);

    emit poolDeletionRequested(poolId);

}

void medParameterPoolManagerToolBox::removePoolFromWidget(int poolId)
{
     QListWidgetItem *item = d->poolItemHash.value(poolId);
     d->pools->model()->removeRow( d->pools->row(item) );

     d->poolHash.remove(poolId);
     d->poolItemHash.remove(poolId);
}

void medParameterPoolManagerToolBox::showLinkParametersWidget()
{
    QList<medAbstractView*> selectedViews = medViewManager::instance()->selectedViews();

    QList<medAbstractParameter*> commonParams = commonParameters(selectedViews);

    d->parameters->clear();

    foreach(medAbstractParameter* commonParam, commonParams)
    {
        d->parameters->blockSignals(true);
        QListWidgetItem *paramItem = new QListWidgetItem();
        paramItem->setText(commonParam->name());
        paramItem->setCheckState(Qt::Unchecked);
        d->parameters->addItem(paramItem);
        d->parameters->blockSignals(false);

        d->itemHash.insert(commonParam, paramItem);
    }

    d->parameters->show();

    d->poolsSelectorWidget->hide();
}


void medParameterPoolManagerToolBox::showPoolsSelectorWidget()
{
    d->parameters->hide();

    d->poolsSelectorWidget->show();
}

void medParameterPoolManagerToolBox::linkAll()
{
    emit linkAllRequested();
}

void medParameterPoolManagerToolBox::buildPool(QListWidgetItem* item)
{
    medAbstractParameter *param = d->itemHash.key(item);

    if(item->checkState()==Qt::Checked)
    {
      emit linkParamRequested(param, -1);
    }
    else if(item->checkState()==Qt::Unchecked)
    {
        QListWidgetItem *currentItem = d->pools->currentItem();
        if(currentItem)
        {
            int poolId = d->poolItemHash.key(currentItem);
            emit unlinkParamRequested(param, poolId);
        }
    }
}

void medParameterPoolManagerToolBox::addViewParamsToPool()
{
     QList<medAbstractView*> selectedViews = medViewManager::instance()->selectedViews();
     int poolId = d->poolsSelector->itemData(d->poolsSelector->currentIndex()).toInt();

     foreach(medAbstractView *view, selectedViews)
     {
         emit addViewParamsToPoolRequested(view, poolId);
     }
}

void medParameterPoolManagerToolBox::displayPoolParameters()
{
    QListWidgetItem *currentItem = d->pools->currentItem();

    QList<medAbstractView*> views;
    if(currentItem)
    {
        int poolId = d->poolItemHash.key(currentItem);
        medParameterPool *pool = d->poolHash.value(poolId);

        if(pool)
        {
            foreach(medAbstractParameter *param, pool->parameters())
            {
                if(medAbstractView* view = dynamic_cast<medAbstractView*>(param->parent()))
                    views.append(view);
            }

            QList<medAbstractParameter*> commonParams = commonParameters(views);

            d->parameters->clear();

            foreach(medAbstractParameter* commonParam, commonParams)
            {
                d->parameters->blockSignals(true);
                QListWidgetItem *paramItem = new QListWidgetItem();
                paramItem->setText(commonParam->name());

                //TODO GPR: find a better way to handle groups
                if(medAbstractGroupParameter *group = dynamic_cast<medAbstractGroupParameter *>(commonParam))
                {
                    bool checked = true;
                    foreach(medAbstractParameter *paramInGroup, group->parametersCandidateToPool())
                    {
                        if( !pool->parametersNames().contains(paramInGroup->name()))
                            checked = false;
                    }
                    if(checked)
                        paramItem->setCheckState(Qt::Checked);
                    else paramItem->setCheckState(Qt::Unchecked);
                }
                else
                {
                    if(pool->parametersNames().contains(commonParam->name()))
                        paramItem->setCheckState(Qt::Checked);
                    else paramItem->setCheckState(Qt::Unchecked);
                }

                d->parameters->addItem(paramItem);
                d->parameters->blockSignals(false);

                d->itemHash.insert(commonParam, paramItem);
            }

        }
    }

    d->parameters->show();

}

QList<medAbstractParameter*> medParameterPoolManagerToolBox::commonParameters(QList<medAbstractView*> views)
{
    QList<medAbstractParameter*> commonParams;

    if(views.count() > 1 )
    {
        // intialize commonParams with parameters from the first view
        commonParams = views.at(0)->navigatorsParameters();
    }

    foreach(medAbstractView *view, views)
    {
        QList<medAbstractParameter*> params = view->navigatorsParameters();
        bool common = false;

        foreach(medAbstractParameter* commonParam, commonParams)
        {
            foreach(medAbstractParameter* param, params)
            {
                if(commonParam->match(param))
                {
                    common = true;
                    break;
                }
            }

            if(!common)
            {
                commonParams.removeAll(commonParam);
            }
        }
    }
    return commonParams;
}

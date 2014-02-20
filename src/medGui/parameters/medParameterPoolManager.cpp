#include "medParameterPoolManager.h"

#include <medAbstractView.h>
#include <medAbstractParameter.h>
#include <medParameterPool.h>
#include <medViewManager.h>
#include <medParameterPoolManagerToolBox.h>

class medParameterPoolManagerPrivate
{
public:
    medParameterPool *currentPool;
    medParameterPoolManagerToolBox* tb;

    QHash<int, medParameterPool *> pools;

    QMultiMap<QString, medParameterPool*> poolsType;

    QMultiHash<medParameterPool*, medAbstractView *> viewsHash;
};

medParameterPoolManager *medParameterPoolManager::instance(void)
{
    if(!s_instance)
        s_instance = new medParameterPoolManager;

    return s_instance;
}

medParameterPoolManager::medParameterPoolManager(void) : d(new medParameterPoolManagerPrivate)
{
    d->currentPool = new medParameterPool();
    d->tb = NULL;

    connect(medViewManager::instance(), SIGNAL(viewsSelectionChanged()), this, SLOT(buildViewPool()));
}


void medParameterPoolManager::buildViewPool()
{
    d->currentPool->clear();

    if(medViewManager::instance()->selectedViews().count() > 1)
    {
        //TODO GPR: move to medViewManager?
        emit multipleViewsSelected();
    }
    else emit singleViewSelected();

    foreach(medAbstractView* v, medViewManager::instance()->selectedViews())
    {
        foreach(medAbstractParameter* param, v->navigatorsParameters())
        {
            d->currentPool->append(param);
        }
    }
}

void medParameterPoolManager::setToolBox(medParameterPoolManagerToolBox* tb)
{
    d->tb = tb;

    connect(tb, SIGNAL(poolDeletionRequested(int)), this, SLOT(removePool(int)));
    connect(tb, SIGNAL(linkAllRequested()), this, SLOT(linkAll()));
    connect(tb, SIGNAL(linkParamRequested(medAbstractParameter*, int)), this, SLOT(linkParameter(medAbstractParameter*, int)));
    connect(tb, SIGNAL(unlinkParamRequested(medAbstractParameter*, int)), this, SLOT(unlinkParameter(medAbstractParameter*, int)));
    connect(tb, SIGNAL(addViewParamsToPoolRequested(medAbstractView*,int)), this, SLOT(addViewParamsToPool(medAbstractView*, int)));
}


void medParameterPoolManager::addColorLabeltoView(medAbstractParameter* parameter, medParameterPool *selectedPool)
{
    if(medAbstractView* view = dynamic_cast<medAbstractView*>(parameter->parent()))
    {
        if( !d->viewsHash.contains(selectedPool, view) )
        {
          d->viewsHash.insert(selectedPool, view);

          QPixmap iconPixmap(14,14);
          iconPixmap.fill(selectedPool->color());

          QLabel *colorLabel = new QLabel;
          colorLabel->setPixmap(iconPixmap);

          view->toolBarWidget()->layout()->addWidget(colorLabel);
        }
    }
}

QList<int> medParameterPoolManager::poolsId(QString type)
{
    QList<int> result;

    QList<medParameterPool *> pools = d->poolsType.values(type);

    foreach(medParameterPool* p, pools)
    {
        result.append(d->pools.key(p));
    }

    return result;
}

void medParameterPoolManager::removePool(int poolId)
{
    medParameterPool *poolToRemove = d->pools.value(poolId);
    if(poolToRemove)
    {
        poolToRemove->clear();
        d->pools.remove(poolId);
        emit poolRemoved(poolId);
        delete poolToRemove;
    }
}

void medParameterPoolManager::linkAll()
{
    medParameterPool *pool = d->currentPool;
    pool->setName("Views ");
    pool->setColor(QColor::fromHsv(qrand() % 256, qrand() % 255, qrand() % 190));

    d->pools.insert(d->pools.count()+1, pool);
    d->currentPool = new medParameterPool(this);

    emit poolCreated(pool, d->pools.count());

    foreach(medAbstractParameter *param, pool->parameters())
    {
        addColorLabeltoView(param, pool);
    }
}

void medParameterPoolManager::linkParameter(medAbstractParameter* parameter, int poolNumber)
{
    medParameterPool *selectedPool;

    if(poolNumber == -1)
    {
        selectedPool = new medParameterPool(this);
        selectedPool->setColor(QColor::fromHsv(qrand() % 256, qrand() % 255, qrand() % 190));
        selectedPool->setName("Pool" + QString::number(d->pools.count()+1));
        d->pools.insert(d->pools.count()+1, selectedPool);
        emit poolCreated(selectedPool, d->pools.count());
    }
    else selectedPool = d->pools.value(poolNumber);

    if(medAbstractGroupParameter *group = dynamic_cast<medAbstractGroupParameter *>(parameter))
    {
        foreach(medAbstractParameter *paramInGroup, group->parametersCandidateToPool())
        {
            foreach(medAbstractParameter *paramInPool, d->currentPool->parameters(paramInGroup->name()))
            {
                selectedPool->append(paramInPool);
                addColorLabeltoView(paramInPool, selectedPool);
            }
        }
    }
    else
    {
        foreach(medAbstractParameter *param, d->currentPool->parameters(parameter->name()))
        {
            selectedPool->append(param);
            addColorLabeltoView(param, selectedPool);
        }
    }

}

void medParameterPoolManager::unlinkParameter(medAbstractParameter* param, int poolId)
{
    medParameterPool *selectedPool = d->pools.value(poolId);

    if(medAbstractGroupParameter *group = dynamic_cast<medAbstractGroupParameter *>(param))
    {
        foreach(medAbstractParameter *paramInGroup, group->parametersCandidateToPool())
        {
            selectedPool->removeAll(paramInGroup->name());
        }
    }
    else selectedPool->removeAll(param->name());

    if(selectedPool->count() == 0)
        removePool(poolId);

}

void medParameterPoolManager::addViewParamsToPool(medAbstractView* view, int poolId)
{
    medParameterPool *selectedPool = d->pools.value(poolId);

    QList<medAbstractParameter*> viewParams = view->navigatorsParameters();

    foreach(medAbstractParameter* viewParam, viewParams)
    {
        if(medAbstractGroupParameter *group = dynamic_cast<medAbstractGroupParameter *>(viewParam))
        {
            foreach(medAbstractParameter *paramInGroup, group->parametersCandidateToPool())
            {
                if(selectedPool->parametersNames().contains(paramInGroup->name()))
                {
                    selectedPool->append(paramInGroup);
                    addColorLabeltoView(viewParam, selectedPool);
                }
            }
        }
        else if(selectedPool->parametersNames().contains(viewParam->name()))
        {
            selectedPool->append(viewParam);
            addColorLabeltoView(viewParam, selectedPool);
        }
    }
}

medParameterPoolManager *medParameterPoolManager::s_instance = NULL;

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
            connect(param, SIGNAL(triggered()), this, SLOT(updateToolBox()));
        }
    }

    qDebug() << "Params in pool: " << d->currentPool->count();
}

void medParameterPoolManager::setToolBox(medParameterPoolManagerToolBox* tb)
{
    d->tb = tb;

    connect(tb, SIGNAL(poolSelected(medAbstractParameter*,int)), this, SLOT(addToPool(medAbstractParameter*,int)));
    connect(tb, SIGNAL(poolDeletionRequested(int)), this, SLOT(removePool(int)));
    connect(tb, SIGNAL(linkAllRequested()), this, SLOT(linkAll()));
}

void medParameterPoolManager::updateToolBox()
{
    medAbstractParameter *param = dynamic_cast<medAbstractParameter *>(this->sender());

    d->tb->addParameter(param/*->name()*/);

}

void medParameterPoolManager::addToPool(medAbstractParameter* parameter, int poolNumber)
{
    medParameterPool *selectedPool;

    if(poolNumber == -1)
    {
        selectedPool = new medParameterPool(this);
        selectedPool->setColor(QColor("red"));
        selectedPool->setName(parameter->name());
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

    d->poolsType.insert(parameter->name(), selectedPool);

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

          view->toolBar()->layout()->addWidget(colorLabel);
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
    pool->setColor("blue");

    d->pools.insert(d->pools.count()+1, pool);
    d->currentPool = new medParameterPool(this);

    emit poolCreated(pool, d->pools.count());

    foreach(medAbstractParameter *param, pool->parameters())
    {
        addColorLabeltoView(param, pool);
    }
}

medParameterPoolManager *medParameterPoolManager::s_instance = NULL;

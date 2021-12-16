#include "medSourceModelPresenter.h"

#include <medDataModel.h>
#include <medSourcesWidget.h>

#include <QWidget>


class medSourceModelPresenterPrivate
{
public:
    medDataModel *model;
};


medSourceModelPresenter::medSourceModelPresenter(medDataModel *parent) : d(new medSourceModelPresenterPrivate())
{
    d->model = parent;
}

medSourceModelPresenter::~medSourceModelPresenter()
{
}

QWidget * medSourceModelPresenter::buildWidget()
{
    return buildTree();
}

medSourcesWidget * medSourceModelPresenter::buildTree()
{
    auto widgetRes = new medSourcesWidget;
    
    widgetRes->addSources(d->model);

    connect(d->model, &medDataModel::sourceAdded, [&](medAbstractSource *sourceInstance) {widgetRes->addSource(d->model, sourceInstance->getInstanceId());});
    connect(d->model, &medDataModel::sourceRemoved, widgetRes, &medSourcesWidget::removeSource);

    return widgetRes;
}

medDataModel * medSourceModelPresenter::dataModel() const
{
    return d->model;
}


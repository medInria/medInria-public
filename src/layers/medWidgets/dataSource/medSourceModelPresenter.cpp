#include "medSourceModelPresenter.h"

#include <medDataModel.h>
#include <medSourcesWidget.h>
#include <medSourceItemModelPresenter.h>

#include <QWidget>
#include <medStringParameterPresenter.h>


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

    connect(this, SIGNAL(filterProxy(const QString &)), widgetRes, SLOT(filter(const QString &)));
    return widgetRes;
}

QStackedWidget *medSourceModelPresenter::buildBrowser()
{
    auto browserRes = new QStackedWidget;
    for (auto sourceModel : d->model->models())
    {
        medSourceItemModelPresenter itemModelPresenter(sourceModel);
        browserRes->addWidget(itemModelPresenter.buildTree());
    }
    return browserRes;
}

QListWidget *medSourceModelPresenter::buildSourceList()
{
    QListWidget *listSourcesRes = new QListWidget;
    for (auto sourceModel : d->model->models())
    {
        QString instanceName = d->model->getInstanceName(sourceModel->getSourceIntanceId());
        listSourcesRes->addItem(instanceName);
    }

    return listSourcesRes;
}

QStackedWidget *medSourceModelPresenter::buildFilters()
{
    QStackedWidget *filterRes = new QStackedWidget;
    for (auto sourceModel : d->model->models())
    {
        auto filterParams = d->model->filteringParameters(sourceModel->getSourceIntanceId());
        // TODO To be continued
    }
    return filterRes;
}

medDataModel * medSourceModelPresenter::dataModel() const
{
    return d->model;
}






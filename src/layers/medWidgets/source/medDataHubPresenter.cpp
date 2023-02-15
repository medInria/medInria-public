#include "medDataHubPresenter.h"

#include <medDataHub.h>
#include <medSourcesWidget.h>
#include <medSourceModelPresenter.h>

#include <medStringParameterPresenter.h>
#include <QGroupBox>
#include <QLabel>

class medDataHubPresenterPrivate
{
public:
    medDataHub *dataHub;
};


medDataHubPresenter::medDataHubPresenter(medDataHub *parent) : d(new medDataHubPresenterPrivate())
{
    d->dataHub = parent;
}

medDataHubPresenter::~medDataHubPresenter()
{
}

QWidget * medDataHubPresenter::buildWidget()
{
    return buildTree();
}

medSourcesWidget * medDataHubPresenter::buildTree()
{
    auto widgetRes = new medSourcesWidget;
    
    widgetRes->addSources(d->dataHub);

    connect(d->dataHub, &medDataHub::sourceAdded, [=](QString sourceInstanceId) {widgetRes->addSource(d->dataHub, sourceInstanceId);});
    connect(d->dataHub, &medDataHub::sourceRemoved, widgetRes, &medSourcesWidget::removeSource);

    connect(this, SIGNAL(filterProxy(const QString &)), widgetRes, SLOT(filter(const QString &)));
    return widgetRes;
}

QStackedWidget *medDataHubPresenter::buildBrowser()
{
    auto browserRes = new QStackedWidget;
    for (auto sourceModel : d->dataHub->models())
    {
        medSourceModelPresenter itemModelPresenter(sourceModel);
        browserRes->addWidget(itemModelPresenter.buildTree());
    }
    return browserRes;
}

QListWidget *medDataHubPresenter::buildSourceList()
{
    QListWidget *listSourcesRes = new QListWidget;
    for (auto sourceModel : d->dataHub->models())
    {
        QString instanceName = medSourceHandler::instance()->getInstanceName(sourceModel->getSourceIntanceId());
        listSourcesRes->addItem(instanceName);
    }

    return listSourcesRes;
}

QStackedWidget *medDataHubPresenter::buildFilters()
{
    QStackedWidget *filterRes = new QStackedWidget;
    for (auto sourceModel : d->dataHub->models())
    {
        auto * pVLayout = new QVBoxLayout;
        QList<medAbstractParameter*> filterParams;
        medSourceHandler::instance()->filteringParameters(sourceModel->getSourceIntanceId(), filterParams);
        for (auto * param : filterParams)
        {
            auto * pHLayout = new QHBoxLayout;
            auto * pParamPresenter = medAbstractParameterPresenter::buildFromParameter(param);            
            auto * pWidget = pParamPresenter->buildWidget();
            if (dynamic_cast<QPushButton*>(pWidget) == nullptr && dynamic_cast<QGroupBox*>(pWidget) == nullptr)
            {
                auto * pLabel = pParamPresenter->buildLabel();
                pHLayout->addWidget(pLabel);
            }
            pHLayout->addWidget(pWidget);

            pVLayout->addLayout(pHLayout);
        }

        if (!filterParams.isEmpty())
        {
            QFrame *line;
            line = new QFrame();
            line->setFrameShape(QFrame::HLine);
            line->setFrameShadow(QFrame::Sunken);
            pVLayout->addWidget(line);

            QPushButton *pApplyButton = new QPushButton("Apply");
            pVLayout->addWidget(pApplyButton);
            connect(pApplyButton, &QPushButton::clicked, [=]()
                    {  sourceModel->refresh(); });
        }
        else
        {
            QLabel *noFilterLabel = new QLabel("The current selected source has no filters");
            pVLayout->addWidget(noFilterLabel);
        }

        auto *pParamListWidget = new QWidget();
        pParamListWidget->setLayout(pVLayout);
        filterRes->addWidget(pParamListWidget);
    }
    return filterRes;
}

//medSourceHandler * medDataHubPresenter::sourcesHandler() const
//{
//    return d->sourcesHandler;
//}






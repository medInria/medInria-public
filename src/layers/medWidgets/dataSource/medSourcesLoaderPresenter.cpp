#include "medSourcesLoaderPresenter.h"


#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QPushButton>

#include <medAbstractParameterPresenter.h>

class medSourcesLoaderPresenterPrivate
{
public:
    medSourcesLoader * pSourceLoader;
};

medSourcesLoaderPresenter::medSourcesLoaderPresenter(medSourcesLoader * parent):d(new medSourcesLoaderPresenterPrivate())
{
    d->pSourceLoader = parent;
}

medSourcesLoaderPresenter::~medSourcesLoaderPresenter()
{
    delete d;
}

QWidget * medSourcesLoaderPresenter::buildWidget()
{
    QWidget * pWidgetRes = new QWidget;

    auto * pMainLayout = new QHBoxLayout();
    auto * pSourceLayout = new QVBoxLayout();
    auto * pControlLayout = new QVBoxLayout();

    for (auto pSource : d->pSourceLoader->sourcesList())
    {
        auto * pSourceParametersBox = new QGroupBox(pSource->getInstanceName());
        auto * pVLayout = new QVBoxLayout;
        auto params = pSource->getAllParameters();
        for (auto * param : params)
        {
            auto * pHLayout = new QHBoxLayout;
            auto * pParamPresenter = medAbstractParameterPresenter::buildFromParameter(param);
            auto * pWidget = pParamPresenter->buildWidget();
            if (dynamic_cast<QPushButton*>(pWidget) == nullptr)
            {
                auto * pLabel = pParamPresenter->buildLabel();
                pHLayout->addWidget(pLabel);
            }
            pHLayout->addWidget(pWidget);
            pVLayout->addLayout(pHLayout);
        }
        pSourceParametersBox->setLayout(pVLayout);
        pSourceLayout->addWidget(pSourceParametersBox);
    }

    pMainLayout->addLayout(pSourceLayout);
    pMainLayout->addLayout(pControlLayout);
    pWidgetRes->setLayout(pMainLayout);

    return pWidgetRes;
}

#include <itkTensorEstimationProcessPresenter.h>

#include <medBoolParameterPresenter.h>
#include <QVBoxLayout>
#include <QPushButton>
#include <QWidget>
#include <QLabel>
#include <QGroupBox>

itkTensorEstimationProcessPresenter::itkTensorEstimationProcessPresenter(medAbstractDiffusionModelEstimationProcess *parent)
    : medAbstractDiffusionModelEstimationProcessPresenter(parent)
{
    m_process = qobject_cast <itkTensorEstimationProcess *> (parent);

    m_noSmoothingPresenter = new medBoolParameterPresenter(m_process->noSmoothing());
    m_lowSmoothingPresenter = new medBoolParameterPresenter(m_process->lowSmoothing());
    m_mediumSmoothingPresenter = new medBoolParameterPresenter(m_process->mediumSmoothing());
    m_highSmoothingPresenter = new medBoolParameterPresenter(m_process->highSmoothing());
}

medAbstractDiffusionModelEstimationProcess* itkTensorEstimationProcessPresenter::process() const
{
    return m_process;
}

QWidget *itkTensorEstimationProcessPresenter::buildToolBoxWidget()
{
    QWidget *tbWidget = medAbstractDiffusionModelEstimationProcessPresenter::buildToolBoxWidget();
    QVBoxLayout *tbLayout = qobject_cast <QVBoxLayout *> (tbWidget->layout());

    QGroupBox *smoothingBox = new QGroupBox;
    smoothingBox->setTitle("DTI smoothing");

    QHBoxLayout *smoothingBoxLayout = new QHBoxLayout;
    smoothingBoxLayout->addWidget(m_noSmoothingPresenter->buildWidget());
    smoothingBoxLayout->addWidget(m_lowSmoothingPresenter->buildWidget());
    smoothingBoxLayout->addWidget(m_mediumSmoothingPresenter->buildWidget());
    smoothingBoxLayout->addWidget(m_highSmoothingPresenter->buildWidget());

    smoothingBox->setLayout(smoothingBoxLayout);

    if (this->useRunControls())
        tbLayout->insertWidget(tbLayout->count() - 2, smoothingBox);
    else
        tbLayout->addWidget(smoothingBox);

    return tbWidget;
}

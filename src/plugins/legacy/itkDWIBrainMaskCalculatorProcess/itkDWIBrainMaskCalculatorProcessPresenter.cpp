#include <itkDWIBrainMaskCalculatorProcessPresenter.h>

#include <medIntParameterPresenter.h>
#include <QVBoxLayout>
#include <QPushButton>
#include <QWidget>
#include <QLabel>

itkDWIBrainMaskCalculatorProcessPresenter::itkDWIBrainMaskCalculatorProcessPresenter(medAbstractDWIMaskingProcess *parent)
    : medAbstractDWIMaskingProcessPresenter(parent)
{
    m_process = qobject_cast <itkDWIBrainMaskCalculatorProcess *> (parent);
    m_offsetPresenter = new medIntParameterPresenter(m_process->offset());
}

medAbstractDWIMaskingProcess* itkDWIBrainMaskCalculatorProcessPresenter::process() const
{
    return m_process;
}

QWidget *itkDWIBrainMaskCalculatorProcessPresenter::buildToolBoxWidget()
{
    QWidget *tbWidget = new QWidget;
    QVBoxLayout *tbLayout = new QVBoxLayout;
    tbWidget->setLayout(tbLayout);

    QLabel *offsetLabel = new QLabel(m_offsetPresenter->parameter()->caption(), tbWidget);
    QHBoxLayout *offsetLayout = new QHBoxLayout;
    offsetLayout->addWidget(offsetLabel);
    offsetLayout->addWidget(m_offsetPresenter->buildWidget());
    tbLayout->addLayout(offsetLayout);

    if (this->useRunControls())
        tbLayout->addWidget(this->buildRunButton());

    return tbWidget;
}

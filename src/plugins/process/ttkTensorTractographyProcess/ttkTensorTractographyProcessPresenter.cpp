#include <ttkTensorTractographyProcessPresenter.h>

#include <medIntParameterPresenter.h>
#include <QVBoxLayout>
#include <QPushButton>
#include <QWidget>
#include <QLabel>
#include <QProgressBar>
#include <QGroupBox>
#include <QSlider>

ttkTensorTractographyProcessPresenter::ttkTensorTractographyProcessPresenter(medAbstractTractographyProcess *parent)
    : medAbstractTractographyProcessPresenter(parent)
{
    m_process = qobject_cast <ttkTensorTractographyProcess *> (parent);

    m_faThreshold = new medIntParameterPresenter(m_process->faThreshold());
    m_faThreshold2 = new medIntParameterPresenter(m_process->faThreshold2());
    m_smoothness = new medIntParameterPresenter(m_process->smoothness());
    m_minLength = new medIntParameterPresenter(m_process->minLength());
    m_sampling = new medIntParameterPresenter(m_process->sampling());

    m_progressionPresenter = new medIntParameterPresenter(m_process->progression());
}

medAbstractTractographyProcess* ttkTensorTractographyProcessPresenter::process() const
{
    return m_process;
}

QWidget *ttkTensorTractographyProcessPresenter::buildToolBoxWidget()
{
    QWidget *tbWidget = new QWidget;
    QVBoxLayout *tbGlobalLayout = new QVBoxLayout;
    tbWidget->setLayout(tbGlobalLayout);

    QGroupBox *tbInternalWidget = new QGroupBox(tbWidget);
    tbInternalWidget->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
    QVBoxLayout *tbLayout = new QVBoxLayout;
    tbInternalWidget->setLayout(tbLayout);
    tbGlobalLayout->addWidget(tbInternalWidget);

    QLabel *faThrLabel = new QLabel(m_faThreshold->parameter()->caption(), tbInternalWidget);
    QHBoxLayout *faThrLayout = new QHBoxLayout;
    faThrLayout->addWidget(faThrLabel);
    QSlider *faSlider = new QSlider (Qt::Horizontal, tbInternalWidget);
    faSlider->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    faSlider->setFocusPolicy(Qt::NoFocus);
    faSlider->setRange (m_faThreshold->parameter()->minimum(),m_faThreshold->parameter()->maximum());
    faSlider->setValue (m_faThreshold->parameter()->value());
    faSlider->setToolTip(m_faThreshold->parameter()->description());
    faThrLayout->addWidget(faSlider);
    QWidget *faThrSpinBox = m_faThreshold->buildWidget();
    faThrSpinBox->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    faThrLayout->addWidget(faThrSpinBox);
    connect(faThrSpinBox,SIGNAL(valueChanged(int)),faSlider,SLOT(setValue(int)));
    connect(faSlider,SIGNAL(valueChanged(int)),faThrSpinBox,SLOT(setValue(int)));
    tbLayout->addLayout(faThrLayout);

    QLabel *faThr2Label = new QLabel(m_faThreshold2->parameter()->caption(), tbInternalWidget);
    QHBoxLayout *faThr2Layout = new QHBoxLayout;
    faThr2Layout->addWidget(faThr2Label);
    QSlider *fa2Slider = new QSlider (Qt::Horizontal, tbInternalWidget);
    fa2Slider->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    fa2Slider->setFocusPolicy(Qt::NoFocus);
    fa2Slider->setRange (m_faThreshold2->parameter()->minimum(),m_faThreshold2->parameter()->maximum());
    fa2Slider->setValue (m_faThreshold2->parameter()->value());
    fa2Slider->setToolTip(m_faThreshold2->parameter()->description());
    faThr2Layout->addWidget(fa2Slider);
    QWidget *faThr2SpinBox = m_faThreshold2->buildWidget();
    faThr2SpinBox->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    faThr2Layout->addWidget(faThr2SpinBox);
    connect(faThr2SpinBox,SIGNAL(valueChanged(int)),fa2Slider,SLOT(setValue(int)));
    connect(fa2Slider,SIGNAL(valueChanged(int)),faThr2SpinBox,SLOT(setValue(int)));
    tbLayout->addLayout(faThr2Layout);

    QLabel *smoothnessLabel = new QLabel(m_smoothness->parameter()->caption(), tbInternalWidget);
    QHBoxLayout *smoothnessLayout = new QHBoxLayout;
    smoothnessLayout->addWidget(smoothnessLabel);
    QSlider *smoothnessSlider = new QSlider (Qt::Horizontal, tbInternalWidget);
    smoothnessSlider->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    smoothnessSlider->setFocusPolicy(Qt::NoFocus);
    smoothnessSlider->setRange (m_smoothness->parameter()->minimum(),m_smoothness->parameter()->maximum());
    smoothnessSlider->setValue (m_smoothness->parameter()->value());
    smoothnessSlider->setToolTip(m_smoothness->parameter()->description());
    smoothnessLayout->addWidget(smoothnessSlider);
    QWidget *smoothnessSpinBox = m_smoothness->buildWidget();
    smoothnessSpinBox->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    smoothnessLayout->addWidget(smoothnessSpinBox);
    connect(smoothnessSpinBox,SIGNAL(valueChanged(int)),smoothnessSlider,SLOT(setValue(int)));
    connect(smoothnessSlider,SIGNAL(valueChanged(int)),smoothnessSpinBox,SLOT(setValue(int)));
    tbLayout->addLayout(smoothnessLayout);

    QLabel *minLengthLabel = new QLabel(m_minLength->parameter()->caption(), tbInternalWidget);
    QHBoxLayout *minLengthLayout = new QHBoxLayout;
    minLengthLayout->addWidget(minLengthLabel);
    QSlider *minLengthSlider = new QSlider (Qt::Horizontal, tbInternalWidget);
    minLengthSlider->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    minLengthSlider->setFocusPolicy(Qt::NoFocus);
    minLengthSlider->setRange (m_minLength->parameter()->minimum(),m_minLength->parameter()->maximum());
    minLengthSlider->setValue (m_minLength->parameter()->value());
    minLengthSlider->setToolTip(m_minLength->parameter()->description());
    minLengthLayout->addWidget(minLengthSlider);
    QWidget *minLengthSpinBox = m_minLength->buildWidget();
    minLengthSpinBox->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    minLengthLayout->addWidget(minLengthSpinBox);
    connect(minLengthSpinBox,SIGNAL(valueChanged(int)),minLengthSlider,SLOT(setValue(int)));
    connect(minLengthSlider,SIGNAL(valueChanged(int)),minLengthSpinBox,SLOT(setValue(int)));
    tbLayout->addLayout(minLengthLayout);

    QLabel *samplingLabel = new QLabel(m_sampling->parameter()->caption(), tbInternalWidget);
    QHBoxLayout *samplingLayout = new QHBoxLayout;
    samplingLayout->addWidget(samplingLabel);
    QSlider *samplingSlider = new QSlider (Qt::Horizontal, tbInternalWidget);
    samplingSlider->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    samplingSlider->setFocusPolicy(Qt::NoFocus);
    samplingSlider->setRange (m_sampling->parameter()->minimum(),m_sampling->parameter()->maximum());
    samplingSlider->setValue (m_sampling->parameter()->value());
    samplingSlider->setToolTip(m_sampling->parameter()->description());
    samplingLayout->addWidget(samplingSlider);
    QWidget *samplingSpinBox = m_sampling->buildWidget();
    samplingSpinBox->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    samplingLayout->addWidget(samplingSpinBox);
    connect(samplingSpinBox,SIGNAL(valueChanged(int)),samplingSlider,SLOT(setValue(int)));
    connect(samplingSlider,SIGNAL(valueChanged(int)),samplingSpinBox,SLOT(setValue(int)));
    tbLayout->addLayout(samplingLayout);

    tbGlobalLayout->addWidget(this->buildRunButton());
    tbGlobalLayout->addWidget(m_progressionPresenter->buildProgressBar());
    tbGlobalLayout->addWidget(this->buildCancelButton());

    return tbWidget;
}

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSlider>
#include <QPushButton>
#include <QLabel>
#include <QStyle>
#include <QStyleOption>
#include <QApplication>

#include <medLayerItemWidget.h>
#include <medAbstractData.h>
#include <medAbstractVtkViewInteractor.h>


medLayerItemWidget::medLayerItemWidget(QString name,
                                       QString thumbnailPath,
                                       medAbstractData * dataItem,
                                       QList<medAbstractVtkViewInteractor*> interactorsList,
                                       QWidget * parent)
    : QWidget(parent)
{

    QHBoxLayout* layout = new QHBoxLayout(this);
    thumbnailButton = new QPushButton(this);
    QIcon thumbnailIcon;
    // Set the off icon to the greyed out version of the regular icon
    thumbnailIcon.addPixmap(QPixmap(thumbnailPath), QIcon::Normal, QIcon::On);
    QStyleOption opt(0);
    opt.palette = QApplication::palette();
    QPixmap pix = QApplication::style()->generatedIconPixmap(QIcon::Disabled, QPixmap(thumbnailPath), &opt);
    thumbnailIcon.addPixmap(pix, QIcon::Normal, QIcon::Off);
    thumbnailButton->setFocusPolicy(Qt::NoFocus);
    thumbnailButton->setIcon(thumbnailIcon);
    thumbnailButton->setIconSize(QSize(22,22));
    thumbnailButton->setCheckable(true);
    thumbnailButton->setChecked(true);
    thumbnailButton->setFlat(true);

    layerName = new QLabel(name, this);
    opacitySlider = new QSlider(Qt::Horizontal, this);
    opacitySlider->setFixedWidth(75);
    opacitySlider->setRange(0,100);
    opacitySlider->setValue(100);
    opacitySlider->setToolTip("Opacity");

    removeButton = new QPushButton;
    removeButton->setIcon(QIcon(":/icons/cross.svg"));

    layout->setContentsMargins(0,0,10,0);

    layout->addWidget(thumbnailButton);
    layout->addWidget(layerName);
    layout->addStretch();
    layout->addWidget(opacitySlider);
    layout->addWidget(removeButton);

    interactors = interactorsList;
    data = dataItem;

    connect(opacitySlider, SIGNAL(valueChanged(int)),
            this,          SLOT(changeOpacity(int)));

    connect(thumbnailButton, SIGNAL(toggled(bool)),
            this,          SLOT(showLayer(bool)));

    connect(removeButton, SIGNAL(clicked(void)),
            this,          SLOT(removeLayer(void)));
}

medLayerItemWidget::~medLayerItemWidget()
{
}

QSize medLayerItemWidget::sizeHint()
{
    QSize size = QWidget::sizeHint();
    size.setHeight(25);
    return size;
}

void medLayerItemWidget::changeOpacity(int opacity)
{
    double opacityDouble = opacity / (double)(opacitySlider->maximum());
    foreach(medAbstractVtkViewInteractor * i, interactors)
        i->setOpacity(data, opacityDouble);
}

void medLayerItemWidget::showLayer(bool show)
{
    foreach(medAbstractVtkViewInteractor * i, interactors)
        i->setVisible(data, show);
}

void medLayerItemWidget::removeLayer()
{
    foreach(medAbstractVtkViewInteractor * i, interactors)
        i->removeData(data);

    emit deletionRequested();
}

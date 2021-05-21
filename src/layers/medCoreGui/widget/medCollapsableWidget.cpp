#include "medCollapsableWidget.h"

#include <QPropertyAnimation>

#include <QFrame>
#include <QGridLayout>
#include <QParallelAnimationGroup>
#include <QScrollArea>
#include <QToolButton>

#include <QPalette>
#include <QApplication>

class medSpoilerWidgetPrivate
{
public:
    //medSpoilerWidget::direction
    QGridLayout mainLayout;
    QToolButton toggleButton;
    QFrame headerLine;
    QParallelAnimationGroup toggleAnimation;
    QScrollArea contentArea;
    int animationDuration;
};

medSpoilerWidget::medSpoilerWidget(const QString & title, const int animationDuration, QWidget *parent) : QWidget(parent), d(new medSpoilerWidgetPrivate())
{
    d->animationDuration = animationDuration;

    //QWidget::palette().color(QWidget::backgroundRole());
    QPalette palette; 
    //QApplication::instance()->styleSheet();
    //d->toggleButton.setStyleSheet("background-color: rgb(46, 46, 46);");
    d->toggleButton.setStyleSheet("background-color: none;");
    d->toggleButton.setStyleSheet("border: 1px solid red");
    //auto color = palette.color(QPalette::Window);
    //palette.setColor(QPalette::Button, color);
    //palette.setBrush(QPalette::Button, Qt::NoBrush);

    //d->toggleButton.setPalette(palette);
    //d->toggleButton.setStyleSheet("QToolButton { border: none; }");
    d->toggleButton.setToolButtonStyle(Qt::ToolButtonIconOnly);
    d->toggleButton.setArrowType(Qt::ArrowType::UpArrow);
    d->toggleButton.setText(title);
    d->toggleButton.setCheckable(true);
    d->toggleButton.setChecked(false);
    d->toggleButton.setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum);
    //d->toggleButton.setToolButtonStyle(Qt::AlignCenter);

    d->headerLine.setFrameShape(QFrame::HLine);
    //d->headerLine.setFrameShadow(QFrame::Sunken);
    d->headerLine.setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum);
    //d->headerLine.setStyleSheet("background-color: red;");

    d->contentArea.setStyleSheet("QScrollArea { border: none; }");
    d->contentArea.setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    // start out collapsed
    d->contentArea.setMaximumHeight(0);
    d->contentArea.setMinimumHeight(0);

    // let the entire widget grow and shrink with its content
    d->toggleAnimation.addAnimation(new QPropertyAnimation(this, "minimumHeight")); //minimumWidth
    d->toggleAnimation.addAnimation(new QPropertyAnimation(this, "maximumHeight")); //maximumWidth
    d->toggleAnimation.addAnimation(new QPropertyAnimation(&d->contentArea, "maximumHeight")); //maximumWidth

    // don't waste space
    int row = 0;
    d->mainLayout.setVerticalSpacing(0);
    d->mainLayout.setContentsMargins(0, 0, 0, 0);
    d->mainLayout.addWidget(&d->toggleButton, row, 0, 1, 1);/* Qt::AlignCenter);*/
    //d->mainLayout.addWidget(&d->headerLine, row++, 2, 1, 1);
    d->mainLayout.addWidget(&d->contentArea, row+1, 0, 1, 3);
    setLayout(&d->mainLayout);

    connect(&d->toggleButton, &QToolButton::clicked, [this](const bool checked) {
        d->toggleButton.setArrowType(checked ? Qt::ArrowType::DownArrow : Qt::ArrowType::UpArrow);
        d->toggleAnimation.setDirection(checked ? QAbstractAnimation::Forward : QAbstractAnimation::Backward);
        d->toggleAnimation.start();
    });
}

medSpoilerWidget::~medSpoilerWidget()
{
    delete d;
    d = nullptr;
}

void medSpoilerWidget::setDirection(direction direc)
{
}

void medSpoilerWidget::setContentLayout(QLayout & contentLayout)
{
    delete d->contentArea.layout();

    d->contentArea.setLayout(&contentLayout);
    const auto collapsedHeight = sizeHint().height() - d->contentArea.maximumHeight();
    auto contentHeight = contentLayout.sizeHint().height();
    for (int i = 0; i < d->toggleAnimation.animationCount() - 1; ++i)
    {
        QPropertyAnimation * spoilerAnimation = static_cast<QPropertyAnimation *>(d->toggleAnimation.animationAt(i));
        spoilerAnimation->setDuration(d->animationDuration);
        spoilerAnimation->setStartValue(collapsedHeight);
        spoilerAnimation->setEndValue(collapsedHeight + contentHeight);
    }
    QPropertyAnimation * contentAnimation = static_cast<QPropertyAnimation *>(d->toggleAnimation.animationAt(d->toggleAnimation.animationCount() - 1));
    contentAnimation->setDuration(d->animationDuration);
    contentAnimation->setStartValue(0);
    contentAnimation->setEndValue(contentHeight);
}
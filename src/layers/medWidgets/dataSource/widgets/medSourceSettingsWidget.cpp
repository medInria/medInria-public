/*
 * medInria
 * Copyright (c) INRIA 2013. All rights reserved.
 * 
 * medInria is under BSD-2-Clause license. See LICENSE.txt for details in the root of the sources or:
 * https://github.com/medInria/medInria-public/blob/master/LICENSE.txt
 * 
 * This software is distributed WITHOUT ANY WARRANTY; without even
 * the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 */
#include "medSourceSettingsWidget.h"

#include <QGroupBox>
#include <QLabel>
#include <QPainter>
#include <QPushButton>
#include <QTreeWidgetItem>
#include <QVBoxLayout>

#include <medAbstractParameterPresenter.h>

medSourceSettingsWidget::medSourceSettingsWidget(medSourcesLoader * pSourceLoader, 
                                                 medAbstractSource * pSource, 
                                                 QTreeWidget * sourceInformation, 
                                                 QWidget * parent) 
: QFrame(parent)
{
    _pSource = pSource;
    _pSourceLoader = pSourceLoader;
    _sourceInformation = sourceInformation;

    auto * widgetLayout = new QVBoxLayout;
    setLayout(widgetLayout);

    // Transparent background for round corners  
    setAttribute(Qt::WA_TranslucentBackground);

    auto * pVLayout = new QVBoxLayout;
    
    //--- Title area
    auto * titleLayout = new QHBoxLayout;
    pVLayout->addLayout(titleLayout);

    // Add on/off icons
    onOffIcon = new QImage;
    imageLabel = new QLabel(""); // QImage is not a widget, we need to add it in a QLabel
    titleLayout->addWidget(imageLabel);

    // Add a title
    titleLabel = new QLabel(pSource->getInstanceName());
    titleLabel->setStyleSheet("font-weight: bold");
    titleLayout->addWidget(titleLabel);
    titleLayout->addStretch();

    // Default message
    defaultLabel = new QLabel("");
    defaultLabel->setStyleSheet("font: italic");
    titleLayout->addWidget(defaultLabel);

    removeSourceButton = new QPushButton("");
    QIcon quitIcon;
    quitIcon.addPixmap(QPixmap(":/icons/cross_white.svg"), QIcon::Normal);
    removeSourceButton->setIcon(quitIcon);
    removeSourceButton->setFixedWidth(12);
    removeSourceButton->setToolTip(tr("Remove the selected source"));
    titleLayout->addWidget(removeSourceButton);

    //--- Fill parameters in body
    auto params = pSource->getAllParameters();
    for (auto * param : params)
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

    //-- Buttons area
    auto * buttonsLayout = new QHBoxLayout;
    pVLayout->addLayout(buttonsLayout);

    connectButton = new QPushButton(tr("Connect"));
    connectButton->setToolTip(tr("Switch ON or OFF the selected source"));
    buttonsLayout->addWidget(connectButton);
    setIconToConnection();

    auto * setDefaultButton = new QPushButton(tr("Set as default"));
    setDefaultButton->setToolTip(tr("Set the selected source as default"));
    buttonsLayout->addWidget(setDefaultButton);
    isDefault = false; // todo get status (not coded yet) and deactivate delete if default

    widgetLayout->addLayout(pVLayout);

    //--- Now that Qt widgets are set: create connections
    connect(connectButton, &QPushButton::clicked, [=](bool checked)
    {
        switchConnection();
    });

    connect(setDefaultButton, &QPushButton::clicked, [=](bool checked)
    {
        switchDefault();
    });

    connect(removeSourceButton, &QPushButton::clicked, [=](bool checked)
    {
        deleteThisSource(pSourceLoader, pSource);
    });
}

/**
 * @brief Define the graphic behavior when the widget needs to be updated
 * 
 * @param event
 */
void medSourceSettingsWidget::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing); // smooth borders
    painter.setBrush(QBrush("#3C464D"));           // visible color of background
    painter.setPen(Qt::transparent);               // thin border color

    // Change border radius
    QRect rect = this->rect();
    rect.setWidth(rect.width()-1);
    rect.setHeight(rect.height()-1);
    painter.drawRoundedRect(rect, 8, 8);

    QWidget::paintEvent(event);
}

void medSourceSettingsWidget::setIconToConnection()
{
    if (_pSource->isOnline())
    {
        setIcon(ON);
    }
    else
    {
        setIcon(OFF);
    }
}

/**
 * @brief 
 * 
 * @param connect 
 */
void medSourceSettingsWidget::switchConnection()
{
    // TODO check signal (not coded yet)
    if (currentIcon == ON)
    {
        setIcon(OFF);
        _pSource->connect(false);
    }
    else
    {
        setIcon(ON);
        _pSource->connect(true);
    }
}

/**
 * @brief Set the icon of the source status
 * 
 * @param askedType
 */
void medSourceSettingsWidget::setIcon(type askedType)
{
    QIcon newIcon;
    switch (askedType)
    {
        case ON:
        {
            newIcon =  QIcon(":/icons/connect_on.svg");
            currentIcon = ON;
            connectButton->setText("Disconnect");
            break;
        }
        case OFF:
        default:
        {
            newIcon = QIcon(":/icons/connect_off.svg");
            currentIcon = OFF;
            connectButton->setText("Connect");
            break;
        }
    }
    QImage newImage = newIcon.pixmap(QSize(15,15)).toImage();
    onOffIcon->swap(newImage);
    imageLabel->setPixmap(QPixmap::fromImage(*onOffIcon));
}

void medSourceSettingsWidget::mouseReleaseEvent(QMouseEvent * event) 
{
    if (event->button() == Qt::LeftButton)
    {
        // TODO change border around current source item
        _sourceInformation->clear();

        QTreeWidgetItem *name = new QTreeWidgetItem(_sourceInformation);
        name->setText(0, "Selected source: ");
        name->setText(1, _pSource->getInstanceName());

        QTreeWidgetItem *hasCache = new QTreeWidgetItem(_sourceInformation);
        hasCache->setText(0, "Has cache: ");
        hasCache->setText(1, QString::number(_pSource->isCached()));

        QTreeWidgetItem *isLocal = new QTreeWidgetItem(_sourceInformation);
        isLocal->setText(0, "Is local: ");
        isLocal->setText(1, QString::number(_pSource->isLocal()));

        QTreeWidgetItem *isWritable = new QTreeWidgetItem(_sourceInformation);
        isWritable->setText(0, "Is writable: ");
        isWritable->setText(1, QString::number(_pSource->isWritable()));
    }
}

/**
 * @brief Get unique source name associated with this widget
 * 
 * @return QString 
 */
QString medSourceSettingsWidget::getInstanceName() 
{
    return _pSource->getInstanceName();
}

/**
 * @brief 
 * 
 */
void medSourceSettingsWidget::switchDefault()
{
    if (isDefault)
    {
        setToDefault(false);
    }
    else
    {
        setToDefault(true);
    }
}

/**
 * @brief 
 * 
 * @param askedDefault 
 */
void medSourceSettingsWidget::setToDefault(bool askedDefault)
{
    isDefault = askedDefault;
    if (askedDefault)
    {
        defaultLabel->setText("default");
        removeSourceButton->setEnabled(false);
        emit(defaultChosen(_pSource->getInstanceName()));
    }
    else
    {
        defaultLabel->setText("");
        removeSourceButton->setEnabled(true);
        // todo check if the only one left as default -> at least one should be left
    }
}

void medSourceSettingsWidget::deleteThisSource(medSourcesLoader * pSourceLoader, medAbstractSource * pSource)
{
    // Delete the source in the source list, and remove the item widget
    QString instanceId = pSource->getInstanceId();
    pSourceLoader->removeCnx(instanceId);
    QObject::deleteLater();
    emit deletedWidget();
}

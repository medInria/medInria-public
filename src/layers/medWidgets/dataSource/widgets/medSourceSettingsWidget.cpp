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
#include <QPainter>
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

    //--- Title area
    auto * titleLayout = new QHBoxLayout;
    widgetLayout->addLayout(titleLayout);

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

    // todo this does not follow medInria style management. Style sheets created here or in medSourceSettingsDragAreaWidget
    // should be in medInria.qss, in order to allow other themes to change colors or styles.
    QString buttonStyle = "QPushButton {"
                          " background-color: none; }"
                          "QPushButton:hover {"
                          " background-color: grey; }";

    // Minimize button
    minimizeSourceButton = new QPushButton("");
    QIcon minimizeIcon;
    minimizeIcon.addPixmap(QPixmap(":/icons/minimize_off_white.svg"), QIcon::Normal);
    minimizeIcon.addPixmap(QPixmap(":/icons/minimize_off_gray.svg"), QIcon::Disabled);
    minimizeSourceButton->setIcon(minimizeIcon);
    minimizeSourceButton->setFixedWidth(25);
    minimizeSourceButton->setToolTip(tr("Show or hide the body of this source item"));
    minimizeSourceButton->setStyleSheet(buttonStyle);
    titleLayout->addWidget(minimizeSourceButton);

    // Delete button
    removeSourceButton = new QPushButton("");
    QIcon quitIcon;
    quitIcon.addPixmap(QPixmap(":/icons/close.svg"), QIcon::Normal);
    quitIcon.addPixmap(QPixmap(":/icons/close_gray.svg"), QIcon::Disabled);
    removeSourceButton->setIcon(quitIcon);
    removeSourceButton->setFixedWidth(12);
    removeSourceButton->setToolTip(tr("Delete this source"));
    removeSourceButton->setStyleSheet(buttonStyle);
    titleLayout->addWidget(removeSourceButton);

    //--- Fill parameters in body
    auto * parametersLayout = new QVBoxLayout;
    parametersWidget = new QWidget();
    parametersWidget->setLayout(parametersLayout);
    widgetLayout->addWidget(parametersWidget);

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
        parametersLayout->addLayout(pHLayout);
    }

    //-- Init parameters
    switchConnectionIcon(_pSource->isOnline());
    setToDefault(false); // todo set status (not coded yet) here or in medSourcesSettings
    sourceSelected = false;

    //--- Now that Qt widgets are set: create connections
    connect(minimizeSourceButton, &QPushButton::clicked, [=](bool checked)
    {
        switchMinimization();
    });

    connect(removeSourceButton, &QPushButton::clicked, [=](bool checked)
    {
        deleteThisSource(pSourceLoader, pSource);
    });
}

/**
 * @brief Set the icon of the source status
 * 
 * @param connection boolean 
 */
void medSourceSettingsWidget::switchConnectionIcon(bool connection)
{
    QIcon newIcon;
    if (connection)
    {
        newIcon =  QIcon(":/icons/connect_on.svg");
    }
    else
    {
        newIcon = QIcon(":/icons/connect_off.svg");
    }

    QImage newImage = newIcon.pixmap(QSize(15,15)).toImage();
    onOffIcon->swap(newImage);
    imageLabel->setPixmap(QPixmap::fromImage(*onOffIcon));
}

/**
 * @brief Switch the icon after minimization or un-minimization, and show or hide the parameter widget
 * 
 */
void medSourceSettingsWidget::switchMinimization()
{
    QIcon newIcon;
    if (parametersWidget->isHidden())
    {
        parametersWidget->show();
        newIcon =  QIcon(":/icons/minimize_off_white.svg");
    }
    else
    {
        parametersWidget->hide();
        newIcon =  QIcon(":/icons/minimize_on_white.svg");
    }
    minimizeSourceButton->setIcon(newIcon);

    emit(minimizationAsked(parametersWidget->isHidden()));
}

/**
 * @brief Define the graphic behavior when the widget needs to be updated
 * 
 * @param event
 */
void medSourceSettingsWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)

    auto backgroundColor = "#3C464D"; // non selected
    if (sourceSelected)
    {
        backgroundColor = "#7B797D"; // selected
    }

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing); // smooth borders
    painter.setBrush(QBrush(backgroundColor));     // visible color of background
    painter.setPen(Qt::transparent);               // thin border color

    // Change border radius
    QRect rect = this->rect();
    rect.setWidth(rect.width()-1);
    rect.setHeight(rect.height()-1);
    painter.drawRoundedRect(rect, 8, 8);
}

/**
 * @brief When the source item is clicked, the information widget is filled with info from this source,
 * and a signal is emitted to say that this item has been chosen
 * 
 * @param event 
 */
void medSourceSettingsWidget::mouseReleaseEvent(QMouseEvent * event)
{
    if (event->button() == Qt::LeftButton)
    {
        updateSourceInformation();
        emit(sourceItemChosen(_pSource));
    }
}

/**
 * @brief This method fills the information widget with info from this selected source
 * 
 */
void medSourceSettingsWidget::updateSourceInformation()
{
    _sourceInformation->clear();

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

/**
 * @brief Change the visualisation of the source item if selected or not
 * 
 * @param selected 
 */
void medSourceSettingsWidget::setSelectedVisualisation(bool selected)
{
    sourceSelected = selected;
    repaint();
}

/**
 * @brief Get unique source name associated with this widget
 * 
 * @return QString the source name
 */
QString medSourceSettingsWidget::getInstanceName()
{
    return _pSource->getInstanceName();
}

/**
 * @brief Get the abstract source associated with this widget
 * 
 * @return medAbstractSource* 
 */
medAbstractSource * medSourceSettingsWidget::getInstanceSource()
{
    return _pSource;
}

/**
 * @brief When this source item is chosen to be default or not, add or remove the "default" label,
 * and enable or not the delete button (a default source item cannot be deleted)
 * 
 * @param askedDefault 
 */
void medSourceSettingsWidget::setToDefault(bool askedDefault)
{
    // TODO apply default status to sources to save the default source 
    if (askedDefault)
    {
        defaultLabel->setText("default");
        removeSourceButton->setEnabled(false);
    }
    else
    {
        defaultLabel->setText("");
        removeSourceButton->setEnabled(true);
    }
}

/**
 * @brief Delete the source in the source list, and remove the widget
 * 
 * @param pSourceLoader the loader of sources
 * @param pSource the source associated with this source widget
 */
void medSourceSettingsWidget::deleteThisSource(medSourcesLoader * pSourceLoader, medAbstractSource * pSource)
{
    emit deletedWidget(pSource->getInstanceName());

    QString instanceId = pSource->getInstanceId();
    pSourceLoader->removeCnx(instanceId);
}

/**
 * @brief This method saves the initial size of the widget at the creation, with all parameters shown
 * 
 * @param initialSize 
 */
void medSourceSettingsWidget::saveInitialSize(QSize initialSize)
{
    sourceWidgetSize = initialSize;
}

/**
 * @brief This method returns the initial size of the widget at the creation, with all parameters shown
 * 
 * @return QSize 
 */
QSize medSourceSettingsWidget::getInitialSize()
{
    return sourceWidgetSize;
}
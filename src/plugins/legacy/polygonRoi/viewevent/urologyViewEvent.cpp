//
// Created by Julien Castelneau on 06/04/2021.
//

#include <medMetaDataKeys.h>
#include "urologyViewEvent.h"

urologyViewEvent::~urologyViewEvent()
{
    uLabelToolBox->clean();
    uLabelToolBox = nullptr;
}

void urologyViewEvent::initialize(medToolBox *medTb, QString &seriesName)
{
    this->identifier = seriesName;

    pirads.append(QPair<QString, QColor>("PIRADS3", Qt::yellow));
    pirads.append(QPair<QString, QColor>("PIRADS4", QColor(139, 69, 19)));
    pirads.append(QPair<QString, QColor>("PIRADS5", Qt::darkRed));

    uLabelToolBox = dynamic_cast<urologyLabelToolBox *>(medTb);
    uLabelToolBox->setCurrentViewEventFilter(this);
    uLabelToolBox->initialize(seriesName);

    uLabelToolBox->selectRow(0);
}

void urologyViewEvent::activateContour(double *mousePosition)
{
    int position = activateContourBase(mousePosition);
    uLabelToolBox->selectRow(position);
    currentView->render();
}

QLineEdit *urologyViewEvent::changeManagerName(polygonLabel *closestManager, QMenu *mainMenu)
{
    QLineEdit *renameLabelEdit = changeManagerNameBase(closestManager, mainMenu);
    if (closestManager->getPosition()<=3)
    {
        renameLabelEdit->setReadOnly(true);
        return renameLabelEdit;
    }

    connect(renameLabelEdit, &QLineEdit::editingFinished, [=](){
        for (polygonLabel *label : labelList)
        {
            if (label->getName()==renameLabelEdit->text())
            {
                pToolBox->displayMessageError("An other contour has already the same name: " + renameLabelEdit->text());
                return;
            }
        }
        QString name = renameLabelEdit->text();
        closestManager->setName(name);
        uLabelToolBox->rename(closestManager->getPosition(), name);
    });
    return renameLabelEdit;
}

QAction *urologyViewEvent::createScoreAction(polygonLabel *manager, const QString &score, const QColor &color)
{
    unselectLabels();

    QPixmap pixmap(50,50);
    pixmap.fill(color);
    auto action  = new QAction(pixmap, score);

    connect(action, &QAction::triggered, manager,[this, manager, score, color](){
        manager->setOptionalNameWithColor(score, color);
        manager->switchColor();
        currentLabel = manager;
        manager->getState().selected = true;
        manager->setRoisSelectedState();
        manager->getState().checked = true;
        manager->getState().selected = true;
        uLabelToolBox->updateItem(manager->getState());
        uLabelToolBox->updateScoreState(manager->getPosition(),
                                        manager->getState().scoreState, manager->getState().checked,
                                        manager->getName(), manager->getOptName());
    });
    return action;
}

void urologyViewEvent::deleteLabel(polygonLabel *manager)
{
    if (manager->hasScore() && manager->getOptName()!=QString())
    {
        uLabelToolBox->resetItem(manager->getPosition(), manager->getName());
    }

    int position = deleteLabelBase(manager);
    if (position==-1)
    {
        uLabelToolBox->unselectAll();
    }
    else
    {
        uLabelToolBox->selectRow(position);
    }
}

void urologyViewEvent::switchContourColor(double *mousePosition)
{
    polygonLabel *pLabel = getClosestPLabel(mousePosition);

    unselectLabels();

    if (pLabel && pLabel->getMinimumDistanceFromNodesToMouse(mousePosition) < 10. )
    {
        QColor color = pLabel->switchColor();
        if (color != QColor::Invalid)
        {
            currentLabel = pLabel;
            pLabel->getState().selected = true;
            pLabel->setRoisSelectedState();
            pLabel->getState().checked = color == pLabel->getOptColor();
            uLabelToolBox->updateScoreState(pLabel->getPosition(),
                                            pLabel->getState().scoreState, pLabel->getState().checked,
                                            pLabel->getName(), pLabel->getOptName());
        }
    }

}

void urologyViewEvent::updateLabelToolBoxState(QString &name)
{
    uLabelToolBox->setName(name);
    uLabelToolBox->setCurrentViewEventFilter(this);
    for (polygonLabel *pLabel : labelList)
    {
        uLabelToolBox->updateItem(pLabel->getState());
    }
    uLabelToolBox->forceItemSelection();
}

QMenu *urologyViewEvent::createScoreMenu(polygonLabel *pLabel)
{
    if (pLabel->getPosition()<2)
        return nullptr;

    auto scoreMenu = new QMenu("Set Score");
    QMenu *ztMenu = scoreMenu->addMenu(QString("ZT"));
    QMenu *zpMenu = scoreMenu->addMenu(QString("ZP"));
    if (pLabel->hasScore())
    {
        for (const QPair<QString, QColor>& pirad : pirads)
        {
            QString score = QString("ZT_") + pirad.first;
            ztMenu->addAction(createScoreAction(pLabel, score, pirad.second));
            score = QString("ZP_") + pirad.first;
            zpMenu->addAction(createScoreAction(pLabel, score, pirad.second));
        }
    }
    return scoreMenu;
}

void urologyViewEvent::loadContours(QVector<medTagContours> &tagContoursSet)
{
    for (medTagContours &tagContour : tagContoursSet)
    {
        QString name = tagContour.getLabelName();
        bool isTarget = tagContour.isTarget();
        QString scoreName = tagContour.getScore();

        if (isContourLoadable(name))
        {
            QColor color;
            int position = uLabelToolBox->getContourPositionAndColor(name, isTarget, color, scoreName);
            if (position != -1)
            {
                // label can be already created without any roi (if user select the label in toolbox without cretae segmentation in view for instance)
                polygonLabel *label = nullptr;
                bool alreadySegmented = false;
                for (auto pLabel: labelList)
                {
                    if (pLabel->getPosition()==position)
                    {
                        // we should never verify this condition
                        // this test is already done in isContourLoadable.
                        // So If we are here, the label should not be a segmentation already
                        // It is the well known "ceinture + bretelle" method
                        if (!pLabel->getRois().empty())
                        {
                            alreadySegmented = true;
                        }
                        label = pLabel;
                    }
                }
                if (alreadySegmented)
                {
                    qDebug()<<QString("loadContours - contour with name %1 is already a segmentation").arg(label->getName());
                    continue;
                }
                if (!label)
                {
                    label = new polygonLabel(currentView, this, color, name, position, false, enableInterpolation);
                    labelList.append(label);
                }
                if (isTarget && tagContour.getScore() != QString())
                {
                    label->getState().scoreState = isTarget;
                    for (const QPair<QString, QColor>& pirad : pirads)
                    {
                        if (QRegExp("Z[T|P]{1}_" + pirad.first).exactMatch(tagContour.getScore()))
                        {
                            label->setOptionalNameWithColor(tagContour.getScore(), pirad.second);
                            break;
                        }
                    }
                }
                label->loadContours(tagContour.getContourNodes());
                label->setRoisSelectedState();
            }
        }
    }
    currentView->render();
}

void urologyViewEvent::setScoreNameAndColor(polygonLabel *label, QString &name)
{
    QColor scoreColor;
    for (int i = 3; i< 6; i++)
    {
        if (name.contains(QString::number(i)))
            scoreColor = pirads[i-3].second;
    }
    label->setOptionalNameWithColor(name, scoreColor);

}

QString urologyViewEvent::createMaskDescription(polygonLabel *label)
{
    QString name = (label->getOptName() == QString()) ? QString(label->getName()) : QString("%1_%2").arg(label->getName()).arg(label->getOptName());
    medAbstractData * data = currentView->layerData(0);
    QString seriesDesc = data->metadata(medMetaDataKeys::SeriesDescription.key());
    QString patientName = data->metadata(medMetaDataKeys::PatientName.key());
    if (seriesDesc.contains("T2"))
    {
        seriesDesc = "T2";
    }
    else if (seriesDesc.contains("ADC"))
    {
        seriesDesc = "ADC";
    }
    QString desc = patientName + QString(" mask ") + name + " (" + seriesDesc + ")";
    return desc;
}

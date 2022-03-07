/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2019. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include "defaultViewEvent.h"

defaultViewEvent::~defaultViewEvent()
{
    dLabelToolBox->clean();
    dLabelToolBox = nullptr;
}

void defaultViewEvent::initialize(medToolBox *medTb, QString &seriesName)
{
    this->identifier = seriesName;
    dLabelToolBox = dynamic_cast<defaultLabelToolBox *>(medTb);
    dLabelToolBox->setCurrentViewEventFilter(this);
    dLabelToolBox->initialize(seriesName);

    dLabelToolBox->selectRow(0);

}

void defaultViewEvent::activateContour(double *mousePosition)
{
    int position = activateContourBase(mousePosition);
    dLabelToolBox->selectRow(position);
    currentView->render();
}

QLineEdit *defaultViewEvent::changeManagerName(polygonLabel *closestManager, QMenu *mainMenu)
{
    QLineEdit *renameLabelEdit = changeManagerNameBase(closestManager, mainMenu);

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
        dLabelToolBox->rename(closestManager->getPosition(), name);
    });
    return renameLabelEdit;
}

void defaultViewEvent::deleteLabel(polygonLabel *manager)
{
    int position = deleteLabelBase(manager);
    if (position==-1)
    {
        dLabelToolBox->unselectAll();
    }
    else
    {
        dLabelToolBox->selectRow(position);
    }
}

void defaultViewEvent::updateLabelToolBoxState(QString &name)
{
    dLabelToolBox->setName(name);
    dLabelToolBox->setCurrentViewEventFilter(this);
    for (polygonLabel *pLabel : labelList)
    {
        dLabelToolBox->updateItem(pLabel->getState());
    }
    dLabelToolBox->forceItemSelection();
}

void defaultViewEvent::loadContours(QVector<medTagContours> &tagContoursSet)
{
    for (medTagContours &tagContour : tagContoursSet)
    {
        QString name = tagContour.getLabelName();
        if (isContourLoadable(name))
        {
            QColor color;
            int position = dLabelToolBox->getContourPositionAndColor(name, color);
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
                label->loadContours(tagContour.getContourNodes());
                label->setRoisSelectedState();
            }
        }
    }
    currentView->render();
}

/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2019. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <urologyViewEvent.h>
#include <medToolBoxFactory.h>

urologyLabelToolBox::urologyLabelToolBox(QWidget *parent):
        defaultLabelToolBox(parent)
{
}

void urologyLabelToolBox::initialize(QString &seriesName)
{
    connect(labels, &QListWidget::itemClicked, labels, [this](QListWidgetItem *item){
        item->setSelected(true);
        QColor color = item->icon().pixmap(QSize(20,20)).toImage().pixelColor(0,0);
        QString name = item->text();
        QString scoreName;
        if (name.contains(QRegExp(" - Z[T|P]{1}_PIRADS[0-9]")))
        {
            scoreName = name.section(' ', -1);
            name = name.remove(QRegExp(" - Z[T|P]{1}_PIRADS[0-9]"));
        }
        bool hasScore = item->flags().testFlag(Qt::ItemIsUserCheckable);
        bool checkState = (item->checkState() == Qt::Checked);
        int position = labels->row(item);

        if (currentViewEvent)
        {
            currentViewEvent->updateContourProperty(name, color, position, true, hasScore, checkState, scoreName);
        }
    }, Qt::UniqueConnection);

    connect(labels, &QListWidget::itemChanged, labels, [this](QListWidgetItem *item){
        QString name = item->text();
        name = name.remove(QRegExp(" - Z[T|P]{1}_PIRADS[0-9]"));
        int position = labels->row(item);

        if (currentViewEvent)
        {
            auto event = dynamic_cast<urologyViewEvent *>(currentViewEvent);
            event->rename(position, name);

        }
    }, Qt::UniqueConnection);

    clear();

    dataName->setText(seriesName);

    targetColors = QList<QColor>({
                                         Qt::white,
                                         QColor(0,128,128),//teal
                                         QColor(224,255,255),//lightcyan
                                         QColor(229,204,255),
                                         QColor(255,153,204),
                                         QColor(64,224,208)//turquoise
                                 });

    QStringList names;
    QList<bool> scores;
    parseJSONFile(names, scores);
    if (names.size() != scores.size())
    {
        displayMessageError("Json File Malformed");
        return;
    }

    int idx = 0;
    for (const QString& name : names)
    {
        if (mainColors.size()<=idx)
        {
            displayMessageError("Unable to add new contour in list : No more color available");
            break;
        }
        bool score = scores.at(idx);
        QColor col = findAvailableColor((score)?targetColors:mainColors);
        idx++;
        QListWidgetItem *item = createWidgetItem(name,
                                                 col);
        itemSpecialization(item, score);
        labels->addItem(item);
    }
    labels->setMaximumHeight((20*labels->count())+5);

    emit labels->itemClicked(labels->item(0));
}

bool urologyLabelToolBox::registered()
{
    return medToolBoxFactory::instance()->registerToolBox<urologyLabelToolBox>();
}


void urologyLabelToolBox::parseJSONFile(QStringList &labelNames, QList<bool> &scores)
{
    QFile file(":/json/specialities.json");
    if (!file.open(QIODevice::ReadOnly))
    {
        qDebug()<<"unable to open config file";
        return;
    }

    QByteArray rawData = file.readAll();
    file.close();
    QJsonDocument doc(QJsonDocument::fromJson(rawData));
    QJsonObject jsonObject = doc.object();
    QJsonArray jsonArray = jsonObject["specialities"].toArray();
    QList<QStringList> listItems;
    QStringList spe;
    for (const auto & value: jsonArray)
    {
        QJsonObject obj = value.toObject();
        for (const QString& key : obj.keys())
        {
            if (obj[key].isString() && key=="name")
            {
                spe << obj[key].toString();
            }
            else if ( obj[key].isArray())
            {
                QJsonArray descArray= obj[key].toArray();
                for (QJsonValue descValue : descArray)
                {
                    if (descValue.isObject())
                    {
                        auto object = descValue.toObject();
                        for (const QString& k : object.keys())
                        {
                            if (object[k].isString() && k=="name")
                            {
                                labelNames << object[k].toString();
                            }
                            else if (object[k].isBool() && k=="hasScore")
                            {
                                scores << object[k].toBool();
                            }
                            else
                            {
                                qDebug()<<"Unexpected key "<<object[k].type();
                            }
                        }
                    }
                    else
                    {
                        qDebug()<<"Unexpected key "<<descValue<<"   "<<descValue.type();
                    }
                }
            }
        }
    }
}

void urologyLabelToolBox::onPlusClicked()
{
    QColor color;
    QString name;
    bool score;
    int maxSize = mainColors.size()+targetColors.size();
    if ( labels->count()==maxSize)
    {
        displayMessageError("Unable to create more label. Maximum size achieved");
        return;
    }

    QListWidgetItem *item;
    if (isTarget())
    {
        int count = 1;
        for (int row=0; row<labels->count(); row++)
        {
            if (labels->item(row)->text().startsWith("Target"))
            {
                count++;
            }
        }
        name = QString("Target %1").arg(count);

        color = findAvailableColor(targetColors);
        if (color == QColor::Invalid)
        {
            displayMessageError(QString("Unable to create %1 : No Color available").arg(name));
            return;
        }
        score = true;
        item = createWidgetItem(name,
                                color);
        itemSpecialization(item, score);
    }
    else
    {
        int count = 0;
        for (int row=0; row<labels->count(); row++)
        {
            if (!labels->item(row)->text().contains("Target"))
            {
                count++;
            }
        }
        name = QString("Label %1").arg(count);
        color = findAvailableColor(mainColors);
        if (color == QColor::Invalid)
        {
            displayMessageError(QString("Unable to create %1 : No Color available.").arg(name));
            return;
        }
        item = createWidgetItem(name,
                                color);
        itemSpecialization(item, false);
        item->setFlags(item->flags() | Qt::ItemIsEditable);
    }

    if (item)
    {
        labels->addItem(item);
        item->setSelected(true);
        labels->setMaximumHeight((20*labels->count())+5);

        emit labels->itemClicked(item);
    }
    else
    {
        displayMessageError("Error : Creation of new label impossible.");
    }
}

bool urologyLabelToolBox::isTarget()
{
    bool retValue = false;
    QMessageBox msgBox;
    msgBox.setWindowTitle("Label or Target");
    msgBox.setIcon(QMessageBox::Question);
    msgBox.setText("Do you want to create a simple label or a target (potentially with score) ?");

    QAbstractButton *targetButton = msgBox.addButton("Target", QMessageBox::YesRole);
    QAbstractButton *labelButton = msgBox.addButton("Label", QMessageBox::NoRole);
    msgBox.setIcon(QMessageBox::Question);
    msgBox.exec();

    if(msgBox.clickedButton() == targetButton)
    {
        retValue = true;
    }
    if(msgBox.clickedButton() == labelButton)
    {
        retValue = false;
    }

    return retValue;
}

void urologyLabelToolBox::onMinusClicked()
{
    int minRowNumber = 3;
    QList<QListWidgetItem *> items = labels->selectedItems();
    if (items.size()==1)
    {
        QListWidgetItem *item = items.at(0);
        int row = labels->row(item);
        QString name = item->text();
        name = name.remove(QRegExp(" - Z[T|P]{1}_PIRADS[0-9]"));
        item->setText(name);
        if (item->flags().testFlag(Qt::ItemIsUserCheckable))
        {
            item->setCheckState(Qt::Unchecked);
        }
        currentViewEvent->deleteLabel(row);
        if (row > minRowNumber)
        {
            labels->takeItem(row);
            updatePolygonLabelPosition(row);
        }
    }
    labels->setMaximumHeight((20*labels->count())+5);
}

void urologyLabelToolBox::updateScoreState(int position, bool scoreState, bool checked,
                                           QString &firstName, QString &secondName)
{
    QListWidgetItem *item = labels->item(position);
    if (item && item->flags().testFlag(Qt::ItemIsUserCheckable) && scoreState)
    {
        Qt::CheckState state = (checked)?Qt::Checked:Qt::Unchecked;
        QString name = QString("%1 - %2").arg(firstName).arg(secondName);
        item->setText(name);
        item->setCheckState(state);
    }
}

void urologyLabelToolBox::rename(int row, QString &newName)
{
    QListWidgetItem *item = labels->item(row);
    if (item)
    {
        QString name = item->text();
        if (name.contains(QRegExp(" - Z[T|P]{1}_PIRADS[0-9]")))
        {
            name = name.section(' ', -2);
            name.prepend(newName + QString(' '));
        }
        else
        {
            name = newName;
        }
        item->setText(name);
    }
}

void urologyLabelToolBox::clean()
{
    while( labels->count() > 4)
    {
        QListWidgetItem *item = labels->takeItem(labels->count()-1);
        delete item;
    }
    for (int row = 0; row<labels->count(); row++)
    {
        QListWidgetItem *item = labels->item(row);
        QString name = item->text();
        name = name.remove(QRegExp(" - Z[T|P]{1}_PIRADS[0-9]"));
        item->setText(name);
        if (item->flags().testFlag(Qt::ItemIsUserCheckable))
        {
            item->setCheckState(Qt::Unchecked);
        }
    }
    labels->setMaximumHeight((20*labels->count())+5);
}

void urologyLabelToolBox::updateItem(medLabelProperty &info)
{
    QListWidgetItem *item = labels->item(info.position);
    if (!item)
    {
        item = createWidgetItem(info.mainName, info.mainColor);
        labels->insertItem(info.position, item);
        labels->setMaximumHeight((20 * labels->count()) + 5);
    }
    else if (item->text() != info.mainName)
    {
        info.mainName = item->text();
    }
    if (info.scoreState && !info.secondName.isEmpty())
    {
        QString name = item->text();
        name = name.remove(QRegExp(" - Z[T|P]{1}_PIRADS[0-9]"));
        name.append(QString(" - %1").arg(info.secondName));
        item->setText(name);
    }
    itemSpecialization(item, info.scoreState, info.checked);
}

void urologyLabelToolBox::itemSpecialization(QListWidgetItem *item, bool scoreState, bool checkState)
{
    if (scoreState)
    {
        item->setFlags(item->flags() | Qt::ItemIsSelectable | Qt::ItemIsUserCheckable);
        item->setFlags(item->flags() & (~Qt::ItemIsDragEnabled));
        Qt::CheckState checked = (checkState)?Qt::Checked:Qt::Unchecked;
        item->setCheckState(checked);
    }
    else
    {
        item->setFlags(item->flags() | Qt::ItemIsSelectable);
        item->setFlags(item->flags() & (~Qt::ItemIsUserCheckable));
        item->setFlags(item->flags() & (~Qt::ItemIsDragEnabled));
    }
}

int urologyLabelToolBox::getContourPositionAndColor(QString &name, bool scoreState, QColor &color, QString &scoreName)
{
    int position = -1;

    if (getPreLoadNames().contains(name))
    {
        for (int row = 0; row < labels->count(); row++)
        {
            QListWidgetItem *item = labels->item(row);
            if (item->text() == name)
            {
                position = row;
                color = item->icon().pixmap(QSize(20,20)).toImage().pixelColor(0,0);
                if (scoreState)
                {
                    QString newName = item->text();
                    newName.append(QString(" - %0").arg(scoreName));
                    item->setText(newName);
                }
                break;
            }
        }
    }
    else
    {
        QString newName = name;
        if (scoreState)
        {
            newName.append(QString(" - %0").arg(scoreName));
        }
        color = findAvailableColor(scoreState?targetColors:mainColors);
        QListWidgetItem *item = createWidgetItem(newName, color);
        itemSpecialization(item, scoreState);
        labels->addItem(item);
        position = labels->row(item);
        labels->setMaximumHeight((20*labels->count())+5);
    }
    return position;
}

bool urologyLabelToolBox::hasScoreOnPosition(int row, QString &score, QColor &color)
{
    bool retValue = false;
    QListWidgetItem *item = labels->item(row);
    if (item)
    {
        QString name = item->text();
        if (name.contains(QRegExp(" - Z[T|P]{1}_PIRADS[0-9]")))
        {
            score = name.section(' ', -2);
            color = item->icon().pixmap(QSize(20,20)).toImage().pixelColor(0,0);
            retValue = true;
        }
    }
    return retValue;
}

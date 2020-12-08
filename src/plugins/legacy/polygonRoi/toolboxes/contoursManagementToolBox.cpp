#include "contoursManagementToolBox.h"
#include "polygonRoiToolBox.h"

#include <medAbstractDataFactory.h>
#include <medAbstractImageData.h>
#include <medAbstractProcessLegacy.h>
#include <medContours.h>
#include <medDataManager.h>
#include <medTabbedViewContainers.h>
#include <medToolBoxFactory.h>
#include <medUtilities.h>

const char* contoursManagementToolBox::urologySpeciality = "urology";

contoursManagementToolBox::contoursManagementToolBox(QWidget *parent):
    medToolBox(parent), currentView(nullptr), savedSelectedIndex(QPair<int,int>(0, 0))
{
    mainColors = QList<QColor>({
        Qt::green,
        Qt::blue,
        Qt::darkMagenta,
        QColor(255,128,0),
        QColor(102,0,204),
        QColor(255,0,127),
        Qt::gray,
        Qt::black
    });

    targetColors = QList<QColor>({
        Qt::white,
        QColor(0,128,128),//teal
        QColor(224,255,255),//lightcyan
        QColor(229,204,255),
        QColor(255,153,204),
        QColor(64,224,208)//turquoise
    });
    QWidget *widget = new QWidget();

    QVBoxLayout *layout = new QVBoxLayout;
    widget->setLayout(layout);

    specialities = new QComboBox();

    plusButton = new QPushButton(QIcon(":/pixmaps/plus.png"), "");
    plusButton->setMaximumSize(QSize(20,20));
    plusButton->hide();

    minusButton = new QPushButton(QIcon(":/pixmaps/minus.png"), "");
    minusButton->setMaximumSize(QSize(20,20));
    minusButton->hide();

    targetLabel = new QLabel("Target");
    targetLabel->hide();
    targetButton = new SwitchButton(this, SwitchButton::YESNO);
    targetButton->hide();
    connect(targetButton, SIGNAL(valueChanged(bool)), this, SLOT(switchTargetState(bool)));

    parseJSONFile();

    QVBoxLayout *listLabelLayout = new QVBoxLayout();
    layout->addLayout(listLabelLayout);
    listLabelLayout->addWidget(specialities);
    for (QListWidget *labelNamesList : labels)
    {
        listLabelLayout->addWidget(labelNamesList);
    }
    QHBoxLayout *plusMinusLayout = new QHBoxLayout;
    plusMinusLayout->addWidget(minusButton, Qt::AlignLeft);
    plusMinusLayout->addWidget(plusButton, Qt::AlignLeft);
    plusMinusLayout->addStretch();
    plusMinusLayout->addWidget(targetLabel);
    plusMinusLayout->addWidget(targetButton);
    listLabelLayout->addLayout(plusMinusLayout);

    disableButtons();
    this->addWidget(widget);

    connect(specialities, SIGNAL(currentIndexChanged(int)), this, SLOT(showWidgetListForIndex(int)));
    connect(minusButton, SIGNAL(pressed()), this, SLOT(removeLabelNameInList()));
    connect(plusButton, SIGNAL(pressed()), this, SLOT(addLabel()));
}

contoursManagementToolBox::~contoursManagementToolBox()
{
    for(QListWidget *widget : labels)
    {
        delete widget;
    }
    labels.clear();

    currentView = nullptr;
}

bool contoursManagementToolBox::registered()
{
    return medToolBoxFactory::instance()->registerToolBox<contoursManagementToolBox>();
}

void contoursManagementToolBox::clear()
{
    currentView = nullptr;

    QListWidget *widget = labels.at(0);
    while( widget->count() > 1)
    {
        QListWidgetItem *item = widget->takeItem(widget->count()-1);
        delete item;
    }
    widget->item(0)->setText("Label 0");
    widget->setMaximumHeight((20*widget->count())+5);

    widget = labels.at(1);
    while( widget->count() > 4)
    {
        QListWidgetItem *item = widget->takeItem(widget->count()-1);
        delete item;
    }
    for (int row = 0; row<widget->count(); row++)
    {
        QListWidgetItem *item = widget->item(row);
        QString name = item->text();
        name = name.remove(QRegExp(" - PIRADS[0-9]"));
        item->setText(name);
        if (item->flags().testFlag(Qt::ItemIsUserCheckable))
        {
            item->setCheckState(Qt::Unchecked);
        }
    }
    widget->setMaximumHeight((20*widget->count())+5);

    specialities->setCurrentIndex(0);

    savedSelectedIndex = QPair<int, int>(0, 0);
}

void contoursManagementToolBox::updateView()
{
    medTabbedViewContainers *containers = this->getWorkspace()->tabbedViewContainers();
    if (containers)
    {
        medAbstractView *view = containers->getFirstSelectedContainerView();
        if (!view)
        {
            return;
        }
        medAbstractImageView *v = qobject_cast<medAbstractImageView*>(view);
        if (!v)
        {
            return;
        }
        QList<medAbstractView*> viewsInTabSelected = containers->viewsInTab(containers->currentIndex());
        if (viewsInTabSelected.size() > 1)
        {
            for (unsigned int i=0; i<v->layersCount(); ++i)
            {
                loadDataAsContours(v, i);
            }
            return;
        }
        for (unsigned int i=0; i<v->layersCount(); ++i)
        {
            medAbstractData *data = v->layerData(i);
            if(!data || data->identifier().contains("vtkDataMesh")
                                    || data->identifier().contains("itkDataImageVector"))
            {
                handleDisplayError(medAbstractProcessLegacy::DIMENSION_3D);
                return;
            }
            else
            {
                if (currentView != v)
                {
                    currentView = v;
                }

                loadDataAsContours(v, i);
            }
        }
    }
    showWidgetListForIndex(specialities->currentIndex());
}

bool contoursManagementToolBox::loadDefaultContours(QListWidget *widget, QVector<medTagContours> &tagContoursSet)
{
    bool retVal = false;
    for (medTagContours &tagContours : tagContoursSet)
    {
        if (widget->count()<mainColors.size())
        {
            QColor color = findAvailableColor(widget, mainColors);
            bool itemFound = false;
            for (int row = 0; row < widget->count(); row++)
            {
                QListWidgetItem *item = widget->item(row);
                if (item->text()==tagContours.getLabelName())
                {
                    itemFound = true;
                    color = item->icon().pixmap(QSize(20,20)).toImage().pixelColor(0,0);
                    break;
                }
            }
            if (!itemFound)
            {
                QListWidgetItem *item = createWidgetItem(tagContours.getLabelName(),
                                                         color);
                widget->addItem(item);
                widget->setMaximumHeight((20*widget->count())+5);
            }
            if (color!=QColor::Invalid)
            {
                emit contoursToLoad(tagContours, color);
                retVal = true;
            }
        }
        else
        {
            displayMessageError("Too many label already imported");
            retVal = false;
            break;
        }
    }
    return retVal;
}

bool contoursManagementToolBox::loadUrologyContours(QListWidget *widget, QVector<medTagContours> &tagContoursSet)
{
    bool retVal = false;
    int maxSize = mainColors.size()+targetColors.size();
    for (medTagContours &tagContours : tagContoursSet)
    {
        QColor color = QColor::Invalid;
        bool itemFound = false;
        for (int row = 0; row < widget->count(); row++)
        {
            QListWidgetItem *item = widget->item(row);
            QString name = item->text().remove(QRegExp(" - PIRADS[0-9]"));
            if (name==tagContours.getLabelName())
            {
                itemFound = true;
                color = item->icon().pixmap(QSize(20,20)).toImage().pixelColor(0,0);
                if (tagContours.isTarget() && tagContours.getScore()!=QString())
                {
                    QString name = QString("%1 - %2").arg(tagContours.getLabelName()).arg(tagContours.getScore());
                    item->setText(name);
                }
                break;
            }
        }
        if (!itemFound)
        {
            QList<QColor> colors;
            bool score;
            if (tagContours.isTarget())
            {
                colors = targetColors;
                score = true;
            }
            else
            {
                colors = mainColors;
                score = false;
            }
            color = findAvailableColor(widget, colors);

            QListWidgetItem *item = createWidgetItem(tagContours.getLabelName(),
                                                     color, score, true);
            if (score)
            {
                QString name = QString("%1 - %2").arg(tagContours.getLabelName()).arg(tagContours.getScore());
                item->setText(name);
            }
            widget->addItem(item);
            widget->setMaximumHeight((20*widget->count())+5);
        }
        if (widget->count()<maxSize && color!=QColor::Invalid)
        {
            emit contoursToLoad(tagContours, color);
            retVal = true;
        }
        else
        {
            displayMessageError("Too many label already imported");
            retVal = false;
            break;
        }
    }
    return retVal;
}

bool contoursManagementToolBox::loadDataAsContours(medAbstractImageView *v, unsigned int layer)
{
    bool retVal = false;
    medAbstractData *data = v->layerData(layer);
    if (data->identifier().contains("medContours") && v==currentView)
    {
        medContours *contours = dynamic_cast<medContours*>(data);
        QVector<medTagContours> &tagContoursSet = contours->getTagContoursSet();
        specialities->setCurrentIndex(tagContoursSet[0].getSpecialityIndex());
        QListWidget *widget = labels.at(tagContoursSet[0].getSpecialityIndex());

        if (specialities->currentIndex()==0)
        {
            retVal = loadDefaultContours(widget, tagContoursSet);
        }
        else if (specialities->currentIndex()==1)
        {
            retVal = loadUrologyContours(widget, tagContoursSet);
        }
        emit repulsorState();

        v->removeLayer(layer);
    }
    return retVal;
}

void contoursManagementToolBox::clickActivationButton(bool state)
{
    specialities->setEnabled(state);
    for (QListWidget *widget : labels)
    {
        widget->setEnabled(state);
    }
    plusButton->setEnabled(state);
    minusButton->setEnabled(state);
    targetButton->setEnabled(state);
    if (state)
    {
        specialities->setCurrentIndex(savedSelectedIndex.first);
        QListWidget *widget = labels.at(savedSelectedIndex.first);
        QListWidgetItem *item = widget->item(savedSelectedIndex.second);
        item->setSelected(true);
        emit widget->itemChanged(item);
    }
}

void contoursManagementToolBox::disableButtons()
{
    specialities->setDisabled(true);
    for (QListWidget *widget : labels)
    {
        widget->setDisabled(true);
    }
    plusButton->setDisabled(true);
    minusButton->setDisabled(true);
    targetButton->setDisabled(true);
}

void contoursManagementToolBox::showCurrentLabels()
{
    showWidgetListForIndex(specialities->currentIndex());
}

void contoursManagementToolBox::showWidgetListForIndex(int index)
{
    for (QListWidget *widget : labels)
    {
        widget->hide();
    }
    QListWidget *widget = labels.at(index);

    updateLabelNamesOnContours(widget);

    if (widget->selectedItems().size()==1)
    {
        QListWidgetItem *item = widget->selectedItems().at(0);
        emit widget->itemClicked(item);
    }
    widget->show();
    plusButton->show();
    minusButton->show();
    if(index==1)
    {
        if (widget->selectedItems().size()==1)
        {
            QListWidgetItem *item = widget->selectedItems().at(0);
            if (widget->row(item)>3)
            {
                targetLabel->show();
                targetButton->show();
            }
            else
            {
                targetLabel->hide();
                targetButton->hide();
            }
        }
    }
    else
    {
        targetLabel->hide();
        targetButton->hide();
    }
}

void contoursManagementToolBox::updateLabelNamesOnContours(QListWidget *widget)
{
    QList<medContourSharedInfo> infos;
    for (int row = 0; row < widget->count(); row++)
    {
        QListWidgetItem *item = widget->item(row);
        QString name = item->text();
        name = name.remove(QRegExp(" - PIRADS[0-9]"));
        QColor col = item->icon().pixmap(QSize(20,20)).toImage().pixelColor(0,0);
        medContourSharedInfo info = medContourSharedInfo(name, col);
        infos.append(info);
    }

    emit sendDatasToView(infos);
}

QListWidget *contoursManagementToolBox::initLabelsList(QStringList names, QList<bool> scores, bool isProstate)
{
    QListWidget *widget = new QListWidget;

    connect(widget, &QListWidget::itemClicked, this, [this](QListWidgetItem *item){
        if (currentView)
        {
           currentView->viewWidget()->setFocus();
        }
        item->setSelected(true);
        QColor color = item->icon().pixmap(QSize(20,20)).toImage().pixelColor(0,0);
        QString name = item->text();
        name = name.remove(QRegExp(" - PIRADS[0-9]"));
        bool hasScore = item->flags().testFlag(Qt::ItemIsUserCheckable);
        bool checkState = (item->checkState()==Qt::Checked)?true:false;

        medContourSharedInfo info = medContourSharedInfo(name, color);
        info.setScoreInfo(hasScore);
        info.setCheckState(checkState);
        emit sendContourState(info);
    }, Qt::UniqueConnection);

    connect(widget, &QListWidget::itemChanged, this, [this](QListWidgetItem *item){
        if (currentView)
        {
            currentView->viewWidget()->setFocus();
        }
        QColor color = item->icon().pixmap(QSize(20,20)).toImage().pixelColor(0,0);
        QString name = item->text();
        name = name.remove(QRegExp(" - PIRADS[0-9]"));
        medContourSharedInfo info = medContourSharedInfo(name, color);
        emit sendContourName(info);
    }, Qt::UniqueConnection);

    connect(widget, &QListWidget::itemSelectionChanged, this, [this, widget](){
        if (currentView)
        {
            currentView->viewWidget()->setFocus();
        }
        if (widget->selectedItems().size()==0)
        {
            targetLabel->hide();
            targetButton->hide();
            QString name = QString();
            QColor color = QColor::Invalid;
            medContourSharedInfo info = medContourSharedInfo(name, color);
            emit sendContourState(info);
        }
        else if (widget->selectedItems().size()==1)
        {
            QListWidgetItem *item = widget->selectedItems().at(0);
            if (widget->row(item)>3)
            {
                if (targetLabel->isHidden() || targetButton->isHidden())
                {
                    targetLabel->show();
                    targetButton->show();
                }
            }
            else
            {
                if (targetLabel->isVisible() || targetButton->isVisible())
                {
                    targetLabel->hide();
                    targetButton->hide();
                }
            }
        }
    }, Qt::UniqueConnection);

    widget->setContentsMargins(0,0,0,0);

    int idx = 0;
    for (QString name : names)
    {
        if (mainColors.size()<=idx)
        {
            displayMessageError("Unable to add new contour in list : No more color available");
            break;
        }
        QColor col;
        if (name.startsWith("Target "))
        {
            col = findAvailableColor(widget, targetColors);
        }
        else
        {
            col = findAvailableColor(widget, mainColors);
        }
        bool score = false;
        if (scores.size()==names.size())
        {
            score = scores.at(idx);
        }
        idx++;
        QListWidgetItem *item = createWidgetItem(name,
                                                 col,
                                                 score,
                                                 isProstate);

        widget->addItem(item);
    }
    if (!isProstate)
    {
        widget->setDragDropMode(QAbstractItemView::InternalMove);
        widget->setDragEnabled(true);
        widget->setAcceptDrops(true);
        widget->setDropIndicatorShown(true);
    }
    widget->setMaximumHeight((20*widget->count())+5);
    widget->hide();
    return widget;
}

void contoursManagementToolBox::parseJSONFile()
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
    QStringList spe;
    QList<QStringList> listItems;

    for (const QJsonValue & value: jsonArray)
    {
        QJsonObject obj = value.toObject();
        QStringList items;
        QList<bool> scores;
        for (QString key : obj.keys())
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
                        QJsonObject obj = descValue.toObject();
                        for (QString k : obj.keys())
                        {
                            if (obj[k].isString() && k=="name")
                            {
                                items << obj[k].toString();
                            }
                            else if (obj[k].isBool() && k=="hasScore")
                            {
                                scores << obj[k].toBool();
                            }
                            else
                            {
                                qDebug()<<"Unexpected key "<<obj[k].type();
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
        bool isProstate = (spe.last()==urologySpeciality)?true:false;
        listItems.append(items);
        QListWidget *widget = initLabelsList(items, scores, isProstate);
        labels.append(widget);
    }
    specialities->addItems(spe);
}

QListWidgetItem * contoursManagementToolBox::createWidgetItem(QString name, QColor col, bool score, bool isProstate)
{
    QPixmap pixmap(20,20);
    pixmap.fill(col);
    QListWidgetItem *item =  new QListWidgetItem(pixmap, name);
    item->setSizeHint(QSize(20,20));
    QFont font;
    font.setBold(true);
    item->setFont(font);
    if (isProstate)
    {
        if (score)
        {
            item->setFlags(item->flags() | Qt::ItemIsSelectable | Qt::ItemIsUserCheckable);
            item->setFlags(item->flags() & (~Qt::ItemIsDragEnabled));
            item->setCheckState(Qt::Unchecked);
        }
        else
        {
            item->setFlags(item->flags() | Qt::ItemIsSelectable);
            item->setFlags(item->flags() & (~Qt::ItemIsUserCheckable));
            item->setFlags(item->flags() & (~Qt::ItemIsDragEnabled));
        }
    }
    else
    {
        item->setFlags(item->flags() | Qt::ItemIsSelectable | Qt::ItemIsEditable);
        item->setFlags(item->flags() & (~Qt::ItemIsUserCheckable));
    }

    return item;
}

void contoursManagementToolBox::addLabel()
{
    QListWidget *widget = labels.at(specialities->currentIndex());
    int maxSize = (specialities->currentIndex()==0)?mainColors.size():(mainColors.size()+targetColors.size());
    if ( widget->count()==maxSize)
    {
        displayMessageError("Unable to create more label. Maximum size achieved");
        return;
    }

    QListWidgetItem *item = nullptr;
    if (specialities->currentText()==urologySpeciality
            && specialities->currentIndex()==1)
    {
        if (targetButton->value())
        {
            QColor color = findAvailableColor(widget, targetColors);
            int count = 1;
            for (int row=0; row<widget->count(); row++)
            {
                if (widget->item(row)->text().startsWith("Target"))
                {
                    count++;
                }
            }
            QString name = QString("Target %1").arg(count);
            if (color == QColor::Invalid)
            {
                displayMessageError(QString("Unable to create %1 : No Color available").arg(name));
                return;
            }
            item = createWidgetItem(name,
                                    color, true, true);
        }
        else
        {
            QColor color = findAvailableColor(widget, mainColors);
            int count = 0;
            for (int row=0; row<widget->count(); row++)
            {
                if (!widget->item(row)->text().contains("Target"))
                {
                    count++;
                }
            }
            QString name = QString("Label %1").arg(count);
            if (color == QColor::Invalid)
            {
                displayMessageError(QString("Unable to create %1 : No Color available.").arg(name));
                return;
            }
            item = createWidgetItem(name,
                                    color, false, true);
            item->setFlags(item->flags() | Qt::ItemIsEditable);
        }
    }
    else
    {
        QColor color = findAvailableColor(widget, mainColors);
        int count = 0;
        for (int row=0; row<widget->count(); row++)
        {
            if (widget->item(row)->text().startsWith("Label"))
            {
                count++;
            }
        }
        QString name = QString("Label %1").arg(QString::number(widget->count()));
        if (color == QColor::Invalid)
        {
            displayMessageError(QString("Unable to create %1 : No Color available.").arg(name));
            return;
        }
        item = createWidgetItem(name,
                                color);
    }
    if (item)
    {
        widget->addItem(item);
        item->setSelected(true);
        widget->setMaximumHeight((20*widget->count())+5);

        emit widget->itemClicked(item);
    }
    else
    {
        displayMessageError("Error : Creation of new label impossible.");
    }
}

void contoursManagementToolBox::removeLabelNameInList()
{
    QListWidget *widget = labels.at(specialities->currentIndex());
    int minRowNumber = 0;
    if (specialities->currentIndex()==1 && specialities->currentText()==urologySpeciality)
    {
        minRowNumber = 3;
    }

    for (int row = 0; row<widget->count(); row++)
    {
        QListWidgetItem *item = widget->item(row);
        if (item->isSelected())
        {
            QString name = item->text();
            name = name.remove(QRegExp(" - PIRADS[0-9]"));
            QColor col = item->icon().pixmap(QSize(20,20)).toImage().pixelColor(0,0);
            medContourSharedInfo info = medContourSharedInfo(name, col);
            emit labelToDelete(info);
            if (row > minRowNumber)
            {
                widget->takeItem(row);
            }
            else
            {
                info.setScoreInfo(item->flags().testFlag(Qt::ItemIsUserCheckable));
                emit sendActivationState(info);
            }
        }
    }
    widget->setMaximumHeight((20*widget->count())+5);
}

void contoursManagementToolBox::switchTargetState(bool state)
{
    if (specialities->currentIndex()!=1)
    {
        qDebug()<<QString("%1 :: Impossible case - Slot only available on speciality %2").arg(metaObject()->className()).arg(urologySpeciality);
    }
    else
    {
        QListWidget *widget = labels.at(specialities->currentIndex());
        QList<QListWidgetItem*> selectedItems = widget->selectedItems();
        if (selectedItems.size()==1)
        {
            QListWidgetItem *item = selectedItems.at(0);
            int row = widget->row(item);
            if (item->isSelected() && row > 3)
            {
                bool itemCheckable = item->flags().testFlag(Qt::ItemIsUserCheckable);
                QString name = "unamed";
                QColor color;
                if (itemCheckable && !state)
                {
                    item->setData(Qt::CheckStateRole, QVariant());
                    item->setFlags(item->flags() & (~Qt::ItemIsUserCheckable));
                    item->setFlags(item->flags() | Qt::ItemIsEditable);
                    int count = 0;
                    for (int row=0; row<widget->count(); row++)
                    {
                        if (!widget->item(row)->text().contains("Target"))
                        {
                            count++;
                        }
                    }
                    name = QString("Label %1").arg(count);
                    color = findAvailableColor(widget, mainColors);
                }
                else if (!itemCheckable && state)
                {
                    item->setData(Qt::CheckStateRole,Qt::Unchecked);
                    item->setCheckState(Qt::Unchecked);
                    item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
                    item->setFlags(item->flags() & (~Qt::ItemIsEditable));
                    int count = 1;
                    for (int row=0; row<widget->count(); row++)
                    {
                        if (widget->item(row)->text().startsWith("Target"))
                        {
                            count++;
                        }
                    }
                    name = QString("Target %1").arg(count);
                    color = findAvailableColor(widget, targetColors);
                }
                else
                {
                    return;
                }
                QColor oldColor = item->icon().pixmap(QSize(20,20)).toImage().pixelColor(0,0);
                item->setText(name);
                QPixmap pixmap(20,20);
                pixmap.fill(color);
                item->setIcon(pixmap);
                bool hasScore = item->flags().testFlag(Qt::ItemIsUserCheckable);
                bool checkState = (item->checkState()==Qt::Checked)?true:false;

                medContourSharedInfo info = medContourSharedInfo(name, oldColor);
                info.setAdditionalNameAndColor(QString(), color);
                info.setScoreInfo(hasScore);
                info.setCheckState(checkState);
                emit sendContourState(info);
            }
        }
    }
}

QColor contoursManagementToolBox::findAvailableColor(QListWidget *widget, QList<QColor> colors)
{
    QColor color;
    QList<QColor> cols = colors;
    for (int row = 0; row < widget->count(); row++)
    {
        QListWidgetItem *item = widget->item(row);
        color = item->icon().pixmap(QSize(20,20)).toImage().pixelColor(0,0);
        if (cols.contains(color))
        {
            cols.removeOne(color);
        }
    }
    if (cols.empty())
    {
        color = QColor::Invalid;
    }
    else
    {
        color = cols.at(0);
    }
    return color;
}

void contoursManagementToolBox::receiveContoursDatasFromView(medContourSharedInfo& info)
{
    QListWidget *widget = labels.at(specialities->currentIndex());
    for (int row = 0; row < widget->count(); row++)
    {
        QListWidgetItem *item = widget->item(row);
        QString itemName = item->text();
        itemName = itemName.remove(QRegExp(" - PIRADS[0-9]"));

        QColor itemColor = item->icon().pixmap(QSize(20,20)).toImage().pixelColor(0,0);
        if ( info.nameChanged() && itemColor==info.getColor())
        {
            if (!item->flags().testFlag(Qt::ItemIsEditable))
            {
                displayMessageError(QString("%1 is not editable").arg(itemName));
                medContourSharedInfo info = medContourSharedInfo(itemName, itemColor);
                emit sendContourName(info);
            }
            else
            {
                item->setText(info.getName());
                item->setSelected(true);
            }
        }
        else if (itemName==info.getName() && itemColor==info.getColor())
        {
            item->setSelected(info.isSelected());
            if (info.hasScore() && item->flags().testFlag(Qt::ItemIsUserCheckable))
            {
                Qt::CheckState state = (info.checkState())?Qt::Checked:Qt::Unchecked;
                QString name = (info.getAdditionalName()!=QString())?QString("%1 - %2").arg(info.getName()).arg(info.getAdditionalName()):
                                                       info.getName();
                item->setText(name);
                item->setCheckState(state);
            }
            else if (info.descUpdated() && item->flags().testFlag(Qt::ItemIsUserCheckable))
            {
                item->setText(info.getAdditionalName());
                item->setCheckState(Qt::Unchecked);
            }
            medContourSharedInfo activationInfo = medContourSharedInfo(itemName, itemColor);
            activationInfo.setScoreInfo(item->flags().testFlag(Qt::ItemIsUserCheckable));
            emit sendActivationState(activationInfo);
        }

        else
        {
            item->setSelected(false);
        }
    }
}

void contoursManagementToolBox::unselectAll()
{
    QListWidget *widget = labels.at(specialities->currentIndex());
    for (int row=0; row<widget->count(); row++)
    {
        QListWidgetItem *item = widget->item(row);
        if (item->isSelected())
        {
            savedSelectedIndex.first = specialities->currentIndex();
            savedSelectedIndex.second = row;
        }
        item->setSelected(false);
    }
}

void contoursManagementToolBox::onContoursSaved(medAbstractImageView *view, vtkMetaDataSet *outputDataSet, QVector<medTagContours> contoursData)
{
    contourOutput = medAbstractDataFactory::instance()->createSmartPointer("medContours");

    medAbstractData * input = view->layerData(0);
    medAbstractImageData *inputData = qobject_cast<medAbstractImageData*>(input);
    QString desc;
    if (contoursData.size()==0)
    {
        return;
    }
    else if (contoursData.size()==1)
    {
        desc = QString("contour %1").arg(contoursData[0].getLabelName());
        contoursData[0].setSpecialityIndex(specialities->currentIndex());
        if ( specialities->currentIndex()==1 && specialities->currentText()==urologySpeciality)
        {
            QListWidget *widget = labels.at(specialities->currentIndex());
            for (int row=0; row<widget->count(); row++)
            {
                QListWidgetItem *item = widget->item(row);
                if (item->text().contains(contoursData[0].getLabelName()))
                {
                    bool target = (item->flags().testFlag(Qt::ItemIsUserCheckable))?true:false;
                    contoursData[0].setTarget(target);
                }
            }
        }
    }
    else
    {
        desc = QString("%1 contours").arg(contoursData.size());
        for (medTagContours& ctr : contoursData )
        {
            ctr.setSpecialityIndex(specialities->currentIndex());
            if ( specialities->currentIndex()==1 && specialities->currentText()==urologySpeciality)
            {
                QListWidget *widget = labels.at(specialities->currentIndex());
                for (int row=0; row<widget->count(); row++)
                {
                    QListWidgetItem *item = widget->item(row);
                    if (item->text().contains(ctr.getLabelName()))
                    {
                        bool target = (item->flags().testFlag(Qt::ItemIsUserCheckable))?true:false;
                        ctr.setTarget(target);
                    }
                }
            }
        }
    }
    medUtilities::setDerivedMetaData(contourOutput, inputData, desc);
    contourOutput->setData(outputDataSet);
    contourOutput->setData(&contoursData, 1);
    outputDataSet->Delete();

    medDataManager::instance()->importData(contourOutput, false);
}

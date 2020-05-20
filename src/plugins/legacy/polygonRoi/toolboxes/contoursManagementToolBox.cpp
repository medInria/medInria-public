#include "contoursManagementToolBox.h"
#include "polygonRoiToolBox.h"

#include <medAbstractProcessLegacy.h>
#include <medContours.h>
#include <medTabbedViewContainers.h>
#include <medToolBoxFactory.h>

contoursManagementToolBox::contoursManagementToolBox(QWidget *parent):
    medToolBox(parent), currentView(nullptr), savedSelectedIndex(QPair<int,int>(0, 0))
{
    colors = QList<QColor>({
        Qt::green,
        Qt::blue,
        QColor(224,255,255),
        QColor(64,224,208),
        QColor(46,139,87),
        Qt::darkMagenta,
        Qt::gray,
        Qt::white,
        Qt::black
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
    listLabelLayout->addLayout(plusMinusLayout);

    disableButtons();
    this->addWidget(widget);

    connect(specialities, SIGNAL(currentIndexChanged(int)), this, SLOT(showWidgetListForIndex(int)));
    connect(minusButton, SIGNAL(pressed()), this, SLOT(removeLabelNameInList()));
    connect(plusButton, SIGNAL(pressed()), this, SLOT(addLabel()));
}

contoursManagementToolBox::~contoursManagementToolBox()
{
    currentView = nullptr;
}

bool contoursManagementToolBox::registered()
{
    return medToolBoxFactory::instance()->registerToolBox<contoursManagementToolBox>();
}

void contoursManagementToolBox::clear()
{
    medToolBox::clear();

    currentView = nullptr;

    QListWidget *widget = labels.at(0);
    while( widget->count() > 1)
    {
        QListWidgetItem *item = widget->takeItem(widget->count()-1);
        delete item;
    }
    widget->setMaximumHeight((20*widget->count())+5);

    widget = labels.at(1);
    while( widget->count() > 4)
    {
        QListWidgetItem *item = widget->takeItem(widget->count()-1);
        delete item;
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

bool contoursManagementToolBox::loadDataAsContours(medAbstractImageView *v, unsigned int layer)
{
    bool retVal = false;
    medAbstractData *data = v->layerData(layer);
    if (data->identifier().contains("medContours") && v==currentView)
    {
        medContours *contours = dynamic_cast<medContours*>(data);
        QVector<medTagContours> &tagContoursSet = contours->getTagContoursSet();
        QListWidget *widget = labels.at(specialities->currentIndex());
        for (medTagContours &tagContours : tagContoursSet)
        {
            if (widget->count()<colors.size())
            {
                QColor color = findAvailableColor(widget);
                bool itemFound = false;
                for (int row = 0; row < widget->count(); row++)
                {
                    QListWidgetItem *item = widget->item(row);
                    if (item->text()==tagContours.getLabelName())
                    {
                        itemFound = true;
                        color = item->icon().pixmap(QSize(20,20)).toImage().pixelColor(0,0);
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
                }
            }
            else
            {
                displayMessageError("Too many label already imported");
            }
        }

        emit repulsorState();

        v->removeLayer(layer);
        retVal = true;
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

    widget->show();
    plusButton->show();
    minusButton->show();
}

void contoursManagementToolBox::updateLabelNamesOnContours(QListWidget *widget)
{
    QList<medContourSharedInfo> infos;
    for (int row = 0; row < widget->count(); row++)
    {
        QListWidgetItem *item = widget->item(row);
        QColor col = item->icon().pixmap(QSize(20,20)).toImage().pixelColor(0,0);
        medContourSharedInfo info = medContourSharedInfo(item->text(), col);
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
        if (widget->selectedItems().size()==0)
        {
            QString name = QString();
            QColor color = QColor::Invalid;
            medContourSharedInfo info = medContourSharedInfo(name, color);
            emit sendContourState(info);
        }
    }, Qt::UniqueConnection);

    widget->setContentsMargins(0,0,0,0);

    int idx = 0;
    for (QString name : names)
    {
        if (colors.size()<=idx)
        {
            displayMessageError("Unable to add new contour in list : No more color available");
            break;
        }
        QColor col = colors.at(idx);
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
        bool isProstate = (spe.last()=="prostate")?true:false;
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
    if ( widget->count()==colors.size())
    {
        displayMessageError("Unable to create more label. Maximum size achieved");
        return;
    }
    QColor color = findAvailableColor(widget);
    QListWidgetItem *item = createWidgetItem(QString("Label-%1").arg(QString::number(widget->count())),
                                             color);
    widget->addItem(item);
    item->setSelected(true);
    widget->setMaximumHeight((20*widget->count())+5);

    emit widget->itemClicked(item);
}

void contoursManagementToolBox::removeLabelNameInList()
{
    QListWidget *widget = labels.at(specialities->currentIndex());
    for (int row = 0; row<widget->count(); row++)
    {
        QListWidgetItem *item = widget->item(row);
        if (item->isSelected())
        {
            QString name = item->text();
            QColor col = item->icon().pixmap(QSize(20,20)).toImage().pixelColor(0,0);
            medContourSharedInfo info = medContourSharedInfo(item->text(), col);
            emit labelToDelete(info);
            widget->takeItem(row);
        }
    }

    if (widget->count()==0)
    {
        medContourSharedInfo info = medContourSharedInfo(QString(), QColor::Invalid);
        emit sendContourState(info);
    }
    else
    {
        widget->setMaximumHeight((20*widget->count())+5);
    }
}

QColor contoursManagementToolBox::findAvailableColor(QListWidget *widget)
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
        if ( info.nameUpdated() && itemColor==info.getColor())
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

#include "contoursManagementToolBox.h"
#include "polygonRoiToolBox.h"

#include <medAbstractProcessLegacy.h>
#include <medContours.h>
#include <medTabbedViewContainers.h>
#include <medToolBoxFactory.h>

contoursManagementToolBox::contoursManagementToolBox(QWidget *parent):
    medToolBox(parent), currentView(nullptr)
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

    QStringList spe;
    spe << "default" << "prostate";
    specialities->addItems(spe);

    // The lines below are temp code
    // Will be replaced soon by values parsed in json configuration file

    QStringList defaultList;
    defaultList << "Label-0";
    QListWidget *defaultNameList = initLabelsList(defaultList);
    defaultNameList->hide();
    labels.append(defaultNameList);

    QStringList prostateList;
    prostateList << "WG (whole gland)" << "TZ (transitional zone)" << "Target 1";
    QListWidget *prostateNameList = initLabelsList(prostateList);
    prostateNameList->hide();
    labels.append(prostateNameList);
    // End of temp code

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
    specialities->setCurrentIndex(0);
    for (QListWidget *widget : labels)
    {
        widget->clear();
        widget->hide();
    }
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
    QList<medContourInfo> infos;
    for (int row = 0; row < widget->count(); row++)
    {
        QListWidgetItem *item = widget->item(row);
        QColor col = item->icon().pixmap(QSize(20,20)).toImage().pixelColor(0,0);
        medContourInfo info = medContourInfo(item->text(), col);
        infos.append(info);
    }

    emit sendDatasToView(infos);
}

QListWidget *contoursManagementToolBox::initLabelsList(QStringList names, bool isProstate)
{
    QListWidget *widget = new QListWidget;
    connect(widget, &QListWidget::itemClicked, [this, widget](QListWidgetItem *item){
        widget->setStyleSheet("QListView::item:selected { background: palette(Highlight) }");
        if (currentView)
        {
           currentView->viewWidget()->setFocus();
        }
        QColor color = item->icon().pixmap(QSize(20,20)).toImage().pixelColor(0,0);
        QString name = item->text();
        bool score = (item->checkState()==Qt::Checked)?true:false;
        qDebug()<<"select contour "<<name;
        if (!score)
        {
            item->setForeground(Qt::white);
        }
        medContourInfo info = medContourInfo(name, color);
        info.setScore(score);
        emit sendContourState(info);
    });
    connect(widget, &QListWidget::itemChanged, [this](QListWidgetItem *item){
        if (currentView)
        {
            currentView->viewWidget()->setFocus();
        }
        QColor color = item->icon().pixmap(QSize(20,20)).toImage().pixelColor(0,0);
        QString name = item->text();
        medContourInfo info = medContourInfo(name, color);
        emit sendContourName(info);
    });

    connect(widget->model(), &QAbstractItemModel::rowsMoved, [this, widget](const QModelIndex &, int, int, const QModelIndex &, int){
//        QListWidgetItem *item = structuresWidget->currentItem();
//        if ( viewEventFilter )
//        {
//            if (currentView)
//            {
//                currentView->viewWidget()->setFocus();
//            }
//            QColor color = item->icon().pixmap(QSize(20,20)).toImage().pixelColor(0,0);
//            QString name = item->text();
//            if ( viewEventFilter)
//            {
//                viewEventFilter->updateContourState(name, color);
//            }
//        }
    });
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
        idx++;
        QListWidgetItem *item = createWidgetItem(name,
                                                 col);

        widget->addItem(item);
        if (isProstate && col!=Qt::green && col!=Qt::blue)
        {
            item->setCheckState(Qt::Checked);
        }
    }
    widget->setDragDropMode(QAbstractItemView::InternalMove);
    widget->setDragEnabled(true);
    widget->setAcceptDrops(true);
    widget->setDropIndicatorShown(true);
    widget->setMaximumHeight((20*widget->count())+5);
    widget->hide();
    return widget;
}

QListWidgetItem * contoursManagementToolBox::createWidgetItem(QString name, QColor col)
{
    QPixmap pixmap(20,20);
    pixmap.fill(col);
    QListWidgetItem *item =  new QListWidgetItem(pixmap, name);
    item->setSizeHint(QSize(20,20));
    QFont font;
    font.setBold(true);
    item->setFont(font);
    item->setFlags(item->flags() | Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsUserCheckable);
    item->setCheckState(Qt::Unchecked);

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
            medContourInfo info = medContourInfo(item->text(), col);
            emit labelToDelete(info);
            widget->takeItem(row);
        }
    }

    if (widget->count()==0)
    {
        medContourInfo info = medContourInfo(QString(), QColor::Invalid);
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

void contoursManagementToolBox::receiveContoursDatasFromView(medContourInfo& info)
{
    QListWidget *widget = labels.at(specialities->currentIndex());
    for (int row = 0; row < widget->count(); row++)
    {
        QListWidgetItem *item = widget->item(row);
        QString itemName = item->text();
        QColor itemColor = item->icon().pixmap(QSize(20,20)).toImage().pixelColor(0,0);
        if ( info.nameUpdated() && itemColor==info.getColor())
        {
            item->setText(info.getName());
            item->setSelected(true);
        }
        else if (itemName==info.getName() && itemColor==info.getColor())
        {
            item->setSelected(info.isSelected());
            if (info.scoreState())
            {
                QColor secondColor = info.getAdditionalColor();
                QColor color = (item->foreground().color()==secondColor)?Qt::white:secondColor;
                item->setForeground(color);
                if ( item->foreground().color()==secondColor )
                {
                    QString style = "QListView::item:selected { background: rgb(%1, %2, %3); }";
                    widget->setStyleSheet(style.arg(secondColor.red()).arg(secondColor.green()).arg(secondColor.blue()));
                }
                else
                {
                    widget->setStyleSheet("QListView::item:selected { background: palette(Highlight) }");
                }
            }
        }
        else
        {
            item->setSelected(false);
        }
    }
}

#include "defaultLabelToolBox.h"

#include <medDataManager.h>
#include <medSettingsManager.h>
#include <medTabbedViewContainers.h>
#include <medToolBoxFactory.h>
#include <medUtilities.h>
#include <defaultViewEvent.h>


defaultLabelToolBox::defaultLabelToolBox(QWidget *parent):
        medToolBox(parent),  currentViewEvent(nullptr)
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

    auto widget = new QWidget();
    auto layout = new QVBoxLayout;
    widget->setLayout(layout);

    dataName = new QLabel("No Data");
    dataName->setWordWrap(true);
    dataName->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Preferred);

    labels = new QListWidget;
    labels->setSelectionMode(QAbstractItemView::SingleSelection);
    labels->setContentsMargins(0,0,0,0);
    labels->setObjectName("labels");

    // Themes
    QVariant themeChosen = medSettingsManager::instance().value("startup","theme");
    int themeIndex = themeChosen.toInt();
    QIcon labelIconPlus;
    QIcon labelIconMinus;
    switch (themeIndex)
    {
        case 0:
        case 1:
        case 2:
        case 4:
        default:
        {
            labelIconPlus  = QIcon(":/pixmaps/plus_white.png");
            labelIconMinus = QIcon(":/pixmaps/minus_white.png");
            break;
        }
        case 3:
        {
            labelIconPlus  = QIcon(":/pixmaps/plus_black.png");
            labelIconMinus = QIcon(":/pixmaps/minus_black.png");
            break;
        }
    }

    plusButton = new QPushButton(labelIconPlus, "");
    plusButton->setMaximumSize(QSize(20,20));
    plusButton->setObjectName("plusBttn");

    minusButton = new QPushButton(labelIconMinus, "");
    minusButton->setMaximumSize(QSize(20,20));
    minusButton->setObjectName("minBttn");

    auto listLabelLayout = new QVBoxLayout();
    layout->addLayout(listLabelLayout);
    listLabelLayout->addWidget(dataName);
    listLabelLayout->addWidget(labels);

    auto plusMinusLayout = new QHBoxLayout;
    plusMinusLayout->addWidget(minusButton, Qt::AlignLeft);
    plusMinusLayout->addWidget(plusButton, Qt::AlignLeft);
    plusMinusLayout->addStretch();
    listLabelLayout->addLayout(plusMinusLayout);

    this->addWidget(widget);

    connect(plusButton, SIGNAL(pressed()), this, SLOT(onPlusClicked()));
    connect(minusButton, SIGNAL(pressed()), this, SLOT(onMinusClicked()));
}

void defaultLabelToolBox::initialize(QString &seriesName)
{
    connect(labels, &QListWidget::itemClicked, labels, [this](QListWidgetItem *item){
        item->setSelected(true);
        QColor color = item->icon().pixmap(QSize(20,20)).toImage().pixelColor(0,0);
        QString name = item->text();
        int position = labels->row(item);

        if (currentViewEvent)
        {
            currentViewEvent->updateContourProperty(name, color, position, true, false, false);
        }
        }, Qt::UniqueConnection);

    connect(labels, &QListWidget::itemChanged, labels, [this](QListWidgetItem *item){
        QString name = item->text();
        if (currentViewEvent)
        {
            auto event = dynamic_cast<defaultViewEvent *>(currentViewEvent);
            event->rename(labels->row(item), name);
        }
    }, Qt::UniqueConnection);

    clear();
    dataName->setText(seriesName);
    QListWidgetItem *item = createWidgetItem("Label 0",
                                             mainColors.at(0));

    addDefaultItem(item);

    emit labels->itemClicked(labels->item(0));

}

void defaultLabelToolBox::addDefaultItem(QListWidgetItem *item)
{
    labels->addItem(item);

    item->setFlags(item->flags() | Qt::ItemIsSelectable | Qt::ItemIsEditable);
    item->setFlags(item->flags() & (~Qt::ItemIsUserCheckable));

    labels->setMaximumHeight((20 * labels->count()) + 5);
}

defaultLabelToolBox::~defaultLabelToolBox()
{
    delete labels;
    currentViewEvent = nullptr;
}

bool defaultLabelToolBox::registered()
{
    return medToolBoxFactory::instance()->registerToolBox<defaultLabelToolBox>();
}

void defaultLabelToolBox::clear()
{
    dataName->setText("No Data");
    while( labels->count() > 0)
    {
        QListWidgetItem *item = labels->takeItem(labels->count()-1);
        delete item;
    }
    labels->clear();

}

void defaultLabelToolBox::clean()
{
    while( labels->count() > 1)
    {
        QListWidgetItem *item = labels->takeItem(labels->count()-1);
        delete item;
    }
    labels->item(0)->setText("Label 0");
    labels->setMaximumHeight((20*labels->count())+5);
}

int defaultLabelToolBox::getContourPositionAndColor(QString &name, QColor &color)
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
                break;
            }
        }
    }
    else
    {
        color = findAvailableColor(mainColors);
        QListWidgetItem *item = createWidgetItem(name,
                                                 color);
        addDefaultItem(item);
        position = labels->row(item);
    }
    return position;
}

QStringList defaultLabelToolBox::getPreLoadNames() const
{
    QStringList preLoadName;
    for (int row = 0; row < labels->count(); row++)
    {
        QListWidgetItem *item = labels->item(row);
        preLoadName << item->text();
    }
    return preLoadName;
}

QListWidgetItem * defaultLabelToolBox::createWidgetItem(const QString &name, const QColor &col)
{
    QPixmap pixmap(20,20);
    pixmap.fill(col);
    auto item =  new QListWidgetItem(pixmap, name);
    item->setSizeHint(QSize(20,20));
    QFont font;
    font.setBold(true);
    item->setFont(font);

    return item;
}

QColor defaultLabelToolBox::findAvailableColor(QList<QColor> &colors)
{
    QColor color;
    QList<QColor> cols = colors;
    for (int row = 0; row < labels->count(); row++)
    {
        QListWidgetItem *item = labels->item(row);
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

void defaultLabelToolBox::selectRow(int position)
{
    QListWidgetItem *item = labels->item(position);
    if (item)
    {
        item->setSelected(true);
    }
}

void defaultLabelToolBox::resetItem(int position, QString &defaultName)
{
    QListWidgetItem *item = labels->item(position);
    if (item)
    {
        item->setSelected(false);
        item->setText(defaultName);
        if (item->flags().testFlag(Qt::ItemIsUserCheckable))
        {
            item->setCheckState(Qt::Unchecked);
        }
    }
}

void defaultLabelToolBox::updateItem(medLabelProperty &info)
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

    item->setFlags(item->flags() | Qt::ItemIsSelectable | Qt::ItemIsEditable);
    item->setFlags(item->flags() & (~Qt::ItemIsUserCheckable));
}

void defaultLabelToolBox::rename(int row, QString &newName)
{
    QListWidgetItem *item = labels->item(row);
    if (item)
    {
        item->setText(newName);
    }
}

void defaultLabelToolBox::onPlusClicked()
{
    int maxSize = mainColors.size();
    if ( labels->count()==maxSize)
    {
        displayMessageError("Unable to create more label. Maximum size achieved");
        return;
    }

    int count = 0;
    for (int row=0; row<labels->count(); row++)
    {
        if (labels->item(row)->text().startsWith("Label"))
        {
            count++;
        }
    }
    QString name = QString("Label %1").arg(QString::number(count));
    QColor color = findAvailableColor(mainColors);
    if (color == QColor::Invalid)
    {
        displayMessageError(QString("Unable to create %1 : No Color available.").arg(name));
        return;
    }
    QListWidgetItem *item = createWidgetItem(name,
                            color);

    addDefaultItem(item);
    item->setSelected(true);

    emit labels->itemClicked(item);
}

void defaultLabelToolBox::onMinusClicked()
{
    QList<QListWidgetItem *> items = labels->selectedItems();
    if (items.size()==1)
    {
        QListWidgetItem *item = items.at(0);
        int row = labels->row(item);
        currentViewEvent->deleteLabel(row);
        if (row > 0)
        {
            labels->takeItem(row);
            updatePolygonLabelPosition(row);
        }
    }
    labels->setMaximumHeight((20*labels->count())+5);
}

void defaultLabelToolBox::updatePolygonLabelPosition(int startPosition)
{
    if (startPosition < labels->count())
    {
        for (int row = startPosition; row < labels->count(); row++)
        {
            QListWidgetItem *item1 = labels->item(row);
            QString name = item1->text();
            name = name.remove(QRegExp(" - Z[T|P]{1}_PIRADS[0-9]"));
            currentViewEvent->updatePosition(name, row);
        }
    }
}

void defaultLabelToolBox::setName(QString &name)
{
    dataName->setText(name);
}

void defaultLabelToolBox::unselectAll()
{
    for (int row = 0; row < labels->count(); row++)
    {
        QListWidgetItem *item = labels->item(row);
        item->setSelected(false);
    }
}

void defaultLabelToolBox::forceItemSelection()
{
    int selectedItem = -1;
    for (int iRow=0; iRow <labels->count(); iRow++)
    {
        QListWidgetItem *item = labels->item(iRow);
        if (item->isSelected())
        {
            selectedItem = iRow;
        }
    }
    if (selectedItem!=-1)
    {
        emit labels->itemClicked(labels->item(selectedItem));
    }
}




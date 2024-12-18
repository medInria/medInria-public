#include "medDataInfoWidget.h"

#include <QPushButton>
#include <QTreeWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <QHeaderView>
#include <QDebug>

medDataInfoWidget::medDataInfoWidget(QMap<QString, QString> dataAttributes, QMultiMap<QString, QString> optionalAttributes, QWidget *parent) : QDialog(parent)
{
    QVBoxLayout *dialogLayout = new QVBoxLayout;
    setLayout(dialogLayout);
    setModal(true);

    this->setWindowTitle("Metadata List");

    QLabel* textTitle = new QLabel(tr("\nNon-empty metadata from the selected data\n"));
    textTitle->setStyleSheet("font-weight: bold;");
    dialogLayout->addWidget(textTitle);

    QLabel* explanation = new QLabel(tr("You can copy/paste a selected item with regular shortcuts from your OS.\n"));
    dialogLayout->addWidget(explanation);

    QPushButton *plusButton = new QPushButton(QIcon::fromTheme("plus"), "");
    plusButton->setToolTip("Show Optional Meta Data");
    plusButton->setMaximumSize(QSize(20,20));
    plusButton->setCheckable(true);
    plusButton->setFlat(true);
    plusButton->setDisabled(optionalAttributes.isEmpty());


    QHBoxLayout *hLayout = new QHBoxLayout;
    hLayout->addWidget(explanation);
    hLayout->addWidget(plusButton, 0, Qt::AlignRight);

    dialogLayout->addLayout(hLayout);

    auto tree = new QTreeWidget();
    tree->setFrameStyle(QFrame::NoFrame);
    tree->setAttribute(Qt::WA_MacShowFocusRect, false);
    tree->setUniformRowHeights(true);
    tree->setAlternatingRowColors(true);
    tree->setAnimated(true);
    tree->setSelectionBehavior(QAbstractItemView::SelectRows);
    tree->setSelectionMode(QAbstractItemView::SingleSelection);
    tree->header()->setStretchLastSection(true);
    tree->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    tree->setColumnCount(2);

    auto optionalTree = new QTreeWidget();
    optionalTree->setFrameStyle(QFrame::NoFrame);
    optionalTree->setAttribute(Qt::WA_MacShowFocusRect, false);
    optionalTree->setUniformRowHeights(true);
    optionalTree->setAlternatingRowColors(true);
    optionalTree->setAnimated(true);
    optionalTree->setSelectionBehavior(QAbstractItemView::SelectRows);
    optionalTree->setSelectionMode(QAbstractItemView::SingleSelection);
    optionalTree->header()->setStretchLastSection(true);
    optionalTree->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    
    bool isTag = false;
    for (auto key : optionalAttributes.uniqueKeys())
    {
        QList<QString> valuesAndTags = optionalAttributes.values(key);
        if (valuesAndTags.size()>1)
        {
            isTag = true;
            break;
        }
    }
    if (isTag)
    {
        optionalTree->setColumnCount(3);
    }    
    else{
        optionalTree->setColumnCount(2);
    }
    optionalTree->setHidden(true);

    connect(plusButton, &QPushButton::toggled, [=](bool checked) {
        if (checked)
        {
            plusButton->setIcon(QIcon::fromTheme("minus"));
            optionalTree->setHidden(false);
        }
        else
        {
            plusButton->setIcon(QIcon::fromTheme("plus"));
            optionalTree->setHidden(true);
        }
    });

    // Create tree columns
    QStringList treeColumns;
    treeColumns << tr("Key") << tr("Value");
    tree->setHeaderLabels(treeColumns);
    tree->setColumnWidth(0, 200);
    tree->setColumnWidth(1, 400);

    dialogLayout->addWidget(tree);

    // Populate the mandatory metadata tree
    for (QString key : dataAttributes.keys())
    {
        // Only keep non empty metadata values
        if (!dataAttributes[key].isEmpty())
        {
            QTreeWidgetItem * item = new QTreeWidgetItem(tree);
            item->setText(0, key);
            item->setText(1, dataAttributes[key]);
            tree->addTopLevelItem(item);
        }
    }
    tree->setSortingEnabled(true);
    tree->sortByColumn(0, Qt::AscendingOrder);

    QStringList optionalColumns;
    optionalColumns << tr("Key") << tr("Value");
    optionalTree->setColumnWidth(0, 200);
    optionalTree->setColumnWidth(1, 300);
    if (isTag)
    {
        optionalColumns << tr("Tags");
        optionalTree->setColumnWidth(2, 300);
     } 
    optionalTree->setHeaderLabels(optionalColumns);

    dialogLayout->addWidget(optionalTree);
    this->resize(800, 700);

    for (QString key: optionalAttributes.uniqueKeys())
    {
        QList<QString> values = optionalAttributes.values(key);
        if (!values.isEmpty())
        {
            QTreeWidgetItem * item = new QTreeWidgetItem(optionalTree);
            item->setText(0, key);
            for (int i = 0; i<values.size(); ++i)
            {
                item->setText(i+1, values[i]);
            }
            optionalTree->addTopLevelItem(item);
        }
    }
    optionalTree->setSortingEnabled(true);
    optionalTree->sortByColumn(0, Qt::AscendingOrder);

}

medDataInfoWidget::~medDataInfoWidget()
{
}

#include "medDataInfoWidget.h"

#include <QTreeWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <QHeaderView>

medDataInfoWidget::medDataInfoWidget(QMap<QString, QString> dataAttributes, QWidget *parent) : QDialog(parent)
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

    // Create tree columns
    QStringList treeColumns;
    treeColumns << tr("Key") << tr("Value");
    tree->setHeaderLabels(treeColumns);
    tree->setColumnWidth(0, 200);
    tree->setColumnWidth(1, 400);
    dialogLayout->addWidget(tree);
    this->resize(700, 700);

    // Populate the tree
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
}

medDataInfoWidget::~medDataInfoWidget()
{
}

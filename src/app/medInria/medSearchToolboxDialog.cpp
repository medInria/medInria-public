/*=========================================================================

medInria

Copyright (c) INRIA 2013 - 2019. All rights reserved.
See LICENSE.txt for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.

=========================================================================*/
#include <medSearchToolboxDialog.h>

#include <QHeaderView>
#include <QLabel>
#include <QLineEdit>
#include <QVBoxLayout>

medSearchToolboxDialog::medSearchToolboxDialog(QWidget *parent, QHash<QString, QStringList> toolboxDataHash)
    : QDialog(parent)
{
    findText.clear();
    toolboxDataHash = toolboxDataHash;

    QVBoxLayout *layout = new QVBoxLayout;
    setLayout(layout);

    setWindowTitle(tr("Find a Toolbox"));

    // Search Bar
    QHBoxLayout *findLayout = new QHBoxLayout;

    QLabel *findLabel = new QLabel(tr("Enter the name of a toolbox, or a tag"));
    findLayout->addWidget(findLabel);

    lineEdit = new QLineEdit;
    findLayout->addWidget(lineEdit);
    connect(lineEdit, SIGNAL(textChanged(const QString&)), this, SLOT(searchTextChanged(const QString&)));

    layout->addLayout(findLayout);

    // Propositions tree
    QLabel *propositionLabel = new QLabel(tr("Searching list (double-click on a toolbox to open it)"));
    layout->addWidget(propositionLabel);

    tree = new QTreeWidget();
    tree->setFrameStyle(QFrame::NoFrame);
    tree->setAttribute(Qt::WA_MacShowFocusRect, false);
    tree->setUniformRowHeights(true);
    tree->setAlternatingRowColors(true);
    tree->setAnimated(true);
    tree->setSelectionBehavior(QAbstractItemView::SelectRows);
    tree->setSelectionMode(QAbstractItemView::SingleSelection);
    tree->header()->setStretchLastSection(true);
    tree->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    tree->setColumnCount(4);
    layout->addWidget(tree);

    // Create tree columns
    QStringList treeColumns;
    treeColumns << tr("Name") << tr("Description") << tr("Workspace") << tr("Internal Name");
    tree->setHeaderLabels(treeColumns);
    tree->setColumnWidth(0, 200);
    tree->setColumnWidth(1, 400);
    tree->setColumnWidth(2, 200);
    tree->setColumnWidth(3, 100);
    layout->addWidget(tree);

    // Populate the tree
    int cpt = 0;
    foreach (auto current, toolboxDataHash)
    {
        QTreeWidgetItem * item = new QTreeWidgetItem(tree);
        item->setText(0, current.at(0));
        item->setText(1, current.at(1));
        item->setText(2, current.at(2));
        item->setText(3, current.at(3));
        tree->addTopLevelItem(item);
        cpt++;
    }

    tree->setSortingEnabled(true);
    tree->sortByColumn(0, Qt::AscendingOrder);
    this->adjustSize();
    tree->setMinimumSize(700,600);

    connect(tree, SIGNAL(itemDoubleClicked(QTreeWidgetItem*, int)), this, SLOT(searchItemDoubleClicked(QTreeWidgetItem*, int)));
}

void medSearchToolboxDialog::searchTextChanged(const QString& text)
{
    // List of items which match the search text
    QList<QTreeWidgetItem*> clist = tree->findItems(text, Qt::MatchContains|Qt::MatchRecursive, 0);

    // First we hide every item from the list
    QList<QTreeWidgetItem *> allItems = tree->findItems(
                QString("*"), Qt::MatchWrap | Qt::MatchWildcard | Qt::MatchRecursive);
    foreach(QTreeWidgetItem* item, allItems)
    {
        item->setHidden(true);
    }

    // Then we show only matching items
    foreach(QTreeWidgetItem* item, clist)
    {
        item->setHidden(false);
    }
}

void medSearchToolboxDialog::searchItemDoubleClicked(QTreeWidgetItem* item, int column)
{
    Q_UNUSED(column);
    QStringList res;
    res.append(item->text(0));
    res.append(item->text(1));
    res.append(item->text(2));
    res.append(item->text(3));
    findText = res;
    accept();
}

QStringList medSearchToolboxDialog::getFindText()
{
    return findText;
}

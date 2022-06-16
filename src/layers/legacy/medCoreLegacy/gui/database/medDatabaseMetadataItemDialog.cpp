/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2019. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medAbstractDatabaseItem.h>
#include <medDatabaseMetadataItemDialog.h>
#include <medDataIndex.h>

#include <QHBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QTreeWidget>

class medDatabaseMetadataItemDialogPrivate
{
public:
    QTreeWidget* tree;
};

medDatabaseMetadataItemDialog::medDatabaseMetadataItemDialog(QList<QString> keyList, QList<QVariant> metadataList, QWidget *parent):
    QDialog(parent, Qt::Dialog | Qt::WindowCloseButtonHint), d (new medDatabaseMetadataItemDialogPrivate)
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

    d->tree = new QTreeWidget();
    d->tree->setFrameStyle(QFrame::NoFrame);
    d->tree->setAttribute(Qt::WA_MacShowFocusRect, false);
    d->tree->setUniformRowHeights(true);
    d->tree->setAlternatingRowColors(true);
    d->tree->setAnimated(true);
    d->tree->setSelectionBehavior(QAbstractItemView::SelectRows);
    d->tree->setSelectionMode(QAbstractItemView::SingleSelection);
    d->tree->header()->setStretchLastSection(true);
    d->tree->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    d->tree->setColumnCount(2);

    // Create tree columns
    QStringList treeColumns;
    treeColumns << tr("Key") << tr("Value");
    d->tree->setHeaderLabels(treeColumns);
    d->tree->setColumnWidth(0, 200);
    d->tree->setColumnWidth(1, 400);
    dialogLayout->addWidget(d->tree);
    this->resize(700, 700);

    // Populate the tree
    int cpt = 0;
    for (QString &key : keyList)
    {
        // Only keep non empty metadata values
        if (!metadataList.at(cpt).toString().isEmpty())
        {
            QTreeWidgetItem * item = new QTreeWidgetItem(d->tree);
            item->setText(0, key);
            item->setText(1, metadataList.at(cpt).toString());
            d->tree->addTopLevelItem(item);
        }
        cpt++;
    }

    d->tree->setSortingEnabled(true);
    d->tree->sortByColumn(0, Qt::AscendingOrder);
}

medDatabaseMetadataItemDialog::~medDatabaseMetadataItemDialog()
{
    delete d;
}

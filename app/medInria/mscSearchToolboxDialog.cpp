#include <QtGui>
#include "mscSearchToolboxDialog.h"

class mscSearchToolboxDialogPrivate
{
public:
    QPushButton *findButton;
    QLineEdit *lineEdit;
    QStringList findText;
    QTreeWidget* tree;
    QHash<QString, QStringList> toolboxDataHash;
};

mscSearchToolboxDialog::mscSearchToolboxDialog(QWidget *parent, QHash<QString, QStringList> toolboxDataHash)
    : QDialog(parent), d (new mscSearchToolboxDialogPrivate)
{
    d->findText.clear();
    d->toolboxDataHash = toolboxDataHash;

    QVBoxLayout *layout = new QVBoxLayout;
    setLayout(layout);

    setWindowTitle(tr("Find a Toolbox"));

    // Search Bar
    QHBoxLayout *findLayout = new QHBoxLayout;

    QLabel *findLabel = new QLabel(tr("Enter the name of a toolbox, or a tag"));
    findLayout->addWidget(findLabel);

    d->lineEdit = new QLineEdit;
    findLayout->addWidget(d->lineEdit);
    connect(d->lineEdit, SIGNAL(textChanged(const QString&)), this, SLOT(searchTextChanged(const QString&)));

    layout->addLayout(findLayout);

    // Propositions tree
    QLabel *propositionLabel = new QLabel(tr("Searching list (double-click on a toolbox to open it)"));
    layout->addWidget(propositionLabel);

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
    d->tree->setColumnCount(4);
    layout->addWidget(d->tree);

    // Create tree columns
    QStringList treeColumns;
    treeColumns << tr("Name") << tr("Description") << tr("Workspace") << tr("Internal Name");
    d->tree->setHeaderLabels(treeColumns);
    d->tree->setColumnWidth(0, 200);
    d->tree->setColumnWidth(1, 400);
    d->tree->setColumnWidth(2, 100);
    d->tree->setColumnWidth(3, 100);
    layout->addWidget(d->tree);

    // Populate the tree
    int cpt = 0;
    foreach (auto current, toolboxDataHash)
    {
        QTreeWidgetItem * item = new QTreeWidgetItem(d->tree);
        item->setText(0, current.at(0));
        item->setText(1, current.at(1));
        item->setText(2, current.at(2));
        item->setText(3, current.at(3));
        d->tree->addTopLevelItem(item);
        cpt++;
    }

    d->tree->setSortingEnabled(true);
    d->tree->sortByColumn(0, Qt::AscendingOrder);
    d->tree->setFixedSize(700,600);

    connect(d->tree, SIGNAL(itemDoubleClicked(QTreeWidgetItem*, int)), this, SLOT(searchItemDoubleClicked(QTreeWidgetItem*, int)));
}

mscSearchToolboxDialog::~mscSearchToolboxDialog()
{
    delete d;
}

void mscSearchToolboxDialog::searchTextChanged(const QString& text)
{
    // List of items which match the search text
    QList<QTreeWidgetItem*> clist = d->tree->findItems(text, Qt::MatchContains|Qt::MatchRecursive, 0);

    // First we hide every item from the list
    QList<QTreeWidgetItem *> allItems = d->tree->findItems(
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

void mscSearchToolboxDialog::searchItemDoubleClicked(QTreeWidgetItem* item, int column)
{
    Q_UNUSED(column);
    QStringList res;
    res.append(item->text(0));
    res.append(item->text(1));
    res.append(item->text(2));
    res.append(item->text(3));
    d->findText = res;
    accept();
}

QStringList mscSearchToolboxDialog::getFindText()
{
    return d->findText;
}

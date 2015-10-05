/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medDatabaseSearchPanel.h>

#include <QtGui>

class medDatabaseSearchPanelPrivate
{
public:

    QWidget *page;
    QComboBox *columnBox;
    QVBoxLayout *layout;
    QPushButton *addButton;
    QPushButton * removeButton;
    QVector<QWidget*> stack;
    QList<int> columnsInUse;
};

EditCombo::EditCombo( QString txt, int clmn )
{
    this->label.setText(txt);
    this->column = clmn;

    QHBoxLayout* layout = new QHBoxLayout(this);
    layout->addWidget(&edit);
    layout->addWidget(&label);

    connect(&edit, SIGNAL(textChanged(const QString &)), this, SLOT(onTextChanged(const QString &)));
}

void EditCombo::onTextChanged( const QString& text)
{
    emit textChanged(text, this->column);
}

EditCombo::~EditCombo()
{
    emit textChanged("", this->column);
}


medDatabaseSearchPanel::medDatabaseSearchPanel( QWidget *parent /*= 0*/ ) : medToolBox(parent), d(new medDatabaseSearchPanelPrivate)
{
    d->page = new QWidget(this);

    d->columnBox = new QComboBox();
	d->columnBox->setToolTip(tr("Search criteria"));

    d->addButton = new QPushButton("+");
    d->addButton->setFocusPolicy(Qt::NoFocus);
	d->addButton->setToolTip(tr("Add a search criterion"));
	
    d->removeButton = new QPushButton("-");
    d->removeButton->setFocusPolicy(Qt::NoFocus);
	d->removeButton->setToolTip(tr("Remove a search criterion"));

    QHBoxLayout *vlayout = new QHBoxLayout();
    vlayout->addWidget(d->columnBox);
    vlayout->addWidget(d->addButton);
    vlayout->addWidget(d->removeButton);

    d->layout = new QVBoxLayout();

    this->setTitle(tr("Filter"));

    QVBoxLayout *mainLayout = new QVBoxLayout();
    mainLayout->addLayout(vlayout);
    mainLayout->addLayout(d->layout);

    d->page->setLayout(mainLayout);
    this->addWidget(d->page);

    connect(d->addButton, SIGNAL(clicked()),this, SLOT(addBox()));
    connect(d->removeButton, SIGNAL(clicked()),this, SLOT(removeBox()));

}

medDatabaseSearchPanel::~medDatabaseSearchPanel()
{
    delete d;
    d = NULL;
}

void medDatabaseSearchPanel::setColumnNames( const QStringList &columns )
{
    if (columns.isEmpty())
        return;

    d->columnBox->clear();

    foreach(QString columnName, columns)
    {
        d->columnBox->addItem(columnName);
    }

    //d->columnBox->setCurrentIndex(2); // debug
    addBox(); // add patient name

}

void medDatabaseSearchPanel::addBox( )
{
    int column = d->columnBox->currentIndex();

    if (d->columnsInUse.contains(column))
        return;
    else
        d->columnsInUse.push_back(column);

    EditCombo* editCombo = new EditCombo(d->columnBox->currentText(), column);
    d->layout->addWidget(editCombo);

    connect(editCombo, SIGNAL(textChanged(const QString &, int)), this, SIGNAL( filter(const QString&, int)) );
    d->stack.push_back(editCombo);
}

void medDatabaseSearchPanel::removeBox()
{
    if (d->stack.isEmpty())
        return;

    d->layout->removeWidget(d->stack.at(d->stack.count()-1));
    delete d->stack.at(d->stack.count()-1);
    d->stack.pop_back();
    d->columnsInUse.pop_back();
}


#include "medDatabaseSearchPanel.h"

#include <QtGui>

class medDatabaseSearchPanelPrivate
{
public:
    QLineEdit *edit;
    QComboBox *columnBox;
};

medDatabaseSearchPanel::medDatabaseSearchPanel( QWidget *parent /*= 0*/ ) : medToolBox(parent), d(new medDatabaseSearchPanelPrivate)
{
    QWidget *page = new QWidget(this);

    d->edit = new QLineEdit(page);
    d->columnBox = new QComboBox(page);
    d->columnBox->addItem(tr("Patient name")); //default
    d->columnBox->addItem(tr("Study name"));

    QHBoxLayout *layout = new QHBoxLayout(page);
    layout->addWidget(d->edit);
    layout->addWidget(d->columnBox);

    this->setTitle(tr("Filter"));
    this->addWidget(page);

    connect(d->edit, SIGNAL(textChanged(const QString &)), this, SLOT(onSetFilter(const QString &)));
}

medDatabaseSearchPanel::~medDatabaseSearchPanel()
{

}

void medDatabaseSearchPanel::onSetFilter( const QString &text )
{
    emit filter(text, d->columnBox->currentIndex());
}

void medDatabaseSearchPanel::setColumnNames( const QStringList &columns )
{
    if (columns.isEmpty())
        return;

    // remove default
    d->columnBox->clear();

    foreach(QString columnName, columns)
    {
        d->columnBox->addItem(columnName);
    }
}

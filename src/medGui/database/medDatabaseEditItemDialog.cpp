
#include "medDatabaseEditItemDialog.h"

#include <medDataIndex.h>
#include <medAbstractDatabaseItem.h>

#include <QtGui>


class medDatabaseEditItemDialogPrivate
{
public:

    medAbstractDatabaseItem *item;

    QList<QVariant> itemDataCopy;
};



medDatabaseEditItemDialog::medDatabaseEditItemDialog(medAbstractDatabaseItem *item, QWidget *parent) : QDialog(parent,
                  Qt::Dialog | Qt::WindowCloseButtonHint), d (new medDatabaseEditItemDialogPrivate)
{
    QVBoxLayout *dialogLayout = new QVBoxLayout;
    QHBoxLayout *buttonLayout = new QHBoxLayout;
    QFormLayout *formLayout = new QFormLayout;

    QPushButton *okButton = new QPushButton(tr("OK"));
    QPushButton *cancelButton = new QPushButton(tr("Cancel"));
    buttonLayout->addWidget(okButton);
    buttonLayout->addWidget(cancelButton);

    dialogLayout->addLayout(formLayout);
    dialogLayout->addStretch();
    dialogLayout->addLayout(buttonLayout);

    d->item = item;
    d->itemDataCopy.reserve(item->columnCount());

    for(int i=0; i<item->columnCount(); i++)
    {
        QVariant attribute = item->attribute(i);

        QVariant data = item->data(i);

        d->itemDataCopy << data;

        if(attribute.toString().isEmpty())
            continue;

        switch(data.type())
        {
        case QVariant::String:
            QLineEdit *textEdit = new QLineEdit(this);
            //textEdit->setObjectName(attribute.toString());
            textEdit->setObjectName(QString::number(i));
            textEdit->setText(data.toString());
            formLayout->addRow(attribute.toString(), textEdit);
            connect(textEdit, SIGNAL(textChanged(const QString &)), this, SLOT(setValue(const QString &)));
            break;
        }
    }

    connect(cancelButton, SIGNAL(clicked()), this, SLOT(cancel()));
    connect(okButton, SIGNAL(clicked()), this, SLOT(validate()));

    setLayout(dialogLayout);
    setModal(true);
}

medDatabaseEditItemDialog::~medDatabaseEditItemDialog()
{
    delete d;
    d = NULL;
}


medAbstractDatabaseItem* medDatabaseEditItemDialog::item()
{
    return d->item;
}

medAbstractDatabaseItem* medDatabaseEditItemDialog::setItem(medAbstractDatabaseItem *item)
{
    d->item = item;
}

void medDatabaseEditItemDialog::setValue(const QString & text)
{
    QWidget *currentWidget = QApplication::focusWidget();
    QString objectName = currentWidget->objectName();
    if(!objectName.isEmpty())
    {
        int index = objectName.toInt();
        d->itemDataCopy[index] = QVariant(text);
    }
}

void medDatabaseEditItemDialog::cancel()
{
    this->reject();
}


void medDatabaseEditItemDialog::validate()
{
    for(int i=0; i<d->item->columnCount(); i++)
    {
        QVariant data = d->itemDataCopy[i];

        d->item->setData(i, data);
    }

    this->accept();
}



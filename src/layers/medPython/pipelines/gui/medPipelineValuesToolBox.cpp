#include "medPipelineValuesToolBox.h"

#include <medPluginManager.h>
#include <medToolBoxFactory.h>

namespace med::pipeline
{

class ValuesToolBoxPrivate
{
public:
    QTreeWidget* tree;
    QPushButton* exportToTxtFile;
};

bool ValuesToolBox::registered()
{
    return medToolBoxFactory::instance()->registerToolBox<ValuesToolBox>();
}

ValuesToolBox::ValuesToolBox(QWidget* parent) : medToolBox(parent), d(new ValuesToolBoxPrivate())
{
    QWidget* mainWidget = new QWidget();
    addWidget(mainWidget);

    QVBoxLayout* mainLayout = new QVBoxLayout();
    mainWidget->setLayout(mainLayout);

    d->tree = new QTreeWidget();
    d->tree->setFrameStyle(QFrame::NoFrame);
    d->tree->setAttribute(Qt::WA_MacShowFocusRect, false);
    d->tree->setUniformRowHeights(true);
    d->tree->setAlternatingRowColors(true);
    d->tree->setAnimated(true);
    d->tree->setSortingEnabled(true);
    d->tree->setSelectionBehavior(QAbstractItemView::SelectRows);
    d->tree->setSelectionMode(QAbstractItemView::SingleSelection);
    d->tree->header()->setStretchLastSection(true);
    d->tree->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
    d->tree->setColumnCount(2);

    QStringList treeColumns;
    treeColumns << tr("Variable") << tr("Value");
    d->tree->setHeaderLabels(treeColumns);
    d->tree->setColumnWidth(0, 200);
    mainLayout->addWidget(d->tree);

    d->exportToTxtFile = new QPushButton("Export to File");
    d->exportToTxtFile->setObjectName("exportToFile");
    d->exportToTxtFile->setToolTip("Export values to a text file");
    d->exportToTxtFile->setEnabled(false);
    mainLayout->addWidget(d->exportToTxtFile);
    connect(d->exportToTxtFile, SIGNAL(clicked()), this, SLOT(exportToFile()));
}

ValuesToolBox::~ValuesToolBox(void)
{
    delete d;
}

void ValuesToolBox::addValues(QHash<QString, QVariant> resultVariables)
{
    QHashIterator<QString, QVariant> i(resultVariables);
    while(i.hasNext())
    {
        i.next();
        QTreeWidgetItem * item = new QTreeWidgetItem(d->tree);
        item->setText(0,i.key());

        switch (static_cast<QMetaType::Type>(i.value().type()))
        {
        case QMetaType::Double:
            item->setText(1, QString::number(i.value().toDouble(), 'f', 2));
            break;
        case QMetaType::Float:
            item->setText(1, QString::number(i.value().toFloat(), 'f', 2));
            break;
        case QMetaType::Int:
            item->setText(1, QString::number(i.value().toInt()));
            break;
        case QMetaType::UInt:
            item->setText(1, QString::number(i.value().toUInt()));
            break;
        case QMetaType::Bool:
            item->setText(1, QString::number(i.value().toBool()));
            break;
        case QMetaType::Short:
            item->setText(1, QString::number(i.value().value<short>()));
            break;
        case QMetaType::UShort:
            item->setText(1, QString::number(i.value().value<unsigned short>()));
            break;
        case QMetaType::Long:
            item->setText(1, QString::number(i.value().value<long>()));
            break;
        case QMetaType::ULong:
            item->setText(1, QString::number(i.value().value<unsigned long>()));
            break;
        case QMetaType::LongLong:
            item->setText(1, QString::number(i.value().toLongLong()));
            break;
        case QMetaType::ULongLong:
            item->setText(1, QString::number(i.value().toULongLong()));
            break;
        case QMetaType::Char:
            item->setText(1, QString(i.value().toChar()));
            break;
        case QMetaType::UChar:
            item->setText(1, QString(i.value().value<unsigned char>()));
            break;
        default:
            item->setText(1, i.value().toString());
            break;
        }

        d->tree->addTopLevelItem(item);
    }

    if (resultVariables.count() > 0)
    {
        // At least one value, we can enable
        d->exportToTxtFile->setEnabled(true);
    }
}

void ValuesToolBox::exportToFile()
{
    QString fileName = QFileDialog::getSaveFileName(0, tr("Export To File"), QDir::home().absolutePath() + "/values.txt");

    if(fileName != "") // if not cancelled
    {
        QFile txtFile(fileName);

        if (txtFile.open(QIODevice::WriteOnly) )
        {
            for( int i = 0; i < d->tree->topLevelItemCount(); ++i )
            {
                QTreeWidgetItem *item = d->tree->topLevelItem( i );
                txtFile.write(item->text(0).toUtf8());
                txtFile.write(": ");
                txtFile.write(item->text(1).toUtf8());
                txtFile.write("\n");
            }

            txtFile.close();
        }
        else
        {
            displayMessageError(QString("Could not open file " + fileName));
        }
    }
}

} // namespace med::pipeline

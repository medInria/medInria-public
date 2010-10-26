#include "medSaveLUTDialog.h"
#include <QWidget>

medSaveLUTDialog::medSaveLUTDialog(const QString & name, QWidget *parent) :
    QInputDialog(parent)
{
    setModal(Qt::WindowModal);
    setInputMode(QInputDialog::TextInput);
    setLabelText(tr("Enter a name for this table, or keep existing one"));
    setTextValue(name);
    setOkButtonText(tr("Save"));
}

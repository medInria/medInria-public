#include "medLoadLUTDialog.h"

medLoadLUTDialog::medLoadLUTDialog(const QStringList & titles, QWidget *parent) :
    QInputDialog(parent)
{
    setModal(Qt::WindowModal);
    setInputMode(QInputDialog::TextInput);
    setLabelText(tr("Choose a transfert Function:"));
    setComboBoxItems(titles);;
    setOkButtonText(tr("Choose"));
}

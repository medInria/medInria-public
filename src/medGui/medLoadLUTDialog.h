#ifndef MEDLOADLUTDIALOG_H
#define MEDLOADLUTDIALOG_H

#include <QInputDialog>

class medLoadLUTDialog : public QInputDialog
{
    Q_OBJECT
public:
    medLoadLUTDialog(const QStringList & titles, QWidget *parent = 0);

signals:

public slots:

};

#endif // MEDLOADLUTDIALOG_H

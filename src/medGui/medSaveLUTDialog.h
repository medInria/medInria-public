#ifndef MEDSAVELUTDIALOG_H
#define MEDSAVELUTDIALOG_H

#include <QInputDialog>
#include <QWidget>

class medSaveLUTDialog : public QInputDialog
{
    Q_OBJECT
public:
    medSaveLUTDialog(const QString & name="Unknown",QWidget *parent = 0);

signals:

public slots:

};

#endif // MEDSAVELUTDIALOG_H

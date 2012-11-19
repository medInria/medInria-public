#ifndef MEDDATABASEEDITITEMDIALOG_H
#define MEDDATABASEEDITITEMDIALOG_H

#include <medCore/medDataIndex.h>
#include <QDialog>


class medAbstractDatabaseItem;

class medDatabaseEditItemDialogPrivate;


/**
 * @brief
 */
class medDatabaseEditItemDialog: public QDialog
{
    Q_OBJECT

public:

    medDatabaseEditItemDialog(medAbstractDatabaseItem *item, QWidget *parent);
    virtual ~medDatabaseEditItemDialog();
    medAbstractDatabaseItem* item();
    medAbstractDatabaseItem* setItem(medAbstractDatabaseItem *item);

public slots:
    void setValue(const QString &text);
    void cancel();
    void validate();


private:

    medDatabaseEditItemDialogPrivate *d;
};







































#endif // MEDSAVEMODIFIEDDIALOG_H







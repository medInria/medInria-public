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
    medDatabaseEditItemDialog(QList<QString> attributes, QList<QVariant> data, QWidget *parent, 
         bool displayPersistency=false,  bool persistent=false);

    virtual ~medDatabaseEditItemDialog();

    QVariant value(QString attribute);
    
    bool isPersistent();

public slots:
    
    void setValue(const QString &text);
    
    void cancel();
    
    void validate();


private:

    medDatabaseEditItemDialogPrivate *d;
};







































#endif // MEDSAVEMODIFIEDDIALOG_H







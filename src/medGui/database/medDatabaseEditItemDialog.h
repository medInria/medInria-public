#pragma once

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
    
    void setValue(const QString &value);
    void setCharValue(const QString &value);
    void setValue(const int &value);
    void setValue(const QDate &value);
    
    void cancel();
    
    void validate();


private:

    medDatabaseEditItemDialogPrivate *d;
};



/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medCoreExport.h>
#include <QtGui>

#include <medListWidget.h>
#include <medCompositeParameter.h>


class medMeshVisibilityMenuPrivate;
class medLabelsVisibilityParameter;

class MEDCORE_EXPORT medMeshVisibilityMenu : public QPushButton
{
    Q_OBJECT

public:
    medMeshVisibilityMenu(QWidget * parent = 0);
    virtual ~medMeshVisibilityMenu();
    void setAvailableLabels(QStringList parameters);
    void setLabels(QList<double*> * listValues);
    void setAnatomicalLabelsForMesh(QList<QPair<QString,int> > );
    QList<QPair<QString,int> > getAllAnatomicalLabels();
    QList<QPair<QString,int> > getAnatomicalLabelsForMesh();

    medLabelsVisibilityParameter * getLabelsParameter();

signals:
    void labelChecked(int);
    void labelUnchecked(int);

private slots:
    void showPopup();
    //void showSubMenu(QListWidgetItem *);
    //void showSubMenu();
    void hideSubMenu();
    void emitLabelUnchecked(QString label);
    void emitLabelChecked(QString label);

protected:
    bool eventFilter(QObject *object, QEvent *event);
    void paintEvent(QPaintEvent *);

private:
    void updateListsPosition();
    void hideMenus();
    void checkAllLabels(bool check);

private:
    medMeshVisibilityMenuPrivate *d;
};

class MEDCORE_EXPORT medLabelsVisibilityParameter : public medCompositeParameter
{
     Q_OBJECT

public:
    medLabelsVisibilityParameter(QString name = "unknown labels visibility parameter", QObject* parent = 0):
        medCompositeParameter(name, parent)
    {
        listWidget=new medListWidget();
        listWidget->setMaximumHeight(300);
        listWidget->setMinimumWidth(300);
        listWidget->setMouseTracking(true);
        listWidget->setAlternatingRowColors(true);
        listWidget->setVerticalScrollMode(QAbstractItemView::ScrollPerItem);
        connect(listWidget, SIGNAL(itemChanged(QListWidgetItem*)), this, SLOT(selectLabel(QListWidgetItem*)));
        connect(listWidget, SIGNAL(itemEntered(QListWidgetItem*)), this, SLOT(highlightItem(QListWidgetItem*)));
        connect(listWidget, SIGNAL(itemPressed(QListWidgetItem*)), this, SLOT(selectItem(QListWidgetItem*)));
    }
    virtual ~medLabelsVisibilityParameter(){};
    virtual QHash<QString,QVariant> hashValues() const{return m_value;};
    QWidget * getWidget(){return listWidget;};
    void addLabel(QString name,bool val,QColor color = QColor(0,0,0))
    {
        if (!m_value.contains(name))
        {
            m_value.insert(name,val);
            emit valuesChanged(m_value);
        }
        
        QListWidgetItem * item = new QListWidgetItem(name);
        item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
        item->setCheckState(val ? Qt::Checked : Qt::Unchecked);
        QPixmap pix(15,15);
        pix.fill(color);
        if (name!="All")
            item->setIcon(QIcon(pix));
        listWidget->addItem(item);

        this->blockSignals(true);
        updateInternWigets();
        this->blockSignals(false);
    };
    void setValue(QString name,bool val)
    {
        m_value[name]=val;
        updateInternWigets();
        emit valuesChanged(m_value);
    };
    void clearWidget()
    {
        listWidget->clear();
    };

public slots:
    void selectLabel(QListWidgetItem *item)
    {
        QString label = item->text();

        if(label == "All")
        {
            if(item->checkState() == Qt::Checked)
                checkAllLabels(true);
            else 
                checkAllLabels(false);
            return;
        }
        int i = listWidget->row(item);
        if(item->checkState() == Qt::Checked)
            setValue(label,true);
        else
        {
            // a param has been unchecked, uncheck 'All' item
            listWidget->blockSignals(true);
            m_value["All"]=false;
            listWidget->item(0)->setCheckState(Qt::Unchecked);
            listWidget->blockSignals(false);
            setValue(label,false);
        }
    };
    void highlightItem(QListWidgetItem *item)
    {
        item->setSelected(true);
    };
    void checkAllLabels(bool check)
    {
        m_value["All"]=check;
        for(int i=0; i<listWidget->count(); i++)
        {
            QListWidgetItem *item = listWidget->item(i);
            if(check)
                item->setCheckState(Qt::Checked);
            else
                item->setCheckState(Qt::Unchecked);
        }
    };

    void selectItem(QListWidgetItem *item)
    {
        if(item->checkState() == Qt::Checked)
            item->setCheckState(Qt::Unchecked);
        else
            item->setCheckState(Qt::Checked);
    };

    virtual void setValues(QHash<QString,QVariant> value)
    {
        if (m_value==value)
            return;
        m_value=value;
        updateInternWigets();
    };
protected:
    virtual void updateInternWigets()
    {
        if (m_value["All"].toBool())
        {
            listWidget->blockSignals(true);
            checkAllLabels(true);
            listWidget->blockSignals(false);
        }
        foreach(QString label,m_value.keys())
        {
            if (!listWidget->findItems(label,Qt::MatchExactly).isEmpty())
            {
                listWidget->blockSignals(true);
                listWidget->findItems(label,Qt::MatchExactly)[0]->setCheckState(m_value[label].toBool() ? Qt::Checked : Qt::Unchecked);
                listWidget->blockSignals(false);
                if (label == "All")
                    continue;
                if (m_value[label].toBool()) 
                    emit labelChecked(label); 
                else 
                    emit labelUnchecked(label);
            }
        }
    };

signals:
    void labelChecked(QString);
    void labelUnchecked(QString);
protected:
    QHash<QString,QVariant> m_value;
    medListWidget * listWidget;

};




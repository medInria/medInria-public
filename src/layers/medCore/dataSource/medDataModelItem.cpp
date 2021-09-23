#include <medDataModelItem.h>
#include <medDataModelElement.h>

#include <QVariant>
#include <QList>
#include <QVector>
#include <QString>

//class medDataModelItemPrivate
//{
//public:
//    medDataModelItem * m_parentItem;
//    QList<medDataModelItem *> m_childItems;
//
//    QVariantList m_itemData;
//
//    int m_iLevel;
//};



medDataModelItem::medDataModelItem(medDataModelElement *model) //: d(new medDataModelItemPrivate())
{
    m_model = model;
    m_parentItem = nullptr; //Invalid parent 
    m_iLevel = -1; //Invalid effective level, used by root item
    m_bCanHaveSubData = true;
}

medDataModelItem::medDataModelItem(medDataModelElement *model, medDataModelItem *parent) //: d(new medDataModelItemPrivate())
{
    m_model = model;
    m_bCanHaveSubData = true;
    setParent(parent);
}

medDataModelItem::~medDataModelItem()
{

}




int medDataModelItem::childCount() const
{
    return m_childItems.size();
}

int medDataModelItem::level() const
{
    return m_iLevel;
}

int medDataModelItem::row() const
{
    int iRowRes = 0;

    if (m_parentItem)
    {
        iRowRes = m_parentItem->m_childItems.indexOf(const_cast<medDataModelItem*>(this));
    }

    return iRowRes;
}

medDataModelItem * medDataModelItem::parent() const
{
    return m_parentItem;
}

bool medDataModelItem::insertChildren(int position, int count)
{
    bool bRes = (position >= 0) && (position <= m_childItems.size());

    if (bRes)
    {
        for (int i = 0; i < count; ++i)
        {
            medDataModelItem *pItemTmp = new medDataModelItem(m_model);
            m_childItems.insert(position, pItemTmp);
        }
    }

    return bRes;
}

bool medDataModelItem::canHaveSubData()
{
    return m_bCanHaveSubData;
}






medDataModelItem * medDataModelItem::child(int row) const
{
    return m_childItems.value(row);
}

QVariant medDataModelItem::data(int column) const
{
    QVariant varRes(".");

    int internalColumn = m_model->getColumnInsideLevel(m_iLevel, column);
    if (internalColumn >= 0)
    {
        varRes = m_itemData.value(internalColumn+1);
    }

    return varRes;
    //return m_itemData.value(column);
}

void medDataModelItem::setParent(medDataModelItem * parent)
{
    m_parentItem = parent;
    m_iLevel = m_parentItem->m_iLevel + 1;
}

void medDataModelItem::setData(QStringList const & value)
{
    if (!m_itemData.isEmpty())
    {
        m_itemData.clear();
    }
    for (auto val : value)
    {
        m_itemData.append(val);
    }    
}

//void medDataModelItem::setIID(QString iid)
//{
//    m_iid = iid;
//}

void medDataModelItem::append(medDataModelItem * child)
{
    m_childItems.append(child);
}

#include <medDataModelItem.h>
#include <medDataModelElement.h>

#include <QVariant>
#include <QList>
#include <QVector>
#include <QString>




medDataModelItem::medDataModelItem(medSourceItemModel *model)
{
    m_model = model;
    m_parentItem = nullptr; //Invalid parent 
    m_iLevel = -1; //Invalid effective level, used by root item
    m_bCanHaveSubData = true;
}

medDataModelItem::medDataModelItem(medDataModelItem *parent)
{
    m_model = parent->m_model;
    m_bCanHaveSubData = true;
    setParent(parent);
}

medDataModelItem::~medDataModelItem()
{
    for (auto &childItem : m_childItems)
    {
        delete childItem;
        childItem = nullptr; //useful ?
    }
    m_childItems.clear(); //useful ?
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

//bool medDataModelItem::insertChildren(int position, int count)
//{
//    bool bRes = (position >= 0) && (position <= m_childItems.size());
//
//    if (bRes)
//    {
//        for (int i = 0; i < count; ++i)
//        {
//            medDataModelItem *pItemTmp = new medDataModelItem(m_model);
//            m_childItems.insert(position, pItemTmp);
//        }
//    }
//
//    return bRes;
//}

bool medDataModelItem::removeRows(int row, int count)
{
    bool bRes = true;

    if (row>-1 && count>0 && (row + count - 1 < m_childItems.size()))
    {
        for (int i = row; i < row + count; ++i)
        {
            auto   childItemToRemove = m_childItems[i];
            delete childItemToRemove;
        }
        m_childItems.erase(m_childItems.begin() + row, m_childItems.begin() + row + count);
    }

    return bRes;
}

bool medDataModelItem::canHaveSubData()
{
    return m_bCanHaveSubData;
}

QString medDataModelItem::uri()
{
    QString uriRes = m_model->getSourceIntanceId() + ":";

    QStringList uriParts;

    medDataModelItem *pItem = this;
    while (pItem->m_parentItem)
    {
        uriParts.push_front(pItem->iid());
        pItem = pItem->m_parentItem;
    }
    int size = uriParts.size();
    if (size > 0)
    {
        int i;
        for (i = 0; (i+1)<size; ++i)
        {
            uriRes += uriParts[i] + "\r\n";
        }
        uriRes += uriParts[i];
    }

    return uriRes;
}







/* ***********************************************************************/
/* *************** Data manipulation *************************************/
/* ***********************************************************************/
int medDataModelItem::childIndex(QString iid)
{
    int iRes = -1;
    
    int i = 0;
    bool bFind = false;
    while (!bFind && i < m_childItems.size())
    {
        bFind = m_childItems[i]->m_itemData[0][0] == iid;
        if (bFind)
        {
            iRes = i;
        }
        ++i;
    }

    return iRes;
}

medDataModelItem * medDataModelItem::hasChildItemWithIID(QString iid)
{
    medDataModelItem* pItemRes = nullptr;

    int iPos = childIndex(iid);
    if (iPos > -1)
    {
        pItemRes = child(iPos);
    }

    return pItemRes;
}

medDataModelItem * medDataModelItem::child(int row) const
{
    return m_childItems.value(row);
}

void medDataModelItem::setParent(medDataModelItem * parent)
{
    m_parentItem = parent;
    m_iLevel = m_parentItem->m_iLevel + 1;
}

void medDataModelItem::setData(QVariant value, int column, int role)
{
    m_itemData[column][role] = value;
}

QVariant medDataModelItem::data(int column, int role) const
{
    QVariant varRes;

    int internalColumn = m_model->getColumnInsideLevel(m_iLevel, column);
    if (m_itemData.contains(internalColumn) && m_itemData[internalColumn].contains(role))
    {
        varRes = m_itemData[internalColumn][role];
    }

    return varRes;
}

void medDataModelItem::append(medDataModelItem * child)
{
    m_childItems.append(child);
}

void medDataModelItem::insert(int position, medDataModelItem *child)
{
    m_childItems.insert(position, child);
}
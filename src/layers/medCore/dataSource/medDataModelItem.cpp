#include <medDataModelItem.h>
#include <medDataModelElement.h>

#include <QVariant>
#include <QList>
#include <QVector>
#include <QString>




medDataModelItem::medDataModelItem(medDataModelElement *model)
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

bool medDataModelItem::removeRows(int row, int count)
{
    bool bRes = true;

    if (row>-1 && count>0 && (row + count < m_childItems.size()))
    {
        for (int i = row; i < row + count; ++i)
        {
            auto   childItemToRemove = m_childItems[i];
            delete childItemToRemove;
        }
        m_childItems.erase(m_childItems.begin() + row, m_childItems.begin() + row + count - 1);
    }

    return bRes;
}

bool medDataModelItem::canHaveSubData()
{
    return m_bCanHaveSubData;
}






int medDataModelItem::childIndex(QString iid)
{
    int iRes = -1;
    
    int i = 0;
    bool bFind = false;
    while (!bFind && i < m_childItems.size())
    {
        bFind = m_childItems[i]->m_itemData[0] == iid;
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

QVariant medDataModelItem::data(int column) const
{
    QVariant varRes;

    int internalColumn = m_model->getColumnInsideLevel(m_iLevel, column);
    if (internalColumn >= 0)
    {
        varRes = m_itemData.value(internalColumn+1);
    }

    return varRes;
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

void medDataModelItem::append(medDataModelItem * child)
{
    m_childItems.append(child);
}

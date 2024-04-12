#include "medSourcesItemDelegate.h"
#include <QDebug>
#include <QMovie>
#include <QPainter>
#include <medSourceModelItem.h>
#include <medSourceModel.h>

medSourcesItemDelegate::medSourcesItemDelegate(QTreeView *treeView)
{
    m_proxy = dynamic_cast<QSortFilterProxyModel *>(treeView->model());
    m_mov = new QMovie(":gif/icon.gif");
    m_mov->start();
    connect(m_mov, SIGNAL(frameChanged(int)), treeView, SLOT(update()));
    m_treeView = treeView;
}

void medSourcesItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QStyleOptionViewItem opt = option;
    initStyleOption(&opt, getIndex(index));
    opt.decorationSize = QSize(16,16);
    // opt.state &= ~QStyle::State_Selected;
    // m_treeView->setStyleSheet("QTreeView { selection-color: #FF8833;}");
    if (index.isValid())
    {
        auto value = getIndex(index).data(DATASTATE_ROLE).toString();
   
        if (value== DATASTATE_ROLE_DATALOADING)
        {
            painter->drawPixmap(QRect(opt.rect.x()-40, opt.rect.y(), opt.decorationSize.width(), opt.rect.height()), m_mov->currentPixmap());//QPixmap(":icons/yellow_spot.svg"));
        }
        else if (value == DATASTATE_ROLE_DATALOADED)
        {
            auto list = m_treeView->selectionModel()->selectedIndexes();
            if(!list.empty())
            {
                if (list[0] == index)
                    m_treeView->setStyleSheet("QTreeView { selection-color: #0EBFEF;}");
                else
                    m_treeView->setStyleSheet("QTreeView { selection-color: #FF8833;}");
            }
            opt.font.setBold(true);
            painter->drawPixmap(QRect(opt.rect.x()-40, opt.rect.y(), opt.decorationSize.width(), opt.rect.height()), QPixmap(":icons/icons8-download-16.png"));//QPixmap(":icons/yellow_spot.svg"));
        }
        else if (value == DATASTATE_ROLE_DATANOTLOADED)
        {
            opt.font.setStrikeOut(true);
            painter->drawPixmap(QRect(opt.rect.x()-40, opt.rect.y(), opt.decorationSize.width(), opt.rect.height()), QPixmap(":icons/icons8-download-error-16.png"));//QPixmap(":icons/yellow_spot.svg"));
        }
        else if (value == DATASTATE_ROLE_DATASAVED)
        {
            //opt.font.setBold(true);
            
            painter->drawPixmap(QRect(opt.rect.x()-40, opt.rect.y(), opt.decorationSize.width(), opt.rect.height()), QPixmap(":icons/icons8-upload-16.png"));//QPixmap(":icons/yellow_spot.svg"));
        }
        else if (value == DATASTATE_ROLE_DATANOTSAVED)
        {
            opt.font.setItalic(true);
            painter->drawPixmap(QRect(opt.rect.x()-40, opt.rect.y(), opt.decorationSize.width(), opt.rect.height()), QPixmap(":icons/icons8-upload-error-16.png"));//QPixmap(":icons/yellow_spot.svg"));
        }

    }
    QStyledItemDelegate::paint(painter, opt, index);
    //QStyledItemDelegate::paint(painter, option, index);

}

QModelIndex medSourcesItemDelegate::getIndex(const QModelIndex &index) const
{
    QModelIndex indexRes;
    if (m_proxy)
    {
        indexRes = m_proxy->mapToSource(index);
    }
    else
    {
        indexRes = index;
    }

    return indexRes;
}

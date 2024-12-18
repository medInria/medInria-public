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

    if (index.isValid())
    {
        auto value = getIndex(index).data(DATASTATE_ROLE).toString();

        int iconY = opt.rect.y() + (opt.rect.height() - opt.decorationSize.height()) / 2;
        QRect iconRect(opt.rect.x()-40, iconY, opt.decorationSize.width(), opt.decorationSize.height());

        if (value== DATASTATE_ROLE_DATALOADING)
        {
            painter->drawPixmap(iconRect, m_mov->currentPixmap());
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
            painter->drawPixmap(iconRect, QIcon::fromTheme("download").pixmap(opt.decorationSize));
        }
        else if (value == DATASTATE_ROLE_DATANOTLOADED)
        {
            opt.font.setStrikeOut(true);
            painter->drawPixmap(iconRect, QIcon::fromTheme("download_fail").pixmap(opt.decorationSize));
        }
        else if (value == DATASTATE_ROLE_DATASAVED)
        {
            painter->drawPixmap(iconRect, QIcon::fromTheme("upload").pixmap(opt.decorationSize));
        }
        else if (value == DATASTATE_ROLE_DATANOTSAVED)
        {
            opt.font.setItalic(true);
            painter->drawPixmap(iconRect, QIcon::fromTheme("upload_fail").pixmap(opt.decorationSize));
        }

    }
    QStyledItemDelegate::paint(painter, opt, index);
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

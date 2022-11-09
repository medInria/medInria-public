#include "medSourcesItemDelegate.h"
#include <QDebug>
#include <QMovie>
#include <QPainter>
#include <medDataModelItem.h>
#include <medSourceItemModel.h>

//medSourcesItemDelegate::medSourcesItemDelegate(QAbstractItemModel *model)
medSourcesItemDelegate::medSourcesItemDelegate(QTreeView *treeView)
{
    //m_proxy = dynamic_cast<QSortFilterProxyModel *>(model);
    m_proxy = dynamic_cast<QSortFilterProxyModel *>(treeView->model());
    m_mov = new QMovie(":gif/icon.gif");
    m_mov->start();
//    connect(m_mov, &QMovie::frameChanged, [=](int frameNumber){
//        treeView->update();
//    });
    connect(m_mov, SIGNAL(frameChanged(int)), treeView, SLOT(update()));
}

void medSourcesItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QStyleOptionViewItem &opt = *const_cast<QStyleOptionViewItem*>(&option);
//    initStyleOption(&opt, index);
//    opt.state &= ~QStyle::State_Selected;
//    opt.decorationPosition = QStyleOptionViewItem::Left;
//    opt.decorationAlignment = Qt::AlignHCenter;
//    opt.decorationSize = QSize(16, 16);
//    opt.features &= QStyleOptionViewItem::HasDecoration;
    if (index.isValid())
    {
        auto value = getIndex(index).data(DATASTATE_ROLE).toString();
        
        std::string val = value.toStdString();

        if (value== DATASTATE_ROLE_DATALOADING)
        {
//            m_mov->jumpToNextFrame();
//            qDebug()<<"pixmap "<<m_mov->currentFrameNumber()<<"  -- "<<m_mov->currentPixmap()<<"   --- "<<m_currentPixmap;
//            opt.icon.addPixmap(m_mov->currentPixmap());
//            opt.icon.addPixmap(QPixmap(":icons/yellow_spot.svg"));
            painter->drawPixmap(QRect(option.rect.x()-20, option.rect.y(), option.rect.height(), option.rect.height()), m_mov->currentPixmap());//QPixmap(":icons/yellow_spot.svg"));

        }
        else if (value== DATASTATE_ROLE_DATALOADED)
        {
            opt.font.setBold(true);
            painter->drawPixmap(QRect(option.rect.x()-20, option.rect.y(), option.rect.height(), option.rect.height()), QPixmap(":icons/check.svg"));//QPixmap(":icons/yellow_spot.svg"));
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

//QSize medSourcesItemDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
//{
////    QStyleOptionViewItem opt = option;
////    opt.decorationSize = ;
////    opt.decorationAlignment = Qt::AlignHCenter;
////    opt.decorationPosition = QStyleOptionViewItem::Left;
////    opt.state &= ~QStyle::State_Selected;
//
//    return QSize(option.rect.width(), option.rect.height());
//}





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
}

void medSourcesItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QStyleOptionViewItem opt = option;
    initStyleOption(&opt, getIndex(index));
    opt.decorationSize = QSize(16,16);
    //((QStyleOptionViewItem*)(&option))->icon.addPixmap(QPixmap(":icons/icons8-download-16.png"));
    
    if (index.isValid())
    {
        auto value = getIndex(index).data(DATASTATE_ROLE).toString();
        
        std::string val = value.toStdString();
    
        if (value== DATASTATE_ROLE_DATALOADING)
        {
            painter->drawPixmap(QRect(opt.rect.x()-40, opt.rect.y(), opt.decorationSize.width(), opt.rect.height()), m_mov->currentPixmap());//QPixmap(":icons/yellow_spot.svg"));
        }
        else if (value == DATASTATE_ROLE_DATALOADED)
        {
            auto v = opt.features;
            opt.font.setBold(true);
            opt.decorationPosition = QStyleOptionViewItem::Left;
            opt.features &= QStyleOptionViewItem::HasDecoration;
            //painter->drawPixmap(QRect(opt.rect.x() - 40, opt.rect.y(), opt.decorationSize.width(), opt.rect.height()), QPixmap(":icons/icons8-download-16.png"));//QPixmap(":icons/yellow_spot.svg"));
            //painter->drawPixmap(QRect(opt.rect.x(), opt.rect.y(), opt.decorationSize.width(), opt.rect.height()), QPixmap(":icons/icons8-download-16.png"));//QPixmap(":icons/yellow_spot.svg"));
            //painter->drawText(QPoint(opt.rect.x() + 60, opt.rect.y()), opt.text);
            //opt.icon.addPixmap(  );
            //((medSourceModelItem*)getIndex(index).internalPointer())->setData(QPixmap(":icons/icons8-download-16.png"), 0, Qt::DecorationRole);


            //opt.decorationPosition = QStyleOptionViewItem::Right;
            //opt.text = "";
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
    auto x = opt.text;
    QStyledItemDelegate::paint(painter, opt, index);
    //QStyledItemDelegate::paint(painter, option, index);

}

//QSize medSourcesItemDelegate::sizeHint(const QStyleOptionViewItem & option, const QModelIndex & index) const
//{
//    QSize size(option.rect.width() + 50, option.rect.height());
//    return size;
//}

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

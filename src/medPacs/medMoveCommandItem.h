#ifndef MEDMOVECOMMANDITEM_H
#define MEDMOVECOMMANDITEM_H

struct medMoveCommandItem
{
    int group;
    int elem;
    QString query;
    QString sourceTitle;
    QString sourceIp;
    int sourcePort;;
    QString targetTitle;
    QString targetIp;
    int targetPort;;

    operator QVariant() const
    {
        return QVariant::fromValue(*this);
    }
};
Q_DECLARE_METATYPE(medMoveCommandItem);

#endif MEDMOVECOMMANDITEM_H
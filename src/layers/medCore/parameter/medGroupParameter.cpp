/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2018. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medGroupParameter.h>

class medGroupParameterPrivate
{
public:
    QList<medAbstractParameter*> parameterList;
};

medGroupParameter::medGroupParameter(QString const& id, QObject* parent)
    : medAbstractParameter(id, parent), d(new medGroupParameterPrivate)
{

}

medGroupParameter::~medGroupParameter()
{

}

QList<medAbstractParameter*> medGroupParameter::value() const
{
    return d->parameterList;
}

bool medGroupParameter::copyValueTo(medAbstractParameter & dest)
{
    bool bRes = typeid(dest) == typeid(*this);

    if (bRes)
    {
        medGroupParameter * pDest = dynamic_cast<medGroupParameter*>(&dest);
        bRes = idList() == pDest->idList();
        if (bRes)
        {
            for (int i=0; i<d->parameterList.size(); ++i)
            {
                d->parameterList[i]->copyValueTo(*pDest->d->parameterList[i]);
            }
        }
    }

    return bRes;
}

QVariantMap medGroupParameter::toVariantMap() const
{
    QVariantMap varMapRes;

    varMapRes.insert("id", id());
    varMapRes.insert("caption", caption());
    varMapRes.insert("description", description());

    QVariantList variantLst;
    for (auto * param : d->parameterList)
    {
        variantLst.push_back(param->toVariantMap());
    }
    varMapRes.insert("value", variantLst);

    return varMapRes;
}

bool medGroupParameter::fromVariantMap(QVariantMap const & pi_variantMap)
{
    bool bRes = true;

    bRes = bRes && pi_variantMap.contains("id");
    bRes = bRes && pi_variantMap.contains("caption");
    bRes = bRes && pi_variantMap.contains("description");
    bRes = bRes && pi_variantMap.contains("value");

    if (bRes)
    {
        QMap<QString, medAbstractParameter*>  idToPramMap;

        setCaption(pi_variantMap["caption"].toString());
        setDescription(pi_variantMap["description"].toString());

        for (auto * param : d->parameterList)
        {
            idToPramMap[param->id()] = param;
        }

        auto paramsValueAsLst = pi_variantMap["value"].toList();
        for (auto & param : paramsValueAsLst)
        {
            auto paramAsMap = param.toMap();
            if (paramAsMap.contains("id"))
            {
                QString id = paramAsMap["id"].toString();
                if (idToPramMap.contains(id))
                {
                    bRes &= idToPramMap[id]->fromVariantMap(paramAsMap);
                }
                else
                {
                    bRes = false;
                }
            }
            else
            {
                bRes = false;
            }
        }
    }

    return bRes;
}

void medGroupParameter::addParameter(medAbstractParameter * param)
{
    if (param != nullptr)
    {
        param->setParent(this);
        d->parameterList.push_back(param);
        connect(param, &medAbstractParameter::triggered, this, &medGroupParameter::trigger);
        emit parameterAdded(param);
    }
}

void medGroupParameter::removeParameter(medAbstractParameter * param)
{
    auto index = d->parameterList.indexOf(param);
    if (index >= 0)
    {
        disconnect(param, &medAbstractParameter::triggered, this, &medGroupParameter::trigger);
        d->parameterList.takeAt(index);
        emit parameterRemoved(param);
    }
}

void medGroupParameter::trigger()
{
    emit triggered();
}

QStringList medGroupParameter::idList()
{
    QStringList idsRes;
    
    for (auto * param : d->parameterList)
    {
        idsRes.push_back(param->id());
    }

    return idsRes;
}

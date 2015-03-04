/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medAbstractNavigator.h>

#include <medAbstractView.h>


/**
 * @fn QWidget* medAbstractNavigator::buildToolBoxWidget()
 * @brief buildToolBoxWidget reimplement it to construct and return the widget displayed in the
 * view settings toolBox when the container of the parent view is single selected.
 * @return
 */

/**
 * @fn QWidget* medAbstractNavigator::buildToolBarWidget()
 * @brief buildToolBarWidget reimplement it to construct and return the widget displayed in the
 * toolbar of the container where the parent view is dsiplayed.
 */



class medAbstractNavigatorPrivate
{
public:
    QPointer<QWidget> toolBoxWidget;
    QPointer<QWidget> toolBarWidget;
};

medAbstractNavigator::medAbstractNavigator(medAbstractView *parent):
    d(new medAbstractNavigatorPrivate)
{
    setParent(parent);
    d->toolBarWidget = NULL;
    d->toolBoxWidget = NULL;
}

medAbstractNavigator::~medAbstractNavigator()
{
    delete d;
}

QWidget* medAbstractNavigator::toolBoxWidget()
{
    if(d->toolBoxWidget.isNull())
    {
        d->toolBoxWidget = this->buildToolBoxWidget();
    }

    return d->toolBoxWidget;
}

QWidget* medAbstractNavigator::toolBarWidget()
{
    if(d->toolBarWidget.isNull())
    {
        d->toolBarWidget = this->buildToolBarWidget();
    }

    return d->toolBarWidget;
}

/**
 * @brief updateWidgets reimplemts it to reupdate your
 * widgets when data in view or orientation change.
 */
void medAbstractNavigator::updateWidgets()
{

}

void medAbstractNavigator::toXMLNode(QDomDocument* doc,QDomElement* currentNode)
{
	QDomElement elmt=doc->createElement("parameters");
	elmt.setAttribute("number",this->linkableParameters().size());
	for(int i=0;i<linkableParameters().size();i++)
	{
		QDomElement paramNode=doc->createElement("parameter");
		linkableParameters()[i]->toXMLNode(doc,&paramNode);
		elmt.appendChild(paramNode);
	}
	currentNode->appendChild(elmt);
}

void medAbstractNavigator::fromXMLNode(QDomElement* element)
{
	if(element->nodeName()!="navigator")
	{
		qWarning()<<"Unable to parse "<<element->nodeName();
		return;
	}
	QDomNodeList parameterList=element->elementsByTagName("parameter");
	qDebug()<<element->elementsByTagName("parameter").size();
	QHash<QString,QString> parameters;
	for(int i=0;i<parameterList.size();i++)
	{
		if(!(parameterList.at(i).isElement()))
		{
			qWarning()<<"failed to read a parameter";
		}
		QDomElement currentParameterElement =parameterList.at(i).toElement();
		QDomNodeList currentParametersValues=currentParameterElement.childNodes();
		QString name,type,value;
		for(int j=0;j<currentParametersValues.size();j++)
		{
			QString nodeName=currentParametersValues.at(j).nodeName();
			if(nodeName=="name")
				name=currentParametersValues.at(j).firstChild().toText().nodeValue();
			else if(nodeName=="type")
				type=currentParametersValues.at(j).firstChild().toText().nodeValue();
			else if(nodeName=="value")
				value=currentParametersValues.at(j).firstChild().toText().nodeValue();
		}
		parameters[name]=value;
	}
	
	restoreParameters(parameters);
}

void medAbstractNavigator::restoreParameters(QHash<QString,QString> parameters)
{
		
}

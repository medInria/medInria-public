/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medStyleSheetParser.h>

/*
 * Format :
 * $var = <value>;
 * border: ${pixel}px solid $color;
 */

#include <QDebug>
#include <QHash>
#include <QRegExp>
#include <QTextStream>

inline void deref(int index, QString & s, const QRegExp & regex, const QHash<QString, QString> & vars, int lineCount) {
    QString varName = regex.cap(1).isEmpty() ? regex.cap(2) : regex.cap(1);
    if (vars.contains(varName)) {
        s.replace(index, regex.matchedLength(), vars[varName]);
    } else {
        s.replace(index, regex.matchedLength(), "");
        qWarning("Unknown variable name line %d: %s", lineCount, qPrintable(varName));
    }
}

medStyleSheetParser::medStyleSheetParser(QString qss) {
    QTextStream stream(&qss);
    QRegExp regexAssign("(^|;)\\s*\\$([a-zA-Z0-9_]+)\\s*=\\s*([^;]+)\\s*;");
    QRegExp regexDeref("\\$([a-zA-Z0-9_]+)|\\$\\{([a-zA-Z0-9_]+)\\}");
    QRegExp regexInlineComment("(?:/\\*(?:[^*]/|[^/])*\\*/)|(?://.*$)");
    regexInlineComment.setMinimal(true);

    QHash<QString, QString> vars;
    int lineCount = 0;
    bool insideComment = false;
    while( ! stream.atEnd() ) {
        QString line = stream.readLine();
        lineCount++;

        // already in a comment, try to find the end, otherwise skip line
        if (insideComment) {
            int endCommentIndex = line.indexOf("*/");
            if (endCommentIndex != -1) {
                line = line.mid(endCommentIndex+2);
                insideComment = false;
            } else {
                continue;
            }
        }

        // strip all comments beginning and ending on this line
        line.remove(regexInlineComment);

        // find a beginning multi-line comment
        int beginCommentIndex = line.indexOf("/*");
        if (beginCommentIndex != -1 ) {
            insideComment = true;
            line = line.left(beginCommentIndex);
        }

        // do all possible assignments and dereferences on this line
        do {
            int assignIndex = line.indexOf(regexAssign);
            int derefIndex = line.indexOf(regexDeref);

            // If we get no match from either, bail
            if (assignIndex == -1 && derefIndex == -1)
                break;

            // Which comes first, assignement or dereference ?
            // if the same, assignment is prioritary, otherwise the "$var ="
            // would be seen as a dereference.
            if (derefIndex == -1 || (assignIndex != -1 && derefIndex >= assignIndex)) {
                int matchedLength = regexAssign.matchedLength();
                // if we started on a previous ";", keep it in the result
                if (regexAssign.cap(1) == ";") {
                    assignIndex++;
                    matchedLength--;
                }
                QString varName = regexAssign.cap(2);
                QString value = regexAssign.cap(3).trimmed();
                int derefIndex2 = -1;
                while((derefIndex2 = value.indexOf(regexDeref, derefIndex2+1)) != -1)
                    deref(derefIndex2, value, regexDeref, vars, lineCount);
                vars[varName] = value;
                line.remove(assignIndex, matchedLength);
            } else {
                deref(derefIndex, line, regexDeref, vars, lineCount);
            }
        } while(true);
        if ( ! line.trimmed().isEmpty() )
            output += line + "\n";
    }
    output.chop(1); // remove last \n
}


QString medStyleSheetParser::result() const {
    return output;
}

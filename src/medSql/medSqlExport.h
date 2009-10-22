/* medSqlExport.h --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Wed Oct 21 15:15:53 2009 (+0200)
 * Version: $Id$
 * Last-Updated: Wed Oct 21 15:16:15 2009 (+0200)
 *           By: Julien Wintz
 *     Update #: 2
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#ifdef WIN32
    #ifdef medSql_EXPORTS
        #define MEDSQL_EXPORT __declspec(dllexport) 
    #else
        #define MEDSQL_EXPORT __declspec(dllimport) 
    #endif
#else
    #define MEDSQL_EXPORT	
#endif

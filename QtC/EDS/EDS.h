/* -*- mode:c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-
** File:       QtC/EDS/EDS.h
** Copyright:  Copyright (c) 2014, Digia Plc. All rights reserved.
**             All other trademarks are the property of their respective owners.
** Comment:    Engin.IO Data Storage
** Author(s):  Jorma Tahtinen <Jorma.Tahtinen@digia.com>
*/

#ifndef QTC_EDS_EDS_H
#define QTC_EDS_EDS_H

#include <string>

#include <QtC/Common/URI.h>
#include <QtC/EDS/Collection.h>

namespace QtC {
    
    class EDS {
        friend class Collection;
    public:
         EDS(const URL &aURL,const std::string &aBackendId);
        ~EDS();
        
        Collection collection(const std::string &aCollectionName);
    protected:
        struct EDSPrivate *pimpl();
    private:
        struct EDSPrivate *iPIMPL;
    };
    
} /* namespace QtC */

#endif /* QTC_EDS_EDS_H */

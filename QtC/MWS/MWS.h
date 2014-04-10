/* -*- mode:c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-
** File:       QtC/MWS/MWS.h
** Copyright:  Copyright (c) 2014, Digia Plc. All rights reserved.
**             All other trademarks are the property of their respective owners.
** Comment:    Engin.IO Data Storage
** Author(s):  Jorma Tahtinen <Jorma.Tahtinen@digia.com>
*/

#ifndef QTC_EDS_EDS_H
#define QTC_EDS_EDS_H

#include <string>

#include <QtC/EDS/Config.h>
#include <QtC/EDS/Collection.h>

namespace QtC {
    
  /*
    class EDS {
        friend class Collection;
    public:
        EDS(const std::string &aBackendId,
            const std::string &aHostname = QTC_EDS_DEFAULT_HOSTNAME,
            const std::string &aService  = QTC_EDS_DEFAULT_SERVICE);
        ~EDS();
        
        Collection collection(const std::string &aCollectionName);
    protected:
        struct EDSPrivate *pimpl();
    private:
        struct EDSPrivate *iPIMPL;
    };
  */
    
} /* namespace QtC */

#endif /* QTC_EDS_EDS_H */

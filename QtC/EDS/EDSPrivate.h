/* -*- mode:c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-
** File:       QtC/EDS/EDSPrivate.h
** Copyright:  Copyright (c) 2014, Digia Plc. All rights reserved.
**             All other trademarks are the property of their respective owners.
** Comment:    Engin.IO Data Storage Private
** Author(s):  Jorma Tahtinen <Jorma.Tahtinen@digia.com>
*/

#ifndef QTC_EDS_EDS_PRIVATE_H
#define QTC_EDS_EDS_PRIVATE_H

#include <string>

#include "QtC/Common/HttpConnection.h"

namespace QtC {
    
    struct EDSPrivate {
        std::string backendId;
        HttpConnectionPool::var connectionPool;
    };
    
} /* namespace QtC */

#endif /* QTC_EDS_EDS_PRIVATE_H */

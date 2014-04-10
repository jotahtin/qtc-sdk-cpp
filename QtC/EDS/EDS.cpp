/* -*- mode:c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-
** File:       QtC/EDS/EDS.cpp
** Copyright:  Copyright (c) 2014, Digia Plc. All rights reserved.
**             All other trademarks are the property of their respective owners.
** Comment:    Engin.IO Data Storage
** Author(s):  Jorma Tahtinen <Jorma.Tahtinen@digia.com>
*/

#include <iostream>
#include <map>
#include <vector>
#include <stack>

#include "QtC/EDS/EDS.h"
#include "QtC/EDS/EDSPrivate.h"

namespace QtC {
        
    EDS::EDS(const URL &aURL,const std::string &aBackendId)
        : iPIMPL(new EDSPrivate)
    {
        iPIMPL->backendId      = aBackendId;
        iPIMPL->connectionPool = HttpConnectionPool::get(aURL);
    }
    
    EDS::~EDS() {
        delete iPIMPL;
    }
    
    Collection EDS::collection(const std::string &aCollectionName) {
        return Collection(*this, aCollectionName);
    }
    
    struct EDSPrivate *EDS::pimpl() {
        return iPIMPL;
    }

} /* namespace QtC */

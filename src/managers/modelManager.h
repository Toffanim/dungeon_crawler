#if !defined(MODELMANAGER_H)
/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: Casey Muratori $
   $Notice: (C) Copyright 2015 by Molly Rocket, Inc. All Rights Reserved. $
   ======================================================================== */

#define MODELMANAGER_H

#include <string>
#include <map>
#include "../mesh/model.h"

class modelManager
{
public:
    static modelManager& getInstance()
    {
        static modelManager instance;
        return instance;
    }

    map<std::string, Model *>& getModels()
    { return( models ); }

private:
    modelManager();
    modelManager(modelManager const&) = delete;
    void operator=(modelManager const&) = delete;
    map<std::string, Model*> models;
    
};

#endif

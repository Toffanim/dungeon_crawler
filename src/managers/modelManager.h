#if !defined(MODELMANAGER_H)
/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: Casey Muratori $
   $Notice: (C) Copyright 2015 by Molly Rocket, Inc. All Rights Reserved. $
   ======================================================================== */

#define MODELMANAGER_H

#include "../mesh/model.h"

class modelManager
{
public:
    static modelManager& getInstance()
    {
        static modelManager instance;
        return instance;
    }

    vector<Model *>& getModels()
    { return( models ); }

private:
    modelManager(modelManager const&) = delete;
    void operator=(modelManager const&) = delete;
    vector<Model*> models;
    modelManager();
};

#endif

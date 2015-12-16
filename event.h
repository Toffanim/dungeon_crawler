#if !defined(EVENT_H)
/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: Casey Muratori $
   $Notice: (C) Copyright 2015 by Molly Rocket, Inc. All Rights Reserved. $
   ======================================================================== */

#define EVENT_H
#include <string>
#include <functional>
#include <utility>
#include <map>
#include <iostream>

class event
{
public:
    event()
    {
    }

    //template< class ... ArgType >
    void Register( std::string eName, std::function<void()> eCallback)
    {
        eMap.insert( std::pair<std::string, std::function<void()>>(eName, eCallback) );
    }

    void fire( std::string eName)
    {
        std::cout << eName << std::endl;
        eMap.find(eName)->second();
    }
private:
    std::map<std::string, std::function<void()> > eMap;
};

#endif

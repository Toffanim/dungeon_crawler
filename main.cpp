/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: Casey Muratori $
   $Notice: (C) Copyright 2015 by Molly Rocket, Inc. All Rights Reserved. $
   ======================================================================== */
#include "main.h"
using namespace std;

int main(int argc, char** argv)
{

    game* mygame = new game(800,600);
    mygame->init();
    mygame->mainLoop();

    return(0);
}

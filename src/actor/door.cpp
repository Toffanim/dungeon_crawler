/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: Casey Muratori $
   $Notice: (C) Copyright 2015 by Molly Rocket, Inc. All Rights Reserved. $
   ======================================================================== */
#include "door.h"

door::door( Model* model, glm::mat4 modelMatrix)
        : actor(model,modelMatrix, "door"), isOpen(false)
{

}

void door::Draw(shader* s )
{
    if(!isOpen)
    {
        getModel()->Draw( *s );
    }
}

void door::doCollision(player* p, float deltaTime)
{
    if(!isOpen)
    {
        if(p->hasKey())
        {
            cout << "door collision" << endl;
            p->addKey(-1);
            //rendre texture du model transparent
            isOpen = true;
        }
        else
        {
            glm::vec3 t = glm::normalize(p->getCamera()->getFront());
            glm::vec3 v = glm::vec3( t.z, t.y, -t.x);
            glm::vec3 u = glm::cross(t,v);
            glm::vec3 r = glm::vec3();
            if (p->getMoveLeft())
                r += v;
            if (p->getMoveRight())
                r -= v;
            if (p->getMoveForward())
                r -= t;
            if (p->getMoveBackward())
                r += t;
    
            if (r.x != 0 && r.x > 0)
            {
                float y1 = (getAABB().max.x - p->getAABB().min.x) / r.x;
                if (r.z != 0 && r.z > 0 )
                {
                    float y2 = (getAABB().max.z - p->getAABB().min.z) / r.z;
                    y1 = utils::absmin( y1, y2);
                    if ( y1 == y2 )
                    {
                        p->setPosition( glm::vec3(p->getPosition().x - (y1 * r.x),
                                                  p->getPosition().y,
                                                  p->getPosition().z + (y1 * r.z) ));
                    }
                    else
                    {
                        p->setPosition( glm::vec3(p->getPosition().x + (y1 * r.x),
                                                  p->getPosition().y,
                                                  p->getPosition().z - (y1 * r.z) ));
           
                    }
                }

                if ( r.z != 0 && r.z < 0 )
                {
                    float y2 = (getAABB().min.z - p->getAABB().max.z) / r.z;
                    y1 = utils::absmin(y1,y2);
                    if ( y1 == y2 )
                    {
                        p->setPosition( glm::vec3(p->getPosition().x - (y1 * r.x),
                                                  p->getPosition().y,
                                                  p->getPosition().z + (y1 * r.z) ));
                    }
                    else
                    {
                        p->setPosition( glm::vec3(p->getPosition().x + (y1 * r.x),
                                                  p->getPosition().y,
                                                  p->getPosition().z - (y1 * r.z) ));
           
                    }
                }
          
            }
    
            if (r.x != 0 && r.x < 0)
            {
                float y1 = (getAABB().min.x - p->getAABB().max.x) / r.x;
                if (r.z != 0 && r.z > 0)
                {
                    float y2 = (getAABB().max.z - p->getAABB().min.z) / r.z;
                    y1 = utils::absmin(y1,y2);
                    if (y1 == y2 )
                    {
                        p->setPosition( glm::vec3(p->getPosition().x - (y1 * r.x),
                                                  p->getPosition().y,
                                                  p->getPosition().z + (y1 * r.z) ));
                    }
                    else
                    {
                        p->setPosition( glm::vec3(p->getPosition().x + (y1 * r.x),
                                                  p->getPosition().y,
                                                  p->getPosition().z - (y1 * r.z) ));
           
                    }
                }

                if ( r.z != 0 && r.z < 0 )
                {
                    float y2 = (getAABB().min.z - p->getAABB().max.z) / r.z;
                    y1 = utils::absmin(y1,y2);
                    if(y1==y2)
                    {
                        p->setPosition( glm::vec3(p->getPosition().x - (y1 * r.x),
                                                  p->getPosition().y,
                                                  p->getPosition().z + (y1 * r.z) ));
                    }
                    else
                    {
                        p->setPosition( glm::vec3(p->getPosition().x + (y1 * r.x),
                                                  p->getPosition().y,
                                                  p->getPosition().z - (y1 * r.z) ));
           
                    }
                }
            }
        }
    }
}

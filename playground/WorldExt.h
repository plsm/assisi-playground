/*! \file    WorldExt.h
    \brief   Extensions of Enki with a ZMQ-based external interface.

    TODO: Add detailed description here...

 */

#ifndef ENKI_WORLD_EXT_H
#define ENKI_WORLD_EXT_H

#include <map>

#include <zmq.hpp>

#include "ExtendedWorld.h"
//#include "PhysicalEngine.h"

#include "handlers/ObjectHandler.h"

namespace Enki
{

    //! The extension of Enki::World with a ZMQ publisher an subscriber
    /*!

     */
    class WorldExt : public ExtendedWorld
    {
        
    public:

        //! Construct a world with walls of radius r, start communication.
        /*!
                               WorldExt takes ownership of the object 
                               and deletes it in the destructor!
            \param pub_address The publisher will be bound to this address.
            \param sub_address The subscriber will connect to this address for data.
         */
        WorldExt(double r, 
                 const std::string& pub_address,
                 const std::string& sub_address,
                 const Color& wallsColor = Color::gray,
                 const World::GroundTexture& groundTexture = World::GroundTexture());

        //! Destructor
        virtual ~WorldExt();

        //! Add robot handler.
        /*! One handler should be added for each robot type
            that will be made available through the external interface.

            The WorldExt object takes ownership of the RobotHandler, i.e.,
it must be created on the heap and should not be deleted!
         */
        bool addHandler(std::string type, ObjectHandler* handler);

    protected:
        virtual void controlStep(double dt);

    private:

        //! Simulation command handling
        /*!
            \param device Can be "spawn"
            \param cmd    Robot type; currently EPuck and Casu are supported
            \param data   AssisiMsg::Spawn message, serialized to string
         */
        bool handleSim_(const std::string& device,
                        const std::string& command,
                        const std::string& data);

        typedef std::map<std::string, ObjectHandler*> HandlerMap;
        // Robot handler pointer, one handler per robot type
        HandlerMap handlers_;
        // Pointers to handlers, grouped by object name
        // All robots of the same type will point to the same handler
        HandlerMap handlers_by_object_;

        // ZMQ connection data members
        std::string pub_address_;
        std::string sub_address_;
        
        zmq::context_t* context_;
        zmq::socket_t* publisher_;
        zmq::socket_t* subscriber_;
    };

}

#endif

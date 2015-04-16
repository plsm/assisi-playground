/*! \file  FishHandler.h
    \brief Communication handler for the Fishes.
*/

#ifndef ENKI_FISH_HANDLER_H
#define ENKI_FISH_HANDLER_H

#include <map>

#include "handlers/ObjectHandler.h"

namespace Enki
{

    class Fish;
    class WorldExt;

    //! Handling of Fishes
    /*!

     */
    class FishHandler : public ObjectHandler
    {
    public:
        FishHandler() { }
        virtual ~FishHandler() { }

        //! Fish factory method
        /*! Creates Fishes.

          Keeps a pointer to the created robot, but does not
          delete it in the destructor.

          \return Returns the name of the created robot.
        */
        virtual std::string createObject(const std::string& data, WorldExt* world);

        //! Handle incoming message
        /*! Handles fish actuator commands.

         */
        virtual int handleIncoming(const std::string& name,
                                   const std::string& device,
                                   const std::string& command,
                                   const std::string& data);

        //! Assemble outgoing messages.
        /*! Sends Fish sensor data messages.

         */
        virtual int sendOutgoing(zmq::socket_t& socket);

        virtual PhysicalObject* getObject(const std::string& name);

    private:
        typedef std::map<std::string, Fish*> FishMap;
        FishMap fishes_;
    };
}

#endif

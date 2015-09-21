/* World extension implementation.

 */

#include <cstdio>
#include <iostream>

#include <boost/foreach.hpp>

#include <zmq.hpp>
#include "zmq_helpers.hpp"

#include "handlers/ObjectHandler.h"
#include "WorldExt.h"

// Autogenerated files for protobuf messages
#include "base_msgs.pb.h"
#include "dev_msgs.pb.h"

#include "interactions/WorldHeat.h"

using namespace std;
using namespace zmq;
using namespace AssisiMsg;

namespace Enki
{

// -----------------------------------------------------------------------------

    WorldExt::WorldExt(double r, 
                       const string& pub_address, 
                       const string& sub_address,
                       const Color& wallsColor, 
                       const World::GroundTexture& groundTexture,
                       unsigned int skewMonitorRate,
                       double skewReportThreshold)
         : ExtendedWorld(r, wallsColor, groundTexture, skewMonitorRate, skewReportThreshold),
          pub_address_(pub_address), sub_address_(sub_address)
    {
        GOOGLE_PROTOBUF_VERIFY_VERSION;

        context_ = new zmq::context_t(1);
        publisher_ = new socket_t(*context_, ZMQ_PUB);
        subscriber_ = new socket_t(*context_, ZMQ_SUB);

        publisher_->bind(pub_address_.c_str());
        //int buff_size = 1;
        //publisher_->setsockopt(ZMQ_SNDHWM, &buff_size, sizeof(int));
        subscriber_->bind(sub_address_.c_str());
        subscriber_->setsockopt(ZMQ_SUBSCRIBE, "Sim", 3);
    }

// -----------------------------------------------------------------------------

    WorldExt::~WorldExt()
    {
        // We own the handlers, so delete them
        BOOST_FOREACH(const HandlerMap::value_type& rh, handlers_)
        {
            delete rh.second;
        }

        delete subscriber_;
        delete publisher_;
        delete context_;
     }

    void WorldExt::addObject(PhysicalObject *po)
    {
        ExtendedWorld::addObject(po);
    }

// -----------------------------------------------------------------------------

    bool WorldExt::addHandler(string type, ObjectHandler* handler)
    {
        if (handlers_.count(type) > 0)
        {
            return false;
        }
        else
        {
            handlers_[type] = handler;
        }
    }

// -----------------------------------------------------------------------------

    /* virtual */
    void WorldExt::controlStep(double dt)
    {
        // TODO: Check if this update sequence is correct

        // Read all incoming messages & apply to robots
        // Icoming messages represent controller outputs
        int in_count = 0;

        string name;
        string device;
        string command;
        string data;
        int len = recv_multipart(*subscriber_, name, device,
                                 command, data, ZMQ_DONTWAIT);
        while (len > 0)
        {
            in_count++;
            if (name == "Sim")
            {
                handleSim_(device, command, data);
            }
            else if (handlers_by_object_.count(name) > 0)
            {
                handlers_by_object_[name]->handleIncoming(name, device, 
                                                          command, data);
            }
            else
            {
                cerr << "Unknown object: " << name << endl;
            }    
            len = recv_multipart(*subscriber_, name, device,
                                 command, data, ZMQ_DONTWAIT);
        }

        // Publish sensor data
        int out_count = 0;

        sendSim_ (*publisher_);
        // Invoke all handlers to send messages
        BOOST_FOREACH(const HandlerMap::value_type& rh, handlers_)
        {
            rh.second->sendOutgoing(*publisher_);
        }

    }

// -----------------------------------------------------------------------------

    bool WorldExt::handleSim_(const string& device, 
                              const string& command,
                              const string& data)
    {
        if (device == "Spawn")
        {
            // Device is Spawn, command is object type
            // Read command contents and spawn object
            const string& object_type = command;
            if (handlers_.count(object_type) > 0)
            {
                string name = handlers_[object_type]->createObject(data, this);
                if (name.length() > 0)
                {
                    // New robot was spawned
                    handlers_by_object_[name] = handlers_[command];
                    subscriber_->setsockopt(ZMQ_SUBSCRIBE, 
                                            name.c_str(), 
                                            name.length());
                }
            }
            else
            {
                cerr << "Unknown object type " << object_type << endl;
            }
        }
        else if (device == "Teleport")
        {
            // Device is Teleport, command is object name
            const string name = command;
            if (handlers_by_object_.count(name) > 0)
            {
                PoseStamped pose;
                assert(pose.ParseFromString(data));
                handlers_by_object_[name]->getObject(name)->pos = Point(pose.pose().position().x(),
                                                                        pose.pose().position().y());
                handlers_by_object_[name]->getObject(name)->angle = pose.pose().orientation().z();
            }
            else
            {
                cerr << "Unknown object " << name << endl;
            }
        }
        else if (device == "Heat")
        {
           if (command == "reset")
           {
              Temperature temp_msg;
              assert (temp_msg.ParseFromString (data));
              PhysicSimulationsIterator iterator = this->physicSimulations.begin ();
              PhysicSimulationsIterator end = this->physicSimulations.end ();
              while (iterator != end) {
                 WorldHeat *worldHeat = dynamic_cast<WorldHeat *> (*iterator);
                 if (worldHeat != NULL) {
                    worldHeat->resetTemperature (temp_msg.temp ());
                 }
                 iterator++;
              }
           }
        }
        else
        {
            cerr << "Unknown sim device!" << endl; 
        }
        return true;
    }

    int WorldExt::sendSim_(zmq::socket_t& socket)
    {
        Time sd;
        double at = this->getAbsoluteTime ();
        long int sec = trunc (at);
        long int nsec = (at - sec) * 1000000000;
        sd.set_sec (sec);
        sd.set_nsec (nsec);
        std::string data;
        sd.SerializeToString (&data);
        zmq::send_multipart (socket, "Sim", "AbsoluteTime", "Value", data);
    }
// -----------------------------------------------------------------------------

}

/*

 */
#include <iostream>

#include <boost/foreach.hpp>

#include <zmq.hpp>
#include "playground/zmq_helpers.hpp"

#include "playground/WorldExt.h"
#include "robots/Fish.h"
#include "handlers/FishHandler.h"

// Protobuf message headers
#include "base_msgs.pb.h"
#include "dev_msgs.pb.h"
#include "sim_msgs.pb.h"

using namespace zmq;

using std::string;
using std::cerr;
using std::endl;
using std::cout;

using namespace AssisiMsg;

namespace Enki
{

    /* virtual */
    string FishHandler::createObject(const std::string& data, 
                                     WorldExt* world)
    {
        string name = "";
        Spawn spawn_msg;     
        assert(spawn_msg.ParseFromString(data));
        if (fishes_.count(spawn_msg.name()) < 1)
        {
            name = spawn_msg.name();
            Point pos(spawn_msg.pose().position().x(),
                      spawn_msg.pose().position().y());
            double yaw(spawn_msg.pose().orientation().z());
            fishes_[name] = new Fish;
            fishes_[name]->pos = pos;
            fishes_[name]->angle = yaw;
            world->addObject(fishes_[name]);
        }
        else
        {
            cerr << "Fish "<< spawn_msg.name() << " already exists." << endl;
        }
        return name;
    }

// -----------------------------------------------------------------------------

    /* virtual */
    int FishHandler::handleIncoming(const std::string& name,
                                    const std::string& device,
                                    const std::string& command,
                                    const std::string& data)
    {
        int count = 0;
        if (device == "Base")
        {
            if (command == "Vel")
            {
                DiffDrive drive;
                assert(drive.ParseFromString(data));
                fishes_[name]->leftSpeed = drive.vel_left();
                fishes_[name]->rightSpeed = drive.vel_right();
                count++;
            }
            else
            {
                cerr << "Unknown command for " << name << "/" << device << endl;
                return 0;
            }
        }
        else if (device == "Color")
        {
            if (command == "Set")
            {
                ColorStamped color_msg;
                assert(color_msg.ParseFromString(data));
                fishes_[name]->setColor(color_msg.color().red(),
                                      color_msg.color().green(),
                                      color_msg.color().blue());
            }
        }
        else
        {
            cerr << "Unknown device " << device << endl;
        }
        return count;
    }

// -----------------------------------------------------------------------------

    /* virtual */
    int FishHandler::sendOutgoing(socket_t& socket)
    {
        int count = 0;
        BOOST_FOREACH(const FishMap::value_type& ca, fishes_)
        {
            std::string data;

            /* Publishing Object Sensor readings */
            ObjectArray objects;
            BOOST_FOREACH(ObjectSensor* obj, ca.second->object_sensors)
            {
                objects.add_range(obj->getDist());                
                objects.add_type(obj->getType());
            }
            objects.SerializeToString(&data);
            send_multipart(socket, ca.first, "Object", "Ranges", data);
            count++;

            /* Publish velocity setpoints */
            DiffDrive drive;
            drive.set_vel_left(ca.second->leftSpeed);
            drive.set_vel_right(ca.second->rightSpeed);
            drive.SerializeToString(&data);
            send_multipart(socket, ca.first, "Base", "VelRef", data);
            count++;

            /* Publish velocities */
            drive.set_vel_left(ca.second->leftEncoder);
            drive.set_vel_right(ca.second->rightEncoder);
            send_multipart(socket, ca.first, "Base", "Enc", data);
            count++;

            /* Publish ground truth */
            PoseStamped pose;
            pose.mutable_pose()->mutable_position()->set_x(ca.second->pos.x);
            pose.mutable_pose()->mutable_position()->set_y(ca.second->pos.y);
            pose.mutable_pose()->mutable_orientation()->set_z(ca.second->angle);
            pose.SerializeToString(&data);
            send_multipart(socket, ca.first, "Base", "GroundTruth", data);

            /* Publish Diagnostic color "actuator" set value */
            ColorStamped color;
            color.mutable_color()->set_red(ca.second->color_r_);
            color.mutable_color()->set_green(ca.second->color_g_);
            color.mutable_color()->set_blue(ca.second->color_b_);
            color.SerializeToString(&data);
            send_multipart(socket, ca.first, "Color", "ColorVal", data);
            
            /* Publish other stuff as necessary */
        }

        return count;
    }
// -----------------------------------------------------------------------------

    /* virtual */
    PhysicalObject* FishHandler::getObject(const std::string& name)
    {
        if (fishes_.count(name) > 0)
        {
            return fishes_[name];
        }
        else
        {
            return 0;
        }
    }

// -----------------------------------------------------------------------------

}

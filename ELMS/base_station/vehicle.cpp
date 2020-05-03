/*
* ELMS - Trevor Frame, Andrew Freitas, Deborah Kretzschmar
*/

#include "vehicle.h"
#include <functional>
using std::vector;

/* initialize class
 * priority 0 = vehicle is 50 meters or less to another vehicle
 * priority 1 = vehicle is 51 - 75 meters away from another vehicle
 * priority 2 = vehicle 76 - 100 meters away from another vehicle
 * priority 3 = vehicle is greater than 100 meters away from another vehicle
 * priority 4 = the distance to any other vehicles has not been set yet */
Vehicle::Vehicle() {
    unit = -1;
    time = -1;
    latitude = -1;
    longitude = -1;
    velocity = -1;
    bearing = -1;
    priority = 4; //initialize the priority to 4
}

//construct vehicle
Vehicle::Vehicle(int vehicleUnit, int vehicleTime, double vehicleLatitude, double vehicleLongitude, double vehicleVelocity, double vehicleBearing, int priorityNum) {
    unit = vehicleUnit;
    time = vehicleTime;
    latitude = vehicleLatitude;
    longitude = vehicleLongitude;
    velocity = vehicleVelocity;
    bearing = vehicleBearing;
    priority = priorityNum;
}

//desctructor
Vehicle::~Vehicle() {
    std::cout << "Vehicle destructor called " << std::endl;

}

/*
=============
set...()
Below are setter functions for setting vehicle object members
=============
*/
void Vehicle::setUnit(int vehicleUnit) {
    unit = vehicleUnit;
}

void Vehicle::setTime(int vehicleTime) {
    time = vehicleTime;
}

void Vehicle::setLatitude(double vehicleLatitude) {
    latitude = vehicleLatitude;
}

void Vehicle::setLongitude(double vehicleLongitude) {
    longitude = vehicleLongitude;
}

void Vehicle::setVelocity(double vehicleVelocity) {
    velocity = vehicleVelocity;
}

void Vehicle::setBearing(double vehicleBearing) {
    bearing = vehicleBearing;
}

void Vehicle::setPriority(int p)
{
    priority = p;
}

/*
=============
get...()
Below are getter functions for getting vehicle object members
=============
*/
int Vehicle::getUnit() {
    return unit;
}

int Vehicle::getTime() {
    return time;
}

double Vehicle::getLatitude() {
    return latitude;
}

double Vehicle::getLongitude() {
    return longitude;
}

double Vehicle::getVelocity() {
    return velocity;
}

double Vehicle::getBearing() {
    return bearing;
}

int Vehicle::getPriorityNumber()
{
    return priority;
}

map<int, double>* Vehicle::getMapOfVehicles()
{
    return &distance_to_other_vehicles;
}

int Vehicle::getMapSize(Vehicle& v)
{
    return v.getMapOfVehicles()->size();
}


/*
=============
sortVehicleVector
This sorts a vector of Vehicles by their priority number.
It uses a bool operator defined in the class public members
=============
*/
void Vehicle::sortVehicleVector(vector<Vehicle*>v)
{

    sort(v.begin(), v.end(), compById);
}

/* reference on updating maps
   https://stackoverflow.com/questions/4527686/how-to-update-stdmap-after-using-the-find-method  */
void Vehicle::updateVehicleMap(Vehicle* v, int vehicle_id, double distance)
{
    bool found = false;
    auto itr = v->distance_to_other_vehicles.find(vehicle_id);
    if (itr != v->distance_to_other_vehicles.end())
    {
        (*itr).second = distance;
        found = true;
    }

    if (found == false)
    {
        v->getMapOfVehicles()->insert(std::make_pair(vehicle_id, distance));
    }

}


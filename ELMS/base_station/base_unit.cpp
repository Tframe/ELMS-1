#include "base_unit.h"
#include <iostream>
#include <stdbool.h>
#include <thread>
#include <mutex>
#include <fstream>
#include <string>
#include <ctime>
#include <cstdio>
#include <cstring>
#include <time.h>

using std::thread;
using std::cout;
using std::endl;

// declare a mutex that locks when the program is writing to a log file
std::mutex mtx_close;
std::mutex mtx_write;
std::mutex mtx_update_master;
std::mutex mtx_udate_vehicle;

/* Static members must be initialized outside of the class
 * Since it is a static member, it must be initialized outside of the class
 */

int Base_Unit::messageCount = 1;
int Base_Unit::alertCount = 1;
int Base_Unit::networkCount = 1;
int Base_Unit::miscCount = 1;
string Base_Unit::logFileName = "";
string Base_Unit::alertFile = "";
string Base_Unit::netFailFile = "";
string Base_Unit::miscErrorFile = "";
vector<Vehicle> Base_Unit::mine_vehicles;


/*
 * Function: checkMessageCount(int type)
 * This function takes the message and checks to see if the messages have
 * reached their limit.
 * types are:
 * type = 0 means that it is an incoming message
 * type = 1 means that it is an alert being sent to a vehicle
 * type = 2 means that it is a network failure message
 * type = 3 means that it is a miscellaneous error.
 * If message count is < MESSAGE_LIMIT, increment the message count and return false.
 * Otherwise, true is returned.
 */
bool Base_Unit::checkMessageCount(int type)
{
    if (getMessageCount(type) < MESSAGE_LIMIT)
    {
        incMessageCount(type);
        return false;
    }
    else
    {
        return true;

    }

}

/*
 * Function logFile
 * This function checks the message count. If it is < MESSAGE_LIMIT, then the inputMessage
 * is written to a log file and the message count is incremented. If the
 * message count is >= MESSAGE_LIMIT, then this last message is written to the log file
 * and the log file is closed.  This log file will be stored in the log file
 * directory and at regular intervals uploaded to the database.  When it is
 * closed it will be saved with a name created by using the local time. (we
 * may need to change this is Zulu time later)
 * References: https://www.cplusplus.com/reference/mutex/mutex/lock/
 * https://cplusplus.com/forum/general/194132
 * Important to use a reference wrapper when passing in threads.
 * https://stackoverflow.com/questions/34078208/passing-object-by-reference-to-stdthread-in-c11

 * type = 0 means that it is an incoming message
 * type = 1 means that it is an alert being sent to a vehicle
 * type = 2 means that it is a network failure message
 * type = 3 means that it is a miscellaneous error.
 *
 * string fileName is created by using b.getFileName(&fileName, 0); where b
 * is a Base_Unit object and the second parameter is the message type
*/

void Base_Unit::logFile(string& fileName, string* inputMessage, int type)
{
    bool needNewFile = false;

    // flag that will be true when both writing and closing to a file have finished. 
    bool done = false;

    //check to see if the number of messages written to the file is at the limit
    // It checks for the message type to determine if more should be written to
    // that particular log file. 
    bool messageCountAtLimit = checkMessageCount(type);

    //this loop while continue until both the writing and the closing of the 
    // log file have finished.  It prevents another message trying to write to
    // a file that is being closed. 
    while (messageCountAtLimit && done == false)
    {
        //declare a thread that will write this last message to the log file
        thread t1 = std::thread(&Base_Unit::lockWriteFile, this, std::ref(fileName), inputMessage);

        // have the thread join
        t1.join();

        done = true;

        //set the bool to true that a new file is needed
        needNewFile = true;
    }

    //messageCount was less than MESSAGE_LIMIT and the count was incremented. 
    // Call the function that writes the data to the current log file.
    if (!messageCountAtLimit)
    {
        bool d = false;
        while (!d)
        {
            thread t1 = std::thread(&Base_Unit::lockWriteFile, this, std::ref(fileName), inputMessage);

            // have the thread join again
            t1.join();
            d = true;
        }

    }
    // if needNewFile is true, then get a new fileName 
    if (needNewFile)
    {
        getFilePath(fileName, type);

    }
}

/* Function lockCloseFile
 * This function locks while closing a file so that it can be stored in its folder
 * type = 0 means that it is an incoming message
 * type = 1 means that it is an alert being sent to a vehicle
 * type = 2 means that it is a network failure message
 * type = 3 means that it is a miscellaneous error.
 * Reference: https://www.daniweb.com/programming/software-development/threads/476954/convert-from-localtime-to-localtime-s
 */

 /* Function lockWriteFile locks any other part of the program writing to
  * log files while another process is writing to it. */
void Base_Unit::lockWriteFile(string& filePath, string* inputMessage)
{
    //lock writing to a log file
    mtx_write.lock();

    storeMessage(filePath, *inputMessage);

    //open the lock
    mtx_write.unlock();

    return;

}

/*Function is a getter for messageCount
 * type = 0 means that it is an incoming message
 * type = 1 means that it is an alert being sent to a vehicle
 * type = 2 means that it is a network failure message
 * type = 3 means that it is a miscellaneous error.
*/
int Base_Unit::getMessageCount(int type)
{
    if (type == 0)
    {
        //cout << "The message count is: " << messageCount << endl;
        return messageCount;
    }
    else if (type == 1)
    {
        return alertCount;
    }
    else if (type == 2)
    {
        return networkCount;
    }
    else
    {
        return miscCount;
    }
}

/*This function increments the current messageCount by 1 depending on the type
 * type = 0 means that it is an incoming message
 * type = 1 means that it is an alert being sent to a vehicle
 * type = 2 means that it is a network failure message
 * type = 3 means that it is a miscellaneous error.
*/
void Base_Unit::incMessageCount(int type)
{
    if (type == 0)
    {
        messageCount++;
    }
    else if (type == 1)
    {
        alertCount++;
    }
    else if (type == 2)
    {
        networkCount++;
    }
    else
    {
        miscCount++;
    }
}

/* Function creates a file name based on the UTC time by year, month, day, hour,
 * min, and second.  It also adds the type of message that it is.
 * type = 0 = I means that it is an incoming message
 * type = 1 = A means that it is an alert being sent to a vehicle
 * type = 2 = N means that it is a network failure message
 * type = 3 = M means that it is a miscellaneous error.
 * .txt is also appended as the extention for the files.
 * Reference: https://stackoverflow.com/questions/997946/how-to-get-current-time-and-date-in-c

 * https://docs.microsoft.com/en-us/previous-versions/visualstudio/visual-studio-2013/3stkd9be(v=vs.120)
*/
void Base_Unit::createFileName(string* fileName, int type)
{
    // we are creating a new FileName which means we need to reset the count
    resetMessageCount(type);

    string type_of_message;
    if (type == 0)
        type_of_message = "I";
    else if (type == 1)
        type_of_message = "A";
    else if (type == 2)
        type_of_message = "N";
    else
        type_of_message = "M";

    //get the current time
    time_t now = time(0);

    //declare a time structure
    struct tm gmtm;

    //use a thread safe call to get the current UTC time
    gmtime_s(&gmtm, &now);

    int year = gmtm.tm_year + 1900;
    int month = gmtm.tm_mon + 1;
    int day = gmtm.tm_mday;
    int hour = gmtm.tm_hour;
    int min = gmtm.tm_min;
    int sec = gmtm.tm_sec;

    //build the string needed for the name of the file. 
    string tempName = std::to_string(year) + '_' + std::to_string(month) + '_' + std::to_string(day) + '_' +
        std::to_string(hour) + '_' + std::to_string(min) + '_' + std::to_string(sec) + '_' + type_of_message;

    //add to the end of the string the file extension type. 
    tempName = tempName + ".txt";

    //set the input string to the temporary string name created using the date/time
    *fileName = tempName;

}

/* Function resets messageCount to 1 when a new log file is opened depending on message type
 * type = 0 = I means that it is an incoming message
 * type = 1 = A means that it is an alert being sent to a vehicle
 * type = 2 = N means that it is a network failure message
 * type = 3 = M means that it is a miscellaneous error. */
void Base_Unit::resetMessageCount(int type)
{
    if (type == 0)
    {
        messageCount = 1;
    }
    if (type == 1)
    {
        alertCount = 1;
    }
    if (type == 2)
    {
        networkCount = 1;
    }
    else
    {
        miscCount = 1;
    }
}

/*This function first checks to see if a directory exists, then it creates the
 * logs directory and the 4 subfolders (1) incoming_messages (2) alerts (3) network_failure
 * (4) misc_errors. It will create the main logs file in the location that the user
 * specified. */
 // https://www.youtube.com/watch?v=B999K9yztnI
void Base_Unit::createFolder()
{
    // bool used to see if a directory exists
    bool exists = false;

    // a string used for the path to directories
    string msg;

    //create a vector that holds the paths to the directories that we need
    // to create and then iterate thru them. Create the directories that do
    // not exist. 
    vector<string>messages;
    messages.push_back(msg = getPathToLogs());
    messages.push_back(msg = getPathToMessages());
    messages.push_back(msg = getPathToAlerts());
    messages.push_back(msg = getPathToMiscErrors());
    messages.push_back(msg = getPathToNetWorkFailure());

    // 
    for (size_t i = 0; i < messages.size(); i++)
    {
        exists = directoryExists(messages[i]);
        int directory;

        // if the exists is false, then create the directory
        if (!exists)
        {
            directory = CreateDirectory(messages[i].c_str(), NULL);

            // if an error occurs creating the directory then notify the user
            if (directory == 0)
            {
                //TO DO: use GetLastError(), format it and determine what the actual
               // error is. 
                cout << " CreateDirectory Failed. The error number is:  " << GetLastError() << endl;
            }

            // otherwise let the user know the directory was successfully made.
            else
            {
                if (pathToLogs.compare(messages[i]) == 0)
                    cout << "Directory C:\\logs\\ was successfully created" << endl;
                else if (getPathToMessages().compare(messages[i]) == 0)
                    cout << "Sub-directory C:\\logs\\incoming_messages was successfully created" << endl;
                else if (getPathToAlerts().compare(messages[i]) == 0)
                    cout << "Sub-directory C:\\logs\\alerts was successfully created" << endl;
                else if (getPathToNetWorkFailure().compare(messages[i]) == 0)
                    cout << "Sub-directory C:\\logs\\network_failure was successfully created" << endl;
                else
                    cout << "Sub-directory C:\\logs\\misc_errors was successfully created" << endl;
            }
        }
    }
}

/*This function first checks to see if the directories exist before creating them
 * The GetFileAttributesA function returns attributes for specific file or
 * directory. If the call is successful, then the attribute is returned.
 * In this case we are looking for it to return "ERROR_PATH_NOT_FOUND" ,
 * "ERROR_FILE_NOT_FOUND", "ERROR_INVALID_NAME", or "ERROR_BAD_NETPATH" in order
 * to know that the file does not exist. */
 // reference
 // https://stackoverflow.com/questions/8233842/how-to-check-if-directory-exist-using-c-and-winapi
 //https://www.youtube.com/watch?v=B999K9yztnI
 //https://docs.microsoft.com/en-us/windows/win32/api/fileapi/nf-fileapi-getfileattributesa
bool Base_Unit::directoryExists(const std::string& directoryName)
{
    DWORD directory_attribute = GetFileAttributesA(directoryName.c_str());
    if (directory_attribute == INVALID_FILE_ATTRIBUTES)
    {
        //option to use GetLastError(), format it and determine what the actual
        // error is. 
        return false;
    }
    //if the directory attribute returns > 0, then this means the directory
    // exists and we return true. 
    if (directory_attribute & FILE_ATTRIBUTE_DIRECTORY)
    {
        return true;
    }
    return false;
}

/*This function allows the path to a logs directory to be set
 * The default set was C:\\logs
 */
void Base_Unit::setPathToLogs(string& path)
{
    pathToLogs = path;
}

/*This function gets the path to the directory logs*/
string Base_Unit::getPathToLogs()
{
    return pathToLogs;
}

/*This function gets the path to the logs/incoming_messages directory*/
string Base_Unit::getPathToMessages()
{
    string messages = getPathToLogs() + "\\incoming_messages\\";
    return messages;
}

/* This function gets the path to the logs/alerts directory*/
string Base_Unit::getPathToAlerts()
{
    string messages = getPathToLogs() + "\\alerts\\";
    return messages;
}

/* This function gets the path to the logs/network_failure directory*/
string Base_Unit::getPathToNetWorkFailure()
{
    string messages = getPathToLogs() + "\\network_failure\\";
    return messages;

}

/* This functions gets the path to the logs/misc_errors directory*/
string Base_Unit::getPathToMiscErrors()
{
    string messages = getPathToLogs() + "\\misc_errors\\";
    return messages;
}

/*This functions returns the name of the current file that is open for a
 * specific type.  For example, if you needed to append to an alert log, you
 * would get the name of the current file that is open for that type using this
 * function.  */
string Base_Unit::getCurrentFileName(int type)
{
    if (type == 0)
    {
        return logFileName;
    }
    else if (type == 1)
    {
        return alertFile;
    }
    else if (type == 2)
    {
        return netFailFile;
    }
    else
    {
        return miscErrorFile;
    }
}
/* This function sets the file name for a log file when it is newly opened.
 *  An example of it's use would be if a previous log file was full and
 * closed and a new name created for a new file, then this function would
 * be called to set the file name for that particular type of log file.
 */
void Base_Unit::setFileName(int type)
{
    if (type == 0)
    {
        string tempName;
        createFileName(&tempName, type);
        logFileName = tempName;
    }
    else if (type == 1)
    {
        string tempName;
        createFileName(&tempName, type);
        alertFile = tempName;
    }
    else if (type == 2)
    {
        string tempName;
        createFileName(&tempName, type);
        netFailFile = tempName;
    }
    else
    {
        string tempName;
        createFileName(&tempName, type);
        miscErrorFile = tempName;
    }
}

void Base_Unit::getFilePath(string& fileName, int type)
{
    createFileName(&fileName, type);
    if (type == 0)
    {
        fileName = getPathToMessages() + fileName;
    }
    else if (type == 1)
    {
        fileName = getPathToAlerts() + fileName;
    }
    else if (type == 2)
    {
        fileName = getPathToNetWorkFailure() + fileName;
    }
    else
    {
        fileName = getPathToMiscErrors() + fileName;
    }
}

void Base_Unit::addToMineVehicles(Vehicle& v)
{
    mine_vehicles.push_back(v);
}

/* prints the contents held in the mine_vehicle vector*/
void Base_Unit::print_vector(vector<Vehicle>& v)
{
    if (v.size() == 0)
        return;
    //cout << "The size of the vector in print function is: " << v.size() << endl;
    cout << "**********************************" << endl << endl;

    for (auto itr : v)
    {
        cout << "*****VEHICLE " << itr.getUnit() << " *****" << endl;
        cout << std::fixed;
        cout << std::setprecision(5);
        cout << "latitude: " << itr.getLatitude() << endl;
        cout << "longitude: " << itr.getLongitude() << endl;
        cout << std::fixed;
        cout << std::setprecision(0);
        cout << "time_stamp: " << itr.getTime() << endl;
        cout << "velocity: " << itr.getVelocity() << endl;
        cout << "bearing: " << itr.getBearing() << endl;
        cout << "priority: " << itr.getPriorityNumber() << endl << endl;
    }
    cout << "***************************************" << endl << endl;

}

vector<Vehicle> Base_Unit::getMineVehicles()
{
    return mine_vehicles;
}

void Base_Unit::addToPriorityQueue(Vehicle& v)
{
    priority_list.push_back(v);
}

vector<Vehicle> Base_Unit::getPriorityQueue()
{
    return priority_list;
}


void Base_Unit::input_data(struct message* ptr, Vehicle& v, Port& p, HANDLE &h)
{
    Calculations calc;
    int size = -1;
    string alertMessage;
    string alertLogMessage;
    string alertFileName;
    char fileName[100];
    double speed, distance = 0, bearing;
    int index = 0;

    size = get_size(mine_vehicles);
    //if the size of the vector is 0, then we know we need to add the 
    // vehicle to the vector
    if (size == 0)
    {
        /* we create a Vehicle object and add it to our vector mine_vehicles */
        v.setBearing(ptr->bearing);
        v.setLatitude(ptr->latitude);
        v.setLongitude(ptr->longitude);
        v.setTime(ptr->time);
        v.setUnit(ptr->vehicle);
        v.setVelocity(ptr->velocity);

        //call the vehicle function that will check the distance that it is
        // from the other vehicles that are currently in the vehicles_in_mine
        // vector. This function should then update the vehicles priority number.
        // These functions should in turn call the Vehicles priority queue.
        // ultimately, this 
        // push the new vehicle into the mine_vehicles vector
        addToMineVehicles(v);

    }
    // otherwise, we check to see if the vehicle id already exists
    else
    {
        // call update_data
        thread t1 = std::thread(&Base_Unit::update_data, this, ptr, std::ref(v), std::ref(mine_vehicles));

        // have the thread join again
        t1.join();

        //now the newly added node or updated node needs to have their distance
        // checked to all the other nodes. 
        vector<int>list = checkDistancesInMasterVector(v, distance, mine_vehicles);
        
        //next we iterate through the list and send out alerts
        if (list.size() > 0)
        {
            for (size_t i = 0; i < list.size(); i++)
            {
                //need to send out an alert message
                // first calculate the speed which is in meters per second
                speed = calc.knots_to_mps(v.getVelocity());

                //The list contains the index in the master list of vehicles that is
                // in immediate danger of collision. Set the second vehicle to this
                // index
                Vehicle v1 = mine_vehicles.at(list[i]);
                

                // now we get the bearing
                bearing = calc.getBearing(&v, &v1);
                // we create the outgoing message;
                outgoing_message(alertMessage, v.getUnit(), v1.getUnit(), v.getTime(),
                    distance, speed, bearing);

                //copy the message to use to create the log file for the alert
                alertLogMessage = alertMessage;

                //convert the outgoing message to a char * so that it can be transmitted
                stringToCharPointer(alertMessage, fileName);
                // send the alerts
                DWORD test = p.writeToSerialPort(fileName, alertMessage.length() + 1, h);
                
                //we also need to create the alert message
                // remove the \n from the end of the message
                alertLogMessage = alertLogMessage.substr(0, alertLogMessage.size() - 2);
                //get the file path
                getFilePath(alertFileName, 1);
                // call logFile to create the log file
                logFile(alertFileName, &alertLogMessage, 1);
            }
        }
        // now we need to update the priority number for the vehicle in the 
        // master list of vehicles.
        // call updateMasterPriorityNums
        updateMasterPriority(v);

        // now we need to sort the mine_vehicles  -- putting this on hold for right now
        //vector<Vehicle> veh = getMineVehicles();
        //v.sortVehicleVector(veh);

        //now we need to add these vehicles to each other's priority queues as well
        // as add the vehicles to the master unit's priority queue. 
        addToPriorityQueue(v);

    }

}

void Base_Unit::update_data(struct message* ptr, Vehicle& v, vector<Vehicle>& mine)
{
    mtx_update_master.lock();
    int duplicate, index;
    duplicate = contains_id_number(mine, ptr->vehicle, index);
    //duplicate = contains_id_number(mine, v.getUnit(), index);
    cout << "Here is the index from update_data from checking the duplicate: " << index << endl;
    cout << "Here is the value of duplicate: " << duplicate << " and here is the unit id: " << ptr->vehicle << endl;
    //if the id number is not a duplicate, then create a new
    //vehicle object and add it to the master vector.
    if (duplicate == 0)
    {
        v.setBearing(ptr->bearing);
        v.setLatitude(ptr->latitude);
        v.setLongitude(ptr->longitude);
        v.setTime(ptr->time);
        v.setUnit(ptr->vehicle);
        v.setVelocity(ptr->velocity);
        addToMineVehicles(v);
    }
    //otherwise, this is a duplicate id and we need to update
    // the Vehicle objects location data
    else
    {
        //To do: update Vehice objects data
        std::cout << "Duplicate id number: " << ptr->vehicle << " and here is the index in the mine_vehicle " << index << std::endl;
        setVehicleInMineVehicles(v, ptr->time, ptr->latitude, ptr->longitude, ptr->velocity, ptr->bearing, -1);
    }
    mtx_update_master.unlock();

}
// returns the size of the vector
int Base_Unit::get_size(vector<Vehicle>& v)
{
    return v.size();
}

//check to see if vector contains a specific id number
int Base_Unit::contains_id_number(vector<Vehicle>& v, int id, int& index)
{
    for (size_t i = 0; i < v.size(); i++)
    {
        if (v.at(i).getUnit() == id)
        {
            index = i;
            cout << "the index is: " << index << endl;
            return 1;
        }
    }
    return 0;
}

/* function iterates through the master list and checks the distance each
 * vehicle is from the other vehicles, it adds any vehicles that are close to that
 * input vehicle to a vector that contains ints which represent the index number
 * in the vector of vehicles where there is risk of collision*/
vector<int> Base_Unit::checkDistancesInMasterVector(Vehicle& v, double& distance, vector<Vehicle> & mine)
{
    Calculations c;
    //This vector stores the vehicle index where they are stored in
    //that are too close to this vehicle,
    // an alert notice will be sent out
    vector<int>list;
    bool set = false;
    
    //iterate through the master list of vehicles
    for (size_t i = 0; i < mine.size(); i++)
    {
        cout << "here is i, v id and second v1 id respsective: " <<i << " and " << v.getUnit() << " and " << mine.at(i).getUnit() << endl;
        if (mine_vehicles.at(i).getUnit() != v.getUnit())
        {
            
            distance = c.haversine(&v, &mine.at(i));
            //distance returns as km, need to change to meters so multiply
            // by 1000
            distance = 1000 * distance;
            cout << endl << endl << "here is the calculated distance: between " << v.getVehicleID() << " and " << mine.at(i).getVehicleID() << ": " << distance << endl << endl;
            //if the result is 50 or less than the index of the vehicle in the 
            // master list is pushed onto the list of vehicles that need notification
            if (distance <= 50)
            {

                list.push_back(i);
                //cout << "Here is the value of list(i): " << list[i] << endl;
                // we need to update that vehicles priority number to a 0
                cout << "Here is the priority value at that index: " << mine.at(i).getPriorityNumber() << endl;
                setVehicleInMineVehicles(v, -1, -1, -1, -1, -1, 0);
                //mining_vehicles.at(index).setPriority(0);
                cout << "Here is the priority value after setting it at that index: " << mine.at(i).getPriorityNumber() << endl;

                // a 0 priority was set, so this needs to remain
                set = true;

            }
            else if (distance > 50 && distance <= 75 && !set)
            {
                
                //mining_vehicles.at(index).setPriority(1);
                setVehicleInMineVehicles(v, -1, -1, -1, -1, -1, 1);
                

            }
            else if (distance > 75 && distance < 100 && !set && v.getPriorityNumber()>2)
            {
                //mining_vehicles.at(index).setPriority(2);
                setVehicleInMineVehicles(v, -1, -1, -1, -1, -1, 2);
                
            }
            else if (distance >= 100 && !set && v.getPriorityNumber() > 3)
            {
                //mining_vehicles.at(index).setPriority(3);
                setVehicleInMineVehicles(v, -1, -1, -1, -1, -1, 3);
          
            }
        }
    }
    return list;
}

void Base_Unit::updateMasterPriority(Vehicle& v)
{
    for (size_t i = 0; i < mine_vehicles.size(); i++)
    {
        if (mine_vehicles.at(i).getUnit() == v.getUnit())
        {
            mine_vehicles.at(i) = v;
        }
    }
}

/* This updates the master vehical vector vehicles at a specific index.  If you only
 *  want to update one value, set the other inputs to -1 and nothing will be changed. */
void Base_Unit::setVehicleInMineVehicles(Vehicle &v, int time, double latitude, double longitude, double velocity, double bearing, int priority)
{
    if (time != -1)
    {
       v.setTime(time);
    }
    if (latitude != -1)
    {
        v.setLatitude(latitude);
    }
    if (longitude != -1)
    {
        v.setLongitude(longitude);
    }
    if (velocity != -1)
    {
        v.setVelocity(velocity);
    }
    if (bearing != -1)
    {
        v.setBearing(bearing);
    }
    if (priority != -1)
    {
        cout << "I reset the priority number here" << endl;
        //cout << "The index is  " << index << " and the priority number I wish to set is: " << priority << endl;
        v.setPriority(priority);
    }
}








/*
 * ELMS - Trevor Frame, Andrew Freitas, Deborah Kretzschmar
*/

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
std::mutex mtx_update_master;
std::mutex mtx_udate_vehicle;

/* Static members must be initialized outside of the class
 * Since it is a static member, it must be initialized outside of the class
 */

<<<<<<< HEAD
int Base_Unit::messageCount = 1;
int Base_Unit::alertCount = 1;
int Base_Unit::networkCount = 1;
int Base_Unit::miscCount = 1;
string Base_Unit::logFileName = "";
string Base_Unit::alertFile = "";
string Base_Unit::netFailFile = "";
string Base_Unit::miscErrorFile = "";
vector<Vehicle*> Base_Unit::mine_vehicles;
||||||| merged common ancestors
int Base_Unit::messageCount = 1;
int Base_Unit::alertCount = 1;
int Base_Unit::networkCount = 1;
int Base_Unit::miscCount = 1;
string Base_Unit::logFileName = "";
string Base_Unit::alertFile = "";
string Base_Unit::netFailFile = "";
string Base_Unit::miscErrorFile = "";
vector<Vehicle> Base_Unit::mine_vehicles;
=======

vector<Vehicle> Base_Unit::mine_vehicles;
>>>>>>> origin/FileHandle

Base_Unit::~Base_Unit()
{
    //cout << "Base_Unit destructor was called" << endl;
    std::for_each(mine_vehicles.begin(), mine_vehicles.end(), deleteVector());
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

<<<<<<< HEAD
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
/* This function takes a fileName and its type (incoming message = 0, alert = 1,
   network failure = 2, and misc = 3) and return the file path.  This file
   path is used so that the file will ultimately be stored in the correct 
   folder*/
void Base_Unit::getFilePath(string& fileName, int type)
{
    createFileName(&fileName, type);
    if (type == 0)
    {
        fileName = getPathToMessages() + fileName;
        logFileName = fileName;
    }
    else if (type == 1)
    {
        fileName = getPathToAlerts() + fileName;
        alertFile = fileName;
    }
    else if (type == 2)
    {
        fileName = getPathToNetWorkFailure() + fileName;
        netFailFile = fileName;
    }
    else
    {
        fileName = getPathToMiscErrors() + fileName;
        miscErrorFile = fileName;
    }
}


// Add the vehicle object pointer to the vector.
void Base_Unit::addToMineVehicles(Vehicle* v)
||||||| merged common ancestors
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
=======
void Base_Unit::addToMineVehicles(Vehicle& v)
>>>>>>> origin/FileHandle
{
    mine_vehicles.push_back(v);
}

/* prints the contents held in the mine_vehicle vector*/
void Base_Unit::print_vector(vector<Vehicle*> v)
{
    if (v.size() == 0)
        return;
    //cout << "The size of the vector in print function is: " << v.size() << endl;
    cout << "**********************************" << endl << endl;

    for (auto itr : v)
    {
        cout << "*****VEHICLE " << itr->getUnit() << " *****" << endl;
        cout << std::fixed;
        cout << std::setprecision(5);
        cout << "latitude: " << itr->getLatitude() << endl;
        cout << "longitude: " << itr->getLongitude() << endl;
        cout << std::fixed;
        cout << std::setprecision(0);
        cout << "time_stamp: " << itr->getTime() << endl;
        cout << "velocity: " << itr->getVelocity() << endl;
        cout << "bearing: " << itr->getBearing() << endl;
        cout << "priority: " << itr->getPriorityNumber() << endl << endl;
        if (itr->getMapSize(*itr) > 0)
        {
            map<int, double>* mapVehicles = itr->getMapOfVehicles();
            printMap(mapVehicles);
        }
    }
    cout << "***************************************" << endl << endl;
}

/* This function returns mine_vehicles*/
vector<Vehicle*> Base_Unit::getMineVehicles()
{
    return mine_vehicles;
}


/* This function takes the new message and either inputs a new vehicle or 
 * updates a current one The reference to use maps is the following:
 * https://www.geeksforgeeks.org/map-associative-containers-the-c-standard-template-library-stl/  
 */
void Base_Unit::input_data(int indice, struct message* ptr, Port& p, HANDLE& h)
{
    Calculations calc;
    int size = -1;
    string alertMessage;
    string alertLogMessage;
    string alertFileName;
    char fileName[100];
    double speed, distance = 0, bearing;
    int index;
    
    //iterator that will be used to iterate thru a map
    map<int, double>::iterator itr;

    //size = get_size(mine_vehicles);
    size = get_size(mine_vehicles);
    //if the size of the vector is 0, then we know we need to add the 
    // vehicle to the vector
    if (size == 0)
    {
  
        mine_vehicles.at(indice)->setBearing(ptr->bearing);
        mine_vehicles.at(indice)->setLatitude(ptr->latitude);
        mine_vehicles.at(indice)->setLongitude(ptr->longitude);
        mine_vehicles.at(indice)->setTime(ptr->time);
        mine_vehicles.at(indice)->setUnit(ptr->vehicle);
        mine_vehicles.at(indice)->setVelocity(ptr->velocity);
        mine_vehicles.at(indice)->setPriority(3);

    }
    // otherwise, we check to see if the vehicle id already exists
    else
    {
        // call update_data
        thread t1 = std::thread(&Base_Unit::update_data, this, ptr, indice);

        // have the thread join again
        t1.join();

        //now the newly added node or updated node needs to have their distance
        // checked to all the other nodes. 
        //need to check for the index again where the vehicles is.
        contains_id_number(ptr->vehicle, index);


        //this map holds vehicles that are within 50 meters or less of the input
        // vehicle.  The int is the index in the master list of that vehicle and
        // the double is the distance.

        map<int, double>list = checkDistancesInMasterVector1(mine_vehicles.at(index));
        //next we iterate through the map with the distances and send out alerts
        if (list.size() > 0)
        {
            for(itr = list.begin(); itr != list.end(); ++ itr)
            {
                //need to send out an alert message
                // first calculate the speed which is in meters per second
                speed = calc.knots_to_mps(ptr->velocity);

                //The list contains the index in the master list of vehicles that is
                // in immediate danger of collision. Set the second vehicle to this
                // index
                Vehicle *v1 = mine_vehicles.at(itr->first);

                // now we get the bearing
                // bearing = calc.getBearing(&mine_vehicles.at(index), &v1);
                bearing = calc.getBearing(mine_vehicles.at(index), v1);
                // we create the outgoing message;
                outgoing_message(alertMessage, ptr->vehicle, v1->getUnit(), ptr->time, speed,
                        itr->second, bearing);

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
<<<<<<< HEAD
                if (getMessageCount(1) == MESSAGE_LIMIT + 1 || getMessageCount(1) == 1)
                {
                    getFilePath(alertFileName, 1);
                }
                else
                {
                    //get the current open file.
                    alertFileName = getCurrentFileName(1);

                }
||||||| merged common ancestors
                getFilePath(alertFileName, 1);
=======
                //getFilePath(alertFileName, 1);
>>>>>>> origin/FileHandle
                // call logFile to create the log file
                fileHandler.logToFile(alertLogMessage, MessageType::alert);
                //logFile(alertFileName, &alertLogMessage, 1);
            }
        }

        // now we need to sort the mine_vehicles
        std::sort(mine_vehicles.begin(), mine_vehicles.end(), Vehicle::compById);
        
        

        //now we need to add these vehicles to each other's priority queues as well
        // as add the vehicles to the master unit's priority queue. 
        //addToPriorityQueue(v);

    }
}
/* This function takes a pointer to a struct message that contains the data
 * that was contained in the most recent message.  The indice represents
 * the index of the mine_vehicles vector that the vehicle object was added to.
 * The function will check for duplicate id's. If the id is not a duplicate,
 * then the information is added to the newly created vehicle object. If it is
 * a duplicate, then the current information for that vehcile object is 
 * updated.  */
void Base_Unit::update_data(struct message* ptr, int indice)
{
    mtx_update_master.lock();
    int duplicate, index;
    //check for a duplicate id
    duplicate = contains_id_number(ptr->vehicle, index);
 
    //if duplicate is 0, this means that this is a new vehicle
    if (duplicate == 0)
    {
        mine_vehicles.at(indice)->setBearing(ptr->bearing);
        mine_vehicles.at(indice)->setLatitude(ptr->latitude);
        mine_vehicles.at(indice)->setLongitude(ptr->longitude);
        mine_vehicles.at(indice)->setTime(ptr->time);
        mine_vehicles.at(indice)->setUnit(ptr->vehicle);
        mine_vehicles.at(indice)->setVelocity(ptr->velocity);
        mine_vehicles.at(indice)->setPriority(3);
    }
    //otherwise, this is a duplicate id and we need to update
    // the Vehicle objects location data
    else
    {
        //first we need to erase the duplicate we added to the end of the
        // vector and then just update the existing vehicle at its index
<<<<<<< HEAD
        //delete the ptr created for the vector.
        delete mine_vehicles.at(mine_vehicles.size() - 1);
||||||| merged common ancestors
        int size = mine_vehicles.size();
=======
        size_t size = mine_vehicles.size();
>>>>>>> origin/FileHandle
        mine_vehicles.erase(mine_vehicles.begin() + mine_vehicles.size() - 1);
        setVehicleInMineVehicles2(mine_vehicles.at(index), ptr->time, ptr->latitude, ptr->longitude, ptr->velocity, ptr->bearing, -1);
    }
    mtx_update_master.unlock();
}
// returns the size of the vector
<<<<<<< HEAD
int Base_Unit::get_size(vector<Vehicle*>& v)
||||||| merged common ancestors
int Base_Unit::get_size(vector<Vehicle>& v)
=======
size_t Base_Unit::get_size(const vector<Vehicle>& v)
>>>>>>> origin/FileHandle
{
    return v.size();
}

/* This function takes a vehicle id and an address to an integer that represents
 * the index of the mine_vehicles vector where the vehicle is located. It iterates
 * thru the vector, finds the index, then sets the value of index to it. If the 
 * the vehicle id exists in the vector, then the function returns a 1 which means
 * true.  If it does not contain that vehicle, then a 0 is returned which means
 * false  */
int Base_Unit::contains_id_number(int id, int& index)
{
    for (size_t i = 0; i < getMineVehicles().size(); i++)
    {
        if (getMineVehicles().at(i)->getUnit() == id)
        {
            index = i;
            return 1;
        }
    }
    return 0;
}

/* Function iterates through the master list and checks the distance each
 * vehicle is from the other vehicles. Tt adds any vehicles that are close to that
 * input vehicle to a vector that contains ints which represent the index number
 * in the vector of vehicles (mine_vehicles) where there is risk of collision*/
//Reference for maps: https://www.geeksforgeeks.org/map-associative-containers-the-c-standard-template-library-stl/
map<int, double> Base_Unit::checkDistancesInMasterVector1(Vehicle* v)
{
    Calculations c;
    //this map holds the list of vehicles that will need alerts sent
    map<int, double> listOfAlerts;
    //these bools represent whether or not a priority of 0, 1, or 2 has been 
    // set in this iteration
    bool set0 = false;
    bool set1 = false;
    bool set2 = false;
    // an integer that holds a priority number
    int p;
    double distance;

    //iterate through the master list of vehicles
    for (size_t i = 0; i < getMineVehicles().size(); i++)
    {
        
        if (getMineVehicles().at(i)->getUnit() != v->getUnit())
        {
            // get the distance
            distance = c.haversine(v, mine_vehicles.at(i));
            //distance returns as km, need to change to meters so multiply
            // by 1000
            distance = 1000 * distance;

            //update the current vehicles distances from other vehicles
            v->updateVehicleMap(v, getMineVehicles().at(i)->getUnit(), distance);

            //we also need to update the other vehicles distances as well. 

            v->updateVehicleMap(getMineVehicles().at(i), v->getUnit(), distance);

            //if the result is 50 or less than the index of the vehicle in the 
            // master list and it's distance is inserted into the map
            if (distance <= 50)
            {
                listOfAlerts.insert(pair<int, double>(i, distance));
                // set both of the vehicles priority numbers to 0
                setVehicleInMineVehicles(i, -1, -1, -1, -1, -1, 0);
                setVehicleInMineVehicles2(v, -1, -1, -1, -1, -1, 0);
                // a 0 priority was set, so this needs to remain
                set0 = true;

            }

            else if (distance > 50 && distance <= 75)
            {
                /*need to search the vehicle at i's list to make sure it does
                 *not have a higher priority before changing. p represents the
                 *priority of the other vehicle this loop is comparing against.*/
                p = checkOtherVehiclesPriorityNumbers(v, i, 1);
                setVehicleInMineVehicles(i, -1, -1, -1, -1, -1, p);

                /* If the vehicle has not had a priority of 0 set yet, we can
                   set it to 1. */
                if (!set0)
                {
                    /*set the priority to 1*/
                    setVehicleInMineVehicles2(v, -1, -1, -1, -1, -1, 1);
                    set1 = true;
                }

            }
            
            else if (distance > 75 && distance < 100)
            {
                /* check the other vehicles distances from other vehicles before
                   we change it's overall priority number
                 */
                p = checkOtherVehiclesPriorityNumbers(v, i, 2);
                /* set the other vehicles priority number */
                setVehicleInMineVehicles(i, -1, -1, -1, -1, -1, p);

                /* for the current vehicle, if it has not already been set to
                 * a priority 0 or 1, then set to a priority 2 */
                if (!set0 && !set1)
                {
                    setVehicleInMineVehicles2(v, -1, -1, -1, -1, -1, 2);
                    set2 = true;
                }

            }
            
            else if (distance >= 100)
            {
                /* check the other vehicles distances from vehicles in its map
                   before we change it's overall priority number
                 */
                p = checkOtherVehiclesPriorityNumbers(v, i, 3);
                /*set the other vehicle to the lowest priority number based
                  on distances it is from other vehicles in its map*/
                setVehicleInMineVehicles(i, -1, -1, -1, -1, -1, p);

                /* for the current vehicle, it it has not yet been set to a 
                 * priority 0, 1, or 2, then set it to a priority 3 */
                if (!set0 && !set1 && !set2)
                {
                    setVehicleInMineVehicles2(v, -1, -1, -1, -1, -1, 3);
                }
            }
        }
    }
    return listOfAlerts;
}

/* This function checks the vehicle at the index of the mine_vehicle to see what
   the smallest distance is to any other vehicle.  It returns this distance*/
int Base_Unit::checkOtherVehiclesPriorityNumbers(Vehicle* v1, int index, int priority_number)
{
    double d;
    int priority = 3;
    d = v1->findSmallestDistance(mine_vehicles.at(index));
    if (d <= 50)
    {
        priority = 0;
    }
    else if (d > 50 && d <= 75 && priority_number > 1)
    {
        priority = 1;
    }
    else if (d > 75 && d <= 100 && priority_number > 2)
    {
        priority = 2;
    }
    else
    {
        priority = priority_number;
    }
    return priority;
}

/* This updates mine_vehicles (master vector) at a specific index.  If you only
 *  want to update one value, set the other inputs to -1 and nothing will be changed. */
void Base_Unit::setVehicleInMineVehicles(int index, int time, double latitude, double longitude, double velocity, double bearing, int priority)
{
    if (time != -1)
    {
        mine_vehicles.at(index)->setTime(time);
    }
    if (latitude != -1)
    {
        mine_vehicles.at(index)->setLatitude(latitude);
    }
    if (longitude != -1)
    {
        mine_vehicles.at(index)->setLongitude(longitude);
    }
    if (velocity != -1)
    {
        mine_vehicles.at(index)->setVelocity(velocity);
    }
    if (bearing != -1)
    {
        mine_vehicles.at(index)->setBearing(bearing);
    }
    if (priority != -1)
    {
        mine_vehicles.at(index)->setPriority(priority);
    }
}
/*This updates mine_vehicles (master vector) at a specific index.If you only
 * want to update one value, set the other inputs to - 1 and nothing will be changed.*/
void Base_Unit::setVehicleInMineVehicles2(Vehicle* v, int time, double latitude, double longitude, double velocity, double bearing, int priority)
{
    if (time != -1)
    {
        v->setTime(time);
    }
    if (latitude != -1)
    {
        v->setLatitude(latitude);
    }
    if (longitude != -1)
    {
        v->setLongitude(longitude);
    }
    if (velocity != -1)
    {
        v->setVelocity(velocity);
    }
    if (bearing != -1)
    {
        v->setBearing(bearing);
    }
    if (priority != -1)
    {
        v->setPriority(priority);
    }
}

void Base_Unit::logToFile(std::string message, MessageType type)
{
    fileHandler.logToFile(message, type);
}
/*
* ELMS - Trevor Frame, Andrew Freitas, Deborah Kretzschmar
Provides the Database class declarations
*/

#ifndef CONNECT_DATABASE_H
#define CONNECT_DATABASE_H

#include "vehicle.h"

#include <bsoncxx/stdx/optional.hpp>
#include <bsoncxx/types/bson_value/view.hpp>
#include <bsoncxx/builder/stream/document.hpp>

#include <bsoncxx/json.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/logger.hpp>
#include <mongocxx/pool.hpp>
#include <mongocxx/uri.hpp>
#include <mongocxx/stdx.hpp>
#include <mongocxx/exception/query_exception.hpp>
#include <bsoncxx/types.hpp>

#include <thread>
#include <time.h>

/* reference for using statement:
 *  https://jira.mongodb.org/browse/CXX-860
*/
using bsoncxx::builder::stream::finalize;
using bsoncxx::builder::stream::open_array;
using bsoncxx::builder::stream::close_array;
using std::chrono::milliseconds;
using std::string;

class Database {

    typedef mongocxx::pool::entry connection;
    typedef mongocxx::instance instance;
    typedef mongocxx::database database;
    typedef mongocxx::collection collection;
    typedef bsoncxx::builder::stream::document document;
    typedef struct bsoncxx::builder::stream::open_document_type open_document;
    typedef struct bsoncxx::builder::stream::close_document_type close_document;
private:
    mongocxx::uri uri;
    mongocxx::pool* pool;
    mongocxx::client client;
    std::thread db;

public:
    Database(std::string);
    ~Database();
    connection getConnection();

    //This function updates an existing vehicle. 
    void updateVehicle(Vehicle* v);

    //This function adds a new vehicle to the database
    void addVehicle(Vehicle* v);

    template <typename T>
    void queryDatabase(std::string, T);
    template <typename T>
    void updateSingleVehicleTrait(std::string, int, T);
    template <typename T>
    void pushNewData(std::string, int, T);
    template <typename T>
    void getPastData(std::string, int, T);
    template <typename T>
    void updatePastData(std::string, int, T);
};
#endif
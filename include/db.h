#ifndef DATABASE_H
#define DATABASE_H

#include <string>
#include <stdexcept>
#include <iostream>
#include <cstdlib> // for malloc/free
#include <ctime>   // for time_t, std::time

class Database {
public:
    // Returns the singleton instance of the Database.
    // On the first call, it creates a new instance using dbName, username, password.
    // On subsequent calls, it checks if the provided dbName, username, password match
    // the existing instance. If not, throws std::runtime_error.
    static Database* getInstance(const std::string& dbName,
        const std::string& username,
        const std::string& password);

    // Resets the singleton instance (deletes and sets instance to nullptr).
    // Implementation is provided in db.cpp.
    static void resetInstance();

    // Connect sets 'connected_' to true.
    void connect();

    // Disconnect sets 'connected_' to false.
    void disconnect();

    // Returns the status of 'connected_' (true/false).
    bool isConnected() const;

    // Setter and Getter for username
    void set_username(const std::string& username);
    const std::string& get_username() const;

    // Setter and Getter for password
    void set_password(const std::string& password);
    const std::string& get_password() const;

    // Overload the new operator.
    // Allocates memory using malloc. If malloc fails, throws std::bad_alloc().
    // Prints "overloaded new " before returning.
    void* operator new(std::size_t size);

    // Overload the delete operator.
    // Deallocates memory using free. Prints "overloaded delete " before freeing.
    void operator delete(void* ptr);

    // If connection time threshold is exceeded, return true, else returns false.
    bool isTimeout();

    // Updates the last_activity with current time
    void refreshConnection();

private:
    // Private constructor to prevent direct instantiation.
    Database(const std::string& dbName,
        const std::string& username,
        const std::string& password);

    // Private destructor.
    // If still connected, it disconnects before destroying.
    ~Database();

    // Disallow copy and assign to further enforce singleton behavior.
    Database(const Database&);
    Database& operator=(const Database&);

    // Disallow move and move assignment
    Database(Database&&) noexcept(false);
    Database& operator=(Database&&) noexcept(false);

private:
    // Member variables
    std::string db_;        // Name of the database (e.g., "sales.db")
    std::string username_;  // Username for database
    std::string password_;  // Password for database
    bool connected_;        // Tracks whether the database is connected

    // Static pointer to the single Database instance
    static Database* instance;

    // Timestamp of alst activity
    time_t last_activity;

    // Timeout parameter
    static const int TIMEOUT = 5;
};

#endif // DATABASE_H

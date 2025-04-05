#include "db.h"

// Initialize static member
Database* Database::instance = nullptr;

// Static method to reset (delete) the singleton instance
void Database::resetInstance() {
    if (instance != nullptr) {
        delete instance;
        instance = nullptr;
    }
}

// Private constructor
Database::Database(const std::string& dbName,
    const std::string& username,
    const std::string& password)
    : db_(dbName),
    username_(username),
    password_(password),
    connected_(false)
{
    // Constructor body can remain empty; initialization list is sufficient
}

// Private destructor
Database::~Database() {
    // If still connected, disconnect
    if (connected_) {
        disconnect();
    }
}

// Copy Constructor
Database::Database(const Database&) {
    throw std::runtime_error("Copy is not allowed");
}

// Copy Assignment
Database& Database::operator=(const Database&) {
    throw std::runtime_error("Copy assignment is not allowed");
    return *this;
}

// Move Constructor
Database::Database(Database&&) {
    throw std::runtime_error("Move is not allowed");
}

// Move Assignment
Database& Database::operator=(Database&&) {
    throw std::runtime_error("Move assignment is not allowed");
    return *this;
}

// The core Singleton retrieval/creation method
Database* Database::getInstance(const std::string& dbName,
    const std::string& username,
    const std::string& password)
{
    // If no instance yet, create one
    if (instance == nullptr) {
        instance = new Database(dbName, username, password);
        return instance;
    }

    // If an instance already exists, check if it matches the requested credentials
    if (instance->db_ == dbName &&
        instance->username_ == username &&
        instance->password_ == password) {
        // Return the existing instance if everything matches
        return instance;
    }
    else {
        // Otherwise, throw an error
        throw std::runtime_error("invalid database name, username or password");
    }
}

// Connect method
void Database::connect() {
    connected_ = true;
}

// Disconnect method
void Database::disconnect() {
    connected_ = false;
}

// Return connected status
bool Database::isConnected() const {
    return connected_;
}

// Set username
void Database::set_username(const std::string& username) {
    username_ = username;
}

// Get username
const std::string& Database::get_username() const {
    return username_;
}

// Set password
void Database::set_password(const std::string& password) {
    password_ = password;
}

// Get password
const std::string& Database::get_password() const {
    return password_;
}

// Overload operator new
void* Database::operator new(std::size_t size) {
    std::cout << "overloaded new ";
    void* ptr = std::malloc(size);
    if (!ptr) {
        throw std::bad_alloc();
    }
    return ptr;
}

// Overload operator delete
void Database::operator delete(void* ptr) {
    std::cout << "overloaded delete ";
    std::free(ptr);
}

// Timeout check
bool Database::isTimeout() {
    if (!connected_) {
        return false;
    }
    time_t currenttime = std::time(nullptr);
    if ((currenttime - last_activity) > TIMEOUT) {
        disconnect();
        return true;
    }
    return false;
}

// Refresh COnnection
void Database::refreshConnection() {
    last_activity = std::time(nullptr);
}

#define CATCH_CONFIG_MAIN
#include "catch2/catch.hpp"
#include "db.h"
#include <thread>
#include <chrono>

TEST_CASE("Database singleton creation and retrieval", "[Database][Singleton]")
{
    // Always reset at start to ensure a clean slate
    Database::resetInstance();

    SECTION("Create the first instance") {
        // Create a new instance with specified credentials
        Database* db1 = Database::getInstance("sales.db", "admin", "1234");
        REQUIRE(db1 != nullptr);

        // Initially, 'connected_' should be false
        REQUIRE_FALSE(db1->isConnected());

        // Test connect()
        db1->connect();
        REQUIRE(db1->isConnected());

        // Test disconnect()
        db1->disconnect();
        REQUIRE_FALSE(db1->isConnected());
    }

    SECTION("Retrieve the same instance with same credentials") {
        Database* db1 = Database::getInstance("sales.db", "admin", "1234");
        Database* db2 = Database::getInstance("sales.db", "admin", "1234");

        // They must be the same pointer
        REQUIRE(db1 == db2);

        // Check that username/password match what we set initially
        REQUIRE(db1->get_username() == "admin");
        REQUIRE(db1->get_password() == "1234");
    }

    SECTION("Attempt to retrieve with different credentials throws") {
        Database* db1 = Database::getInstance("sales.db", "admin", "1234");
        REQUIRE(db1 != nullptr);

        // Different db name or different credentials -> should throw std::runtime_error
        REQUIRE_THROWS_AS(
            Database::getInstance("other.db", "admin", "1234"),
            std::runtime_error
        );
        REQUIRE_THROWS_AS(
            Database::getInstance("sales.db", "root", "abcd"),
            std::runtime_error
        );
    }
}

TEST_CASE("Resetting the singleton instance", "[Database][Singleton]")
{
    // Start fresh
    Database::resetInstance();
    REQUIRE_NOTHROW(Database::getInstance("test.db", "user", "pass"));

    // Confirm we have a valid instance now
    Database* db = Database::getInstance("test.db", "user", "pass");
    REQUIRE(db != nullptr);

    SECTION("Reset instance deletes the old singleton and allows new creation") {
        // Reset and confirm we can create a new instance with different credentials
        Database::resetInstance();
        REQUIRE_NOTHROW(Database::getInstance("new.db", "newuser", "newpass"));

        Database* dbNew = Database::getInstance("new.db", "newuser", "newpass");
        REQUIRE(dbNew != nullptr);
        REQUIRE(dbNew != db); // Since we called reset, it should be a different pointer
    }
}

TEST_CASE("Database disconnects after timeout", "[Database][Timeout]") {
    // Reset to ensure a clean slate
    Database::resetInstance();
    Database* db = Database::getInstance("timeout.db", "user", "pass");
    REQUIRE(db != nullptr);

    // Connect the database and verify it is active
    db->connect();
    REQUIRE(db->isConnected());

    // Wait longer than the TIMEOUT period (5 seconds)
    std::this_thread::sleep_for(std::chrono::seconds(6));

    // Calling isTimeout() should disconnect the connection and return true
    bool timeoutOccurred = db->isTimeout();
    REQUIRE(timeoutOccurred);
    REQUIRE_FALSE(db->isConnected());
}

TEST_CASE("Database refreshConnection prevents timeout", "[Database][Timeout]") {
    // Reset to ensure a clean slate
    Database::resetInstance();
    Database* db = Database::getInstance("timeout.db", "user", "pass");
    REQUIRE(db != nullptr);

    // Connect the database and verify it is active
    db->connect();
    REQUIRE(db->isConnected());

    // Sleep for a shorter period than TIMEOUT
    std::this_thread::sleep_for(std::chrono::seconds(2));

    // Refresh the connection to reset the inactivity timer
    db->refreshConnection();

    // Sleep again, ensuring the total inactive time (since refresh) is less than TIMEOUT
    std::this_thread::sleep_for(std::chrono::seconds(2));

    // The connection should still be active and isTimeout() should return false
    bool timeoutOccurred = db->isTimeout();
    REQUIRE_FALSE(timeoutOccurred);
    REQUIRE(db->isConnected());
}
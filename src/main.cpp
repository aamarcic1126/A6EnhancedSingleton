#include "db.h"

int main() {
    // Initial creation
    Database* db1 = Database::getInstance("sales.db", "admin", "1234");
    db1->connect();

    // Attempt to retrieve the same instance with the same credentials
    Database* db2 = Database::getInstance("sales.db", "admin", "1234");
    // db1 and db2 are the same pointer

    // The following would throw std::runtime_error because credentials differ
    // Database* db3 = Database::getInstance("prod.db", "admin", "1234");

    std::cout << "Is connected? " << std::boolalpha << db1->isConnected() << std::endl;

    Database::resetInstance(); // cleans up the singleton
    return 0;
}
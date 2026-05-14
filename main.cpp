#include <iostream>
#include <string>
#include <cstdlib>
#include <vector>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <sqlite3.h>

static sqlite3 *db = nullptr;

void clearScreen() {
    system("cls");
}

std::string currentTimestamp() {
    std::time_t now = std::time(nullptr);
    std::tm *lt = std::localtime(&now);
    std::ostringstream oss;
    oss << std::setfill('0') << (lt->tm_year + 1900)
        << "-" << std::setw(2) << (lt->tm_mon + 1)
        << "-" << std::setw(2) << lt->tm_mday
        << " " << std::setw(2) << lt->tm_hour
        << ":" << std::setw(2) << lt->tm_min;
    return oss.str();
}

struct Feeding {
    int id;
    std::string feedName;
    std::string foodType;
    int timesPerDay;
    std::string notes;
    std::vector<std::string> log; // loaded on demand
};

struct Medication {
    int id;
    std::string name;
    std::string dosage;
    int timesPerDay;
    std::string notes;
    std::vector<std::string> log;
};

struct GroomingEntry {
    int id;
    std::string type;
    std::string timestamp;
    std::string notes;
};

class Pet {
public:
    int id;
    std::string name;
    std::string species;
    std::string breed;
    int age;

    std::vector<Feeding> feedings;
    std::vector<Medication> medications;
    std::vector<GroomingEntry> groomingLog;

    Pet(int i, const std::string &n, const std::string &s,
        const std::string &b, int a)
        : id(i), name(n), species(s), breed(b), age(a) {}
};

void dbExec(const std::string &sql) {
    char *err = nullptr;
    if (sqlite3_exec(db, sql.c_str(), nullptr, nullptr, &err) != SQLITE_OK) {
        std::cerr << "DB error: " << err << "\n";
        sqlite3_free(err);
    }
}

void initDB() {
    if (sqlite3_open("kendra.db", &db) != SQLITE_OK) {
        std::cerr << "Cannot open database: " << sqlite3_errmsg(db) << "\n";
        exit(1);
    }

    dbExec("PRAGMA foreign_keys = ON;");

    dbExec(R"(
        CREATE TABLE IF NOT EXISTS pets (
            id      INTEGER PRIMARY KEY AUTOINCREMENT,
            name    TEXT NOT NULL,
            species TEXT NOT NULL,
            breed   TEXT NOT NULL,
            age     INTEGER NOT NULL
        );
    )");

    dbExec(R"(
        CREATE TABLE IF NOT EXISTS feedings (
            id           INTEGER PRIMARY KEY AUTOINCREMENT,
            pet_id       INTEGER NOT NULL REFERENCES pets(id) ON DELETE CASCADE,
            feed_name    TEXT NOT NULL,
            food_type    TEXT NOT NULL,
            times_per_day INTEGER NOT NULL,
            notes        TEXT DEFAULT ''
        );
    )");

    dbExec(R"(
        CREATE TABLE IF NOT EXISTS feeding_log (
            id         INTEGER PRIMARY KEY AUTOINCREMENT,
            feeding_id INTEGER NOT NULL REFERENCES feedings(id) ON DELETE CASCADE,
            entry      TEXT NOT NULL
        );
    )");

    dbExec(R"(
        CREATE TABLE IF NOT EXISTS medications (
            id           INTEGER PRIMARY KEY AUTOINCREMENT,
            pet_id       INTEGER NOT NULL REFERENCES pets(id) ON DELETE CASCADE,
            name         TEXT NOT NULL,
            dosage       TEXT NOT NULL,
            times_per_day INTEGER NOT NULL,
            notes        TEXT DEFAULT ''
        );
    )");

    dbExec(R"(
        CREATE TABLE IF NOT EXISTS medication_log (
            id            INTEGER PRIMARY KEY AUTOINCREMENT,
            medication_id INTEGER NOT NULL REFERENCES medications(id) ON DELETE CASCADE,
            entry         TEXT NOT NULL
        );
    )");

    dbExec(R"(
        CREATE TABLE IF NOT EXISTS grooming_log (
            id        INTEGER PRIMARY KEY AUTOINCREMENT,
            pet_id    INTEGER NOT NULL REFERENCES pets(id) ON DELETE CASCADE,
            type      TEXT NOT NULL,
            timestamp TEXT NOT NULL,
            notes     TEXT DEFAULT ''
        );
    )");
}

std::vector<Pet> loadPets() {
    std::vector<Pet> pets;
    sqlite3_stmt *stmt;
    sqlite3_prepare_v2(db, "SELECT id, name, species, breed, age FROM pets ORDER BY id;", -1, &stmt, nullptr);
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        int id = sqlite3_column_int(stmt, 0);
        std::string nm = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        std::string sp = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        std::string br = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
        int age = sqlite3_column_int(stmt, 4);
        pets.emplace_back(id, nm, sp, br, age);
    }
    sqlite3_finalize(stmt);
    return pets;
}

void loadFeedings(Pet &pet) {
    pet.feedings.clear();
    sqlite3_stmt *stmt;
    sqlite3_prepare_v2(db,
        "SELECT id, feed_name, food_type, times_per_day, notes FROM feedings WHERE pet_id=? ORDER BY id;",
        -1, &stmt, nullptr);
    sqlite3_bind_int(stmt, 1, pet.id);
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        Feeding f;
        f.id = sqlite3_column_int(stmt, 0);
        f.feedName = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        f.foodType = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        f.timesPerDay = sqlite3_column_int(stmt, 3);
        f.notes = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4));
        pet.feedings.push_back(f);
    }
    sqlite3_finalize(stmt);

    for (auto &f : pet.feedings) {
        sqlite3_stmt *ls;
        sqlite3_prepare_v2(db, "SELECT entry FROM feeding_log WHERE feeding_id=? ORDER BY id;", -1, &ls, nullptr);
        sqlite3_bind_int(ls, 1, f.id);
        while (sqlite3_step(ls) == SQLITE_ROW) {
            f.log.push_back(reinterpret_cast<const char*>(sqlite3_column_text(ls, 0)));
        }
        sqlite3_finalize(ls);
    }
}

void loadMedications(Pet &pet) {
    pet.medications.clear();
    sqlite3_stmt *stmt;
    sqlite3_prepare_v2(db,
        "SELECT id, name, dosage, times_per_day, notes FROM medications WHERE pet_id=? ORDER BY id;",
        -1, &stmt, nullptr);
    sqlite3_bind_int(stmt, 1, pet.id);
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        Medication m;
        m.id = sqlite3_column_int(stmt, 0);
        m.name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        m.dosage = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        m.timesPerDay = sqlite3_column_int(stmt, 3);
        m.notes = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4));
        pet.medications.push_back(m);
    }
    sqlite3_finalize(stmt);

    for (auto &m : pet.medications) {
        sqlite3_stmt *ls;
        sqlite3_prepare_v2(db, "SELECT entry FROM medication_log WHERE medication_id=? ORDER BY id;", -1, &ls, nullptr);
        sqlite3_bind_int(ls, 1, m.id);
        while (sqlite3_step(ls) == SQLITE_ROW) {
            m.log.push_back(reinterpret_cast<const char*>(sqlite3_column_text(ls, 0)));
        }
        sqlite3_finalize(ls);
    }
}

void loadGrooming(Pet &pet) {
    pet.groomingLog.clear();
    sqlite3_stmt *stmt;
    sqlite3_prepare_v2(db,
        "SELECT id, type, timestamp, notes FROM grooming_log WHERE pet_id=? ORDER BY id;",
        -1, &stmt, nullptr);
    sqlite3_bind_int(stmt, 1, pet.id);
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        GroomingEntry g;
        g.id = sqlite3_column_int(stmt, 0);
        g.type = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        g.timestamp = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        g.notes = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
        pet.groomingLog.push_back(g);
    }
    sqlite3_finalize(stmt);
}

int dbInsertPet(const std::string &name, const std::string &species,
                const std::string &breed, int age) {
    sqlite3_stmt *stmt;
    sqlite3_prepare_v2(db,
        "INSERT INTO pets (name, species, breed, age) VALUES (?,?,?,?);",
        -1, &stmt, nullptr);
    sqlite3_bind_text(stmt, 1, name.c_str(),    -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, species.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 3, breed.c_str(),   -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 4, age);
    sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    return static_cast<int>(sqlite3_last_insert_rowid(db));
}

void dbUpdatePet(const Pet &pet) {
    sqlite3_stmt *stmt;
    sqlite3_prepare_v2(db,
        "UPDATE pets SET name=?, species=?, breed=?, age=? WHERE id=?;",
        -1, &stmt, nullptr);
    sqlite3_bind_text(stmt, 1, pet.name.c_str(),    -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, pet.species.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 3, pet.breed.c_str(),   -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 4, pet.age);
    sqlite3_bind_int(stmt, 5, pet.id);
    sqlite3_step(stmt);
    sqlite3_finalize(stmt);
}

void dbDeletePet(int petId) {
    sqlite3_stmt *stmt;
    sqlite3_prepare_v2(db, "DELETE FROM pets WHERE id=?;", -1, &stmt, nullptr);
    sqlite3_bind_int(stmt, 1, petId);
    sqlite3_step(stmt);
    sqlite3_finalize(stmt);
}

int dbInsertFeeding(int petId, const Feeding &f) {
    sqlite3_stmt *stmt;
    sqlite3_prepare_v2(db,
        "INSERT INTO feedings (pet_id, feed_name, food_type, times_per_day, notes) VALUES (?,?,?,?,?);",
        -1, &stmt, nullptr);
    sqlite3_bind_int(stmt, 1, petId);
    sqlite3_bind_text(stmt, 2, f.feedName.c_str(),  -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 3, f.foodType.c_str(),  -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 4, f.timesPerDay);
    sqlite3_bind_text(stmt, 5, f.notes.c_str(),     -1, SQLITE_TRANSIENT);
    sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    return static_cast<int>(sqlite3_last_insert_rowid(db));
}

void dbDeleteFeeding(int feedingId) {
    sqlite3_stmt *stmt;
    sqlite3_prepare_v2(db, "DELETE FROM feedings WHERE id=?;", -1, &stmt, nullptr);
    sqlite3_bind_int(stmt, 1, feedingId);
    sqlite3_step(stmt);
    sqlite3_finalize(stmt);
}

void dbLogFeeding(int feedingId, const std::string &entry) {
    sqlite3_stmt *stmt;
    sqlite3_prepare_v2(db,
        "INSERT INTO feeding_log (feeding_id, entry) VALUES (?,?);",
        -1, &stmt, nullptr);
    sqlite3_bind_int(stmt, 1, feedingId);
    sqlite3_bind_text(stmt, 2, entry.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_step(stmt);
    sqlite3_finalize(stmt);
}

int dbInsertMedication(int petId, const Medication &m) {
    sqlite3_stmt *stmt;
    sqlite3_prepare_v2(db,
        "INSERT INTO medications (pet_id, name, dosage, times_per_day, notes) VALUES (?,?,?,?,?);",
        -1, &stmt, nullptr);
    sqlite3_bind_int(stmt, 1, petId);
    sqlite3_bind_text(stmt, 2, m.name.c_str(),   -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 3, m.dosage.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 4, m.timesPerDay);
    sqlite3_bind_text(stmt, 5, m.notes.c_str(),  -1, SQLITE_TRANSIENT);
    sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    return static_cast<int>(sqlite3_last_insert_rowid(db));
}

void dbDeleteMedication(int medId) {
    sqlite3_stmt *stmt;
    sqlite3_prepare_v2(db, "DELETE FROM medications WHERE id=?;", -1, &stmt, nullptr);
    sqlite3_bind_int(stmt, 1, medId);
    sqlite3_step(stmt);
    sqlite3_finalize(stmt);
}

void dbLogMedication(int medId, const std::string &entry) {
    sqlite3_stmt *stmt;
    sqlite3_prepare_v2(db,
        "INSERT INTO medication_log (medication_id, entry) VALUES (?,?);",
        -1, &stmt, nullptr);
    sqlite3_bind_int(stmt, 1, medId);
    sqlite3_bind_text(stmt, 2, entry.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_step(stmt);
    sqlite3_finalize(stmt);
}

void dbLogGrooming(int petId, const GroomingEntry &g) {
    sqlite3_stmt *stmt;
    sqlite3_prepare_v2(db,
        "INSERT INTO grooming_log (pet_id, type, timestamp, notes) VALUES (?,?,?,?);",
        -1, &stmt, nullptr);
    sqlite3_bind_int(stmt, 1, petId);
    sqlite3_bind_text(stmt, 2, g.type.c_str(),      -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 3, g.timestamp.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 4, g.notes.c_str(),     -1, SQLITE_TRANSIENT);
    sqlite3_step(stmt);
    sqlite3_finalize(stmt);
}


void logo() {
    std::cout << R"(
========================================
 _  __                _               
| |/ / ___  _ __   __| |_ __  __ _   
| ' / / _ \| '_ \ / _` | '__/ _` |  
| . \|  __/| | | | (_| | | | (_| |  
|_|\_\\___||_| |_|\__,_|_|  \__,_|  
========================================
)" << "\n";
}

void pause() {
    std::cout << "\nPress Enter to continue...";
    std::cin.ignore();
    std::cin.get();
}

void printSeparator() {
    std::cout << "----------------------------------------\n";
}

void feedingMenu(Pet &pet) {
    bool running = true;
    while (running) {
        loadFeedings(pet);
        clearScreen();
        logo();
        std::cout << "=== Feedings - " << pet.name << " ===\n\n";

        if (pet.feedings.empty()) {
            std::cout << "No feedings on record.\n";
        } 
        else {
            for (size_t i = 0; i < pet.feedings.size(); ++i) {
                const auto &f = pet.feedings[i];
                std::cout << i + 1 << ". " << f.feedName << " | " << f.foodType << " | " << f.timesPerDay << "x/day";
                if (!f.notes.empty()) {
                    std::cout << " | " << f.notes;
                }
                std::cout << "\n";
            }
        }

        std::cout << "\n";
        printSeparator();
        std::cout << "1. Add feeding\n";
        std::cout << "2. Log a feeding (select)\n";
        std::cout << "3. View feeding log (select)\n";
        std::cout << "4. Remove feeding\n";
        std::cout << "5. Back\n";
        printSeparator();
        std::cout << "Enter your choice: ";

        int choice;
        std::cin >> choice;
        clearScreen();

        switch (choice) {
            case 1: {
                logo();
                std::cout << "=== Add Feeding ===\n\n";
                Feeding f;
                f.id = 0;
                std::cout << "Feeding name (e.g. Breakfast, Lunch, Dinner): ";
                std::cin.ignore();
                std::getline(std::cin, f.feedName);
                std::cout << "Food type (e.g. dry kibble): ";
                std::getline(std::cin, f.foodType);
                std::cout << "Times per day: ";
                std::cin >> f.timesPerDay;
                std::cin.ignore();
                std::cout << "Notes - leave blank to skip:\n> ";
                std::getline(std::cin, f.notes);
                dbInsertFeeding(pet.id, f);
                std::cout << "\nFeeding added!\n";
                pause();
                break;
            }
            case 2: {
                if (pet.feedings.empty()) { 
                    std::cout << "No feedings to log.\n"; 
                    pause(); 
                    break; 
                }
                logo();
                std::cout << "=== Log Feeding - Select ===\n\n";
                for (size_t i = 0; i < pet.feedings.size(); ++i) {
                    std::cout << i + 1 << ". " << pet.feedings[i].feedName << "\n";
                }
                std::cout << "0. Cancel\nChoice: ";
                int idx; 
                std::cin >> idx;
                if (idx >= 1 && idx <= static_cast<int>(pet.feedings.size())) {
                    auto &f = pet.feedings[static_cast<size_t>(idx - 1)];
                    std::string entry = currentTimestamp() + "  -  " + f.foodType;
                    dbLogFeeding(f.id, entry);
                    std::cout << "\nFeeding logged: " << entry << "\n";
                }
                pause();
                break;
            }
            case 3: {
                if (pet.feedings.empty()) { 
                    std::cout << "No feedings on record.\n"; 
                    pause(); 
                    break; 
                }
                logo();
                std::cout << "=== View Feeding Log - Select ===\n\n";
                for (size_t i = 0; i < pet.feedings.size(); ++i) {
                    std::cout << i + 1 << ". " << pet.feedings[i].feedName << "\n";
                }
                std::cout << "0. Cancel\nChoice: ";
                int idx; 
                std::cin >> idx;
                clearScreen();
                if (idx >= 1 && idx <= static_cast<int>(pet.feedings.size())) {
                    const auto &f = pet.feedings[static_cast<size_t>(idx - 1)];
                    logo();
                    std::cout << "=== Feeding Log - " << f.feedName << " ===\n\n";
                    if (f.log.empty()) {
                        std::cout << "No feedings logged yet.\n";
                    } 
                    else {
                        for (size_t j = 0; j < f.log.size(); ++j) { 
                            std::cout << j + 1 << ". " << f.log[j] << "\n";
                    }
                    }   
                }
                pause();
                break;
            }
            case 4: {
                if (pet.feedings.empty()) { 
                    std::cout << "No feedings to remove.\n"; 
                    pause(); 
                    break; 
                }
                logo();
                std::cout << "=== Remove Feeding ===\n\n";
                for (size_t i = 0; i < pet.feedings.size(); ++i) {
                    std::cout << i + 1 << ". " << pet.feedings[i].feedName << "\n";
                }
                std::cout << "0. Cancel\nChoice: ";
                int idx; 
                std::cin >> idx;
                if (idx >= 1 && idx <= static_cast<int>(pet.feedings.size())) {
                    auto &f = pet.feedings[static_cast<size_t>(idx - 1)];
                    std::string removed = f.feedName;
                    dbDeleteFeeding(f.id);
                    std::cout << "\n" << removed << " removed.\n";
                }
                pause();
                break;
            }
            case 5:
                running = false;
                break;
            default:
                std::cout << "Invalid choice.\n";
                pause();
        }
    }
}

void medicationMenu(Pet &pet) {
    bool running = true;
    while (running) {
        loadMedications(pet);
        clearScreen();
        logo();
        std::cout << "=== Medications - " << pet.name << " ===\n\n";

        if (pet.medications.empty()) {
            std::cout << "No medications on record.\n";
        } 
        else {
            for (size_t i = 0; i < pet.medications.size(); ++i) {
                const auto &m = pet.medications[i];
                std::cout << i + 1 << ". " << m.name << " | " << m.dosage << " | " << m.timesPerDay << "x/day";
                if (!m.notes.empty()) {
                    std::cout << " | " << m.notes;
                }
                std::cout << "\n";
            }
        }

        std::cout << "\n";
        printSeparator();
        std::cout << "1. Add medication\n";
        std::cout << "2. Log a dose (select medication)\n";
        std::cout << "3. View dose log (select medication)\n";
        std::cout << "4. Remove medication\n";
        std::cout << "5. Back\n";
        printSeparator();
        std::cout << "Enter your choice: ";

        int choice;
        std::cin >> choice;
        clearScreen();

        switch (choice) {
            case 1: {
                logo();
                std::cout << "=== Add Medication ===\n\n";
                Medication med;
                med.id = 0;
                std::cout << "Medication name: ";
                std::cin.ignore();
                std::getline(std::cin, med.name);
                std::cout << "Dosage (e.g. 10mg, 1 tablet): ";
                std::getline(std::cin, med.dosage);
                std::cout << "Times per day: ";
                std::cin >> med.timesPerDay;
                std::cin.ignore();
                std::cout << "Notes - leave blank to skip:\n> ";
                std::getline(std::cin, med.notes);
                dbInsertMedication(pet.id, med);
                std::cout << "\nMedication added!\n";
                pause();
                break;
            }
            case 2: {
                if (pet.medications.empty()) { 
                    std::cout << "No medications to log.\n"; 
                    pause(); 
                    break; 
                }
                logo();
                std::cout << "=== Log Dose - Select Medication ===\n\n";
                for (size_t i = 0; i < pet.medications.size(); ++i) {
                    std::cout << i + 1 << ". " << pet.medications[i].name << "\n";
                }
                std::cout << "0. Cancel\nChoice: ";
                int idx; 
                std::cin >> idx;
                if (idx >= 1 && idx <= static_cast<int>(pet.medications.size())) {
                    auto &m = pet.medications[static_cast<size_t>(idx - 1)];
                    std::string entry = currentTimestamp() + "  -  " + m.dosage + " of " + m.name;
                    dbLogMedication(m.id, entry);
                    std::cout << "\nDose logged: " << entry << "\n";
                }
                pause();
                break;
            }
            case 3: {
                if (pet.medications.empty()) { 
                    std::cout << "No medications on record.\n"; 
                    pause(); 
                    break; 
                }
                logo();
                std::cout << "=== View Dose Log - Select Medication ===\n\n";
                for (size_t i = 0; i < pet.medications.size(); ++i) {
                    std::cout << i + 1 << ". " << pet.medications[i].name << "\n";
                }
                std::cout << "0. Cancel\nChoice: ";
                int idx; 
                std::cin >> idx;
                clearScreen();
                if (idx >= 1 && idx <= static_cast<int>(pet.medications.size())) {
                    const auto &m = pet.medications[static_cast<size_t>(idx - 1)];
                    logo();
                    std::cout << "=== Dose Log - " << m.name << " ===\n\n";
                    if (m.log.empty()) {
                        std::cout << "No doses logged yet.\n";
                    } 
                    else {
                        for (size_t j = 0; j < m.log.size(); ++j) {
                            std::cout << j + 1 << ". " << m.log[j] << "\n";
                        }
                    }
                }
                pause();
                break;
            }
            case 4: {
                if (pet.medications.empty()) { 
                    std::cout << "No medications to remove.\n"; 
                    pause(); 
                    break; 
                }
                logo();
                std::cout << "=== Remove Medication ===\n\n";
                for (size_t i = 0; i < pet.medications.size(); ++i) {
                    std::cout << i + 1 << ". " << pet.medications[i].name << "\n";
                }
                std::cout << "0. Cancel\nChoice: ";
                int idx; 
                std::cin >> idx;
                if (idx >= 1 && idx <= static_cast<int>(pet.medications.size())) {
                    auto &m = pet.medications[static_cast<size_t>(idx - 1)];
                    std::string removed = m.name;
                    dbDeleteMedication(m.id);
                    std::cout << "\n" << removed << " removed.\n";
                }
                pause();
                break;
            }
            case 5:
                running = false;
                break;
            default:
                std::cout << "Invalid choice.\n";
                pause();
        }
    }
}

void groomingMenu(Pet &pet) {
    const std::vector<std::string> groomingTypes = {
        "Bath", "Haircut", "Nail trim", "Brushing", "Ear cleaning", "Other"
    };

    bool running = true;
    while (running) {
        clearScreen();
        logo();
        std::cout << "=== Grooming - " << pet.name << " ===\n\n";
        printSeparator();
        std::cout << "1. Log a grooming session\n";
        std::cout << "2. View grooming log\n";
        std::cout << "3. Back\n";
        printSeparator();
        std::cout << "Enter your choice: ";

        int choice;
        std::cin >> choice;
        clearScreen();

        switch (choice) {
            case 1: {
                logo();
                std::cout << "=== Log Grooming Session ===\n\n";
                for (size_t i = 0; i < groomingTypes.size(); ++i) {
                    std::cout << "  " << i + 1 << ". " << groomingTypes[i] << "\n";
                }
                std::cout << "Choice: ";
                int idx; 
                std::cin >> idx;

                GroomingEntry entry;
                entry.id = 0;
                if (idx >= 1 && idx <= static_cast<int>(groomingTypes.size())) {
                    entry.type = groomingTypes[static_cast<size_t>(idx - 1)];
                    if (entry.type == "Other") {
                        std::cout << "Describe the grooming type: ";
                        std::cin.ignore();
                        std::getline(std::cin, entry.type);
                    } 
                    else {
                        std::cin.ignore();
                    }
                } 
                else {
                    std::cout << "Invalid choice.\n";
                    pause();
                    break;
                }

                std::cout << "Notes - leave blank to skip:\n> ";
                std::getline(std::cin, entry.notes);
                entry.timestamp = currentTimestamp();
                dbLogGrooming(pet.id, entry);
                std::cout << "\nGrooming session logged!\n";
                pause();
                break;
            }
            case 2: {
                loadGrooming(pet);
                logo();
                std::cout << "=== Grooming Log - " << pet.name << " ===\n\n";
                if (pet.groomingLog.empty()) {
                    std::cout << "No grooming sessions logged yet.\n";
                } 
                else {
                    for (size_t i = 0; i < pet.groomingLog.size(); ++i) {
                        const auto &g = pet.groomingLog[i];
                        std::cout << i + 1 << ". [" << g.timestamp << "]  " << g.type;
                        if (!g.notes.empty()) {
                            std::cout << "  -  " << g.notes;
                        }
                        std::cout << "\n";
                    }
                }
                pause();
                break;
            }
            case 3:
                running = false;
                break;
            default:
                std::cout << "Invalid choice.\n";
                pause();
        }
    }
}

void welcomeMessage() {
    logo();
    std::cout << "Welcome to Kendra, your pet care manager!\n";
    std::cout << "1. Add a new pet\n";
    std::cout << "2. Delete a pet\n";
    std::cout << "3. Select a pet\n";
    std::cout << "4. Exit\n\n";
}

void addPet(std::vector<Pet> &pets) {
    logo();
    std::string name;
    std::string species;
    std::string breed;
    int age;

    std::cout << "Enter pet's name: ";    
    std::cin >> name;
    std::cout << "Enter pet's species: "; 
    std::cin >> species;
    std::cout << "Enter pet's breed: ";   
    std::cin >> breed;
    std::cout << "Enter pet's age: ";     
    std::cin >> age;

    int newId = dbInsertPet(name, species, breed, age);
    pets.emplace_back(newId, name, species, breed, age);
    clearScreen();
    std::cout << "Pet added successfully!\n";
}

void deletePet(std::vector<Pet> &pets) {
    if (pets.empty()) { 
        std::cout << "No pets to delete.\n"; 
        return; 
    }

    for (size_t i = 0; i < pets.size(); ++i) {
        std::cout << i + 1 << ". " << pets[i].name << " (" << pets[i].species << ")\n";
    }
    std::cout << "Enter the number of the pet to delete: ";
    int choice; 
    std::cin >> choice;

    if (choice >= 1 && choice <= static_cast<int>(pets.size())) {
        int petId = pets[static_cast<size_t>(choice - 1)].id;
        dbDeletePet(petId);                              // cascades to all child tables
        pets.erase(pets.begin() + choice - 1);
        clearScreen();
        std::cout << "Pet deleted successfully!\n";
    } 
    else {
        std::cout << "Invalid choice.\n";
    }
}

void petMenu(Pet &pet) {
    bool running = true;
    while (running) {
        clearScreen();
        logo();
        std::cout << "=== " << pet.name << " (" << pet.species << ", " << pet.breed << ", age " << pet.age << ") ===\n\n";
        std::cout << "1. View info\n";
        std::cout << "2. Update info\n";
        printSeparator();
        std::cout << "3. Feeding\n";
        std::cout << "4. Medication\n";
        std::cout << "5. Grooming\n";
        printSeparator();
        std::cout << "6. Back to main menu\n\n";
        std::cout << "Enter your choice: ";

        int choice; 
        std::cin >> choice;
        clearScreen();

        switch (choice) {
            case 1:
                std::cout << "Name:    " << pet.name    << "\n";
                std::cout << "Species: " << pet.species << "\n";
                std::cout << "Breed:   " << pet.breed   << "\n";
                std::cout << "Age:     " << pet.age     << "\n";
                pause();
                break;
            case 2:
                std::cout << "Enter new name: ";    std::cin >> pet.name;
                std::cout << "Enter new species: "; std::cin >> pet.species;
                std::cout << "Enter new breed: ";   std::cin >> pet.breed;
                std::cout << "Enter new age: ";     std::cin >> pet.age;
                dbUpdatePet(pet);
                std::cout << "Pet info updated!\n";
                pause();
                break;
            case 3: 
                feedingMenu(pet);    
                break;
            case 4: 
                medicationMenu(pet); 
                break;
            case 5: 
                groomingMenu(pet);   
                break;
            case 6: 
                running = false;     
                break;
            default:
                std::cout << "Invalid choice.\n";
                pause();
        }
    }
}

void selectPet(std::vector<Pet> &pets) {
    if (pets.empty()) { 
        std::cout << "No pets added yet.\n"; 
        return; 
    }

    for (size_t i = 0; i < pets.size(); ++i) {
        std::cout << i + 1 << ". " << pets[i].name << " (" << pets[i].species << ")\n";
    }

    std::cout << "Enter the number of the pet to select (0 to cancel): ";
    int choice; 
    std::cin >> choice;

    if (choice >= 1 && choice <= static_cast<int>(pets.size())) {
        petMenu(pets[static_cast<size_t>(choice - 1)]);
    }
}

int main() {
    initDB();
    clearScreen();

    std::vector<Pet> pets = loadPets();
    bool running = true;

    while (running) {
        welcomeMessage();
        std::cout << "Enter your choice: ";
        int choice; 
        std::cin >> choice;
        clearScreen();

        switch (choice) {
            case 1: 
                addPet(pets);    
                break;
            case 2: 
                deletePet(pets); 
                break;
            case 3: 
                selectPet(pets); 
                break;
            case 4:
                std::cout << "Thank you for using Kendra!\n";
                running = false;
                break;
            default:
                std::cout << "Invalid choice. Please try again.\n";
        }

        if (running) {
            std::cout << "\nPress Enter to continue...";
            std::cin.ignore();
            std::cin.get();
            clearScreen();
        }
    }

    sqlite3_close(db);
    return 0;
}
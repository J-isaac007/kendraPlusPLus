#include <iostream>
#include <string>
#include <cstdlib>
#include <vector>
#include <ctime>
#include <iomanip>
#include <sstream>

void clearScreen(){
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

struct FeedingSchedule {
    std::string foodType;       // e.g. "dry kibble"
    int timesPerDay;            // e.g. 2
    std::string notes;          // e.g. "1 cup per serving"
    std::vector<std::string> log; // timestamped feeding entries
};

struct Medication {
    std::string name;
    std::string dosage;
    int timesPerDay;
    std::string notes;
    std::vector<std::string> log;
};

struct GroomingEntry {
    std::string type;       // Bath, Haircut, Nails, Brushing, Other
    std::string timestamp;
    std::string notes;
};

class Pet {
public:
    std::string name;
    std::string species;
    std::string breed;
    int age;

    FeedingSchedule feeding;
    std::vector<Medication> medications;
    std::vector<GroomingEntry> groomingLog;

    Pet(const std::string &n, const std::string &s,
        const std::string &b, int a)
        : name(n), species(s), breed(b), age(a) {}
};

void logo() {
    std::cout << R"(
========================================
 _  __                _               
| |/ / ___  _ __   __| |_ __  __ _   
| ' / / _ \| '_ \ / _` | '__/ _` |  
| . \|  __/| | | | (_| | | | (_| |  
|_|\_\\___||_| |_|\__,_|_|  \__,_|  
========================================
)";
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
        clearScreen();
        logo();
        std::cout << "=== Feeding - " << pet.name << " ===\n\n";

        // Show current schedule if set
        if (!pet.feeding.foodType.empty()) {
            std::cout << "Current schedule:\n";
            std::cout << "  Food type  : " << pet.feeding.foodType << "\n";
            std::cout << "  Times/day  : " << pet.feeding.timesPerDay << "\n";
            if (!pet.feeding.notes.empty()) {
                std::cout << "  Notes      : " << pet.feeding.notes << "\n";
            }
        } else {
            std::cout << "No feeding schedule set yet.\n";
        }

        std::cout << "\n";
        printSeparator();
        std::cout << "1. Set / update feeding schedule\n";
        std::cout << "2. Log a feeding now\n";
        std::cout << "3. View feeding log\n";
        std::cout << "4. Back\n";
        printSeparator();
        std::cout << "Enter your choice: ";
        int choice; 
        std::cin >> choice;
        clearScreen();

        switch (choice) {
            case 1: {
                logo();
                std::cout << "=== Set Feeding Schedule ===\n\n";
                std::cout << "Food type (e.g. dry kibble): ";
                std::cin.ignore();
                std::getline(std::cin, pet.feeding.foodType);
                std::cout << "Times per day: ";
                std::cin >> pet.feeding.timesPerDay;
                std::cin.ignore();
                std::cout << "Notes (leave blank to skip):\n> ";
                std::getline(std::cin, pet.feeding.notes);
                std::cout << "\nSchedule saved!\n";
                pause();
                break;
            }
            case 2: {
                if (pet.feeding.foodType.empty()) {
                    std::cout << "Please set a feeding schedule first.\n";
                } 
                else {
                    std::string entry = currentTimestamp() + "  -  " + pet.feeding.foodType;
                    pet.feeding.log.push_back(entry);
                    std::cout << "Feeding logged at " << currentTimestamp() << ".\n";
                }
                pause();
                break;
            }
            case 3: {
                logo();
                std::cout << "=== Feeding Log - " << pet.name << " ===\n\n";
                if (pet.feeding.log.empty()) {
                    std::cout << "No feedings logged yet.\n";
                } 
                else {
                    for (size_t i = 0; i < pet.feeding.log.size(); ++i)
                        std::cout << i + 1 << ". " << pet.feeding.log[i] << "\n";
                }
                pause();
                break;
            }
            case 4:
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
                pet.medications.push_back(med);
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
                std::cout << "0. Cancel\n";
                std::cout << "Choice: ";
                int idx; 
                std::cin >> idx;
                if (idx >= 1 && idx <= static_cast<int>(pet.medications.size())) {
                    auto &med = pet.medications[static_cast<size_t>(idx - 1)];
                    std::string entry = currentTimestamp() + "  -  " + med.dosage + " of " + med.name;
                    med.log.push_back(entry);
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
                std::cout << "0. Cancel\n";
                std::cout << "Choice: ";
                int idx; 
                std::cin >> idx;
                clearScreen();
                if (idx >= 1 && idx <= static_cast<int>(pet.medications.size())) {
                    const auto &med = pet.medications[static_cast<size_t>(idx - 1)];
                    logo();
                    std::cout << "=== Dose Log - " << med.name << " ===\n\n";
                    if (med.log.empty()) {
                        std::cout << "No doses logged yet.\n";
                    } 
                    else {
                        for (size_t j = 0; j < med.log.size(); ++j)
                            std::cout << j + 1 << ". " << med.log[j] << "\n";
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
                std::cout << "0. Cancel\n";
                std::cout << "Choice: ";
                int idx; 
                std::cin >> idx;
                if (idx >= 1 && idx <= static_cast<int>(pet.medications.size())) {
                    std::string removed = pet.medications[static_cast<size_t>(idx - 1)].name;
                    pet.medications.erase(pet.medications.begin() + idx - 1);
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
                std::cout << "Type of grooming:\n";
                for (size_t i = 0; i < groomingTypes.size(); ++i)
                    std::cout << "  " << i + 1 << ". " << groomingTypes[i] << "\n";
                std::cout << "Choice: ";
                int idx; 
                std::cin >> idx;

                GroomingEntry entry;
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
                pet.groomingLog.push_back(entry);
                std::cout << "\nGrooming session logged!\n";
                pause();
                break;
            }
            case 2: {
                logo();
                std::cout << "=== Grooming Log - " << pet.name << " ===\n\n";
                if (pet.groomingLog.empty()) {
                    std::cout << "No grooming sessions logged yet.\n";
                } else {
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

// ─── Shared pet/main menus (unchanged structure) ─────────────────────────────

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

    pets.emplace_back(name, species, breed, age);
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
                std::cout << "Enter new name: ";    
                std::cin >> pet.name;
                std::cout << "Enter new species: "; 
                std::cin >> pet.species;
                std::cout << "Enter new breed: ";   
                std::cin >> pet.breed;
                std::cout << "Enter new age: ";     
                std::cin >> pet.age;
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

// ─── Entry point ─────────────────────────────────────────────────────────────

int main() {
    clearScreen();
    std::vector<Pet> pets;
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

    return 0;
}
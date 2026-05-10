#include <iostream>
#include <string>
#include <cstdlib>
#include <vector>

void clearScreen(){
    system("cls");
}

class Pet {
    public:
        std::string name;
        std::string species;
        std::string breed;
        int age;

        Pet(const std::string &construct_name, const std::string &construct_species, const std::string &construct_breed, int construct_age)
            : name(construct_name), species(construct_species), breed(construct_breed), age(construct_age) {}

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

// Sub-menu that runs for a selected pet
void petMenu(Pet &pet) {
    bool running = true;
    while (running) {
        clearScreen();
        logo();
        std::cout << "=== " << pet.name << " (" << pet.species << ", " << pet.breed << ", age " << pet.age << ") ===\n\n";
        std::cout << "1. View info\n";
        std::cout << "2. Update info\n";
        std::cout << "=========================\n";
        std::cout << "3. feeding\n";
        std::cout << "4. medication\n";
        std::cout << "5. grooming\n";
        std::cout << "=========================\n";
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
                std::cout << "\nPress Enter to continue...";
                std::cin.ignore(); 
                std::cin.get();
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
                std::cout << "\nPress Enter to continue...";
                std::cin.ignore(); 
                std::cin.get();
                break;
            case 3:
                std::cout << "Feeding " << pet.name << "...\n";
                std::cout << "\nPress Enter to continue...";
                std::cin.ignore();
                std::cin.get();
                break;
            case 4:
                std::cout << "Administering medication to " << pet.name << "...\n";
                std::cout << "\nPress Enter to continue...";
                std::cin.ignore();
                std::cin.get();
                break;
            case 5:
                std::cout << "Grooming " << pet.name << "...\n";
                std::cout << "\nPress Enter to continue...";
                std::cin.ignore();
                std::cin.get();
                break;
            case 6:
                running = false; // ← exits THIS loop, returns to main menu
                break;
            default:
                std::cout << "Invalid choice.\n";
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
        clearScreen();
        petMenu(pets[static_cast<size_t>(choice - 1)]); // ← enters sub-loop for selected pet
    }
}

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
                running = false; // ← cleanly exits the main loop
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
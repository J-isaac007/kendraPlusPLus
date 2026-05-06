#include <iostream>
#include <string>
#include <cstdlib>
#include <vector>
#include <fstream>
#include <sstream>

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

void savePet(const std::vector<Pet> &pets) {
    std::ofstream file("pets.txt");
    if (!file) {
        std::cerr << "Error opening file for writing.\n";
        return;
    }

    for (const auto &pet : pets) {
        file << pet.name << "," << pet.species << "," << pet.breed << "," << pet.age << "\n";
    }
}

void loadPets(std::vector<Pet> &pets) {
    std::ifstream file("pets.txt");
    if (!file) {
        return;
    }

    std::string line;
    while (std::getline(file, line)) {
        std::istringstream ss(line);
        std::string name;
        std::string species;
        std::string breed;
        std::string ageStr;

        std::getline(ss, name, ',');
        std::getline(ss, species, ',');
        std::getline(ss, breed, ',');
        std::getline(ss, ageStr, ',');
        if (!pets.empty()) {
            pets.emplace_back(name, species, breed, std::stoi(ageStr));
        }
    }
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
    if (pets.empty()) { std::cout << "No pets to delete.\n"; return; }

    for (size_t i = 0; i < pets.size(); ++i)
        std::cout << i + 1 << ". " << pets[i].name << " (" << pets[i].species << ")\n";

    std::cout << "Enter the number of the pet to delete: ";
    int choice; std::cin >> choice;

    if (choice >= 1 && choice <= static_cast<int>(pets.size())) {
        pets.erase(pets.begin() + choice - 1);
        clearScreen();
        std::cout << "Pet deleted successfully!\n";
    } else {
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
        std::cout << "2. Update age\n";
        std::cout << "3. Back to main menu\n\n";
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
                std::cout << "Enter new age: ";
                std::cin >> pet.age;
                std::cout << "Age updated!\n";
                std::cout << "\nPress Enter to continue...";
                std::cin.ignore(); 
                std::cin.get();
                break;
            case 3:
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
        int choice; std::cin >> choice;
        clearScreen();

        switch (choice) {
            case 1: 
                addPet(pets);    
                savePet(pets);
                break;
            case 2: 
                deletePet(pets); 
                savePet(pets);
                break;
            case 3: 
                selectPet(pets); 
                savePet(pets);
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
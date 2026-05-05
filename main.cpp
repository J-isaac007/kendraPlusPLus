#include <iostream>
#include <string>
#include <cstdlib>
#include <vector>

void clearScreen() {
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif
}
class Pet {
    public:
        std::string name;
        std::string species;
        std::string breed;
        int age;

        Pet(const std::string &a, const std::string &b, const std::string &c, int d)
            : name(a), species(b), breed(c), age(d) {}
};

void logo() {
    std::cout << R"(
----------------------------------------
 _  __                _               
| |/ / ___  _ __   __| |_ __  __ _   
| ' / / _ \| '_ \ / _` | '__/ _` |  
| . \|  __/| | | | (_| | | | (_| |  
|_|\_\\___||_| |_|\__,_|_|  \__,_|  
----------------------------------------
)";
}
void welcomeMessage() {
    logo();
    std::cout << "Welcome to Kendra, your pet care manager!" << '\n';
    std::cout << "Please select an option:" << '\n';
    std::cout << "1. Add a new pet" << '\n';
    std::cout << "2. Delete a pet" << '\n';
    std::cout << "3. Select a pet" << '\n';
    std::cout << "4. Exit" << '\n';
    std::cout << '\n';
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

    Pet newPet(name, species, breed, age);
    pets.push_back(newPet);
    std::cout << "Pet added successfully!" << '\n';


}

void deletePet(std::vector<Pet> &pets) {
    for (size_t i = 0; i < pets.size(); ++i) {
        std::cout << i + 1 << ". " << pets[i].name << " (" << pets[i].species << ")" << '\n';
    }
    std::cout << "Enter the number of the pet to delete: ";
    int choice;
    std::cin >> choice;
    
    if (choice >= 1 && choice <= static_cast<int>(pets.size())) {
        pets.erase(pets.begin() + choice - 1);
        std::cout << "Pet deleted successfully!" << '\n';
    } else {
        std::cout << "Invalid choice. Please try again." << '\n';
    }
}

void selectPet(std::vector<Pet> &pets) {
    for (size_t i = 0; i < pets.size(); ++i) {
        std::cout << i + 1 << ". " << pets[i].name << " (" << pets[i].species << ")" << '\n';
    }
}


int main() {
    clearScreen();
    bool running1 = true;
    std::vector<Pet> pets;
    while (running1) {
        welcomeMessage();
        int choice;
     std::cout << "Enter your choice: ";
        std::cin >> choice;
        clearScreen();
        switch(choice) {
            case 1:
                addPet(pets);
                return false;
                break;
            case 2:
                deletePet(pets);
                return false;
                break;
            case 3:
                selectPet(pets);
                return false;
                break;
            case 4:
                std::cout << "Thank you for using Kendra!" << '\n';
                return false;
                break;
            default:
                std::cout << "Invalid choice. Please try again." << '\n';
                return false;
        }
    }
    std::cout << running1 << '\n';
    return 0;
}
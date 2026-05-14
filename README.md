# KENDRA

Kendra is a cli based pet care manager that helps with the scheduling and logging of pet care activities. 
It provides a user-friendly interface for pet owners to keep track of their pets' needs and routines.

specific problem:
1. Pet owners may forget or inconsistently follow pet feeding schedules.
2. Pet owners may miss important care tasks such as grooming or medication.
3. Managing schedules for multiple pets can be confusing.

objectives:
1. Develop a system that records and displays feeding schedules for each pet.
2. Design the system to track and display upcoming pet care activities.
3. Create a system that organizes care schedules for each pet profile.

## Data Structures
Kendra organizes your data around three core ideas:
Pets are the top-level profiles. Each pet has a name, species, breed, and age. Everything else belongs to a pet.
Schedules (Feedings and Medications) define a routine — for example, "Breakfast, dry kibble, twice a day." Each schedule keeps its own log of every time you recorded it being done, with a timestamp.
Grooming entries are one-off session records. Instead of a recurring schedule, each grooming log is just a snapshot: what was done, when, and any notes.

- Structs
- Vectors
- Class
- sqlite database

## How to Use
When you launch Kendra, you'll see the main menu. From here you can add a new pet, delete one, or select an existing pet to manage.
Once you select a pet, you can:

- View or update its basic info (name, species, breed, age)
- Feeding — add feeding schedules, log when a feeding happens, and review the full history
- Medication — add medications, log each dose, and view the dose history
- Grooming — log a grooming session and browse past sessions

Every action is driven by numbered menus, so you just type a number and press Enter to navigate.
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_DONORS 100
#define FILE_NAME "donors.dat"

// Structured Data for Donor Information
typedef struct {
    char name[50];
    int age;
    char bloodGroup[10];
    char contact[15];
    char lastDonation[15];
} Donor;

// Global array for structured data (Slide 11 & 12)
Donor donors[MAX_DONORS];
int donorCount = 0;

// =========================================================
// MODULE PLANNING (Slide 12)
// =========================================================

// Module 1: Donor Registration
void addDonor();
int validateAge(int age);
int validateBloodGroup(char* group);

// Module 2: Blood Stock Management
void viewBloodStock();
void issueBlood(); // Support hospital blood requests (Slide 14)

// Module 3: Blood Group Search
void searchByBloodGroup();

// Module 4: File Storage Management
void saveToFile();
void loadFromFile();

void updateDonor();
void deleteDonor();

// Utility Functions
void displayMenu();
void displayHeader();
void displayAllDonors();
void clearScreen();

int main() {
    int choice;

    // Load existing records from file (Module 4)
    loadFromFile();

    while (1) {
        displayHeader();
        displayMenu();
        printf("\nSelect Module Operation (1-7): ");
        if (scanf("%d", &choice) != 1) {
            printf("\n[Error] Invalid input. Please enter a number.\n");
            while (getchar() != '\n'); 
            continue;
        }
        getchar(); // Consume newline

        switch (choice) {
            case 1: addDonor(); break;           // Module 1
            case 2: viewBloodStock(); break;      // Module 2
            case 3: searchByBloodGroup(); break;  // Module 3
            case 4: issueBlood(); break;          // Module 2 (Request)
            case 5: updateDonor(); break;         // Update Record
            case 6: deleteDonor(); break;         // Delete Record
            case 7: displayAllDonors(); break;    // Database View
            case 8: 
                saveToFile();                     // Module 4
                printf("\n[Success] Data saved. Contribution to SDG 3 complete.\n");
                exit(0);
            case 9:
                printf("\nExiting without saving. Goodbye!\n");
                exit(0);
            default:
                printf("\n[Error] Invalid choice. Please try again.\n");
        }
        printf("\nPress Enter to return to menu...");
        getchar();
    }

    return 0;
}

void displayHeader() {
    clearScreen();
    printf("===========================================================\n");
    printf("           BLOOD BANK MANAGEMENT SYSTEM                    \n");
    printf("       Automated Methodology & Algorithm (Slide 11)        \n");
    printf("===========================================================\n");
}

void displayMenu() {
    printf(" --- MODULE 1: DONOR REGISTRATION ---\n");
    printf(" 1. Register New Donor\n");
    printf(" --- MODULE 2: STOCK MANAGEMENT ---\n");
    printf(" 2. Monitor Blood Availability\n");
    printf(" 4. Issue Blood (Hospital Request)\n");
    printf(" --- MODULE 3: SEARCH MODULE ---\n");
    printf(" 3. Emergency Blood Group Search\n");
    printf(" --- MAINTENANCE MODULE ---\n");
    printf(" 5. Update Existing Donor\n");
    printf(" 6. Delete Donor Record\n");
    printf(" --- ADDITIONAL OPERATIONS ---\n");
    printf(" 7. View Full Donor Database\n");
    printf(" 8. Save & Exit (Module 4: Storage)\n");
    printf(" 9. Close Program\n");
}

// =========================================================
// MODULE 1: DONOR REGISTRATION (Slide 13)
// =========================================================
void addDonor() {
    if (donorCount >= MAX_DONORS) {
        printf("\n[Error] System memory full.\n");
        return;
    }

    Donor d;
    printf("\n>>> REGISTER NEW DONOR <<<\n");
    
    printf("Full Name: ");
    fgets(d.name, sizeof(d.name), stdin);
    strtok(d.name, "\n");

    do {
        printf("Age (18-65): ");
        scanf("%d", &d.age);
        getchar();
        if (!validateAge(d.age)) printf("[Error] Ineligible age for donation.\n");
    } while (!validateAge(d.age));

    do {
        printf("Blood Group (A+, A-, B+, B-, O+, O-, AB+, AB-): ");
        fgets(d.bloodGroup, sizeof(d.bloodGroup), stdin);
        strtok(d.bloodGroup, "\n");
        for(int i = 0; d.bloodGroup[i]; i++) d.bloodGroup[i] = toupper(d.bloodGroup[i]);
        if (!validateBloodGroup(d.bloodGroup)) printf("[Error] Invalid blood group format.\n");
    } while (!validateBloodGroup(d.bloodGroup));

    printf("Contact Number: ");
    fgets(d.contact, sizeof(d.contact), stdin);
    strtok(d.contact, "\n");

    printf("Last Donation Date (DD/MM/YYYY): ");
    fgets(d.lastDonation, sizeof(d.lastDonation), stdin);
    strtok(d.lastDonation, "\n");

    donors[donorCount++] = d;
    printf("\n[Success] Donor validated and added to database.\n");
}

int validateAge(int age) {
    return (age >= 18 && age <= 65);
}

int validateBloodGroup(char* group) {
    char* valid[] = {"A+", "A-", "B+", "B-", "O+", "O-", "AB+", "AB-"};
    for (int i = 0; i < 8; i++) {
        if (strcmp(group, valid[i]) == 0) return 1;
    }
    return 0;
}

// =========================================================
// MODULE 2: BLOOD STOCK MANAGEMENT (Slide 14)
// =========================================================
void viewBloodStock() {
    char* groups[] = {"A+", "A-", "B+", "B-", "O+", "O-", "AB+", "AB-"};
    int counts[8] = {0};

    for (int i = 0; i < donorCount; i++) {
        for (int j = 0; j < 8; j++) {
            if (strcmp(donors[i].bloodGroup, groups[j]) == 0) {
                counts[j]++;
                break;
            }
        }
    }

    printf("\n>>> CURRENT BLOOD STOCK (UNITS) <<<\n");
    printf("-----------------------------\n");
    printf("| Blood Group | Available    |\n");
    printf("-----------------------------\n");
    for (int i = 0; i < 8; i++) {
        printf("|    %-8s |      %-7d |\n", groups[i], counts[i]);
    }
    printf("-----------------------------\n");
}

void issueBlood() {
    char target[10];
    int foundIndex = -1;

    printf("\nEnter Blood Group requested by Hospital: ");
    fgets(target, sizeof(target), stdin);
    strtok(target, "\n");
    for(int i = 0; target[i]; i++) target[i] = toupper(target[i]);

    // Find the first donor with this blood group to "issue" their unit
    for (int i = 0; i < donorCount; i++) {
        if (strcmp(donors[i].bloodGroup, target) == 0) {
            foundIndex = i;
            break;
        }
    }

    if (foundIndex != -1) {
        printf("\n[Hospital Request] Issuing 1 unit of %s blood (Donor: %s)\n", 
               target, donors[foundIndex].name);
        
        // Remove this record from active stock (Slide 14: Update stock after donation/request)
        for (int i = foundIndex; i < donorCount - 1; i++) {
            donors[i] = donors[i + 1];
        }
        donorCount--;
        printf("[Success] Stock updated accurately.\n");
    } else {
        printf("\n[Error] Blood Group %s is currently Out of Stock.\n", target);
    }
}

// =========================================================
// MODULE 3: BLOOD GROUP SEARCH (Slide 15)
// =========================================================
void searchByBloodGroup() {
    char target[10];
    int found = 0;

    printf("\nEnter Blood Group to Search: ");
    fgets(target, sizeof(target), stdin);
    strtok(target, "\n");
    for(int i = 0; target[i]; i++) target[i] = toupper(target[i]);

    printf("\n>>> SEARCH RESULTS FOR %s <<<\n", target);
    printf("--------------------------------------------------------------------------------\n");
    printf("| %-20s | %-12s | %-15s |\n", "Donor Name", "Contact", "Last Donation");
    printf("--------------------------------------------------------------------------------\n");

    for (int i = 0; i < donorCount; i++) {
        if (strcmp(donors[i].bloodGroup, target) == 0) {
            printf("| %-20s | %-12s | %-15s |\n", 
                   donors[i].name, donors[i].contact, donors[i].lastDonation);
            found = 1;
        }
    }

    if (!found) printf("| %-53s |\n", "No matching donors available.");
    printf("--------------------------------------------------------------------------------\n");
}

// =========================================================
// MAINTENANCE MODULE: UPDATE & DELETE
// =========================================================
void updateDonor() {
    char searchName[50];
    int found = 0;
    printf("\n>>> UPDATE DONOR RECORD <<<\n");
    printf("Enter Full Name to Update: ");
    fgets(searchName, sizeof(searchName), stdin);
    strtok(searchName, "\n");

    for (int i = 0; i < donorCount; i++) {
        if (_stricmp(donors[i].name, searchName) == 0) {
            printf("\nRecord Found. Current Age: %d, Group: %s\n", donors[i].age, donors[i].bloodGroup);
            
            printf("Enter New Age: ");
            scanf("%d", &donors[i].age);
            getchar(); // Consume newline

            printf("Enter New Blood Group: ");
            fgets(donors[i].bloodGroup, sizeof(donors[i].bloodGroup), stdin);
            strtok(donors[i].bloodGroup, "\n");
            for(int k = 0; donors[i].bloodGroup[k]; k++) donors[i].bloodGroup[k] = toupper(donors[i].bloodGroup[k]);

            printf("Update Contact Number: ");
            fgets(donors[i].contact, sizeof(donors[i].contact), stdin);
            strtok(donors[i].contact, "\n");

            saveToFile();
            printf("\n[Success] Record updated successfully.\n");
            found = 1;
            break;
        }
    }
    if (!found) printf("\n[Error] No donor found with name: %s\n", searchName);
}

void deleteDonor() {
    char searchName[50];
    int foundIndex = -1;
    printf("\n>>> DELETE DONOR RECORD <<<\n");
    printf("Enter Full Name to Delete: ");
    fgets(searchName, sizeof(searchName), stdin);
    strtok(searchName, "\n");

    for (int i = 0; i < donorCount; i++) {
        if (_stricmp(donors[i].name, searchName) == 0) {
            foundIndex = i;
            break;
        }
    }

    if (foundIndex != -1) {
        for (int i = foundIndex; i < donorCount - 1; i++) {
            donors[i] = donors[i + 1];
        }
        donorCount--;
        saveToFile();
        printf("\n[Success] Record for %s deleted permanently.\n", searchName);
    } else {
        printf("\n[Error] No donor found with name: %s\n", searchName);
    }
}

// =========================================================
// MODULE 4: FILE STORAGE MANAGEMENT (Slide 12)
// =========================================================
void saveToFile() {
    FILE *fp = fopen(FILE_NAME, "wb");
    if (fp == NULL) return;
    fwrite(&donorCount, sizeof(int), 1, fp);
    fwrite(donors, sizeof(Donor), donorCount, fp);
    fclose(fp);
}

void loadFromFile() {
    FILE *fp = fopen(FILE_NAME, "rb");
    if (fp == NULL) return;
    fread(&donorCount, sizeof(int), 1, fp);
    if (donorCount > MAX_DONORS) donorCount = MAX_DONORS;
    fread(donors, sizeof(Donor), donorCount, fp);
    fclose(fp);
}

// =========================================================
// UTILITIES
// =========================================================
void displayAllDonors() {
    if (donorCount == 0) {
        printf("\nNo donor records found.\n");
        return;
    }
    printf("\n>>> TOTAL DONOR LIST <<<\n");
    for (int i = 0; i < donorCount; i++) {
        printf("%d. %s [%s] - %s\n", i+1, donors[i].name, donors[i].bloodGroup, donors[i].contact);
    }
}

void clearScreen() {
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif
}


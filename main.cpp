#include <iostream>
#include <fstream>
#include <iomanip>
#include <string>
#include <cctype>
#include <cstdlib>
#include <sstream>

const std::string INPUT_FILE = "car_inventory.txt";
const std::string ERROR_FILE = "error_records.txt";
const int MAX_RECORDS = 100;

struct CarRecord {
    std::string carID;
    std::string model;
    std::string manufacturer;
    int quantity;
    double price;
};

std::string trim(const std::string &str) {
    size_t first = str.find_first_not_of(" \t");
    if (first == std::string::npos)
        return "";
    size_t last = str.find_last_not_of(" \t");
    return str.substr(first, last - first + 1);
}

bool validateCarID(const std::string &carID, std::string &errMsg) {
    bool valid = true;
    errMsg = "";
    if(carID.length() != 8) {
        errMsg += "Car ID must be 8 characters; ";
        valid = false;
    } else {
        for (int i = 0; i < 4; i++) {
            if (!std::isalpha(carID[i])) {
                errMsg += "First 4 must be letters; ";
                valid = false;
                break;
            }
            if (std::toupper(carID[i]) == 'O') {
                errMsg += "No 'O' allowed; ";
                valid = false;
                break;
            }
        }
        for (int i = 4; i < 8 && valid; i++) {
            if (!std::isdigit(carID[i])) {
                errMsg += "Last 4 must be digits; ";
                valid = false;
                break;
            }
            if (carID[i] == '0') {
                errMsg += "No '0' allowed; ";
                valid = false;
                break;
            }
        }
    }
    return valid;
}

bool validateModel(const std::string &model, std::string &errMsg) {
    bool valid = true;
    errMsg = "";
    if(model.length() < 3) {
        errMsg += "Model needs at least 3 chars; ";
        valid = false;
    }
    if(!std::isalpha(model[0])) {
        errMsg += "Model must start with a letter; ";
        valid = false;
    }
    for (size_t i = 0; i < model.length(); i++) {
        if(!std::isalnum(model[i])) {
            errMsg += "Model must be alphanumeric; ";
            valid = false;
            break;
        }
    }
    return valid;
}

bool validateManufacturer(const std::string &manufacturer, std::string &errMsg) {
    bool valid = true;
    errMsg = "";
    if(manufacturer.empty()) {
        errMsg += "Manufacturer can't be empty; ";
        valid = false;
    }
    return valid;
}

bool validateQuantity(int quantity, std::string &errMsg) {
    bool valid = true;
    errMsg = "";
    if(quantity < 0) {
        errMsg += "Quantity can't be negative; ";
        valid = false;
    }
    return valid;
}

bool validatePrice(double price, std::string &errMsg) {
    bool valid = true;
    errMsg = "";
    if(price <= 12999.00) {
        errMsg += "Price must be above 12999.00; ";
        valid = false;
    }
    return valid;
}

bool validateRecord(const CarRecord &record, std::string &errMsg) {
    bool valid = true;
    std::string err;
    errMsg = "";
    if(!validateCarID(record.carID, err)) { errMsg += err; valid = false; }
    if(!validateModel(record.model, err)) { errMsg += err; valid = false; }
    if(!validateManufacturer(record.manufacturer, err)) { errMsg += err; valid = false; }
    if(!validateQuantity(record.quantity, err)) { errMsg += err; valid = false; }
    if(!validatePrice(record.price, err)) { errMsg += err; valid = false; }
    return valid;
}

void getData(CarRecord records[], int &count) {
    std::ifstream infile(INPUT_FILE.c_str());
    if (!infile) {
        std::cout << "Can't open " << INPUT_FILE << std::endl;
        std::exit(EXIT_FAILURE);
    }
    std::ofstream errorFile(ERROR_FILE.c_str());
    if (!errorFile) {
        std::cout << "Can't open " << ERROR_FILE << std::endl;
        std::exit(EXIT_FAILURE);
    }
    count = 0;
    std::string line;
    while (std::getline(infile, line)) {
        if(line.empty())
            continue;
        std::istringstream ss(line);
        std::string field;
        CarRecord temp;
        if(std::getline(ss, field, ',')) {
            temp.carID = trim(field);
        } else {
            errorFile << line << " - Missing Car ID; \n";
            continue;
        }
        if(std::getline(ss, field, ',')) {
            temp.model = trim(field);
        } else {
            errorFile << line << " - Missing Model; \n";
            continue;
        }
        if(std::getline(ss, field, ',')) {
            temp.manufacturer = trim(field);
        } else {
            errorFile << line << " - Missing Manufacturer; \n";
            continue;
        }
        if(std::getline(ss, field, ',')) {
            try {
                temp.quantity = std::stoi(trim(field));
            } catch(...) {
                errorFile << line << " - Invalid Quantity; \n";
                continue;
            }
        } else {
            errorFile << line << " - Missing Quantity; \n";
            continue;
        }
        if(std::getline(ss, field)) {
            try {
                temp.price = std::stod(trim(field));
            } catch(...) {
                errorFile << line << " - Invalid Price; \n";
                continue;
            }
        } else {
            errorFile << line << " - Missing Price; \n";
            continue;
        }
        std::string errorMsg;
        if (validateRecord(temp, errorMsg)) {
            if (count < MAX_RECORDS) {
                records[count] = temp;
                count++;
            } else {
                std::cout << "Record limit reached. Extra records ignored." << std::endl;
                break;
            }
        } else {
            errorFile << temp.carID << ", " << temp.model << ", " << temp.manufacturer << ", "
                      << temp.quantity << ", " << std::fixed << std::setprecision(2) << temp.price
                      << " - " << errorMsg << "\n";
        }
    }
    infile.close();
    errorFile.close();
}

void printInventory(const CarRecord records[], int count) {
    if (count == 0) {
        std::cout << "No valid records to show." << std::endl;
        return;
    }
    std::cout << "\nValid Inventory Records:\n";
    std::cout << std::left << std::setw(10) << "CarID" << std::setw(10) << "Model"
              << std::setw(15) << "Manufacturer" << std::right << std::setw(10) << "Qty"
              << std::setw(15) << "Price" << "\n";
    std::cout << std::string(60, '-') << "\n";
    for (int i = 0; i < count; i++) {
        std::cout << std::left << std::setw(10) << records[i].carID
                  << std::setw(10) << records[i].model
                  << std::setw(15) << records[i].manufacturer
                  << std::right << std::setw(10) << records[i].quantity
                  << std::setw(15) << std::fixed << std::setprecision(2) << records[i].price << "\n";
    }
}

void printErrorRecords() {
    std::ifstream errorFile(ERROR_FILE.c_str());
    if (!errorFile) {
        std::cout << "Can't open " << ERROR_FILE << std::endl;
        return;
    }
    std::cout << "\nInvalid Records (from error file):\n";
    std::string line;
    while (std::getline(errorFile, line)) {
        std::cout << line << "\n";
    }
    errorFile.close();
}

void displayMenu() {
    std::cout << "\nMenu:\n";
    std::cout << "1. Print Valid Inventory Records\n";
    std::cout << "2. Print Invalid Records\n";
    std::cout << "3. Quit\n";
}

int main() {
    CarRecord validRecords[MAX_RECORDS];
    int validCount = 0;
    getData(validRecords, validCount);

    int choice;
    do {
        displayMenu();
        std::cout << "Enter your choice: ";
        std::cin >> choice;
        if(std::cin.fail()){
            std::cin.clear();
            std::cin.ignore(1000, '\n');
            std::cout << "Please enter a valid number.\n";
            continue;
        }
        switch (choice) {
            case 1:
                printInventory(validRecords, validCount);
                break;
            case 2:
                printErrorRecords();
                break;
            case 3:
                std::cout << "Take care!\n";
                break;
            default:
                std::cout << "Not a valid option, try again.\n";
        }
    } while (choice != 3);

    return 0;
}

/*
Sample Test Run:

Menu:
1. Print Valid Inventory Records
2. Print Invalid Records
3. Quit
Enter your choice:q

Please enter a valid number.

Menu:
1. Print Valid Inventory Records
2. Print Invalid Records
3. Quit
Enter your choice:`

Please enter a valid number.

Menu:
1. Print Valid Inventory Records
2. Print Invalid Records
3. Quit
Enter your choice:1


Valid Inventory Records:
CarID     Model     Manufacturer          Qty          Price
------------------------------------------------------------
MPKZ1265  RX5       SomeManufacturer        10       25000.00
QRST8642  Elite     Audi                   12       22000.00

Menu:
1. Print Valid Inventory Records
2. Print Invalid Records
3. Quit
Enter your choice:2


Invalid Records (from error file):
BPAZ3478, Tucker_48, Tucker_Corp, 51, 1663000.00 - Model must be alphanumeric;
MPKZ1205, RX3, Lexus, -5, 3000.00 - No '0' allowed; Quantity can't be negative; Price must be above 12999.00;
ABCD1234, ModelX, , 5, 20000.00 - Manufacturer can't be empty;
WXYZ5678, ModelZ, Ford, 7, - Missing Price;
LMNO4321, Speedster, Porsche, , 35000.00 - Invalid Quantity;

Menu:
1. Print Valid Inventory Records
2. Print Invalid Records
3. Quit
Enter your choice:3

Take care!

Process finished with exit code 0
*/
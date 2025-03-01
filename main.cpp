#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <string>
#include <limits>
using namespace std;

const string INPUT_FILE = "car_inventory.txt";
const string ERROR_FILE = "error_records.txt";
const int MAX_RECORDS = 100;
const int ID_LENGTH = 8;
const double MIN_PRICE = 12999.00;

struct Record {
    string carID;
    string model;
    string manufacturer;
    int quantity;
    double price;

    void setRecord(string id, string mod, string manu, int qty, double pr) {
        carID = id;
        model = mod;
        manufacturer = manu;
        quantity = qty;
        price = pr;
    }

    string toString() const {
        stringstream ss;
        ss << setw(10) << left << carID << setw(15) << model << setw(15) << manufacturer
           << setw(10) << right << quantity << setw(15) << fixed << setprecision(2) << price;
        return ss.str();
    }
};

enum MenuOption { DISPLAY_VALID = 1, DISPLAY_INVALID, EXIT_PROGRAM };

bool isValidRecord(const Record &rec, string &errorMessage) {
    errorMessage = "";

    if (rec.carID.length() != ID_LENGTH || !isalpha(rec.carID[0]) || !isalpha(rec.carID[1]) ||
        !isalpha(rec.carID[2]) || !isalpha(rec.carID[3]) || rec.carID.find('O') != string::npos ||
        !isdigit(rec.carID[4]) || !isdigit(rec.carID[5]) || !isdigit(rec.carID[6]) || !isdigit(rec.carID[7]) ||
        rec.carID[4] == '0' || rec.carID[5] == '0' || rec.carID[6] == '0' || rec.carID[7] == '0') {
        errorMessage += " Invalid ID: Must have 4 letters (A-Z, no 'O') and 4 digits (1-9);";
    }

    if (rec.model.length() < 3 || !isalpha(rec.model[0])) {
        errorMessage += " Invalid Model;";
    }

    if (rec.manufacturer.empty() || rec.manufacturer == "UNKNOWN") {
        errorMessage += " Missing Manufacturer;";
    }

    if (rec.price <= MIN_PRICE) {
        errorMessage += " Price must be above $12,999.00;";
    }

    return errorMessage.empty();
}

int loadRecords(Record records[], int &validCount) {
    ifstream infile(INPUT_FILE);
    ofstream errfile(ERROR_FILE);
    if (!infile || !errfile) {
        cerr << "Error opening file." << endl;
        return 0;
    }

    string id, model, manufacturer;
    int quantity;
    double price;
    validCount = 0;

    while (infile >> id >> model >> manufacturer >> quantity >> price) {
        Record temp;
        temp.setRecord(id, model, manufacturer, quantity, price);
        string errorMessage;

        if (isValidRecord(temp, errorMessage)) {
            if (validCount < MAX_RECORDS) {
                records[validCount++] = temp;
            } else {
                cout << "Max records limit reached." << endl;
                break;
            }
        } else {
            errfile << setw(10) << left << id << setw(15) << model << setw(15) << manufacturer
                    << setw(10) << right << quantity << setw(15) << fixed << setprecision(2) << price
                    << "  /// " << errorMessage << endl;
        }
    }
    infile.close();
    errfile.close();
    return validCount;
}

void displayRecords(const Record records[], int count) {
    cout << left << setw(10) << "Car ID" << setw(15) << "Model" << setw(15) << "Manufacturer"
         << setw(10) << "Quantity" << setw(15) << "Price" << endl;
    for (int i = 0; i < count; i++) {
        cout << records[i].toString() << endl;
    }
}

void displayInvalidRecords() {
    ifstream errfile(ERROR_FILE);
    if (!errfile) {
        cout << "Error: Could not open " << ERROR_FILE << " or no invalid records found.\n";
        return;
    }

    cout << "\nInvalid Records:\n";
    cout << left << setw(10) << "Car ID" << setw(15) << "Model" << setw(15) << "Manufacturer"
         << setw(10) << "Quantity" << setw(15) << "Price" << "  /// Reason" << endl;

    string id, model, manufacturer, reason;
    int quantity;
    double price;
    while (errfile >> id >> model >> manufacturer >> quantity >> price) {
        getline(errfile, reason);
        cout << setw(10) << left << id << setw(15) << model << setw(15) << manufacturer
             << setw(10) << right << quantity << setw(15) << fixed << setprecision(2) << price
             << "  ///" << reason << endl;
    }
    errfile.close();
}

void menu() {
    Record records[MAX_RECORDS];
    int validCount = 0;
    int choice;

    do {
        cout << "\n1. Display Valid Records" << endl;
        cout << "2. Display Invalid Records" << endl;
        cout << "3. Exit" << endl;
        cout << "Enter choice: ";
        cin >> choice;

        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        switch (choice) {
            case DISPLAY_VALID:
                validCount = loadRecords(records, validCount);
                displayRecords(records, validCount);
                break;
            case DISPLAY_INVALID:
                displayInvalidRecords();
                break;
            case EXIT_PROGRAM:
                cout << "Exiting program, have a good day." << endl;
                break;
            default:
                cout << "Invalid choice. Try again." << endl;
        }
    } while (choice != EXIT_PROGRAM);
}

int main() {
    menu();
    return 0;
}

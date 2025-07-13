#include <iostream>
#include <string>
#include <ctime>
#include <fstream>
using namespace std;

const int MAX_ACCOUNTS = 100;
const int MAX_TRANSACTIONS = 5;

class Transaction {
public:
    string type;
    double amount;
    string timestamp;

    Transaction() : type(""), amount(0.0), timestamp("") {}

    void setTransaction(const string& t, double a) {
        type = t;
        amount = a;
        time_t now = time(0);
        timestamp = ctime(&now);
        if (!timestamp.empty())
            timestamp.pop_back(); 
    }

    void display() {
        cout << type << " of $" << amount << " on " << timestamp << endl;
    }
};

class Account {
protected:
    string name;
    string dob;
    string phone;
    string cnic;
    string religion;
    string gender;
    string address;
    double balance;
    string pin;
    Transaction transactions[MAX_TRANSACTIONS];
    int transCount;

public:
    int accountNumber;
    Account() : accountNumber(0), balance(0), transCount(0) {}
    virtual ~Account() {}

    virtual void create(int accNum) {
        accountNumber = accNum;
        cout << "Enter Name: ";
        cin.ignore();
        getline(cin, name);

        cout << "Enter Date of Birth (dd/mm/yyyy): ";
        getline(cin, dob);

        cout << "Enter Phone Number: ";
        getline(cin, phone);

        cout << "Enter CNIC: ";
        getline(cin, cnic);

        cout << "Enter Religion: ";
        getline(cin, religion);

        cout << "Enter Gender: ";
        getline(cin, gender);

        cout << "Enter Address: ";
        getline(cin, address);

        cout << "Set 4-digit PIN: ";
        getline(cin, pin);

        do {
            cout << "Enter Initial Deposit (> 0): ";
            cin >> balance;
            if (balance <= 0) {
                cout << "Initial deposit must be greater than zero.\n";
            }
        } while (balance <= 0);

        addTransaction("Initial Deposit", balance);

        cout << "Account Created with Number: " << accountNumber << endl;
        cin.ignore(); // clear newline left in buffer
    }

    virtual void save(ofstream& out) {
        out << getAccountType() << "\n";
        out << accountNumber << "\n";
        out << name << "\n" << dob << "\n" << phone << "\n"
            << cnic << "\n" << religion << "\n" << gender << "\n" << address << "\n"
            << balance << "\n" << pin << "\n";
    }

    virtual void load(ifstream& in) {
        getline(in, name);
        getline(in, dob);
        getline(in, phone);
        getline(in, cnic);
        getline(in, religion);
        getline(in, gender);
        getline(in, address);
        in >> balance;
        in.ignore();
        getline(in, pin);
        transCount = 0;
    }

    virtual string getAccountType() const {
        return "Account";
    }

    virtual void displayAccount() {
        cout << "Account Number: " << accountNumber << "\nName: " << name
             << "\nDate of Birth: " << dob << "\nPhone: " << phone
             << "\nCNIC: " << cnic << "\nReligion: " << religion
             << "\nGender: " << gender << "\nAddress: " << address
             << "\nBalance: $" << balance << endl;
    }

    virtual void deposit(double amt) {
        if (amt > 0) {
            balance += amt;
            addTransaction("Deposit", amt);
            cout << "Deposit Successful. New Balance: $" << balance << endl;
        } else {
            cout << "Deposit amount must be positive.\n";
        }
    }

    virtual void withdraw(double amt) {
        if (amt > 0 && amt <= balance) {
            balance -= amt;
            addTransaction("Withdraw", amt);
            cout << "Withdrawal Successful. Remaining Balance: $" << balance << endl;
        } else {
            cout << "Invalid amount or insufficient balance.\n";
        }
    }

    virtual bool transferTo(Account* receiver, double amt) {
        if (amt > 0 && amt <= balance) {
            balance -= amt;
            receiver->balance += amt;
            addTransaction("Transfer to " + to_string(receiver->accountNumber), amt);
            receiver->addTransaction("Received from " + to_string(accountNumber), amt);
            return true;
        }
        return false;
    }

    void miniStatement() {
        cout << "Last " << transCount << " Transactions:\n";
        for (int i = 0; i < transCount; i++) {
            transactions[i].display();
        }
    }

    bool verifyPin(const string& enteredPin) {
        return pin == enteredPin;
    }

    int getAccountNumber() const {
        return accountNumber;
    }

    string getName() const {
        return name;
    }

protected:
    void addTransaction(const string& type, double amount) {
        if (transCount < MAX_TRANSACTIONS) {
            transactions[transCount++].setTransaction(type, amount);
        } else {
            for (int i = 1; i < MAX_TRANSACTIONS; i++) {
                transactions[i - 1] = transactions[i];
            }
            transactions[MAX_TRANSACTIONS - 1].setTransaction(type, amount);
        }
    }
};

class SavingsAccount : public Account {
    double interestRate;

public:
    SavingsAccount() : Account(), interestRate(3.5) {}

    void applyInterest() {
        double interest = balance * (interestRate / 100);
        balance += interest;
        addTransaction("Interest Added", interest);
    }

    void save(ofstream& out) override {
        out << getAccountType() << "\n";
        out << accountNumber << "\n";
        out << name << "\n" << dob << "\n" << phone << "\n"
            << cnic << "\n" << religion << "\n" << gender << "\n" << address << "\n"
            << balance << "\n" << pin << "\n";
        out << interestRate << "\n";
    }

    void load(ifstream& in) override {
        getline(in, name);
        getline(in, dob);
        getline(in, phone);
        getline(in, cnic);
        getline(in, religion);
        getline(in, gender);
        getline(in, address);
        in >> balance;
        in.ignore();
        getline(in, pin);
        in >> interestRate;
        in.ignore();
        transCount = 0;
    }

    string getAccountType() const override {
        return "SavingsAccount";
    }

    void displayAccount() override {
        Account::displayAccount();
        cout << "Account Type: Savings\nInterest Rate: " << interestRate << "%" << endl;
    }
};

class CheckingAccount : public Account {
    double overdraftLimit;

public:
    CheckingAccount() : Account(), overdraftLimit(10000) {}

    void withdraw(double amt) override {
        if (amt > 0 && amt <= balance + overdraftLimit) {
            balance -= amt;
            addTransaction("Withdraw", amt);
            cout << "Withdrawal Successful. Remaining Balance: $" << balance << endl;
        } else {
            cout << "Insufficient funds including overdraft limit.\n";
        }
    }

    void save(ofstream& out) override {
        out << getAccountType() << "\n";
        out << accountNumber << "\n";
        out << name << "\n" << dob << "\n" << phone << "\n"
            << cnic << "\n" << religion << "\n" << gender << "\n" << address << "\n"
            << balance << "\n" << pin << "\n";
        out << overdraftLimit << "\n";
    }

    void load(ifstream& in) override {
        getline(in, name);
        getline(in, dob);
        getline(in, phone);
        getline(in, cnic);
        getline(in, religion);
        getline(in, gender);
        getline(in, address);
        in >> balance;
        in.ignore();
        getline(in, pin);
        in >> overdraftLimit;
        in.ignore();
        transCount = 0;
    }

    string getAccountType() const override {
        return "CheckingAccount";
    }

    void displayAccount() override {
        Account::displayAccount();
        cout << "Account Type: Checking\nOverdraft Limit: $" << overdraftLimit << endl;
    }

    double getOverdraftLimit() const {
        return overdraftLimit;
    }
};

class Bank {
private:
    Account* accounts[MAX_ACCOUNTS];
    int totalAccounts;

public:
    Bank() : totalAccounts(0) {
        for (int i = 0; i < MAX_ACCOUNTS; i++)
            accounts[i] = nullptr;
    }

    ~Bank() {
        for (int i = 0; i < totalAccounts; i++) {
            delete accounts[i];
        }
    }

    void loadFromFile() {
        ifstream in("accounts.txt");
        if (!in) {
            cout << "No saved accounts found, starting fresh.\n";
            return;
        }
        totalAccounts = 0;
        while (totalAccounts < MAX_ACCOUNTS) {
            string accType;
            if (!getline(in, accType)) break;

            int accNum;
            if (!(in >> accNum)) break;
            in.ignore();

            Account* acc = nullptr;

            if (accType == "SavingsAccount") {
                acc = new SavingsAccount();
            } else if (accType == "CheckingAccount") {
                acc = new CheckingAccount();
            } else if (accType == "Account") {
                acc = new Account();
            } else {
                cout << "Unknown account type in file: " << accType << "\n";
                break;
            }

            acc->accountNumber = accNum;
            acc->load(in);
            accounts[totalAccounts++] = acc;
        }
        in.close();
    }

    void saveToFile() {
        ofstream out("accounts.txt");
        for (int i = 0; i < totalAccounts; i++) {
            accounts[i]->save(out);
        }
        out.close();
    }

    int generateAccountNumber() {
        return 10000 + totalAccounts + 1;
    }

    void createAccount() {
        if (totalAccounts >= MAX_ACCOUNTS) {
            cout << "Account limit reached.\n";
        }

					
        int accTypeChoice;
        cout << "Select Account Type:\n1. Savings\n2. Checking\nChoice: ";
        cin >> accTypeChoice;

        Account* acc = nullptr;

        if (accTypeChoice == 1)
            acc = new SavingsAccount();
        else if (accTypeChoice == 2)
            acc = new CheckingAccount();
        else {
            cout << "Invalid type, defaulting to generic Account.\n";
            acc = new Account();
        }

        int accNum = generateAccountNumber();
        acc->create(accNum);
        accounts[totalAccounts++] = acc;

        saveToFile();
    }

    void deleteAccount() {
        int accNum;
        cout << "Enter Account Number to Delete: ";
        cin >> accNum;
        int index = findAccount(accNum);
        if (index == -1) {
            cout << "Account not found.\n";
            return;
        }
        delete accounts[index];
        for (int i = index; i < totalAccounts - 1; i++) {
            accounts[i] = accounts[i + 1];
        }
        accounts[totalAccounts - 1] = nullptr;
        totalAccounts--;
        cout << "Account Deleted.\n";
        saveToFile();
    }

    void updateAccount() {
        int accNum;
        cout << "Enter Account Number to Update: ";
        cin >> accNum;
        int index = findAccount(accNum);
        if (index != -1) {
            cout << "Re-enter details to update account.\n";
            accounts[index]->create(accNum);
            saveToFile();
        } else {
            cout << "Account not found.\n";
        }
    }

    void loginUser() {
        int accNum;
        string pin;
        cout << "Enter Account Number: ";
        cin >> accNum;
        cout << "Enter PIN: ";
        cin >> pin;
        int index = findAccount(accNum);
        if (index != -1 && accounts[index]->verifyPin(pin)) {
            userMenu(index);
            saveToFile();
        } else {
            cout << "Login failed.\n";
        }
    }

    void showAllAccounts() {
        if (totalAccounts == 0) {
            cout << "No accounts to show.\n";
            return;
        }
        for (int i = 0; i < totalAccounts; i++) {
            accounts[i]->displayAccount();
            cout << "----------------------\n";
        }
    }

    void searchByName() {
        string search;
        cin.ignore();
        cout << "Enter name to search: ";
        getline(cin, search);
        bool found = false;
        for (int i = 0; i < totalAccounts; i++) {
            if (accounts[i]->getName().find(search) != string::npos) {
                accounts[i]->displayAccount();
                cout << "----------------------\n";
                found = true;
            }
        }
        if (!found) cout << "No matching account found.\n";
    }

    void applyInterestAll(double rate) {
        for (int i = 0; i < totalAccounts; i++) {
            SavingsAccount* sa = dynamic_cast<SavingsAccount*>(accounts[i]);
            if (sa) {
                sa->applyInterest();
            }
        }
        cout << "Interest applied to all savings accounts.\n";
        saveToFile();
    }

private:
    int findAccount(int accNum) {
        for (int i = 0; i < totalAccounts; i++) {
            if (accounts[i]->getAccountNumber() == accNum) return i;
        }
        return -1;
    }

    void showAccountTypeInfo(int index) {
        cout << "Account Type: " << accounts[index]->getAccountType() << endl;
        if (accounts[index]->getAccountType() == "SavingsAccount") {
            cout << "You can apply interest to your savings account from the admin panel.\n";
        } else if (accounts[index]->getAccountType() == "CheckingAccount") {
            CheckingAccount* ca = dynamic_cast<CheckingAccount*>(accounts[index]);
            if (ca) {
                cout << "Your overdraft limit is: $" << ca->getOverdraftLimit() << endl;
            }
        }
    }

    void userMenu(int index) {
        cout << "\nWelcome, " << accounts[index]->getName() << "!" << endl;
        showAccountTypeInfo(index);

        int choice;
        do {
            cout << "\n1. Deposit\n2. Withdraw\n3. View Details\n4. Mini Statement\n5. Transfer\n6. Logout\nChoice: ";
            cin >> choice;
            switch (choice) {
                case 1: {
                    double amt; cout << "Amount: "; cin >> amt; accounts[index]->deposit(amt); break;
                }
                case 2: {
                    double amt; cout << "Amount: "; cin >> amt; accounts[index]->withdraw(amt); break;
                }
                case 3:
                    accounts[index]->displayAccount();
                    break;
                case 4:
                    accounts[index]->miniStatement();
                    break;
                case 5: {
                    int recvAccNum;
                    double amt;
                    cout << "Enter receiver account number: ";
                    cin >> recvAccNum;
                    int recvIndex = findAccount(recvAccNum);
                    if (recvIndex == -1) {
                        cout << "Receiver account not found.\n";
                        break;
                    }
                    cout << "Enter amount to transfer: ";
                    cin >> amt;
                    if (accounts[index]->transferTo(accounts[recvIndex], amt)) {
                        cout << "Transfer successful.\n";
                    } else {
                        cout << "Transfer failed. Check your balance.\n";
                    }
                    break;
                }
                case 6:
                    cout << "Logging out...\n";
                    break;
                default:
                    cout << "Invalid choice.\n";
            }
        } while (choice != 6);
    }
};

class Admin {
private:
    string username = "admin";
    string password = "1234";

public:
    bool login() {
        string u, p;
        cout << "Enter Admin Username: ";
        cin >> u;
        cout << "Enter Admin Password: ";
        cin >> p;
        return u == username && p == password;
    }
};

class BankSystem {
private:
    Bank bank;
    Admin admin;

public:
    void run() {
        bank.loadFromFile();

        int choice;
        do {
            cout << "*** Bank Management System ***"<<endl;
            cout <<"==============================="<<endl;
            cout <<"-----**** HUGS TO BUGS ***-----"<<endl;
            cout << "\n1. Admin Login\n2. User Login\n3. Exit\nChoice: ";
            cin >> choice;
            switch (choice) {
                case 1:
                    if (admin.login()) {
                        adminMenu();
                    } else {
                        cout << "Invalid admin credentials.\n";
                    }
                    break;
                case 2:
                    bank.loginUser();
                    break;
                case 3:
                    cout << "Exiting...\n";
                    break;
                default:
                    cout << "Invalid choice.\n";
            }
        } while (choice != 3);
    }

private:
    void adminMenu() {
        int choice;
        do {
            cout << "\n--- Admin Panel ---\n";
            cout << "1. Create Account\n2. Delete Account\n3. Update Account\n4. View All Accounts\n5. Search Account By Name\n6. Apply Interest\n7. Logout\nChoice: ";
            cin >> choice;
            switch (choice) {
                case 1:
                    bank.createAccount();
                    break;
                case 2:
                    bank.deleteAccount();
                    break;
                case 3:
                    bank.updateAccount();
                    break;
                case 4:
                    bank.showAllAccounts();
                    break;
                case 5:
                    bank.searchByName();
                    break;
                case 6: {
                    double rate;
                    cout << "Enter interest rate (%): ";
                    cin >> rate;
                    bank.applyInterestAll(rate);
                    break;
                }
                case 7:
                    cout << "Logging out...\n";
                    break;
                default:
                    cout << "Invalid choice.\n";
            }
        } while (choice != 7);
    }
};

int main() {
    BankSystem system;
    system.run();
    return 0;
}

#include <iostream>
#include <vector>
#include <set>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <string>
#include <limits>
#include <iomanip>

using namespace std;

class Account
{
public:
    int accountNumber;
    string name;
    double balance;
    string password; // Password for user authentication

    Account(int accNum, const string &accName, double initialBalance, const string &pwd = "")
        : accountNumber(accNum), name(accName), balance(initialBalance), password(pwd) {}

    string serialize() const
    {
        ostringstream oss;
        oss << accountNumber << " " << name << " " << balance << " " << password << "\n";
        return oss.str();
    }

    static Account deserialize(const string &data)
    {
        istringstream iss(data);
        int accNum;
        string name, password;
        double balance;
        iss >> accNum;
        iss.ignore(); // Ignore the space after the account number
        getline(iss, name, ' ');
        iss >> balance >> ws;
        getline(iss, password);
        return Account(accNum, name, balance, password);
    }

    bool authenticate(const string &pwd) const
    {
        return password == pwd;
    }
};

class Bank
{
private:
    vector<Account> accounts;
    set<int> availableAccountNumbers;
    int nextAccountNumber = 1001;

    const Account *findAccount(int accountNumber) const
    {
        auto it = find_if(accounts.begin(), accounts.end(), [accountNumber](const Account &acc)
                          { return acc.accountNumber == accountNumber; });
        return (it != accounts.end()) ? &(*it) : nullptr;
    }

    void loadAccounts()
    {
        ifstream file("accounts.txt");
        if (!file)
        {
            cerr << "Error opening file for reading." << endl;
            return;
        }

        string line;
        int highestAccountNumber = 1000;
        while (getline(file, line))
        {
            if (!line.empty())
            {
                accounts.push_back(Account::deserialize(line));
                int accNum = accounts.back().accountNumber;
                if (accNum > highestAccountNumber)
                {
                    highestAccountNumber = accNum;
                }
            }
        }
        nextAccountNumber = highestAccountNumber + 1;
    }

    void saveAccounts() const
    {
        ofstream file("accounts.txt");
        if (!file)
        {
            cerr << "Error opening file for writing." << endl;
            return;
        }

        for (const auto &account : accounts)
        {
            file << account.serialize();
        }
    }

    void clearInputBuffer() const
    {
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }

    void adminMenu()
    {
        int choice;
        do
        {
            cout << "\nAdmin Menu" << endl;
            cout << "1. Create New Account" << endl;
            cout << "2. Show All Accounts" << endl;
            cout << "3. Search Account" << endl;
            cout << "4. Deposit Money" << endl;
            cout << "5. Withdraw Money" << endl;
            cout << "6. Delete Account" << endl;
            cout << "7. Balance Inquiry" << endl;
            cout << "8. Logout" << endl;
            cout << "Enter your choice: ";
            cin >> choice;
            clearInputBuffer(); // Clear input buffer after reading choice

            switch (choice)
            {
            case 1:
                createNewAccount();
                break;
            case 2:
                showAllAccounts();
                break;
            case 3:
                searchAccount();
                break;
            case 4:
                depositMoney(); // No need for an Account* argument here
                break;
            case 5:
                withdrawMoney(); // No need for an Account* argument here
                break;
            case 6:
                deleteAccount();
                break;
            case 7:
                balanceInquiry(); // No need for an Account* argument here
                break;
            case 8:
                cout << "Logging out..." << endl;
                break;
            default:
                cout << "Invalid choice. Please try again." << endl;
            }
        } while (choice != 8);
    }

    void userMenu(Account *account)
    {
        int choice;
        do
        {
            cout << "\nUser Menu" << endl;
            cout << "1. Deposit Money" << endl;
            cout << "2. Withdraw Money" << endl;
            cout << "3. Balance Inquiry" << endl;
            cout << "4. Logout" << endl;
            cout << "Enter your choice: ";
            cin >> choice;
            clearInputBuffer(); // Clear input buffer after reading choice

            switch (choice)
            {
            case 1:
                depositMoney(account);
                break;
            case 2:
                withdrawMoney(account);
                break;
            case 3:
                balanceInquiry(account);
                break;
            case 4:
                cout << "Logging out..." << endl;
                break;
            default:
                cout << "Invalid choice. Please try again." << endl;
            }
        } while (choice != 4);
    }

    void createNewAccount()
    {
        string name, password;
        double initialBalance;

        cout << "Enter account holder's name: ";
        getline(cin, name);

        cout << "Enter initial deposit amount: ";
        while (!(cin >> initialBalance) || initialBalance < 0)
        {
            cout << "Invalid input. Please enter a non-negative amount: ";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }

        cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Clear remaining newline

        cout << "Enter password for the account: ";
        getline(cin, password);

        int accountNumber;
        if (!availableAccountNumbers.empty())
        {
            accountNumber = *availableAccountNumbers.begin();
            availableAccountNumbers.erase(availableAccountNumbers.begin());
        }
        else
        {
            accountNumber = nextAccountNumber++;
        }

        accounts.emplace_back(accountNumber, name, initialBalance, password);
        cout << "Account created successfully with account number: " << accountNumber << endl;
    }

    void showAllAccounts() const
    {
        if (accounts.empty())
        {
            cout << "No accounts available." << endl;
            return;
        }

        cout << fixed << setprecision(2);
        cout << "Account Number | Name           | Balance" << endl;
        cout << "----------------------------------------" << endl;
        for (const auto &account : accounts)
        {
            cout << setw(15) << account.accountNumber << " | "
                 << setw(15) << account.name << " | "
                 << setw(10) << account.balance << endl;
        }
    }

    void searchAccount() const
    {
        int accountNumber;
        cout << "Enter account number to search: ";
        cin >> accountNumber;
        clearInputBuffer(); // Clear input buffer after reading account number

        const Account *account = findAccount(accountNumber);
        if (account)
        {
            cout << "Account Number: " << account->accountNumber << endl;
            cout << "Account Holder's Name: " << account->name << endl;
            cout << "Balance: " << account->balance << endl;
        }
        else
        {
            cout << "Account not found." << endl;
        }
    }

    void depositMoney()
    {
        int accountNumber;
        double amount;

        cout << "Enter account number to deposit into: ";
        cin >> accountNumber;
        clearInputBuffer(); // Clear input buffer after reading account number

        Account *account = const_cast<Account *>(findAccount(accountNumber));
        if (account)
        {
            cout << "Enter amount to deposit: ";
            while (!(cin >> amount) || amount < 0)
            {
                cout << "Invalid input. Please enter a non-negative amount: ";
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
            }

            account->balance += amount;
            cout << "Deposited successfully. New balance: " << account->balance << endl;
        }
        else
        {
            cout << "Account not found." << endl;
        }
        clearInputBuffer();
    }

    void withdrawMoney()
    {
        int accountNumber;
        double amount;

        cout << "Enter account number to withdraw from: ";
        cin >> accountNumber;
        clearInputBuffer(); // Clear input buffer after reading account number

        Account *account = const_cast<Account *>(findAccount(accountNumber));
        if (account)
        {
            cout << "Enter amount to withdraw: ";
            while (!(cin >> amount) || amount < 0)
            {
                cout << "Invalid input. Please enter a non-negative amount: ";
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
            }

            if (amount > account->balance)
            {
                cout << "Insufficient balance." << endl;
            }
            else
            {
                account->balance -= amount;
                cout << "Withdrawn successfully. New balance: " << account->balance << endl;
            }
        }
        else
        {
            cout << "Account not found." << endl;
        }
        clearInputBuffer();
    }

    void deleteAccount()
    {
        int accountNumber;
        cout << "Enter account number to delete: ";
        cin >> accountNumber;
        clearInputBuffer(); // Clear input buffer after reading account number

        auto it = remove_if(accounts.begin(), accounts.end(), [accountNumber](const Account &acc)
                            { return acc.accountNumber == accountNumber; });

        if (it != accounts.end())
        {
            availableAccountNumbers.insert(accountNumber);
            accounts.erase(it, accounts.end());
            cout << "Account deleted successfully." << endl;
        }
        else
        {
            cout << "Account not found." << endl;
        }
    }

    void balanceInquiry() const
    {
        int accountNumber;
        cout << "Enter account number to check balance: ";
        cin >> accountNumber;
        clearInputBuffer(); // Clear input buffer after reading account number

        const Account *account = findAccount(accountNumber);
        if (account)
        {
            cout << "Account Number: " << account->accountNumber << endl;
            cout << "Balance: " << account->balance << endl;
        }
        else
        {
            cout << "Account not found." << endl;
        }
    }

    void depositMoney(Account *account)
    {
        double amount;

        cout << "Enter amount to deposit: ";
        while (!(cin >> amount) || amount < 0)
        {
            cout << "Invalid input. Please enter a non-negative amount: ";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }

        account->balance += amount;
        cout << "Deposited successfully. New balance: " << account->balance << endl;
    }

    void withdrawMoney(Account *account)
    {
        double amount;

        cout << "Enter amount to withdraw: ";
        while (!(cin >> amount) || amount < 0)
        {
            cout << "Invalid input. Please enter a non-negative amount: ";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }

        if (amount > account->balance)
        {
            cout << "Insufficient balance." << endl;
        }
        else
        {
            account->balance -= amount;
            cout << "Withdrawn successfully. New balance: " << account->balance << endl;
        }
    }

    void balanceInquiry(Account *account) const
    {
        cout << "Account Number: " << account->accountNumber << endl;
        cout << "Balance: " << account->balance << endl;
    }

    void loginAsAdmin()
    {
        string password;
        cout << "Enter admin password: ";
        cin >> password;

        if (password == "admin123")
        {
            adminMenu();
        }
        else
        {
            cout << "Invalid admin password." << endl;
        }
    }

    void loginAsUser()
    {
        int accountNumber;
        string password;

        cout << "Enter account number: ";
        cin >> accountNumber;

        cout << "Enter password: ";
        cin >> password;

        Account *account = const_cast<Account *>(findAccount(accountNumber));
        if (account && account->authenticate(password))
        {
            userMenu(account);
        }
        else
        {
            cout << "Invalid account number or password." << endl;
        }
    }

public:
    Bank()
    {
        loadAccounts();
    }

    ~Bank()
    {
        saveAccounts();
    }

    void menu()
    {
        int choice;
        do
        {
            cout << "\nBank Management System" << endl;
            cout << "1. Login as Admin" << endl;
            cout << "2. Login as User" << endl;
            cout << "3. Exit" << endl;
            cout << "Enter your choice: ";
            cin >> choice;
            clearInputBuffer(); // Clear input buffer after reading choice

            switch (choice)
            {
            case 1:
                loginAsAdmin();
                break;
            case 2:
                loginAsUser();
                break;
            case 3:
                cout << "Exiting..." << endl;
                break;
            default:
                cout << "Invalid choice. Please try again." << endl;
            }
        } while (choice != 3);
    }
};

int main()
{
    Bank bank;
    bank.menu();
    return 0;
}

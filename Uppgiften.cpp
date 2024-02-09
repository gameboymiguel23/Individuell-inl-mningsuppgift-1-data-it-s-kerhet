#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <regex>
#include <openssl/md5.h>

using namespace std;

// Funktion för att skapa MD5-hash från en sträng
string md5(const string& input) {
    unsigned char digest[MD5_DIGEST_LENGTH];
    MD5((const unsigned char*)input.c_str(), input.length(), digest);

    char md5string[33];
    for (int i = 0; i < 16; i++)
        sprintf(&md5string[i * 2], "%02x", (unsigned int)digest[i]);
    return string(md5string);
}

// Funktion för att kontrollera lösenordskrav
bool validatePassword(const string& password) {
    // Regex för att kontrollera lösenordet
    regex passwordRegex("^(?=.*[a-z])(?=.*[A-Z])(?=.*\\d)(?=.*[@$!%*?&])[A-Za-z\\d@$!%*?&]{8,}$");
    return regex_match(password, passwordRegex);
}

// Funktion för att kontrollera e-postadress
bool validateEmail(const string& email) {
    // Regex för att kontrollera e-postadressen
    regex emailRegex(R"(([^@\s]+)@((?:[-a-z0-9]+\.)+[a-z]{2,}))");
    return regex_match(email, emailRegex);
}

// Funktion för att skapa en användare och spara den i filen
void createUser(const string& filename) {
    ofstream file(filename, ios_base::app);
    if (!file.is_open()) {
        cerr << "Kan inte öppna filen för att spara användare." << endl;
        return;
    }

    string username, password;
    cout << "Ange användarnamn (e-postadress): ";
    cin >> username;

    if (!validateEmail(username)) {
        cerr << "Ogiltig e-postadress." << endl;
        return;
    }

    // Kontrollera om användaren redan finns
    ifstream checkFile(filename);
    string line;
    while (getline(checkFile, line)) {
        size_t pos = line.find(":");
        if (pos != string::npos && line.substr(0, pos) == username) {
            cerr << "Användaren finns redan." << endl;
            return;
        }
    }
    checkFile.close();

    cout << "Ange lösenord: ";
    cin >> password;

    if (!validatePassword(password)) {
        cerr << "Ogiltigt lösenord. Det måste vara minst 8 tecken långt, innehålla minst en stor bokstav, en liten bokstav, en siffra och ett specialtecken." << endl;
        return;
    }

    string hashedPassword = md5(password);
    file << username << ":" << hashedPassword << endl;
    cout << "Användaren skapades framgångsrikt." << endl;
    file.close();
}

// Funktion för att testa inloggning
void testLogin(const string& filename) {
    string username, password;
    cout << "Ange användarnamn (e-postadress): ";
    cin >> username;
    cout << "Ange lösenord: ";
    cin >> password;

    string hashedPassword = md5(password);

    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Kan inte öppna filen för att verifiera inloggning." << endl;
        return;
    }

    string line;
    bool found = false;
    while (getline(file, line)) {
        size_t pos = line.find(":");
        if (pos != string::npos && line.substr(0, pos) == username) {
            found = true;
            if (line.substr(pos + 1) == hashedPassword) {
                cout << "OK Det gick att logga in." << endl;
            } else {
                cerr << "Fel lösenord." << endl;
            }
            break;
        }
    }
    if (!found) {
        cerr << "Användaren hittades inte." << endl;
    }
    file.close();
}

int main() {
    string filename = "users.txt";
    int choice;
    do {
        cout << "1. Skapa användare" << endl;
        cout << "2. Testa inloggning" << endl;
        cout << "0. Avsluta" << endl;
        cout << "Välj ett alternativ: ";
        cin >> choice;

        switch (choice) {
            case 1:
                createUser(filename);
                break;
            case 2:
                testLogin(filename);
                break;
            case 0:
                cout << "Avslutar programmet." << endl;
                break;
            default:
                cerr << "Ogiltigt val. Försök igen." << endl;
        }
    } while (choice != 0);

    return 0;
}

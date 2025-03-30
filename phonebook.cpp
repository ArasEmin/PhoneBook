#include <iostream>
#include <fstream>
#include <string>
#include <locale.h>
#include <windows.h>
#include <algorithm> // transform için

using namespace std;

struct PhoneBook {
    string name;
    string number;
};

// Şifreleme ve şifre çözme fonksiyonu
string encryptDecrypt(const string& data, const string& key) {
    string result = data;
    for (size_t i = 0; i < data.size(); ++i) {
        result[i] = data[i] ^ key[i % key.size()];
    }
    return result;
}

const string ENCRYPTION_KEY = "ARSEEnter"; // Gerçek uygulamada daha karmaşık ve güvenli bir anahtar kullanın

void addNo(PhoneBook ph) {
    setlocale(LC_ALL,"Turkish");
    system("cls");
    ofstream outfile("PhoneBook.dat", ios::app | ios::binary); // Binary modda açıyoruz
    if (!outfile) {
        cout << "\tHata: Dosya Açılamadı!" << endl;
        return;
    }

    cout << "\tİsim giriniz: ";
    cin >> ph.name;
    cout << "\tNumara giriniz: ";
    cin >> ph.number;

    // Verileri şifrele
    string encryptedName = encryptDecrypt(ph.name, ENCRYPTION_KEY);
    string encryptedNumber = encryptDecrypt(ph.number, ENCRYPTION_KEY);

    // Uzunluk bilgisiyle birlikte kaydet
    size_t nameLen = encryptedName.size();
    size_t numLen = encryptedNumber.size();
    
    outfile.write(reinterpret_cast<const char*>(&nameLen), sizeof(nameLen));
    outfile.write(encryptedName.c_str(), nameLen);
    outfile.write(reinterpret_cast<const char*>(&numLen), sizeof(numLen));
    outfile.write(encryptedNumber.c_str(), numLen);

    outfile.close();

    cout << "\tKayıt başarıyla eklendi!" << endl;
    Sleep(3000);
}

bool readEntry(ifstream& infile, PhoneBook& ph) {
    size_t nameLen, numLen;
    
    if (!infile.read(reinterpret_cast<char*>(&nameLen), sizeof(nameLen)))
        return false;
    
    char* nameBuf = new char[nameLen+1];
    infile.read(nameBuf, nameLen);
    nameBuf[nameLen] = '\0';
    ph.name = encryptDecrypt(string(nameBuf), ENCRYPTION_KEY);
    delete[] nameBuf;
    
    if (!infile.read(reinterpret_cast<char*>(&numLen), sizeof(numLen)))
        return false;
    
    char* numBuf = new char[numLen+1];
    infile.read(numBuf, numLen);
    numBuf[numLen] = '\0';
    ph.number = encryptDecrypt(string(numBuf), ENCRYPTION_KEY);
    delete[] numBuf;
    
    return true;
}

void searchNo() {
    system("cls");
    ifstream infile("PhoneBook.dat", ios::binary);
    if (!infile) {
        cout << "\tHata: Dosya Açılamadı!" << endl;
        Sleep(3000);
        return;
    }

    string search;
    cout << "\tAranacak İsmi Girin: ";
    cin >> search;

    PhoneBook ph;
    bool found = false;
    
    while (readEntry(infile, ph)) {
        if (ph.name.find(search) != string::npos) {
            cout << "\tBulundu: " << ph.name << " : " << ph.number << endl;
            found = true;
            break;
        }
    }

    infile.close();
    if (!found)
        cout << "\tKayıt bulunamadı!" << endl;

    Sleep(3000);
}

void listAllContacts() {
    system("cls");
    ifstream infile("PhoneBook.dat", ios::binary);
    if (!infile) {
        cout << "\tHata: Dosya Açılamadı veya henüz kayıt yok!" << endl;
    } else {
        PhoneBook ph;
        cout << "\t--- Kayıtlı Tüm Kullanıcılar ---" << endl;
        while (readEntry(infile, ph)) {
            cout << "\t" << ph.name << " : " << ph.number << endl;
        }
    }
    infile.close();
    cout << "\n\tDevam etmek için bir tuşa basın...";
    cin.ignore();
    cin.get();
}

void deleteContact() {
    setlocale(LC_ALL,"Turkish");
    system("cls");
    listAllContacts();
    
    string nameToDelete;
    cout << "\n\tSilmek İstediğiniz Kullanıcının İsmini Girin: ";
    cin >> nameToDelete;

    ifstream infile("PhoneBook.dat", ios::binary);
    ofstream tempFile("Temp.dat", ios::binary);

    bool deleted = false;
    if (!infile || !tempFile) {
        cout << "\tHata: Dosya Açılamadı!" << endl;
        Sleep(3000);
        return;
    }

    PhoneBook ph;
    while (readEntry(infile, ph)) {
        if (ph.name != nameToDelete) {
            // Silinmeyecek kayıtları yeni dosyaya yaz
            string encryptedName = encryptDecrypt(ph.name, ENCRYPTION_KEY);
            string encryptedNumber = encryptDecrypt(ph.number, ENCRYPTION_KEY);
            
            size_t nameLen = encryptedName.size();
            size_t numLen = encryptedNumber.size();
            
            tempFile.write(reinterpret_cast<const char*>(&nameLen), sizeof(nameLen));
            tempFile.write(encryptedName.c_str(), nameLen);
            tempFile.write(reinterpret_cast<const char*>(&numLen), sizeof(numLen));
            tempFile.write(encryptedNumber.c_str(), numLen);
        } else {
            deleted = true;
        }
    }

    infile.close();
    tempFile.close();

    remove("PhoneBook.dat");
    rename("Temp.dat", "PhoneBook.dat");

    if (deleted) {
        cout << "\tKullanıcı başarıyla silindi!" << endl;
    } else {
        cout << "\tKullanıcı bulunamadı!" << endl;
    }

    Sleep(3000);
}


int main() {
    setlocale(LC_ALL,"Turkish");
    PhoneBook ph;
    int val;
    bool exit = false;

    while (!exit) {
        system("cls");
        cout << "\n\t========= ARSE TELEFON REHBERİ =========" << endl;
        cout << "\t1. Numara Ekle." << endl;
        cout << "\t2. Numara Ara." << endl;
        cout << "\t3. Tüm Kayıtları Listele." << endl;
        cout << "\t4. Kayıt Sil." << endl;
        cout << "\t5. Çıkış." << endl;
        cout << "\tSeçiminiz: ";
        cin >> val;

        switch (val) {
            case 1:
                addNo(ph);
                break;
            case 2:
                searchNo();
                break;
            case 3:
                listAllContacts();
                break;
            case 4:
                deleteContact();
                break;
            case 5:
                system("cls");
                cout << "\tİyi Günler...!" << endl;
                Sleep(3000);
                exit = true;
                break;
            default:
                cout << "\tGeçersiz seçim, tekrar deneyin." << endl;
                Sleep(2000);
                break;
        }
    }

    return 0;
}
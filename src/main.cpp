#include <iostream>
#include <string>
#include <cstdlib>  // Для system("clear") на Linux/Mac OS
#include <limits>   // Для очистки потока ввода
#include <algorithm> // Для std::transform
#include <fstream>  // Для работы с файлами
#include <sys/stat.h> // Для проверки существования папки на Unix-подобных системах
#include <direct.h>   // Для Windows

using namespace std;

// Перечисление платформ для улучшенной безопасности
enum class Platform {
    Windows,
    Linux,
    MacOS,
    Unknown
};

// Функция для вывода реалистичного глаза в ASCII-арт
void printArt() {
    cout << "                               ,--,                                                " << endl;
    cout << "                 ,----..   ,---.'|  ,-.----.           ,--,                 ,--. " << endl;
    cout << "    ,---,       /   /   \\  |   | :  \\    /  \\        ,--.'|   ,---,       ,--.'| " << endl;
    cout << "  .'  .' `\\    /   .     : :   : |  |   :    \\    ,--,  | :,`--.' |   ,--,:  : | " << endl;
    cout << ",---.'     \\  .   /   ;.  \\|   ' :  |   |  .\\ :,---.'|  : '|   :  :,`--.'`|  ' : " << endl;
    cout << "|   |  .`\\  |.   ;   /  ` ;;   ; '  .   :  |: ||   | : _' |:   |  '|   :  :  | | " << endl;
    cout << ":   : |  '  |;   |  ; \\ ; |'   | |__|   |   \\ ::   : |.'  ||   :  |:   | '  '; | " << endl;
    cout << "|   ' '  ;  :|   :  | ; | '|   | :.'|   : .   /|   ' '  ; :'   '  ;|   : '  '; | " << endl;
    cout << "'   | ;  .  |.   |  ' ' ' :'   :    ;   | |`-' '   |  .'. ||   |  |'   ' ;.    ; " << endl;
    cout << "|   | :  |  ''   ;  \\; /  ||   |  ./|   | ;    |   | :  | ''   :  ;|   | | \\   | " << endl;
    cout << "'   : | /  ;  \\   \\ .'   '---'    |   | :    ;   : ;--'  ;   |.' '   : |  ; .' " << endl;
    cout << "|   ,.'          `---`              `---'.|    |   ,/      '---'   ;   |.'       " << endl;
    cout << "'---'                                 `---`    '---'               '---'         " << endl;
}

// Функция для очистки экрана
void clearScreen() {
    #ifdef _WIN32
        system("cls");  // Для Windows
    #else
        system("clear");  // Для Linux/Mac OS
    #endif
}

// Функция сборки для разных платформ
void build(Platform platform) {
    cout << "\033[1;32m[INFO]\033[0m Starting build process..." << endl;

    string programName, version;
    string command;

    cout << "\033[1;36m[INFO]\033[0m Enter program name: ";
    getline(cin, programName);

    cout << "\033[1;36m[INFO]\033[0m Enter version: ";
    getline(cin, version);

    // Создаем папку build, если она не существует
    #ifdef _WIN32
        if (_mkdir("build") != 0 && errno != EEXIST) {
            cout << "\033[1;31m[ERROR]\033[0m Failed to create build directory." << endl;
            return;
        }
    #else
        struct stat st = {0};
        if (stat("build", &st) == -1) {
            if (mkdir("build", 0700) != 0) {
                cout << "\033[1;31m[ERROR]\033[0m Failed to create build directory." << endl;
                return;
            }
        }
    #endif

    // Определяем путь к папке src и собираем команду для компиляции
    cout << "\033[1;34m[INFO]\033[0m Compiling for selected platform..." << endl;

    // Для Windows собираем полный путь файлов
    if (platform == Platform::Windows) {
        // Открываем файл для записи путей файлов
        ofstream fileList("filelist.txt");

        // Собираем полный путь для каждого файла в папке src
        system("dir /b /a-d src\\*.cpp > temp.txt"); // Сначала получаем имена файлов в папке src
        ifstream tempFile("temp.txt");
        string fileName;
        while (getline(tempFile, fileName)) {
            fileList << "src/" << fileName << endl;  // Записываем полный путь в файл
        }

        // Удаляем временный файл с именами
        tempFile.close();
        remove("temp.txt");

        // Собираем команду для компиляции с полными путями в папку build
        command = "g++ @filelist.txt -o build/" + programName + ".exe && del filelist.txt";
    } else if (platform == Platform::Linux || platform == Platform::MacOS) {
        // Для Unix-подобных систем командой оболочки
        command = "g++ src/*.cpp -o build/" + programName;
    } else {
        cout << "\033[1;31m[ERROR]\033[0m Unknown platform for building." << endl;
        return;
    }

    // Выполняем команду сборки
    int result = system(command.c_str());
    if (result == 0) {
        cout << "\033[1;32m[SUCCESS]\033[0m Build completed successfully!" << endl;
    } else {
        cout << "\033[1;31m[ERROR]\033[0m Build failed!" << endl;
    }
}

// Функция для выбора платформы с улучшенной обработкой ввода
Platform choosePlatform() {
    cout << "\033[1;36m[INFO]\033[0m Enter platform (Windows, Linux, Mac OS): ";
    string platform;
    getline(cin, platform); // Используем getline для ввода с пробелами

    // Приводим строку к нижнему регистру
    transform(platform.begin(), platform.end(), platform.begin(), ::tolower);

    // Теперь сравниваем строки без учета регистра
    if (platform == "windows" || platform == "win") {
        return Platform::Windows;
    } else if (platform == "linux" || platform == "unix") {
        return Platform::Linux;
    } else if (platform == "mac" || platform == "macos") {
        return Platform::MacOS;
    } else {
        return Platform::Unknown;
    }
}

// Функция для отображения меню
void displayMenu() {
    cout << "\033[48;5;235m\033[38;5;255m### Build System Console ###\033[0m\n" << endl;
    cout << "\033[1;33m1. Build\033[0m\n";
    cout << "\033[1;33m2. Choose platform\033[0m\n";
    cout << "\033[1;33m3. Exit\033[0m\n";
}

// Основной цикл программы
int main() {
    int choice;
    Platform selectedPlatform = Platform::Unknown;

    // Основной цикл программы
    while (true) {
        clearScreen();

        // Выводим ASCII-арт глаза
        printArt();

        // Отображаем меню
        displayMenu();

        cout << "\033[1;36mEnter your choice: \033[0m";
        cin >> choice;

        // Очистка потока ввода после ввода числа
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        switch (choice) {
            case 1:
                if (selectedPlatform != Platform::Unknown) {
                    build(selectedPlatform);  // Строим для выбранной платформы
                } else {
                    cout << "\033[1;31m[ERROR]\033[0m No platform selected!" << endl;
                }
                break;
            case 2:
                selectedPlatform = choosePlatform();
                if (selectedPlatform == Platform::Unknown) {
                    cout << "\033[1;31m[ERROR]\033[0m Unknown platform selected!" << endl;
                } else {
                    cout << "\033[1;36m[INFO]\033[0m Platform selected successfully!" << endl;
                }
                break;
            case 3:
                cout << "\033[1;32m[INFO]\033[0m Exiting..." << endl;
                return 0;
            default:
                cout << "\033[1;31m[ERROR]\033[0m Invalid choice. Please select again." << endl;
                break;
        }

        // Задержка перед возвращением в меню
        system("pause");
    }
}

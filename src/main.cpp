#include <iostream>
#include <string>
#include <cstdlib>  // Для system("clear") на Linux/Mac OS
#include <limits>   // Для очистки потока ввода
#include <algorithm> // Для std::transform
#include <fstream>  // Для работы с файлами
#include <sys/stat.h> // Для проверки существования папки на Unix-подобных системах
#include <direct.h>   // Для Windows
#include <filesystem>  // Для работы с папками и файлами

namespace fs = std::filesystem;  // Для работы с файловой системой

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

// Функция поиска всех заголовочных файлов и библиотек в папках include и lib
void findLibrariesAndIncludes(string& libraries, string& includes) {
    // Папки для поиска
    string includeDir = "include";
    string libDir = "lib";

    // Проверяем, существует ли папка include
    if (fs::exists(includeDir) && fs::is_directory(includeDir)) {
        // Поиск заголовочных файлов (*.h) в папке include
        for (const auto& entry : fs::directory_iterator(includeDir)) {
            if (entry.is_regular_file() && entry.path().extension() == ".h") {
                includes += " -I" + entry.path().parent_path().string();
            }
        }
    }

    // Проверяем, существует ли папка lib
    if (fs::exists(libDir) && fs::is_directory(libDir)) {
        // Поиск библиотек (*.lib или *.a) в папке lib
        for (const auto& entry : fs::directory_iterator(libDir)) {
            if (entry.is_regular_file() && (entry.path().extension() == ".lib" || entry.path().extension() == ".a")) {
                libraries += " -L" + entry.path().parent_path().string() + " -l" + entry.path().stem().string();
            }
        }
    }
}

// Функция сборки для разных платформ
void build(Platform platform, const string& libraries, const string& includes) {
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
        command = "g++ @filelist.txt " + includes + " " + libraries + " -o build/" + programName + ".exe && del filelist.txt";
    } else if (platform == Platform::Linux || platform == Platform::MacOS) {
        // Для Unix-подобных систем командой оболочки
        command = "g++ src/*.cpp " + includes + " " + libraries + " -o build/" + programName;
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

    // Приводим к нижнему регистру
    transform(platform.begin(), platform.end(), platform.begin(), ::tolower);

    if (platform == "windows") {
        return Platform::Windows;
    } else if (platform == "linux") {
        return Platform::Linux;
    } else if (platform == "macos" || platform == "osx") {
        return Platform::MacOS;
    } else {
        return Platform::Unknown;
    }
}

int main() {
    Platform selectedPlatform = Platform::Unknown;
    string libraries, includes;
    int choice;

    // Поиск библиотек и заголовочных файлов
    findLibrariesAndIncludes(libraries, includes);

    while (true) {
        clearScreen();
        printArt();  // Печать ASCII-арт глаза

        // Меню
        cout << "\033[1;36mPlease select an option:\033[0m" << endl;
        cout << "1. Build Program" << endl;
        cout << "2. Select Platform" << endl;
        cout << "3. Add Libraries (automatic from include/lib)" << endl;
        cout << "4. Exit" << endl;
        cout << "\033[1;36mEnter choice (1-4): \033[0m";
        cin >> choice;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');  // Очистка потока после ввода числа

        switch (choice) {
            case 1: {
                // Сборка программы
                if (selectedPlatform == Platform::Unknown) {
                    cout << "\033[1;31m[ERROR]\033[0m Please select a platform first." << endl;
                } else {
                    build(selectedPlatform, libraries, includes);
                }
                break;
            }
            case 2: {
                // Выбор платформы
                selectedPlatform = choosePlatform();
                if (selectedPlatform == Platform::Unknown) {
                    cout << "\033[1;31m[ERROR]\033[0m Unknown platform selected. Please try again." << endl;
                } else {
                    cout << "\033[1;32m[INFO]\033[0m Platform selected successfully." << endl;
                }
                break;
            }
            case 3: {
                // Дополнительный шаг - автоматический выбор библиотек
                cout << "\033[1;32m[INFO]\033[0m Automatically adding libraries from 'include' and 'lib' directories." << endl;
                break;
            }
            case 4:
                cout << "Exiting program..." << endl;
                return 0;
            default:
                cout << "\033[1;31m[ERROR]\033[0m Invalid choice, try again." << endl;
        }

        // Задержка, чтобы пользователь мог увидеть вывод
        system("pause");
    }

    return 0;
}

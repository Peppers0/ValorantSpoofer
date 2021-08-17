
#include "main.h"

extern "C" {
#include "clean.h"
}

int main() {

    SetConsoleTitleA(utils::randstr(32).c_str());

    std::cout << _xor_("Starting The Hwid Spoof Process") << std::endl;

    Sleep(2000);

    for (int i = 0; i < 22; i++) {
        system(_xor_("cls").c_str());
        std::cout << elements::commands[i] << std::endl;
        
        Sleep(250);
    }

    CleanProcess();

    system(_xor_("cls").c_str());
    std::cout << _xor_("FINISH") << std::endl;

    DWORD cchBuffer;
    WCHAR* driveStrings;
    UINT driveType;
    PWSTR driveTypeString;
    ULARGE_INTEGER freeSpace;

    cchBuffer = GetLogicalDriveStrings(0, NULL);
    driveStrings = (WCHAR*)malloc((cchBuffer + 1) * sizeof(TCHAR));

    if (driveStrings == NULL)
        return -1;

    GetLogicalDriveStrings(cchBuffer, driveStrings);

    while (*driveStrings)
    {
        driveType = GetDriveType(driveStrings);
        GetDiskFreeSpaceEx(driveStrings, &freeSpace, NULL, NULL);
        std::string spoofDrive;
        switch (driveType)
        {
        case DRIVE_FIXED:
            system(_xor_("cls").c_str());
            spoofDrive = utils::utf8_encode(driveStrings).substr(0, 2);
            system(utils::stringer(_xor_("Volumeid64 "), spoofDrive, _xor_(" "), utils::randnum(4), _xor_("-"), utils::randnum(4)).c_str());
            break;
        default:
            break;
        }
        driveStrings += lstrlen(driveStrings) + 1;
    }

    free(driveStrings);
    return 0;
}
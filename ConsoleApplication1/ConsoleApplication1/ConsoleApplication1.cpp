#include <iostream>
#include <windows.h>
#include <tlhelp32.h>
#include <string>

using namespace std;

void createProcess() {
    // Creating a new process
    STARTUPINFOA si = { sizeof(si) };
    PROCESS_INFORMATION pi;
    std::string commandLine = "notepad.exe C:\example.txt";
    if (CreateProcessA(NULL, const_cast<char*>(commandLine.c_str()), NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi)) {
        cout << "Process created successfully!" << endl;
        cout << "Process ID: " << pi.dwProcessId << endl;
        cout << "Thread ID: " << pi.dwThreadId << endl;
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
    }
    else {
        cerr << "Error creating process: " << GetLastError() << endl;
    }
}

void listProcesses() {
    // Listing all processes
    HANDLE hProcessSnap;
    PROCESSENTRY32 pe32;
    hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hProcessSnap == INVALID_HANDLE_VALUE) {
        cerr << "Error getting process snapshot!" << endl;
        return;
    }
    pe32.dwSize = sizeof(PROCESSENTRY32);
    if (!Process32First(hProcessSnap, &pe32)) {
        cerr << "Error getting the first process!" << endl;
        CloseHandle(hProcessSnap);
        return;
    }
    cout << "Process list:" << endl;
    do {
        HANDLE hProcess = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pe32.th32ProcessID);
        if (hProcess != NULL) {
            char processName[MAX_PATH] = { 0 };
            DWORD size = sizeof(processName);
            if (QueryFullProcessImageNameA(hProcess, 0, processName, &size)) {
                cout << "Name: " << processName << endl;
            }
            CloseHandle(hProcess);
        }
        cout << "ID: " << pe32.th32ProcessID << endl;
        cout << "Parent Process ID: " << pe32.th32ParentProcessID << endl;
        cout << "Thread Count: " << pe32.cntThreads << endl;
    } while (Process32Next(hProcessSnap, &pe32));
    CloseHandle(hProcessSnap);
}

void terminateProcess(DWORD processId) {
    // Terminating a selected process
    HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, FALSE, processId);
    if (hProcess == NULL) {
        cerr << "Error opening process for termination!" << endl;
        return;
    }
    if (TerminateProcess(hProcess, 0)) {
        cout << "Process terminated successfully!" << endl;
    }
    else {
        cerr << "Error terminating process!" << endl;
    }
    CloseHandle(hProcess);
}

void listThreads(DWORD processId) {
    // Listing all threads of a selected process
    HANDLE hThreadSnap = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
    if (hThreadSnap == INVALID_HANDLE_VALUE) {
        cerr << "Error getting thread snapshot!" << endl;
        return;
    }

    THREADENTRY32 te32;
    te32.dwSize = sizeof(THREADENTRY32);

    if (!Thread32First(hThreadSnap, &te32)) {
        cerr << "Error getting the first thread!" << endl;
        CloseHandle(hThreadSnap);
        return;
    }

    cout << "Thread list for process with ID " << processId << ":" << endl;
    do {
        if (te32.th32OwnerProcessID == processId) {
            cout << "Thread ID: " << te32.th32ThreadID << endl;
        }
    } while (Thread32Next(hThreadSnap, &te32));

    CloseHandle(hThreadSnap);
}

void listModules(DWORD processId) {
    // Listing all modules of a selected process
    HANDLE hModuleSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, processId);
    if (hModuleSnap == INVALID_HANDLE_VALUE) {
        cerr << "Error getting module snapshot!" << endl;
        return;
    }

    MODULEENTRY32 me32;
    me32.dwSize = sizeof(MODULEENTRY32);

    if (!Module32First(hModuleSnap, &me32)) {
        cerr << "Error getting the first module!" << endl;
        CloseHandle(hModuleSnap);
        return;
    }

    cout << "Module list for process with ID " << processId << ":" << endl;
    do {
        cout << "Name: " << me32.szModule << endl;
        cout << "Path: " << me32.szExePath << endl;
    } while (Module32Next(hModuleSnap, &me32));

    CloseHandle(hModuleSnap);
}
void runNewProcess(const string& commandLine) {
    // Running a new process
    STARTUPINFOA si = { sizeof(si) };
    PROCESS_INFORMATION pi;

    if (CreateProcessA(NULL, const_cast<char*>(commandLine.c_str()), NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi)) {
        cout << "New process launched successfully!" << endl;
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
    }
    else {
        cerr << "Error launching new process!" << endl;
    }
}

int main() {
    int choice;
    do {
        cout << "Menu:" << endl;
        cout << "1. Create process" << endl;
        cout << "2. List all processes" << endl;
        cout << "3. Terminate selected process" << endl;
        cout << "4. List all threads of selected process" << endl;
        cout << "5. List all modules of selected process" << endl;
        cout << "6. Run new process" << endl;
        cout << "0. Exit" << endl;
        cout << "Select an option: ";
        cin >> choice;

        switch (choice) {
        case 1:
            createProcess();
            break;
        case 2:
            listProcesses();
            break;
        case 3: {
            DWORD processId;
            cout << "Enter the process ID to terminate: ";
            cin >> processId;
            terminateProcess(processId);
            break;
        }
        case 4: {
            DWORD processId;
            cout << "Enter the process ID: ";
            cin >> processId;
            listThreads(processId);
            break;
        }
        case 5: {
            DWORD processId;
            cout << "Enter the process ID: ";
            cin >> processId;
            listModules(processId);
            break;
        }
        case 6: {
            string commandLine;
            cout << "Enter the command line to run a new process: ";
            cin.ignore(); // Clear input buffer
            getline(cin, commandLine);
            runNewProcess(commandLine);
            break;
        }
        case 0:
            cout << "Program exiting." << endl;
            break;
        default:
            cerr << "Invalid option!" << endl;
        }
    } while (choice != 0);

    return 0;
}
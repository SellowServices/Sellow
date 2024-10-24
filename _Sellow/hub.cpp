#include "hub.h"
#include "./ui_hub.h"

#include <QFileDialog>
#include <QPushButton>
#include <QString>
#include <QMessageBox>
#include <QStringListModel>
#include <QListView>
#include <QDir>
#include <windows.h>
#include <tlhelp32.h>

Hub::Hub(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Hub)
{
    ui->setupUi(this);
    loadActiveApplications(); // Load active applications on startup
}

Hub::~Hub()
{
    delete ui;
}

void Hub::loadActiveApplications() {
    QStringList appList;
    HANDLE hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hProcessSnap == INVALID_HANDLE_VALUE) {
        return; // Handle error
    }

    PROCESSENTRY32 pe32;
    pe32.dwSize = sizeof(PROCESSENTRY32);

    if (Process32First(hProcessSnap, &pe32)) {
        do {
            HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pe32.th32ProcessID);
            if (hProcess != NULL) {
                HWND hwnd = FindWindow(NULL, pe32.szExeFile);
                if (hwnd != NULL) {
                    char title[256];
                    GetWindowTextA(hwnd, title, sizeof(title));
                    appList.append(QString::fromLocal8Bit(title));
                }
                CloseHandle(hProcess);
            }
        } while (Process32Next(hProcessSnap, &pe32));
    }

    CloseHandle(hProcessSnap);

    QStringListModel *model = new QStringListModel(this);
    model->setStringList(appList);
    ui->listView->setModel(model);  // Assuming listView is a QListView in your UI
}

void Hub::on_injectButton_clicked()
{
    QString dllPath = ui->fileLineEdit->text();
    if (dllPath.isEmpty()) {
        QMessageBox::warning(this, tr("Warning"), tr("Please select a DLL file first!"));
        return;
    }

    // Convert slashes to the native format for the current operating system
    dllPath = QDir::toNativeSeparators(dllPath);

    const wchar_t* windowTitle = L"Untitled - Notepad"; // Update this if necessary
    std::wstring dllName = dllPath.toStdWString();  // Store the DLL path correctly

    InjectorTool injector(dllName.c_str(), windowTitle);
    if (!injector.inject()) {
        QMessageBox::critical(this, tr("Error"), tr("DLL injection failed! Please check the DLL and the target application."));
    } else {
        QMessageBox::information(this, tr("Success"), tr("DLL injected successfully!"));
    }
}

void Hub::on_fileSelectButton_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Select DLL File"), "", tr("DLL Files (*.dll);;All Files (*)"));
    if (!fileName.isEmpty()) {
        ui->fileLineEdit->setText(fileName);
    }
}

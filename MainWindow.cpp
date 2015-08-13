#include "MainWindow.h"
#include "ui_MainWindow.h"
#include <QDirIterator>
#include <QDebug>
#include <QFileDialog>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_OFPath("")
    , m_OFAddonsPath("")
    , m_OFAppTemplatePath("")
    , m_AppPath("")
    , m_AddonsPath("")
    , m_IsAppNameValid(false)
    , m_IsOFPathValid(false)
    , m_IsAppFolderValid(false)
{
    ui->setupUi(this);
    connect(ui->lineEditOfPath, SIGNAL(textChanged(QString)), this, SLOT(getAddonNames()));
    connect(ui->lineEditAppName, SIGNAL(textChanged(QString)), this, SLOT(checkAppNameValidity(QString)));
    connect(ui->lineEditAppPath, SIGNAL(textChanged(QString)), this, SLOT(checkAppFolderValidity(QString)));
    connect(ui->listWidget, SIGNAL(itemClicked(QListWidgetItem *)), this, SLOT(getSelectedAddons(QListWidgetItem *)));
    connect(ui->buttonOfPath, SIGNAL(pressed()), this, SLOT(browseOFPath()));
    connect(ui->buttonAppPath, SIGNAL(pressed()), this, SLOT(browseAppPath()));
    connect(ui->buttonGenerate, SIGNAL(pressed()), this, SLOT(generateProject()));
}

MainWindow::~MainWindow()
{
    for (int i = 0; i < m_AddonItems.size(); i++) {
        delete m_AddonItems[i];
    }
    m_AddonItems.clear();
    delete ui;
}

void MainWindow::getAddonNames()
{
    ui->listWidget->clear();
    m_OFPath = ui->lineEditOfPath->text() + "/";
    m_OFAddonsPath = m_OFPath + "addons/";
    m_OFAppTemplatePath = m_OFPath + "apps/myApps/emptyExample/src/";

    QDirIterator dirIteratorTop(m_OFAddonsPath, QDirIterator::NoIteratorFlags);
    if (dirIteratorTop.hasNext() == false) {
        ui->lineEditOfPath->setStyleSheet("color: red");
        m_IsOFPathValid = false;
    }
    else {
        ui->lineEditOfPath->setStyleSheet("");
        m_IsOFPathValid = true;
    }
    while (dirIteratorTop.hasNext()) {
        dirIteratorTop.next();
        if (dirIteratorTop.fileName() == "." || dirIteratorTop.fileName() == "..") {
            continue;
        }
        QListWidgetItem *wid = new QListWidgetItem(ui->listWidget);
        wid->setText(dirIteratorTop.fileName());
        wid->setCheckState(Qt::CheckState::Unchecked);
        m_AddonItems.append(wid);
    }
}

void MainWindow::checkAppNameValidity(QString str)
{
    if (str.contains(" ")) {
        ui->lineEditAppName->setStyleSheet("color: red");
        m_IsAppNameValid = false;
    }
    else if (str.length() == 0) {
        ui->lineEditAppName->setStyleSheet("color: red");
        m_IsAppNameValid = false;
    }
    else {
        ui->lineEditAppName->setStyleSheet("");
        m_IsAppNameValid = true;
    }
}

void MainWindow::checkAppFolderValidity(QString str)
{
    QDir dir(str);
    if (dir.exists() == false) {
        ui->lineEditAppPath->setStyleSheet("color: red");
        m_IsAppFolderValid = false;
    }
    else if (str.length() == 0) {
        ui->lineEditAppPath->setStyleSheet("color: red");
        m_IsAppFolderValid = false;
    }
    else if (dir.exists()) {
        ui->lineEditAppPath->setStyleSheet("");
        m_IsAppFolderValid = true;
    }
}

void MainWindow::browseOFPath()
{
    QFileDialog dialog(this, "Select OF Path");
    const QString selectedDir = dialog.getExistingDirectory();
    ui->lineEditOfPath->setText(selectedDir);
}

void MainWindow::browseAppPath()
{
    QFileDialog dialog(this, "Select App Path");
    const QString selectedDir = dialog.getExistingDirectory();
    ui->lineEditAppPath->setText(selectedDir);
}

void MainWindow::getSelectedAddons(QListWidgetItem *selectedItem)
{
    if (selectedItem == nullptr) {
        return;
    }

    if (selectedItem->checkState() == Qt::Checked && m_SelectedAddons.contains(selectedItem->text()) == false) {
        m_SelectedAddons.append(selectedItem->text());
    }
    else if (selectedItem->checkState() == Qt::Unchecked && m_SelectedAddons.contains(selectedItem->text())) {
        m_SelectedAddons.removeAt(m_SelectedAddons.indexOf(selectedItem->text()));
    }
}

void MainWindow::generateProject()
{
    const QString errStr = getErrorString();
    if (errStr.length() > 0) {
        QMessageBox::warning(this, "Error!", "Please correct the following misteke(s).\n" + errStr, QMessageBox::Ok);
        return;
    }
    QFile priFile("./data/openFrameworks-0.8.4.pri");
    QDir dir(ui->lineEditAppPath->text() + "/");
    //Create the project dir and copy the template
    if (dir.exists()) {
        dir.mkdir(ui->lineEditAppName->text());
        m_AppPath = ui->lineEditAppPath->text() + "/" + ui->lineEditAppName->text() + "/";
        QFile::copy(m_OFAppTemplatePath + "main.cpp", m_AppPath + "main.cpp");
        QFile::copy(m_OFAppTemplatePath + "ofApp.cpp", m_AppPath + "ofApp.cpp");
        QFile::copy(m_OFAppTemplatePath + "ofApp.h", m_AppPath + "ofApp.h");
        QFile::copy("./data/proFileTemplate.pro", m_AppPath + ui->lineEditAppName->text() + ".pro");
    }

    if (priFile.exists() && priFile.open(QIODevice::ReadOnly)) {
        QString contents = QString(priFile.readAll());
        //Insert OF path
        QString ofPathWithoutSuffix = m_OFPath;
        contents.replace("#OF_PATH#", "\"" + ofPathWithoutSuffix.remove(ofPathWithoutSuffix.length() - 1, 1) + "\"");
        insertAddons(contents);

        //Write the changed pri file to the new path
        QFile newProFile(m_AppPath + "openFrameworks-0.8.4.pri");
        if (newProFile.open(QIODevice::WriteOnly)) {
            newProFile.write(contents.toStdString().c_str());
            newProFile.close();
        }

    }
    priFile.close();
    ui->statusBar->showMessage("Project generated", 5000);
    ui->lineEditAppName->clear();
    for (int i = 0; i < m_AddonItems.size(); i++) {
        QListWidgetItem *item = m_AddonItems.at(i);
        item->setCheckState(Qt::Unchecked);
    }
}

QString MainWindow::getErrorString() const
{
    QString errStr;
    if (m_IsAppNameValid == false) {
        errStr += "* Type a valid app name";
    }
    if (m_IsAppFolderValid == false) {
        errStr += "\n* Type a valid app path";
    }
    if (m_IsOFPathValid == false) {
        errStr += "\n* Type a valid openFrameworks path";
    }
    return errStr;
}

void MainWindow::insertAddons(QString &priContent)
{
    if (m_SelectedAddons.size() == 0) {
        return;
    }

    bool isCopyEnabled = ui->checkBox->isChecked();
    QString addonRootPath = "$$OF/addons/";
    if (isCopyEnabled) {
        QDir dir(m_AppPath);
        if (dir.exists()) {
            dir.mkdir("addons");
            m_AddonsPath = m_AppPath + "addons/";
        }
        addonRootPath = m_AddonsPath;
        for (int i = 0; i < m_SelectedAddons.size(); i++) {
            const QString currentAddonName = m_SelectedAddons.at(i);
            const QString srcAddonPath = m_OFAddonsPath + currentAddonName;
            QDir dir(m_AddonsPath);
            dir.mkdir(currentAddonName);
            copyRecursively(srcAddonPath + "/src", m_AddonsPath + currentAddonName);
            copyRecursively(srcAddonPath + "/libs", m_AddonsPath + currentAddonName);
        }
    }

    QStringList includePaths;
    for (int i = 0; i < m_SelectedAddons.size(); i++) {
        const QString addonName = m_SelectedAddons.at(i);
        priContent += "#" + addonName + "\n";
        const QString addonPath = addonRootPath + addonName;
        QStringList folderList;
        folderList.append("/src");
        folderList.append("/libs");
        if (isCopyEnabled) {
            priContent += "INCLUDEPATH += \"$$PWD/addons/" + addonName + "/src/" + "\"\n";
            QDir libsDir(addonPath + "/libs");
            if (libsDir.exists()) {
                priContent += "INCLUDEPATH += \"$$PWD/addons/" + addonName + "/libs/" + "\"\n";
            }
        }
        else {
            priContent += "INCLUDEPATH += \"" + addonPath + "/src/" + "\"\n";
            QDir libsDir(addonPath + "/libs");
            if (libsDir.exists()) {
                priContent += "INCLUDEPATH += \"" + addonPath + "/libs/" + "\"\n";
            }
        }
        foreach (const QString &folder, folderList) {
            QDirIterator dirIt(addonPath + folder, QDirIterator::Subdirectories);
            while (dirIt.hasNext()) {
                dirIt.next();
                if (dirIt.fileName() == "." || dirIt.fileName() == "..") {
                    continue;
                }

                const QString filePath = isCopyEnabled ? dirIt.filePath().replace(m_AddonsPath, "$$PWD/addons/") : dirIt.filePath();
                if (dirIt.fileInfo().isDir()) {
                    if (includePaths.contains(dirIt.fileInfo().absoluteDir().absolutePath()) == false) {
                        priContent += "INCLUDEPATH += \"" + filePath + "\"\n";
                    }
                    continue;
                }

                if (dirIt.fileInfo().suffix() == "cpp") {
                    priContent += "SOURCES += \"" + filePath + "\"\n";
                }
                else if (dirIt.fileInfo().suffix() == "h") {
                    priContent += "HEADERS += \"" + filePath + "\"\n";
                }
                else if (dirIt.fileInfo().suffix() == "lib") {
                    priContent += "LIBS += \"" + filePath + "\"\n";
                }
            }
        }
    }
}

bool MainWindow::copyRecursively(const QString &srcFilePath, const QString &tgtFilePath)
{
    QFileInfo srcFileInfo(srcFilePath);
    if (srcFileInfo.isDir()) {
        QDir targetDir(tgtFilePath);
        if (targetDir.exists() == false) {
            targetDir.cdUp();
        }
        if (!targetDir.mkdir(srcFileInfo.fileName())) {
            return false;
        }

        targetDir.cd(srcFileInfo.fileName());
        QDir sourceDir(srcFilePath);
        QStringList fileNames = sourceDir.entryList(QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot | QDir::Hidden | QDir::System);
        foreach (const QString &fileName, fileNames) {
            const QString newSrcFilePath = srcFilePath + QLatin1Char('/') + fileName;
            const QString newTgtFilePath = targetDir.absolutePath() + QLatin1Char('/') + fileName;
            if (!copyRecursively(newSrcFilePath, newTgtFilePath)) {
                return false;
            }
        }
    }
    else {
        if (!QFile::copy(srcFilePath, tgtFilePath)) {
            return false;
        }
    }
    return true;
}

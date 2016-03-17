#include "MainWindow.h"
#include "ui_MainWindow.h"
#include <QDirIterator>
#include <QDebug>
#include <QFileDialog>
#include <QMessageBox>
#include <QSettings>
#include <QDesktopServices>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_OFPath("")
    , m_OFAddonsPath("")
    , m_OFAppTemplatePath("")
    , m_AppPath("")
    , m_AddonsPath("")
    , m_PriFile("./data/openFrameworks-0.8.4.pri")
    , m_IsAppNameValid(false)
    , m_IsOFPathValid(false)
    , m_IsAppFolderValid(false)
    , m_OFVersion(0)
{
    ui->setupUi(this);
    connect(ui->lineEditOfPath, SIGNAL(textChanged(QString)), this, SLOT(getAddonNames()));
    connect(ui->lineEditAppName, SIGNAL(textChanged(QString)), this, SLOT(checkAppNameValidity(QString)));
    connect(ui->lineEditAppPath, SIGNAL(textChanged(QString)), this, SLOT(checkAppFolderValidity(QString)));
    connect(ui->listWidget, SIGNAL(itemClicked(QListWidgetItem *)), this, SLOT(getSelectedAddons(QListWidgetItem *)));
    connect(ui->buttonOfPath, SIGNAL(pressed()), this, SLOT(browseOFPath()));
    connect(ui->buttonAppPath, SIGNAL(pressed()), this, SLOT(browseAppPath()));
    connect(ui->buttonGenerate, SIGNAL(pressed()), this, SLOT(generateProject()));
    connect(ui->comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(changeOfVersion(int)));

    QSettings settings;
    ui->lineEditOfPath->setText(settings.value("of_path").toString());
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
        QSettings settings;
        settings.setValue("of_path", ui->lineEditOfPath->text());
        settings.sync();
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

    if (ui->radioButtonCmake->isChecked()) {
        if (m_OFVersion == 0) {
            QMessageBox::information(this, "Sorry... :(", "Version 0.8.4 is not yet supported with Cmake.");
            return;
        }

        generateCMakeProject();
    }
    else if (ui->radioButtonQmake->isChecked()) {
        generateQMakeProject();
    }

    QFile folder(m_AppPath);
    if (folder.exists()) {
        QDesktopServices::openUrl(QUrl("file:///" + m_AppPath));
    }
}

void MainWindow::generateQMakeProject()
{
    QFile priFile(m_PriFile);
    copyOFTemplateFiles();
    QFile::copy("./data/proFileTemplate.pro", m_AppPath + ui->lineEditAppName->text() + ".pro");
    QFile proFile(m_AppPath + ui->lineEditAppName->text() + ".pro");
    if (proFile.exists() && proFile.open(QIODevice::ReadWrite)) {
        QString contents = QString(proFile.readAll());
        QString priFileName = m_PriFile;
        contents.replace("#PRI_FILE#", priFileName.replace(m_PriFile.left(m_PriFile.lastIndexOf("/") + 1), ""));
        proFile.resize(0);
        proFile.write(contents.toStdString().c_str());
        proFile.close();
    }

    if (priFile.exists() && priFile.open(QIODevice::ReadOnly)) {
        QString contents = QString(priFile.readAll());
        //Insert OF path
        QString ofPathWithoutSuffix = m_OFPath;
        contents.replace("#OF_PATH#", "\"" + ofPathWithoutSuffix.remove(ofPathWithoutSuffix.length() - 1, 1) + "\"");
        contents.replace("#AR#", m_OFVersion == 1 ? "x64" : "Win32");
        insertAddonsQMake(contents);

        //Write the changed pri file to the new path
        QFile newPriFile(m_AppPath + m_PriFile.right(m_PriFile.length() - m_PriFile.lastIndexOf("/")));
        if (newPriFile.open(QIODevice::WriteOnly)) {
            newPriFile.write(contents.toStdString().c_str());
            newPriFile.close();
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

void MainWindow::generateCMakeProject()
{
    const QString projCmakeFile = "./data/project_CMakeLists.txt";
    const QString findCmakeFile = "./data/findOpenFrameworks-v0.9.cmake";
    const QString ofCmakeFile = "./data/of_CMakeLists.txt";

    copyOFTemplateFiles();

    //These files don't require any change, so copy them as they are.
    QFile::copy(ofCmakeFile, m_OFPath + "/CMakeLists.txt");
    QFile file(findCmakeFile);
    if (file.open(QIODevice::ReadOnly)) {
        QString contents = QString(file.readAll());
        contents.replace("${ARCHITECTURE}", m_OFVersion == 1 ? "x64" : "Win32");
        QFile newFile(m_OFPath + "/findOpenFrameworks-v0.9.cmake");
        if (newFile.open(QIODevice::WriteOnly)) {
            newFile.write(contents.toUtf8());
            newFile.close();
        }
    }
    file.close();

    //Open projCmakeFile and change the OF_PATH and PROJ_NAME
    file.setFileName(projCmakeFile);
    if (file.exists() == false) {
        QMessageBox::warning(this, "Error!", "./data/project_CMakeLists.txt file doesn't exist!");
        return;
    }

    file.close();
    file.setFileName(projCmakeFile);
    if (file.open(QIODevice::ReadOnly)) {
        QString contents = QString(file.readAll());
        contents.replace("${PROJ_NAME}", ui->lineEditAppName->text());
        contents.replace("${OPENFRAMEWORKS_PATH}", m_OFPath);
        QFile newFile(m_AppPath + "/CMakeLists.txt");
        if (newFile.open(QIODevice::WriteOnly)) {
            newFile.write(contents.toUtf8());
            newFile.close();
        }
    }
    file.close();

    insertAddonsCMake();
    ui->statusBar->showMessage("Project generated", 5000);
    ui->lineEditAppName->clear();
    for (int i = 0; i < m_AddonItems.size(); i++) {
        QListWidgetItem *item = m_AddonItems.at(i);
        item->setCheckState(Qt::Unchecked);
    }
}

void MainWindow::changeOfVersion(int currentIndex)
{
    m_OFVersion = currentIndex;
    if (currentIndex == 0) {
        m_PriFile = "./data/openFrameworks-0.8.4.pri";
    }
    else {
        m_PriFile = "./data/openFrameworks-0.9.pri";
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

void MainWindow::insertAddonsQMake(QString &priContent)
{
    if (m_SelectedAddons.size() == 0) {
        return;
    }

    bool isCopyEnabled = ui->checkBox->isChecked();
    QString addonRootPath = m_OFAddonsPath;
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
            priContent += "INCLUDEPATH += \"$$OF/addons/" + addonName + "/src/" + "\"\n";
            QDir libsDir(addonPath + "/libs");
            if (libsDir.exists()) {
                priContent += "INCLUDEPATH += \"$$OF/addons/" + addonName + "/libs/" + "\"\n";
            }
        }

        foreach (const QString &folder, folderList) {
            QDirIterator dirIt(addonPath + folder, QDirIterator::Subdirectories);
            while (dirIt.hasNext()) {
                dirIt.next();
                if (dirIt.fileName() == "." || dirIt.fileName() == "..") {
                    continue;
                }

                QString filePath = isCopyEnabled ? dirIt.filePath().replace(m_AddonsPath, "$$PWD/addons/") : dirIt.filePath();
                filePath.replace(addonRootPath, "$$OF/addons/");
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

void MainWindow::insertAddonsCMake()
{
    if (m_SelectedAddons.size() == 0) {
        return;
    }

    QString content;
    bool isCopyEnabled = ui->checkBox->isChecked();
    QString addonRootPath = m_OFAddonsPath;
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

    QStringList sources, headers, libs;
    QStringList includePaths;
    for (int i = 0; i < m_SelectedAddons.size(); i++) {
        const QString addonName = m_SelectedAddons.at(i);
        content += "#" + addonName + "\n";
        const QString addonPath = addonRootPath + addonName;
        QStringList folderList;
        folderList.append("/src");
        folderList.append("/libs");
        if (isCopyEnabled) {
            includePaths.append("addons/" + addonName + "/src");
            QDir libsDir(addonPath + "/libs");
            if (libsDir.exists()) {
                includePaths.append("addons/" + addonName + "/libs");
            }
        }
        else {
            includePaths.append("${ADDONS_PATH}/" + addonName + "/src");
            QDir libsDir(addonPath + "/libs");
            if (libsDir.exists()) {
                includePaths.append("${ADDONS_PATH}/" + addonName + "/libs");
            }
        }

        for (const QString &folder : folderList) {
            QDirIterator dirIt(addonPath + folder, QDirIterator::Subdirectories);
            while (dirIt.hasNext()) {
                dirIt.next();
                if (dirIt.fileName() == "." || dirIt.fileName() == "..") {
                    continue;
                }

                QString filePath = isCopyEnabled ? dirIt.filePath().replace(m_AddonsPath, "addons/") : dirIt.filePath();
                filePath.replace(addonRootPath, "${ADDONS_PATH}/");
                if (dirIt.fileInfo().isDir()) {
                    if (includePaths.contains(dirIt.fileInfo().absoluteDir().absolutePath()) == false) {
                        includePaths.append(filePath);
                    }
                    continue;
                }

                if (dirIt.fileInfo().suffix() == "cpp") {
                    sources.append(filePath);
                }
                else if (dirIt.fileInfo().suffix() == "h") {
                    headers.append(filePath);
                }
                else if (dirIt.fileInfo().suffix() == "lib") {
                    libs.append(filePath);
                }
            }
        }

        //Add the paths to the content
        content += "list(APPEND ADDONS_SRC\n";
        for (const QString &src : sources) {
            content += src + "\n";
        }
        content += ")\n";

        content += "list(APPEND ADDONS_HEADERS\n";
        for (const QString &header : headers) {
            content += header + "\n";
        }
        content += ")\n";

        content += "list(APPEND ADDONS_INCLUDE_PATH\n";
        for (const QString &inc : includePaths) {
            content += inc + "\n";
        }
        content += ")\n";

        content += "list(APPEND ADDONS_LIBS\n";
        for (const QString &lib : libs) {
            content += lib + "\n";
        }
        content += ")\n";

        QFile file(m_AppPath + "/ofAddons.cmake");
        if (file.open(QIODevice::WriteOnly)) {
            file.write(content.toUtf8());
        }
        file.close();
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
        for (const QString &fileName : fileNames) {
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

void MainWindow::copyOFTemplateFiles()
{
    QDir dir(ui->lineEditAppPath->text() + "/");
    //Create the project dir and copy the template
    if (dir.exists()) {
        dir.mkdir(ui->lineEditAppName->text());
        m_AppPath = ui->lineEditAppPath->text() + "/" + ui->lineEditAppName->text() + "/";
        QFile::copy(m_OFAppTemplatePath + "main.cpp", m_AppPath + "main.cpp");
        QFile::copy(m_OFAppTemplatePath + "ofApp.cpp", m_AppPath + "ofApp.cpp");
        QFile::copy(m_OFAppTemplatePath + "ofApp.h", m_AppPath + "ofApp.h");
    }
}

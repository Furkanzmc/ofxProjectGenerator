#include "MainWindow.h"
#include "ui_MainWindow.h"
#include <QDirIterator>
#include <QDebug>
#include <QFileDialog>
#include <QMessageBox>
#include <QSettings>
#include <QDesktopServices>
#include <QListWidgetItem>
#include <QJsonDocument>
#include <QJsonObject>

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
    , m_IsUpdateProject(false)
    , m_OFVersion(0)
{
    ui->setupUi(this);
    this->setWindowTitle("ofxProjectGenerator");

    connect(ui->listWidget, &QListWidget::itemClicked, this, &MainWindow::updateSelectedAddons);
    connect(ui->lineEditOfPath, &QLineEdit::textChanged, this, &MainWindow::listAddonNames);
    connect(ui->lineEditAppName, &QLineEdit::textChanged, this, &MainWindow::checkAppNameValidity);
    connect(ui->lineEditAppPath, &QLineEdit::textChanged, this, &MainWindow::checkAppFolderValidity);

    connect(ui->buttonOfPath, &QPushButton::pressed, this, &MainWindow::browseOFPath);
    connect(ui->buttonAppPath, &QPushButton::pressed, this, &MainWindow::browseAppPath);
    connect(ui->buttonGenerate, &QPushButton::pressed, this, &MainWindow::generateProject);
    connect(ui->menuRecent_Projects, &QMenu::triggered, this, &MainWindow::recentProjectSelected);

    connect(ui->comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(changeOfVersion(int)));

    QSettings settings;
    ui->lineEditOfPath->setText(settings.value("of_path").toString());

    fillRecentsMenu();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::listAddonNames()
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

        QListWidgetItem *item = new QListWidgetItem(ui->listWidget);
        item->setText(dirIteratorTop.fileName());
        item->setCheckState(Qt::CheckState::Unchecked);
    }
}

void MainWindow::checkAppNameValidity(const QString &str)
{
    if (str.contains(" ") || str.length() == 0) {
        ui->lineEditAppName->setStyleSheet("color: red");
        m_IsAppNameValid = false;
    }
    else {
        ui->lineEditAppName->setStyleSheet("");
        m_IsAppNameValid = true;
    }
}

void MainWindow::checkAppFolderValidity(const QString &str)
{
    QDir dir(str);
    if (str.length() == 0 || dir.exists() == false) {
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

void MainWindow::updateSelectedAddons(QListWidgetItem *selectedItem)
{
    if (selectedItem == nullptr) {
        for (int i = 0; i < ui->listWidget->count(); i++) {
            QListWidgetItem *item = ui->listWidget->item(i);
            item->setCheckState(m_SelectedAddons.contains(item->text()) ? Qt::Checked : Qt::Unchecked);
        }
    }
    else {
        if (selectedItem->checkState() == Qt::Checked && m_SelectedAddons.contains(selectedItem->text()) == false) {
            m_SelectedAddons.append(selectedItem->text());
        }
        else if (selectedItem->checkState() == Qt::Unchecked && m_SelectedAddons.contains(selectedItem->text())) {
            m_SelectedAddons.removeAt(m_SelectedAddons.indexOf(selectedItem->text()));
        }
    }
}

void MainWindow::generateProject()
{
    const QString errStr = getErrorString();
    if (errStr.length() > 0) {
        QMessageBox::warning(this, "Error!", "Please correct the following misteke(s).\n" + errStr, QMessageBox::Ok);
        return;
    }

    // Put them in varriables in case we add new types of projects
    const bool isCMakeProject = ui->radioButtonCmake->isChecked();
    const bool isQMakeProject = ui->radioButtonQmake->isChecked();
    if (isCMakeProject && m_OFVersion == 0) {
        QMessageBox::information(this, "Sorry... :(", "Version 0.8.4 is not yet supported with Cmake.");
        return;
    }

    copyOFTemplateFiles();
    if (isCMakeProject) {
        generateCMakeProject();
    }
    else if (isQMakeProject) {
        generateQMakeProject();
    }

    QFile folder(m_AppPath);
    if (folder.exists()) {
        QDesktopServices::openUrl(QUrl("file:// /" + m_AppPath));
    }

    saveProjectToRecents();
    fillRecentsMenu();

    // Clear the fields AFTER the project is saved to recents
    ui->statusBar->showMessage("Project generated", 5000);
    ui->lineEditAppName->clear();
    for (int i = 0; i < ui->listWidget->count(); i++) {
        QListWidgetItem *item = ui->listWidget->item(i);
        item->setCheckState(Qt::Unchecked);
    }
}

void MainWindow::generateQMakeProject()
{
    QFile priFile(m_PriFile);
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
        // Insert OF path
        QString ofPathWithoutSuffix = m_OFPath;
        contents.replace("#OF_PATH#", "\"" + ofPathWithoutSuffix.remove(ofPathWithoutSuffix.length() - 1, 1) + "\"");
        contents.replace("#AR#", m_OFVersion == 1 ? "x64" : "Win32");
        insertAddonsQMake(contents);

        // Write the changed pri file to the new path
        QFile newPriFile(m_AppPath + m_PriFile.right(m_PriFile.length() - m_PriFile.lastIndexOf("/")));
        if (newPriFile.open(QIODevice::WriteOnly)) {
            newPriFile.write(contents.toStdString().c_str());
            newPriFile.close();
        }
    }

    priFile.close();
}

void MainWindow::generateCMakeProject()
{
    const QString projCmakeFile = "./data/project_CMakeLists.txt";
    const QString findCmakeFile = "./data/findOpenFrameworks-v0.9.cmake";
    const QString ofCmakeFile = "./data/of_CMakeLists.txt";

    // Open projCmakeFile and change the OF_PATH and PROJ_NAME
    QFile file(projCmakeFile);
    if (file.exists() == false) {
        QMessageBox::warning(this, "Error!", "./data/project_CMakeLists.txt file doesn't exist!");
        return;
    }

    // These files don't require any change, so copy them as they are.
    QFile::copy(ofCmakeFile, m_OFPath + "/CMakeLists.txt");

    file.setFileName(findCmakeFile);
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

    file.setFileName(projCmakeFile);
    if (file.open(QIODevice::ReadOnly)) {
        QString contents = QString(file.readAll());
        contents.replace("${PROJ_NAME}", ui->lineEditAppName->text());
        contents.replace("${OPENFRAMEWORKS_PATH}", m_OFPath);
        QFile newFile(m_AppPath + "/CMakeLists.txt");
        if (newFile.exists() == false && newFile.open(QIODevice::WriteOnly)) {
            newFile.write(contents.toUtf8());
            newFile.close();
        }
    }

    file.close();

    insertAddonsCMake();
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

    QString content = "";
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

    for (int i = 0; i < m_SelectedAddons.size(); i++) {
        QStringList sources, headers, libs;
        QStringList includePaths;
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

        // Add the paths to the content
        content += "list(APPEND ADDONS_SRC\n";
        for (const QString &src : sources) {
            content += src + "\n";
        }
        content += ")\n\n";

        content += "list(APPEND ADDONS_HEADERS\n";
        for (const QString &header : headers) {
            content += header + "\n";
        }
        content += ")\n\n";

        content += "list(APPEND ADDONS_INCLUDE_PATH\n";
        for (const QString &inc : includePaths) {
            content += inc + "\n";
        }
        content += ")\n\n";

        content += "list(APPEND ADDONS_LIBS\n";
        for (const QString &lib : libs) {
            content += lib + "\n";
        }
        content += ")\n\n";

        QFile file(m_AppPath + "/ofAddons.cmake");
        if (file.open(QIODevice::WriteOnly)) {
            file.write(content.toUtf8());
        }
        file.close();
    }
}

bool MainWindow::copyRecursively(const QString &srcFilePath, const QString &tgtFilePath) const
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
    else if (!QFile::copy(srcFilePath, tgtFilePath)) {
        return false;
    }

    return true;
}

void MainWindow::copyOFTemplateFiles()
{
    QDir dir(ui->lineEditAppPath->text() + "/");
    // Create the project dir and copy the template
    if (dir.exists()) {
        dir.mkdir(ui->lineEditAppName->text());
        m_AppPath = ui->lineEditAppPath->text() + "/" + ui->lineEditAppName->text() + "/";
        QFile::copy(m_OFAppTemplatePath + "main.cpp", m_AppPath + "main.cpp");
        QFile::copy(m_OFAppTemplatePath + "ofApp.cpp", m_AppPath + "ofApp.cpp");
        QFile::copy(m_OFAppTemplatePath + "ofApp.h", m_AppPath + "ofApp.h");
    }
}

void MainWindow::saveProjectToRecents()
{
    QSettings settings;

    QJsonDocument doc = QJsonDocument::fromBinaryData(settings.value("recent").toByteArray());
    QJsonArray array = doc.object()["projects"].toArray();
    int existingIndex = -1;
    for (int i = 0; i < array.size(); i++) {
        const QJsonObject proj = array.at(i).toObject();
        if (proj["app_path"].toString() == ui->lineEditAppPath->text()) {
            existingIndex = i;
            break;
        }
    }

    QJsonObject obj;
    obj["of_path"] = ui->lineEditOfPath->text();
    obj["app_name"] = ui->lineEditAppName->text();
    obj["app_path"] = ui->lineEditAppPath->text();
    obj["selected_addons"] = QJsonArray::fromStringList(m_SelectedAddons);
    obj["project_type"] = ui->radioButtonCmake->isChecked() ? "cmake" : "qmake";
    obj["of_version"] = ui->comboBox->currentIndex();

    if (existingIndex == -1) {
        array.push_back(obj);
    }
    else {
        array[existingIndex] = obj;
    }

    obj = doc.object();
    obj["projects"] = array;
    doc.setObject(obj);

    settings.setValue("recent", doc.toBinaryData());
}

void MainWindow::fillRecentsMenu()
{
    QSettings settings;
    const QJsonDocument doc = QJsonDocument::fromBinaryData(settings.value("recent").toByteArray());
    const QJsonObject obj = doc.object();
    m_RecentProjectArray = obj["projects"].toArray();
    ui->menuRecent_Projects->clear();

    for (int i = 0; i < m_RecentProjectArray.size(); i++) {
        const QJsonObject proj = m_RecentProjectArray.at(i).toObject();
        const QString title = proj["app_name"].toString() + ": " + proj["app_path"].toString();
        ui->menuRecent_Projects->addAction(title);
    }
}

QJsonObject MainWindow::getRecentProject(const QString &appPath)
{
    QJsonObject obj;
    for (int i = 0; i < m_RecentProjectArray.size(); i++) {
        const QJsonObject proj = m_RecentProjectArray.at(i).toObject();
        if (proj["app_path"].toString() == appPath) {
            obj = proj;
            break;
        }
    }

    return obj;
}

void MainWindow::recentProjectSelected(QAction *selected)
{
    QString apppath = selected->text();
    const QJsonObject project = getRecentProject(apppath.right(apppath.length() - apppath.indexOf(":") - 2));

    ui->lineEditAppName->setText(project["app_name"].toString());
    ui->lineEditAppPath->setText(project["app_path"].toString());
    ui->lineEditOfPath->setText(project["of_path"].toString());
    ui->comboBox->setCurrentIndex(project["of_version"].toInt());
    const bool isCmake = project["project_type"].toString() == "cmake";
    if (isCmake) {
        ui->radioButtonCmake->setChecked(true);
        ui->radioButtonQmake->setChecked(false);
    }
    else {
        ui->radioButtonCmake->setChecked(false);
        ui->radioButtonQmake->setChecked(true);
    }

    m_SelectedAddons = project["selected_addons"].toVariant().toStringList();
    updateSelectedAddons();
}

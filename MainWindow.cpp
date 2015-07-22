#include "MainWindow.h"
#include "ui_MainWindow.h"
#include <QDirIterator>
#include <QDebug>
#include <QFileDialog>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_OFPath("")
    , m_OFAddonsPath("")
    , m_OFAppTemplatePath("")
    , m_AppPath("")
    , m_AddonsPath("")
    , m_IsAppNameValid(false)
{
    ui->setupUi(this);
    connect(ui->lineEditOfPath, SIGNAL(textChanged(QString)), this, SLOT(getAddonNames()));
    connect(ui->lineEditAppName, SIGNAL(textChanged(QString)), this, SLOT(checkAppNameValidity(QString)));
    connect(ui->buttonOfPath, SIGNAL(pressed()), this, SLOT(browseOFPath()));
    connect(ui->buttonAppPath, SIGNAL(pressed()), this, SLOT(browseAppPath()));
    connect(ui->buttonGenerate, SIGNAL(pressed()), this, SLOT(generateProject()));
    connect(ui->listWidget, SIGNAL(itemClicked(QListWidgetItem *)), this, SLOT(getSelectedAddons(QListWidgetItem *)));
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
    }
    else {
        ui->lineEditOfPath->setStyleSheet("");
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
    else {
        ui->lineEditAppName->setStyleSheet("");
        m_IsAppNameValid = true;
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
        contents.replace("#OF_PATH#", m_OFPath);
        insertAddons(contents);

        //Write the changed pri file to the new path
        QFile newProFile(m_AppPath + "openFrameworks-0.8.4.pri");
        if (newProFile.open(QIODevice::WriteOnly)) {
            newProFile.write(contents.toStdString().c_str());
            newProFile.close();
        }

    }
    priFile.close();
}

void MainWindow::insertAddons(QString &priContent)
{
    if (m_SelectedAddons.size() == 0) {
        return;
    }

    bool isCopyEnabled = false;
    if (ui->checkBox->isChecked()) {
        QDir dir(m_AppPath);
        if (dir.exists()) {
            dir.mkdir("addons");
            isCopyEnabled = true;
            m_AddonsPath = m_AppPath + "addons/";
        }
    }
    for (int i = 0; i < m_SelectedAddons.size(); i++) {
        if (isCopyEnabled) {
            copyRecursively(m_OFAddonsPath + m_SelectedAddons.at(i), m_AddonsPath);
            QFile::copy(m_OFAddonsPath + m_SelectedAddons.at(i), m_AddonsPath + m_SelectedAddons.at(i));
        }
    }
}

bool MainWindow::copyAddon(const QString &addonName)
{
    //First create folders for the files and then do the copying
    QDir dir(m_AddonsPath);
    if (dir.exists()) {
        dir.mkdir(addonName);

        QDirIterator it(m_OFAddonsPath + addonName);
        while (it.hasNext()) {
            it.next();
            if (it.fileInfo().isDir() == false || it.fileName() == "." || it.fileName() == "..") {
                continue;
            }

            const QString subDir = it.filePath().remove(0, it.filePath().lastIndexOf("/addons") + 7);
            QDir asd(m_AddonsPath + subDir);
            qDebug() << asd.path();
            dir.mkdir(subDir);
        }
    }
    return true;
}

bool MainWindow::copyRecursively(const QString &srcFilePath, const QString &tgtFilePath)
{
    QFileInfo srcFileInfo(srcFilePath);
    if (srcFileInfo.isDir()) {
        QDir targetDir(tgtFilePath);
        targetDir.cdUp();
        if (!targetDir.mkdir(QFileInfo(tgtFilePath).fileName()))
            return false;
        QDir sourceDir(srcFilePath);
        QStringList fileNames = sourceDir.entryList(QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot | QDir::Hidden | QDir::System);
        foreach (const QString &fileName, fileNames) {
            const QString newSrcFilePath = srcFilePath + QLatin1Char('/') + fileName;
            const QString newTgtFilePath = tgtFilePath + QLatin1Char('/') + fileName;
            if (!copyRecursively(newSrcFilePath, newTgtFilePath)) {
                return false;
            }
        }
    }
    else {
        if (!QFile::copy(srcFilePath, tgtFilePath))
            return false;
    }
    return true;
}

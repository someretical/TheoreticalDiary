/*
 * This file is part of Theoretical Diary.
 * Copyright (C) 2022 someretical
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include <fstream>
#include <sstream>

#include "../core/diaryholder.h"
#include "../core/googlewrapper.h"
#include "../core/internalmanager.h"
#include "../util/custommessageboxes.h"
#include "../util/encryptor.h"
#include "../util/hashcontroller.h"
#include "../util/zipper.h"
#include "mainmenu.h"
#include "mainwindow.h"
#include "ui_mainmenu.h"

MainMenu::MainMenu(bool const show_locked_message, QWidget *parent) : QWidget(parent), ui(new Ui::MainMenu)
{
    ui->setupUi(this);
    ui->version->setText("Version " + QApplication::applicationVersion());
    ui->version->update();
    ui->app_name->setText(QApplication::applicationName());
    ui->app_name->update();
    ui->pwd_alert_text->set_text(show_locked_message ? "Diary locked due to inactivity." : "");

    QTimer::singleShot(0, [&]() { ui->password_box->setFocus(); });

    // Pressing enter will try to decrypt
    enter_shortcut = new QShortcut(QKeySequence(Qt::Key_Return), this);
    connect(
        enter_shortcut, &QShortcut::activated, this,
        [&]() { QMetaObject::invokeMethod(ui->decrypt_button, "clicked"); }, Qt::QueuedConnection);

    connect(ui->decrypt_button, &QPushButton::clicked, this, &MainMenu::decrypt_diary, Qt::QueuedConnection);
    connect(ui->new_button, &QPushButton::clicked, this, &MainMenu::new_diary, Qt::QueuedConnection);
    connect(ui->import_button, &QPushButton::clicked, this, &MainMenu::import_diary, Qt::QueuedConnection);
    connect(ui->options_button, &QPushButton::clicked, this, &MainMenu::open_options, Qt::QueuedConnection);
    connect(ui->quit_button, &QPushButton::clicked, qobject_cast<MainWindow *>(parent), &MainWindow::close,
        Qt::QueuedConnection);

    //    connect(InternalManager::instance(), &InternalManager::update_theme, this, &MainMenu::update_theme,
    //        Qt::QueuedConnection);
    //    update_theme();
}

MainMenu::~MainMenu()
{
    delete ui;
    delete enter_shortcut;
}

void MainMenu::update_theme() {}

void MainMenu::open_options()
{
    qDebug() << "Showing options menu.";
    MainWindow::instance()->show_options_menu();
}

bool MainMenu::get_diary_contents()
{
    // Attempt to load file contents
    std::ifstream first(InternalManager::instance()->data_location().toStdString() + "/diary.dat");
    std::stringstream stream;

    if (!first.fail()) {
        stream << first.rdbuf();
        qDebug() << "Loaded diary contents from primary file.";
    }
    else {
        std::ifstream second(InternalManager::instance()->data_location().toStdString() + "/diary.dat.bak");

        if (!second.fail()) {
            stream << second.rdbuf();
            qDebug() << "Loaded diary contents from backup file.";
        }
        else {
            qDebug() << "Failed to get diary contents from primary AND backup file.";
            return false;
        }
    }

    Encryptor::instance()->encrypted_str.assign(stream.str());
    return true;
}

void MainMenu::decrypt_diary()
{
    qDebug() << "Attempting to decrypt diary...";
    InternalManager::instance()->start_busy_mode(__LINE__, __func__, __FILE__);

    auto const &password = ui->password_box->text().toStdString();
    ui->password_box->setText("");
    ui->password_box->update();
    ui->pwd_alert_text->set_text("");

    auto const &opt = get_diary_contents();
    if (!opt)
        return cmb::ok_messagebox(
            this, []() {}, "No local diary was found.",
            "You can create a new diary by clicking the new button in the main menu.");

    auto &str = Encryptor::instance()->encrypted_str;
    // If the password box is empty, try to decompress immediately.
    // If the encrypted string length is too short, throw the same error.
    // This is the simplest way of letting a user know their file is scuffed.
    // The app can't just crash when there is indeed a file present.
    if (password.empty() || str.size() <= tencrypt::SALT_SIZE + tencrypt::IV_SIZE) {
        if (str.size() <= tencrypt::SALT_SIZE + tencrypt::IV_SIZE)
            qDebug() << "Diary file found but way too short to be decrypted.";
        else
            qDebug() << "No password to hash in decrypt_diary.";

        ui->pwd_alert_text->set_text("Parsing diary...", false);
        return decrypt_diary_cb(false);
    }

    Encryptor::instance()->parse_encrypted_string(str);

    ui->pwd_alert_text->set_text("Decrypting...", false);

    auto hash_controller = new HashController();
    connect(hash_controller, &HashController::sig_done, this, &MainMenu::decrypt_diary_cb, Qt::QueuedConnection);
    connect(hash_controller, &HashController::sig_delete, hash_controller, &HashController::deleteLater,
        Qt::QueuedConnection);
    emit hash_controller->operate(password);
}

void MainMenu::decrypt_diary_cb(bool const perform_decrypt)
{
    std::string decrypted;
    if (perform_decrypt) {
        auto const &res = Encryptor::instance()->decrypt(Encryptor::instance()->encrypted_str);
        if (!res)
            return QTimer::singleShot(2000, [&]() {
                ui->pwd_alert_text->set_text("Wrong password.");
                InternalManager::instance()->end_busy_mode(__LINE__, __func__, __FILE__);
            });

        decrypted.assign(*res);
    }

    std::string decompressed;
    if (Zipper::unzip((perform_decrypt ? decrypted : Encryptor::instance()->encrypted_str), decompressed) &&
        DiaryHolder::instance()->load(decompressed)) {

        GoogleWrapper::instance()->decrypt_credentials(perform_decrypt);
        qDebug() << "Decryption and decompression of diary successful. Loading diary menu...";
        return MainWindow::instance()->show_diary_menu();
        // show_diary_menu instantiates the diary pixels tab which in turn will call end_busy_mode once it's done
        // rendering.
    }

    // 2000ms delay here to stop the app from getting spammed.
    QTimer::singleShot(2000, [&]() {
        ui->pwd_alert_text->set_text("Unable to parse diary.");
        InternalManager::instance()->end_busy_mode(__LINE__, __func__, __FILE__);
    });
}

void MainMenu::new_diary()
{
    auto cb = [](int const res) {
        if (QMessageBox::RejectRole == res)
            return;

        Encryptor::instance()->reset();
        DiaryHolder::instance()->init();
        InternalManager::instance()->internal_diary_changed = true;

        qDebug() << "Showing diary menu from new diary.";
        MainWindow::instance()->show_diary_menu();
    };

    cmb::prompt_confirm_overwrite(this, cb);
}

void MainMenu::import_diary()
{
    auto cb = [this](int const res) {
        if (QMessageBox::RejectRole == res)
            return;

        auto const &filename =
            QFileDialog::getOpenFileName(this, "Import diary", QDir::homePath(), "JSON (*.json);;All files");
        if (filename.isEmpty())
            return;

        std::ifstream ifs(filename.toStdString());
        if (ifs.fail())
            return cmb::display_io_error(this, []() {});

        std::stringstream stream;
        stream << ifs.rdbuf();

        if (!DiaryHolder::instance()->load(stream.str()))
            return cmb::display_io_error(this, []() {});

        InternalManager::instance()->internal_diary_changed = true;
        qDebug() << "Showing diary menu from import diary.";
        MainWindow::instance()->show_diary_menu();
    };

    cmb::prompt_confirm_overwrite(this, cb);
}

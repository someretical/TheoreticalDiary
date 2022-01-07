/*
 * This file is part of Theoretical Diary.
 * Copyright (C) 2021  someretical
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

#include "mainmenu.h"
#include "ui_mainmenu.h"

MainMenu::MainMenu(QWidget *parent) : QWidget(parent), ui(new Ui::MainMenu)
{
    ui->setupUi(this);
    ui->version->setText("Version " + QApplication::applicationVersion());
    ui->version->update();
    ui->alert_text->setText("");
    ui->alert_text->update();

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

    connect(InternalManager::instance(), &InternalManager::update_theme, this, &MainMenu::update_theme,
        Qt::QueuedConnection);
    update_theme();
}

MainMenu::~MainMenu()
{
    delete ui;
    delete enter_shortcut;
}

void MainMenu::update_theme()
{
    QFile file(":/global/mainmenu.qss");
    file.open(QIODevice::ReadOnly);
    setStyleSheet(file.readAll());
}

void MainMenu::open_options()
{
    MainWindow::instance()->show_options_menu();
}

bool MainMenu::get_diary_contents()
{
    // Attempt to load file contents
    std::ifstream first(InternalManager::instance()->data_location().toStdString() + "/diary.dat");
    std::stringstream stream;

    if (!first.fail()) {
        stream << first.rdbuf();
    }
    else {
        std::ifstream second(InternalManager::instance()->data_location().toStdString() + "/diary.dat.bak");

        if (!second.fail())
            stream << second.rdbuf();
        else
            return false;
    }

    Encryptor::instance()->encrypted_str.assign(stream.str());

    return true;
}

void MainMenu::decrypt_diary()
{
    InternalManager::instance()->start_busy_mode(__LINE__, __func__, __FILE__);

    auto const &password = ui->password_box->text().toStdString();
    ui->password_box->setText("");
    ui->password_box->update();
    ui->alert_text->setText("");
    ui->alert_text->update();

    auto const &opt = get_diary_contents();
    if (!opt) {
        td::ok_messagebox(
            this, "No diary was found.", "You can create a new diary by clicking the \"New\" button in the main menu.");
        return InternalManager::instance()->end_busy_mode(__LINE__, __func__, __FILE__);
    }

    // If the password box is empty, try to decompress immediately.
    if (password.empty())
        return decrypt_diary_cb(false);

    auto &str = Encryptor::instance()->encrypted_str;
    Encryptor::instance()->set_salt(str.substr(0, SALT_SIZE));
    str.erase(0, SALT_SIZE);
    Encryptor::instance()->set_decrypt_iv(str.substr(0, IV_SIZE));
    str.erase(0, IV_SIZE);

    ui->alert_text->setText("Decrypting...");
    ui->alert_text->update();

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
                ui->alert_text->setText("Wrong password.");
                ui->alert_text->update();
                InternalManager::instance()->end_busy_mode(__LINE__, __func__, __FILE__);
            });

        decrypted.assign(*res);
    }

    std::string decompressed;
    if (Zipper::unzip((perform_decrypt ? decrypted : Encryptor::instance()->encrypted_str), decompressed) &&
        DiaryHolder::instance()->load(decompressed)) {

        GoogleWrapper::instance()->decrypt_credentials(perform_decrypt);
        return MainWindow::instance()->show_diary_menu();
        // show_diary_menu instantiates the diary pixels tab which in turn will call end_busy_mode once it's done
        // rendering.
    }

    // 2000ms delay here to stop the app from getting spammed.
    QTimer::singleShot(2000, [&]() {
        ui->alert_text->setText("Unable to parse diary.");
        ui->alert_text->update();
        InternalManager::instance()->end_busy_mode(__LINE__, __func__, __FILE__);
    });
}

void MainMenu::new_diary()
{
    if (!cmb::prompt_confirm_overwrite(this))
        return;

    Encryptor::instance()->reset();
    DiaryHolder::instance()->init();
    InternalManager::instance()->internal_diary_changed = true;

    MainWindow::instance()->show_diary_menu();
}

void MainMenu::import_diary()
{
    if (!cmb::prompt_confirm_overwrite(this))
        return;

    auto const &filename =
        QFileDialog::getOpenFileName(this, "Import diary", QDir::homePath(), "JSON (*.json);;All files");
    if (filename.isEmpty())
        return;

    std::ifstream ifs(filename.toStdString());
    if (ifs.fail()) {
        td::ok_messagebox(this, "Read error.", "The app was unable to read the contents of the file.");
        return;
    }

    std::stringstream stream;
    stream << ifs.rdbuf();

    if (!DiaryHolder::instance()->load(stream.str())) {
        td::ok_messagebox(this, "Read error.", "The app was unable to read the contents of the file.");
    }
    else {
        InternalManager::instance()->internal_diary_changed = true;
        MainWindow::instance()->show_diary_menu();
    }
}

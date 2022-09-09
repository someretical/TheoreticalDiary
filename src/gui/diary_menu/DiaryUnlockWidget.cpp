/*
 *  This file is part of Theoretical Diary.
 *  Copyright (C) 2022 someretical
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include "gui/diary_menu/DiaryUnlockWidget.h"
#include "ui_DiaryUnlockWidget.h"

#include <Logger.h>
#include <QDialogButtonBox>

DiaryUnlockWidget::DiaryUnlockWidget(QWidget *parent) : QWidget(parent), m_ui(new Ui::DiaryUnlockWidget)
{
    m_ui->setupUi(this);

    connect(
        m_ui->buttonBox, &QDialogButtonBox::rejected, this,
        []() { diaryTabWidget()->closeDiaryTab(diaryTabWidget()->currentIndex(), true); }, Qt::QueuedConnection);

    connect(m_ui->buttonBox, SIGNAL(accepted()), this, SLOT(unlockDiary()));

    connect(m_ui->checkBoxNoPassword, &QCheckBox::stateChanged, [this]() {
        if (m_ui->lineEditPassword->isEnabled())
            m_ui->lineEditPassword->setText("");

        m_ui->lineEditPassword->setEnabled(!m_ui->lineEditPassword->isEnabled());
    });
}

DiaryUnlockWidget::~DiaryUnlockWidget()
{
    delete m_ui;
}

void DiaryUnlockWidget::setFilePath(const QString &filePath)
{
    m_ui->filePath->setText(filePath);
}

auto DiaryUnlockWidget::getDiaryWidget() -> DiaryWidget *
{
    return qobject_cast<DiaryWidget *>(parentWidget()->parentWidget()->parentWidget());
}

void DiaryUnlockWidget::unlockDiary() {
    LOG_INFO() << "Open button pressed";
    // TODO implement actual decryption
    getDiaryWidget()->changeCurrentWidget(TD::DiaryWidget::DiaryMainMenu);
}

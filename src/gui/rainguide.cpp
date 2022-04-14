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

#include "rainguide.h"
#include "ui_rainguide.h"

QString const TEXT = R"(
<h3>Recognise</h3>
<p>Name the emotions and feelings I am having.</p>
<h3>Allow</h3>
<p>Tell myself that it is okay to have these emotions and feelings.</p>
<p>Create a space for those emotions.</p>
<h3>Investigate</h3>
<p>What really wants my attention?</p>
<p>What am I believing right now?</p>
<p>What do I need right now?</p>
<h3>Non-identify</h3>
<p>Acknowledge that I am a whole person.</p>
<p>These negative emotions are just a tiny part of who I am so don't let them consume me.</p>
<p>Be kind to myself and offer myself what I need.</p>
<h3>Sources:</h3>
<ul>
    <li><a title="How to Use the RAIN Method for Difficult Emotions"
            href="https://www.grwhealth.com/post/how-to-use-the-rain-method-for-difficult-emotions">https://www.grwhealth.com/post/how-to-use-the-rain-method-for-difficult-emotions</a>
    </li>
    <li><a title="Feeling Anxious? Here's a Quick Tool To Center Your Soul"
            href="https://www.npr.org/2020/02/03/802347757/a-conversation-with-tara-brach-mindfulness-tools-for-big-feelings">https://www.npr.org/2020/02/03/802347757/a-conversation-with-tara-brach-mindfulness-tools-for-big-feelings<a>
    </li>
</ul>
)";

RAINGuide::RAINGuide(QWidget *parent) : QWidget(parent), m_ui(new Ui::RAINGuide)
{
    m_ui->setupUi(this);
    m_ui->text->setText(TEXT);

    connect(m_ui->ok_button, &QPushButton::clicked, this, &RAINGuide::close, Qt::QueuedConnection);
}

RAINGuide::~RAINGuide()
{
    delete m_ui;
}

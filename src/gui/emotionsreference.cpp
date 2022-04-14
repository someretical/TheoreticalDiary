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

#include "emotionsreference.h"
#include "ui_emotionsreference.h"

QString const TEXT = R"(
<h3>Eight basic emotions (Robert Plutchik's theory)</h3>
<ol>
    <li>Fear - feeling of being afraid, frightened, scared.</li>
    <li>Anger - feeling angry. A stronger word for anger is rage.</li>
    <li>Sadness - feeling sad. Other words are sorrow, grief (a stronger feeling, for example when someone has died).</li>
    <li>Joy - the inward feeling of happiness that exists no matter the circumstance, whether good or bad.</li>
    <li>Disgust - feeling something is wrong or nasty. Strong disapproval.</li>
    <li>Surprise - being unprepared for something.</li>
    <li>Trust - a positive emotion; admiration is stronger; acceptance is weaker.</li>
    <li>Anticipation - in the sense of looking forward positively to something which is going to happen. Expectation is more neutral.</li>
</ol>
<h3>Feelings</h3>
<p>Feelings are what happens when these emotions are filtered through all of our memories, beliefs, experiences, and conscious awareness.<br></p>
<table style="border-style: none; border-width: 5px;">
    <tr>
        <th align="center">Fear</td>
        <th align="center">Anger</td>
        <th align="center">Sadness</td>
        <th align="center">Surprise</td>
        <th align="center">Joy</td>
        <th align="center">Love</td>
    </tr>
    <tr>
        <td align="center"><u>Scared</u></td>
        <td align="center"><u>Rage</u></td>
        <td align="center"><u>Suffering</u></td>
        <td align="center"><u>Stunned</u></td>
        <td align="center"><u>Content</u></td>
        <td align="center"><u>Affectionate</u></td>
    </tr>
    <tr>
        <td align="center">Frightened</td>
        <td align="center">Hate</td>
        <td align="center">Agony</td>
        <td align="center">Shocked</td>
        <td align="center">Satisfied</td>
        <td align="center">Romantic</td>
    </tr>
    <tr>
        <td align="center">Helpless</td>
        <td align="center">Hostile</td>
        <td align="center">Hurt</td>
        <td align="center">Dismayed</td>
        <td align="center">Pleased</td>
        <td align="center">Fondness</td>
    </tr>
    <tr>
        <td align="center">&nbsp;</td>
        <td align="center">&nbsp;</td>
        <td align="center">&nbsp;</td>
        <td align="center">&nbsp;</td>
        <td align="center">&nbsp;</td>
        <td align="center">&nbsp;</td>
    </tr>
    <tr>
        <td align="center"><u>Terror</u></td>
        <td align="center"><u>Exasperated</u></td>
        <td align="center"><u>Sadness</u></td>
        <td align="center"><u>Confused</u></td>
        <td align="center"><u>Happy</u></td>
        <td align="center"><u>Longing</u></td>
    </tr>
    <tr>
        <td align="center">Panic</td>
        <td align="center">Agitated</td>
        <td align="center">Depressed</td>
        <td align="center">Disillusioned</td>
        <td align="center">Amused</td>
        <td align="center">Sentimental</td>
    </tr>
    <tr>
        <td align="center">Hysterical</td>
        <td align="center">Fustrated</td>
        <td align="center">Sorrow</td>
        <td align="center">Perplexed</td>
        <td align="center">Delighted</td>
        <td align="center">Attracted</td>
    </tr>
    <tr>
        <td align="center">&nbsp;</td>
        <td align="center">&nbsp;</td>
        <td align="center">&nbsp;</td>
        <td align="center">&nbsp;</td>
        <td align="center">&nbsp;</td>
        <td align="center">&nbsp;</td>
    </tr>
    <tr>
        <td align="center"><u>Insecure</u></td>
        <td align="center"><u>Irritable</u></td>
        <td align="center"><u>Disappointed</u></td>
        <td align="center"><u>Amazed</u></td>
        <td align="center"><u>Cheerful</u></td>
        <td align="center"><u>Desire</u></td>
    </tr>
    <tr>
        <td align="center">Inferior</td>
        <td align="center">Annoyed</td>
        <td align="center">Dismayed</td>
        <td align="center">Astonished</td>
        <td align="center">Jovial</td>
        <td align="center">Passion</td>
    </tr>
    <tr>
        <td align="center">Inadequate</td>
        <td align="center">Aggravated</td>
        <td align="center">Displeased</td>
        <td align="center">Awestruck</td>
        <td align="center">Blissful</td>
        <td align="center">Infatuation</td>
    </tr>
    <tr>
        <td align="center">&nbsp;</td>
        <td align="center">&nbsp;</td>
        <td align="center">&nbsp;</td>
        <td align="center">&nbsp;</td>
        <td align="center">&nbsp;</td>
        <td align="center">&nbsp;</td>
    </tr>
    <tr>
        <td align="center"><u>Nervous</u></td>
        <td align="center"><u>Envy</u></td>
        <td align="center"><u>Shameful</u></td>
        <td align="center"><u>Overcome</u></td>
        <td align="center"><u>Proud</u></td>
        <td align="center"><u>Tenderness</u></td>
    </tr>
    <tr>
        <td align="center">Worried</td>
        <td align="center">Resentful</td>
        <td align="center">Regretful</td>
        <td align="center">Speechless</td>
        <td align="center">Triumphant</td>
        <td align="center">Caring</td>
    </tr>
    <tr>
        <td align="center">Anxious</td>
        <td align="center">Jealous</td>
        <td align="center">Guilty</td>
        <td align="center">Astounded</td>
        <td align="center">Illustrious</td>
        <td align="center">Compassionate</td>
    </tr>
    <tr>
        <td align="center">&nbsp;</td>
        <td align="center">&nbsp;</td>
        <td align="center">&nbsp;</td>
        <td align="center">&nbsp;</td>
        <td align="center">&nbsp;</td>
        <td align="center">&nbsp;</td>
    </tr>
    <tr>
        <td align="center"><u>Horror</u></td>
        <td align="center"><u>Disgust</u></td>
        <td align="center"><u>Neglected</u></td>
        <td align="center"><u>Moved</u></td>
        <td align="center"><u>Optimistic</u></td>
        <td align="center"><u>Peaceful</u></td>
    </tr>
    <tr>
        <td align="center">Mortified</td>
        <td align="center">Contempt</td>
        <td align="center">Isolated</td>
        <td align="center">Stimulated</td>
        <td align="center">Eager</td>
        <td align="center">Relieved</td>
    </tr>
    <tr>
        <td align="center">Dread</td>
        <td align="center">Revolted</td>
        <td align="center">Lonely</td>
        <td align="center">Touched</td>
        <td align="center">Hopeful</td>
        <td align="center">Satisfied</td>
    </tr>
    <tr>
        <td align="center">&nbsp;</td>
        <td align="center">&nbsp;</td>
        <td align="center">&nbsp;</td>
        <td align="center">&nbsp;</td>
        <td align="center">&nbsp;</td>
        <td align="center">&nbsp;</td>
    </tr>
    <tr>
        <td align="center">&nbsp;</td>
        <td align="center">&nbsp;</td>
        <td align="center"><u>Despair</u></td>
        <td align="center">&nbsp;</td>
        <td align="center"><u>Enthusiastic</u>
        </td>
        <td align="center">&nbsp;</td>
    </tr>
    <tr>
        <td align="center">&nbsp;</td>
        <td align="center">&nbsp;</td>
        <td align="center">Grief</td>
        <td align="center">&nbsp;</td>
        <td align="center">Excited</td>
        <td align="center">&nbsp;</td>
    </tr>
    <tr>
        <td align="center">&nbsp;</td>
        <td align="center">&nbsp;</td>
        <td align="center">Powerless</td>
        <td align="center">&nbsp;</td>
        <td align="center">Zeal</td>
        <td align="center">&nbsp;</td>
    </tr>
    <tr>
        <td align="center">&nbsp;</td>
        <td align="center">&nbsp;</td>
        <td align="center">&nbsp;</td>
        <td align="center">&nbsp;</td>
        <td align="center">&nbsp;</td>
        <td align="center">&nbsp;</td>
    </tr>
    <tr>
        <td align="center">&nbsp;</td>
        <td align="center">&nbsp;</td>
        <td align="center">&nbsp;</td>
        <td align="center">&nbsp;</td>
        <td align="center"><u>Elation</u></td>
        <td align="center">&nbsp;</td>
    </tr>
    <tr>
        <td align="center">&nbsp;</td>
        <td align="center">&nbsp;</td>
        <td align="center">&nbsp;</td>
        <td align="center">&nbsp;</td>
        <td align="center">Euphoric</td>
        <td align="center">&nbsp;</td>
    </tr>
    <tr>
        <td align="center">&nbsp;</td>
        <td align="center">&nbsp;</td>
        <td align="center">&nbsp;</td>
        <td align="center">&nbsp;</td>
        <td align="center">Jubilation</td>
        <td align="center">&nbsp;</td>
    </tr>
    <tr>
        <td align="center">&nbsp;</td>
        <td align="center">&nbsp;</td>
        <td align="center">&nbsp;</td>
        <td align="center">&nbsp;</td>
        <td align="center">&nbsp;</td>
        <td align="center">&nbsp;</td>
    </tr>
    <tr>
        <td align="center">&nbsp;</td>
        <td align="center">&nbsp;</td>
        <td align="center">&nbsp;</td>
        <td align="center">&nbsp;</td>
        <td align="center"><u>Enthralled</u></td>
        <td align="center">&nbsp;</td>
    </tr>
    <tr>
        <td align="center">&nbsp;</td>
        <td align="center">&nbsp;</td>
        <td align="center">&nbsp;</td>
        <td align="center">&nbsp;</td>
        <td align="center">Enchanted</td>
        <td align="center">&nbsp;</td>
    </tr>
    <tr>
        <td align="center">&nbsp;</td>
        <td align="center">&nbsp;</td>
        <td align="center">&nbsp;</td>
        <td align="center">&nbsp;</td>
        <td align="center">Rapture</td>
        <td align="center">&nbsp;</td>
    </tr>
</table>
<h3>Sources</h3>
<ul>
  <li><a title="A list of feelings, or is it a list of emotions?" href="http://www.visualisingmentalhealth.com/projects-in-development/a-list-of-feelings-or-is-it-a-list-of-emotions/">http://www.visualisingmentalhealth.com/projects-in-development/a-list-of-feelings-or-is-it-a-list-of-emotions/</a></li>
</ul>
)";

EmotionsReference::EmotionsReference(QWidget *parent) : QWidget(parent), m_ui(new Ui::EmotionsReference)
{
    m_ui->setupUi(this);
    m_ui->text->setText(TEXT);

    connect(m_ui->ok_button, &QPushButton::clicked, this, &EmotionsReference::close, Qt::QueuedConnection);
}

EmotionsReference::~EmotionsReference()
{
    delete m_ui;
}

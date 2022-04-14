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

#include "gratitudeguide.h"
#include "ui_gratitudeguide.h"

QString const TEXT = R"(
<h3>Considerations</h3>
<ol>
    <li>Be as specific as possible - specificity is key to fostering gratitude. "I'm grateful that my co-workers brought
        me soup when I was sick on Tuesday" will be more effective than "I'm grateful for my co-workers."</li>
    <li>Go for depth over breadth. Elaborating in detail about a particular person or thing for which you're grateful
        carries more benefits than a superficial list of many things.</li>
    <li>Get personal. Focusing on people to whom you are grateful has more of an impact than focusing on things for
        which you are grateful.</li>
    <li>Try subtraction, not just addition. Consider what your life would be like without certain people or things,
        rather than just tallying up all the good stuff. Be grateful for the negative outcomes you avoided, escaped,
        prevented, or turned into something positive - try not to take that good fortune for granted.</li>
    <li>See good things as "gifts." Thinking of the good things in your life as gifts guards against taking them for
        granted. Try to relish and savor the gifts you've received.</li>
    <li>Savor surprises. Try to record events that were unexpected or surprising, as these tend to elicit stronger
        levels of gratitude.</li>
    <li>Revise if you repeat. Writing about some of the same people and things is OK, but zero in on a different aspect
        in detail.</li>
    <li>Write regularly. Whether you write daily or every other day, commit to a regular time to journal, then honor
        that commitment.</li>
</ol>
<h3>Resilience</h3>
<p>To meet our need for safety, we can draw on:</p>
<ul>
    <li><strong>Compassion</strong>: Being sensitive to the burdens and suffering of others and ourselves, along with
        the desire to help with these if we can.</li>
    <li><strong>Grit</strong>: Being doggedly tough and resourceful.</li>
    <li><strong>Calm</strong>: Emotional balance and a sense of capability in the face of threats.</li>
    <li><strong>Courage</strong>: Protecting and standing up for ourselves, including with others.</li>
</ul>
<p>To meet our need for satisfaction, we can draw on:</p>
<ul>
    <li><strong>Mindfulness</strong>: Staying present in the moment as it is, rather than daydreaming, ruminating, or
        being distracted.</li>
    <li><strong>Gratitude</strong>: Appreciating and feeling good about what already exists.</li>
    <li><strong>Motivation</strong>: Pursuing opportunities in the face of challenges.</li>
    <li><strong>Aspiration</strong>: Reaching for and achieving results that are important to us.</li>
</ul>
<p>To meet our need for connection, we can draw on:</p>
<ul>
    <li><strong>Learning</strong>: Growing and developing, a process that allows us to cultivate all the other
        strengths.</li>
    <li><strong>Confidence</strong>: Feeling a sense of being cared about, worthy, and self-assured.</li>
    <li><strong>Intimacy</strong>: Being open to knowing and being known by others.</li>
    <li><strong>Generosity</strong>: Giving to others through altruism, compassion, and forgiveness.</li>
</ul>
<h4>Considerations</h4>
<ul>
    <li>What, if it were more present in my mind these days, would really help?</li>
    <li>What inner strengths could help me stay peaceful, content, and loving when I&rsquo;m dealing with this
        challenge?</li>
    <li>If this challenge began in the past, what would have been really helpful to have experienced back then?</li>
    <li>Deep down, what experience do I still very much long for?</li>
</ul>
<p>The answers to these questions point to which resources you might need to get through your challenge.</p>
<h4>Have a good experience</h4>
<p>To have beneficial experiences in the first place, it helps to be alert to the good facts around you - for example,
    fortunate circumstances, the beauty of nature, tasks you are completing, people who care about you, or your own
    talents and skills. You can even find the good in hard times, such as seeing the kindness of others as you go
    through a loss.</p>
<p>Besides simply <em>noticing</em> useful or pleasurable thoughts, feelings, or sensations that are already present in
    your awareness, you could <em>create</em> beneficial experiences, such as by getting some exercise (to help build
    the resource of grit) or deliberately recognizing your own good heart (for confidence). Or you could make something
    good happen in a relationship, such as by listening carefully to someone (for intimacy).</p>
<p>Over time, you can learn to directly <em>evoke</em> a positive experience, such as relaxing at will, calling up a
    sense of determination, or letting go of resentment. Because of experience-dependent neuroplasticity, repeatedly
    having and internalizing a particular experience in the past makes it easier and easier to evoke it in the present.
    It's like being able to push a button on your inner jukebox and quickly get the song of a useful experience playing
    in your mind, since you&rsquo;ve recorded it again and again.</p>
<h4>Enrich it</h4>
<ul>
    <li><strong>Lengthen it.</strong> Stay with it for five, ten, or more seconds. The longer that neurons fire
        together, the more they tend to wire together. Protect the experience from distractions, focus on it, and come
        back to it if your mind wanders.</li>
    <li><strong>Intensify it.</strong> Open to it and let it be big in your mind. Turn up the volume by breathing more
        fully or getting a little excited.</li>
    <li><strong>Expand it.</strong> Notice other elements of the experience. For example, if you're having a useful
        thought, look for related sensations or emotions.</li>
    <li><strong>Freshen it.</strong> The brain is a novelty detector, designed to learn from what's new or unexpected.
        Look for what&rsquo;s interesting or surprising about an experience. Imagine that you are having it for the very
        first time.</li>
    <li><strong>Value it.</strong> We learn from what is personally relevant. Be aware of why the experience is
        important to you, why it matters, and how it could help you.</li>
</ul>
<h4>Absorb it</h4>
<ul>
    <li><strong>Intend to receive it.</strong> Consciously choose to take in the experience.</li>
    <li><strong>Sense it sinking into you.</strong> You could imagine that the experience is like a warm, soothing balm
        or a jewel being placed in the treasure chest of your heart. Give over to it, allowing it to become a part of
        you.</li>
    <li><strong>Reward yourself.</strong> Tune into whatever is pleasurable, reassuring, helpful, or hopeful about the
        experience. Doing this will tend to increase the activity of two neurotransmitter systems - dopamine and
        norepinephrine - that will flag the experience as a "keeper" for long-term storage.</li>
</ul>
<p>Don't try and cling onto experiences in the stream of consciousness, rather gently encourage whatever is beneficial
    to arise and stick around and sink in.</p>
<h4>Link it</h4>
<p>In Linking, you are simply conscious of both "negative" and "positive" material at the same time. For example, off to
    the side of awareness could be old feelings of being left out and unwanted (perhaps from a rocky childhood) while in
    the foreground of awareness are feelings of being liked and included by people at work. The brain naturally
    associates things together, so if you keep the positive material more prominent and intense in awareness, it will
    tend to soothe, ease, and even gradually replace the negative material.</p>
<p>For example, challenges to safety are often indicated by a sense of anxiety, anger, powerlessness, or trauma - and a
    sense of calm or grit can really help with these. Challenges to our need for satisfaction are frequently experienced
    as frustration, disappointment, drivenness, addiction, blahness, or boredom. Feeling thankful, awestruck, or already
    contented are well-matched to these issues. Challenges to connection can be experienced as loneliness, resentment,
    or inadequacy - and feeling either caring or cared about is a wonderful relief, since love is love whether it is
    flowing in or out.</p>
<p>If you get pulled into the negative, drop it and focus only on the positive. And remember that this step is optional:
    If the challenge you're facing is too powerful, you can grow mental resources for addressing it through the first
    three HEAL steps alone.</p>
<h3>Sources</h3>
<ul>
    <li><a title="Gratitude Journal"
            href="https://ggia.berkeley.edu/practice/gratitude_journal">https://ggia.berkeley.edu/practice/gratitude_journal</a>
    </li>
    <li><a title="How to Hardwire Resilience into Your Brain"
            href="https://greatergood.berkeley.edu/article/item/how_to_hardwire_resilience_into_your_brain">https://greatergood.berkeley.edu/article/item/how_to_hardwire_resilience_into_your_brain</a>
    </li>
</ul>
)";

GratitudeGuide::GratitudeGuide(QWidget *parent) : QWidget(parent), m_ui(new Ui::GratitudeGuide)
{
    m_ui->setupUi(this);
    m_ui->text->setText(TEXT);

    connect(m_ui->ok_button, &QPushButton::clicked, this, &GratitudeGuide::close, Qt::QueuedConnection);
}

GratitudeGuide::~GratitudeGuide()
{
    delete m_ui;
}

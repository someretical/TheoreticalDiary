#ifndef THEORETICALCALENDAR_H
#define THEORETICALCALENDAR_H

#include <QDate>
#include <QIcon>
#include <QPushButton>
#include <QString>
#include <QWidget>

namespace Ui {
class TheoreticalCalendar;
}

// Forward declaration of TheoreticalCalendar to fix cyclical reference
class TheoreticalCalendar;

// CalendarButton class
class CalendarButton : public QPushButton {
  Q_OBJECT

signals:
  void sig_clicked(const int code);

public:
  explicit CalendarButton(void (TheoreticalCalendar::*slot)(const int),
                          const int __base_0_day, const int _rating,
                          const QIcon &icon, const QString &text,
                          QWidget *parent = nullptr);
  ~CalendarButton();

  int *base_0_day;
  int *rating;

public slots:
  void clicked_on();
};

// TheoreticalCalendar class
class TheoreticalCalendar : public QWidget {
  Q_OBJECT

public:
  explicit TheoreticalCalendar(QWidget *parent = nullptr);
  ~TheoreticalCalendar();
  void set_button_stylesheet(CalendarButton &button, const bool selected);
  void change_month(const int year, const int month = 1,
                    const bool reset = false);
  void rerender_day(const int index, const bool selected = false);

  QString *s_default;
  QString *s_very_bad;
  QString *s_bad;
  QString *s_ok;
  QString *s_good;
  QString *s_very_good;
  QString *s_selected;

  int *current_month_offset;
  int *last_selected_index;

  QIcon *star_icon;
  QDate *first_created;

public slots:
  void clicked_on(const int base_0_day);
  void dropdown_changed(const int index);
  void date_changed(const QDate &date);
  void next_month();
  void last_month();

private:
  Ui::TheoreticalCalendar *ui;
};

#endif // THEORETICALCALENDAR_H

#ifndef DIARYENTRYLIST_H
#define DIARYENTRYLIST_H

#include "diaryholder.h"

#include <QDate>
#include <QDialog>
#include <QLabel>
#include <QMouseEvent>
#include <QString>
#include <QWidget>
#include <string>

namespace Ui {
class DiaryEntryList;
}

class DiaryEntryList : public QDialog {
  Q_OBJECT

public:
  explicit DiaryEntryList(QWidget *parent = nullptr);
  ~DiaryEntryList();
  void change_month(const QDate date);
  void render_month(std::optional<td::EntryMap *>);
  void rerender_current_month();
  QLabel *create_day_label(std::pair<const int, const td::Entry> const i);

public slots:
  void dropdown_changed(const int index);
  void date_changed(const QDate &date);
  void next_month();
  void last_month();
  void reset_month();

private:
  Ui::DiaryEntryList *ui;
  QDate *first_created;

  QString *s_base;
  QString *s_very_bad;
  QString *s_bad;
  QString *s_ok;
  QString *s_good;
  QString *s_very_good;
  QString *s_starred;
};

//
//
// Clickable label
class ClickableLabel : public QLabel {
  Q_OBJECT

public:
  explicit ClickableLabel(const int &d, const std::string &t, QWidget *parent);
  ~ClickableLabel();

  static void get_trunc_first_line(std::string const &input, std::string &res);

signals:
  void clicked();

private:
  int *day;
  std::string *full_text;
  bool *expanded;

protected:
  void mouseReleaseEvent(QMouseEvent *event);
};

#endif // DIARYENTRYLIST_H

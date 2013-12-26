#ifndef __NOTESWIDGET_H__
#define __NOTESWIDGET_H__
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QTableView>
#include <QSqlTableModel>
#include <QSqlQueryModel>
#include <QHeaderView>
#include <QFile>
#include <QMessageBox>
#include <QSqlError>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QTextEdit>
#include <QLineEdit>
#include <QPushButton>
#include <QSplitter>
#include <QDebug>
#include <QModelIndex>
//CREATE TABLE notes (id integer primary key, nodeId text,word text,tag text,note text,created text,amended text);
/// these are the columns in the table
enum {
  Notes_Id = 0,
  Notes_NodeId = 1,
  Notes_Word = 2,
  Notes_Note = 3,
  Notes_Tag = 4,
  Notes_CreationDate = 5,
  Notes_AmendDate = 6
};
class NotesWidget : public QWidget {
  Q_OBJECT
 public:

  NotesWidget(QWidget * parent = 0);
  ~NotesWidget();
 public slots:
  void onShowClicked();
  void showSelectedNote();
 private:
  QString m_baseQuery;
  QSqlDatabase m_db;
  QSqlQueryModel * m_model;
  QTableView * m_view;
  bool createConnection();
  QTextEdit * m_note;
  QLineEdit * m_show;
  QLineEdit * m_tag;
  QPushButton * m_showBtn;
  QWidget * createQueryWidget();
  QWidget * createEditWidget();
};
#endif

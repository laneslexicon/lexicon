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
class EditableSqlModel : public QSqlQueryModel
{
    Q_OBJECT

public:
    EditableSqlModel(QObject *parent = 0);

    Qt::ItemFlags flags(const QModelIndex &index) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role);
    void setSearch(const QString & t ) { m_searchItem = t;};
    void setNote(int row, const QString &);
    void setTag(int row, const QString &);
    void addNote(const QString & node,const QString & word,const QString & note,const QString & tag);
    void refresh();
 private:
    bool updateNote(int id, const QString &);
    bool updateTag(int id, const QString &);

    bool createConnection();
    QString m_searchItem;
    QString m_baseQuery;
    QSqlDatabase m_db;
};

class NotesWidget : public QWidget {
  Q_OBJECT
 public:

  NotesWidget(QWidget * parent = 0);
  ~NotesWidget();
 public slots:
  void onShowClicked();
  void onSaveClicked();
  void onNewClicked();
  void showSelectedNote();
 private:
  int m_currentRow;
  QString m_baseQuery;
  QSqlDatabase m_db;
  //QSqlQueryModel * m_model;
  EditableSqlModel * m_model;
  QTableView * m_view;
  bool createConnection();
  QLineEdit * m_node;
  QLineEdit * m_word;
  QTextEdit * m_note;
  QLineEdit * m_show;
  QLineEdit * m_tag;
  QPushButton * m_newBtn;
  QPushButton * m_showBtn;
  QPushButton * m_saveBtn;
  QPushButton * m_deleteBtn;
  QWidget * createQueryWidget();
  QWidget * createEditWidget();
};
#endif

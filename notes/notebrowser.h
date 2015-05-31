#ifndef __NOTEBROWSER_H__
#define __NOTEBROWSER_H__
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
#include <QFormLayout>
#include <QLabel>
#include <QComboBox>
#include <QTextEdit>
#include <QLineEdit>
#include <QPushButton>
#include <QSplitter>
#include <QTableWidget>
#include <QDebug>
#include <QModelIndex>
#include <QKeyEvent>
class Place;
class NoteBrowser : public QWidget {
  Q_OBJECT
 public:

  NoteBrowser(QWidget * parent = 0);
  ~NoteBrowser();
  public slots:
    //   void onNoteSelected(int );
    void onCellClicked(int,int);
    void onDeleteClicked();
    void onViewClicked();
    void loadNotes();
 protected:
    bool eventFilter(QObject *, QEvent *);
 private:
    void readSettings();
    void showEntry(const Place &);
    bool readCssFromFile(const QString &);
    bool m_debug;
    QString m_css;
    QString m_xsltSource;
    QString transform(const QString & xml);
    QString m_style;
    QMap<int,int> getRowIdMap();
    QTableWidget * m_list;
    QTextEdit * m_note;
    QLineEdit * m_subject;
    QLineEdit * m_tag;
    QComboBox * m_type;
    QPushButton * m_newBtn;
    QPushButton * m_showBtn;
    QPushButton * m_saveBtn;
    QPushButton * m_deleteButton;
    QPushButton * m_printButton;
    QPushButton * m_viewButton;
};

#endif

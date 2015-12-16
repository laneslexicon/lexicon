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
    void onCellDoubleClicked(int,int);
    void onDeleteClicked();
    void onViewEntryClicked();
    void onViewNoteClicked();
    void loadNotes();
    void stateChanged(int);
    private slots:
      void sectionDoubleClicked(int);
 protected:
    bool eventFilter(QObject *, QEvent *);
 private:
    QMap<int,QString> m_cols;
    void readSettings();
    void showEntry(const Place &);
    void afterLoad();
    bool readCssFromFile(const QString &);
    bool startsWithArabic(const QString &) const;
    bool m_debug;
    bool m_noNotes;
    int m_substrLength;
    QString m_css;
    QString m_xsltSource;
    QString transform(const QString & xml);
    QString m_style;
    QLabel * createLabel(const QString &,bool removeLineBreak = true) const;
    QTableWidget * m_list;
    QTextEdit * m_note;
    QLineEdit * m_subject;
    QLineEdit * m_tag;
    QComboBox * m_type;
    QPushButton * m_newBtn;
    QPushButton * m_showBtn;
    QPushButton * m_saveBtn;
    QPushButton * m_refreshButton;
    QPushButton * m_deleteButton;
    QPushButton * m_printButton;
    QPushButton * m_viewButton;
    QPushButton * m_viewNoteButton;
 signals:
    void showNode(const QString &);
    void printNode(const QString &);
    void noteDeleted(const QStringList &);
};

#endif

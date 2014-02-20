#ifndef __ENTRYITEM_H__
#define __ENTRYITEM_H__
#include <QWidget>
#include <QPushButton>
#include <QLineEdit>
#include <QTextEdit>
#include <QTextBrowser>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTextStream>
#include <QColor>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGraphicsTextItem>
#include <QLabel>
#include <QFile>
#include <QFileInfo>
#include <QDir>
#include <QDebug>
#include <QSplitter>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlDriver>
#include <QApplication>
#include <QTextOption>
#include <QScrollBar>
#include <QMenu>
#include <QAction>
#include <QGraphicsSceneContextMenuEvent>
#include <QKeyEvent>
#include <QFocusEvent>
#include <QSettings>
#include <QRegExp>
#include <QClipboard>
#include "QsLog.h"
#include "xsltsupport.h"
#include "history.h"
#include "place.h"


class EntryItem : public QGraphicsTextItem {
  Q_OBJECT

 public:
  EntryItem(const QString &, QGraphicsItem * parent = 0);
  EntryItem(QGraphicsItem * parent = 0);
  void setNode(const QString & id);
  void setRoot(const QString & root,bool isRootEntry = false);
  void setWord(const QString & word);
  void setSupplement(int v);
  void setPage(int x);
  void setPlace(const Place &);
  void setBackground(QColor & c) { m_backgroundColor = c;}
  Place getPlace();
  QString getNode() { return m_place.getNode();}
  QString getRoot() { return m_place.getRoot();}
  QString getWord() { return m_place.getWord();}
  bool isRoot() { return m_place.isRoot();}
  int getSupplement() { return m_place.getSupplement();}
  int getPage() { return m_place.getPage();}
  QString getOutputHTML() { return m_html; }
  void setOutputHTML(const QString & html) { m_html = html;}
  QTextCursor highlight(const QString &);
  QTextCursor highlightRx(const QString &);
  public slots:
    void searchItem();
    void copy();
 signals:
    void showPerseus(const Place &);
    void placeChanged(const Place &);
    void bookmarkAdd(const QString & id,const Place & p);
 protected:
  void contextMenuEvent(QGraphicsSceneContextMenuEvent * event);
  void hoverEnterEvent(QGraphicsSceneHoverEvent * event);
  void focusInEvent(QFocusEvent *);
  void paint( QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *);
  QColor m_backgroundColor;
  private:
  Place m_place;
  QString m_html;    /// saves the generated HTML for debug, only set when dumpOutputHTM is true
};
#endif
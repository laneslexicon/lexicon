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
#include <QGraphicsWidget>
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
#include <QToolButton>
#include <QMap>
#include "place.h"
class Note;
class NoteDialog;
class ToolButtonData : public QToolButton {
  Q_OBJECT
 public:
  ToolButtonData(int id);
  ~ToolButtonData();
  int getIndex() { return m_id;}
 private:
  int m_id;
};
class EntryItem : public QGraphicsTextItem {
  Q_OBJECT

 public:
  EntryItem(const QString &, QGraphicsItem * parent = 0);
  EntryItem(QGraphicsItem * parent = 0);
  ~EntryItem();
  void setProxy(QGraphicsWidget *);
  void setNode(const QString & id);
  void setRoot(const QString & root,bool isRootEntry = false);
  void setWord(const QString & word);
  void setSupplement(int v);
  void setPage(int x);
  void setPlace(const Place &);
  void setBackground(QColor & c) { m_backgroundColor = c;}
  void selectAll();
  QPair<int,int> find(const QRegExp &,int,bool highlight = true);
  Place getPlace();
  QString getNode();
  QString getRoot();
  QString getWord();
  bool isRoot();
  int getSupplement();
  int getPage();
  QString getOutputHtml() const { return m_html; }
  void setOutputHtml(const QString & html) { m_html = html;}
  void setFocusOnHover(bool v) { m_focusOnHover = v;};
  bool  getFocusOnHover() const { return m_focusOnHover;}
  int findCount() const { return m_highlights.size(); }
  void setXml(const QString &);
  QString getXml() const;
  void setNotesEnabled(bool v) { m_notesEnabled = v;}
  void showNote();
  void addNote();
  void deleteNote();
  void setNotes();
  QList<Note *> getNotes(bool erase = false);
  void destroyNotes();
  bool hasNotes() const;
  bool hasHighlights();
  void clearHighlights(bool keepResults = true);
  void showHighlights();
  int showHighlight(int);
  public slots:
    void searchItem();
    void clearSelection();
    void notesRejected();
    void notesAccepted();
 signals:
    void showPerseus(const Place &);
    void showHtml();
    void placeChanged(const Place &);
    void bookmarkAdd(const QString & id,const Place & p);
    void selectAllItems();
    void clearAllItems();
    void copy();
    void gotoNode(const Place & p, bool,bool);
    void addButton(bool);
    void printNode(const QString & node);
    void printPage();
 protected:
  void contextMenuEvent(QGraphicsSceneContextMenuEvent * event);
  void hoverEnterEvent(QGraphicsSceneHoverEvent * event);
  void focusInEvent(QFocusEvent *);
  void focusOutEvent(QFocusEvent *);
  void paint( QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *);


  QColor m_backgroundColor;
  bool m_focusOnHover;
 private:
  QString m_xml;
  bool m_notesEnabled;
  QGraphicsWidget * m_noteWidget;
  NoteDialog * m_note;
  QList<Note *> m_notes;
  QList<int> m_searchPositions;
  QList<int> m_highlights;
  QMap<int,QString> m_finds;
  QColor m_defaultBackground;
  Place m_place;
  QString m_searchText;
  QString m_html;
};
#endif

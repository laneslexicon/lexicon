#ifndef __GRAPHICSENTRY_H__
#define __GRAPHICSENTRY_H__
#include <QWidget>
#include <QPushButton>
#include <QLineEdit>
#include <QTextEdit>
#include <QTextBrowser>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTextStream>
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
#include "QsLog.h"
#include "xsltsupport.h"
#include "history.h"
//#include "laneslexicon.h"

class EntryItem : public QGraphicsTextItem {
 public:
  EntryItem(const QString &, QGraphicsItem * parent = 0);
  EntryItem(QGraphicsItem * parent = 0);
  void setNode(const QString & id) { m_nodeId = id;}
  void setRoot(const QString & root,bool isRootEntry = false);
  void setWord(const QString & word) { m_word = word; }
  void setSupplement(bool v)  { m_supplement = v;}
  bool isNode(const QString & id) { return m_nodeId ==  id;}
  bool isRoot() { return m_isRoot;}
  QString getNode() { return m_nodeId;}
  QString getRoot() { return m_root;}
  QString getWord() { return m_word;}
  bool isSupplement() { return m_supplement;}
 protected:
  void contextMenuEvent(QGraphicsSceneContextMenuEvent * event);
  void hoverEnterEvent(QGraphicsSceneHoverEvent * event);
  void paint( QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *);

  private:
  QString m_nodeId;
  bool m_isRoot;
  int m_page;
  bool m_supplement;
  QString m_root;
  QString m_word;
};
class LaneGraphicsView;
class GraphicsEntry : public QWidget {
  Q_OBJECT

 public:
  GraphicsEntry(QWidget * parent = 0);
  ~GraphicsEntry();
    bool prepareQueries();
    void getXmlForRoot(const QString &,int supplement = 0,const QString & anchor = QString());
    void getXmlForNode(const QString &, int supplement = 0);
    int hasRoot(const QString & root,bool focus = true);
    QString currentRoot() { return m_currentRoot;}
    /// return the first/last root in the scene
    QString lastRoot();
    QString firstRoot();
    void setPagingForward() {
      m_pagingDir = 0;
    }
    void setPagingBackward() {
      m_pagingDir = 1;
    }
  public slots:
    void on_findNode();
    void anchorClicked(const QUrl &);
    void linkActivated(const QString &);
    void linkHovered(const QString &);
    void anchorTest();
    void onZoomIn();
    void onZoomOut();
    void onClearScene();
    void nextPageRequested();
    void prevPageRequested();
 private:
    void readSettings();
    void writeDefaultSettings();
    bool readCssFromFile(const QString &name);
    int m_pagingDir;
    int m_entryMargin;
    bool m_debug;
    void appendEntries(int);
    void prependEntries(int);
    QList<EntryItem *> m_items;
    EntryItem * createEntry(const QString & xml);
    const XalanCompiledStylesheet * m_compiledXsl;
    bool showNode(const QString &,bool thisPageOnly = false);
    qreal m_scale;

    QTransform m_transform;
    QString transform(const QString & xsl,const QString & xml);

    QTextOption m_textOption;

    QPushButton * m_clearSceneBtn;
    QPushButton * m_zoomIn;
    QPushButton * m_zoomOut;


    LaneGraphicsView * m_view;
    QGraphicsScene * m_scene;
    QGraphicsTextItem * m_item;
    QSqlQuery * m_nodeQuery;
    QSqlQuery * m_rootQuery;
    QSqlQuery * m_nextRootQuery;
    QString m_standardCSS;

    QString m_currentHtml;
    QString m_currentRoot;

    // read/set from readSettings

    QString m_currentCSS;
    QString m_xsltSource;
    int m_textWidth;
    XalanTransformer * m_xalan;
 protected:
    void keyPressEvent(QKeyEvent *);
    void focusInEvent(QFocusEvent *);
 signals:
    void focusItemChanged(QGraphicsItem *, QGraphicsItem *, Qt::FocusReason);
    void rootChanged(const QString & root,const QString & node);
    void nextRoot(const QString &);
    void prevRoot(const QString &);
    void cssChanged();
};

class LaneGraphicsView : public QGraphicsView {
  Q_OBJECT
 public:
  LaneGraphicsView(QGraphicsScene *,GraphicsEntry * parent = 0);
 protected:
  virtual void scrollContentsBy(int,int);
  void keyPressEvent(QKeyEvent *);
 signals:
  void nextPage();
  void backPage();
};
#endif

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
#include "placewidget.h"
//#include "laneslexicon.h"

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
  void setPlace(Place p) { m_place = p;}
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
 protected:
  void contextMenuEvent(QGraphicsSceneContextMenuEvent * event);
  void hoverEnterEvent(QGraphicsSceneHoverEvent * event);
  void paint( QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *);
  QColor m_backgroundColor;
  private:
  Place m_place;
  QString m_html;    /// saves the generated HTML for debug, only set when dumpOutputHTM is true
};
class LaneGraphicsView;
class GraphicsEntry : public QWidget {
  Q_OBJECT

 public:
  GraphicsEntry(QWidget * parent = 0);
  ~GraphicsEntry();
  enum SearchType { RootSearch, WordSearch, NodeSearch };
  //  Place getXmlForRoot(const QString &,int supplement = 0,const QString & anchor = QString(),bool nodeOnly = false);
    Place getXmlForRoot(const Place &);
    Place getPage(const int );
    Place getXmlForPlace(const Place &);

    Place getPlace() { return m_place;}
    //    int hasRoot(const QString & root,bool focus = true);
    int hasPlace(const Place & ,int searchtype,bool focus = true);
    QString currentRoot() { return m_currentRoot;}
    /// return the first/last root in the scene
    void setPagingForward() {
      m_pagingDir = 0;
    }
    void setPagingBackward() {
      m_pagingDir = 1;
    }
    void highlight(const QString & t);
  public slots:
    void anchorClicked(const QUrl &);
    void linkActivated(const QString &);
    void linkHovered(const QString &);
    void anchorTest();
    void onZoomIn();
    void onZoomOut();
    void onClearScene();
    void nextPageRequested();
    void prevPageRequested();
    void nodeChanged(QGraphicsItem *, QGraphicsItem *, Qt::FocusReason);
    void showPerseus(const Place &);
 private:
    QColor m_supplementBg;
    bool prepareQueries();
    bool m_clearScene;
    QString lastRoot();
    QString firstRoot();
    void readSettings();
    void writeDefaultSettings();
    bool readCssFromFile(const QString &name);
    int m_pagingDir;
    int m_entryMargin;
    bool m_dumpXML;
    bool m_dumpHTML;
    bool m_dumpOutputHTML;
    void appendEntries(int);
    void prependEntries(int);
    QList<EntryItem *> m_items;
    EntryItem * createEntry(const QString & xml);
    //    const XalanCompiledStylesheet * m_compiledXsl;
    //    bool showNode(const QString &,bool thisPageOnly = false);
    bool showPlace(const Place &,bool thisPageOnly = false);
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

    QSqlQuery * m_pageQuery;
    QString m_standardCSS;

    QString m_currentHtml;
    QString m_currentRoot;
    Place m_place;
    PlaceWidget * m_showPlace;
    // read/set from readSettings

    QString m_currentCSS;
    QString m_xsltSource;
    int m_textWidth;
    //    XalanTransformer * m_xalan;
 protected:
    void keyPressEvent(QKeyEvent *);
    void focusInEvent(QFocusEvent *);
 signals:
    void focusItemChanged(QGraphicsItem *, QGraphicsItem *, Qt::FocusReason);
    void rootChanged(const QString & root,const QString & node);
    void nextRoot(const QString &);
    void prevRoot(const QString &);
    void cssChanged();
    void placeChanged(const Place &);
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

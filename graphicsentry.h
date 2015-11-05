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
#include <QProgressDialog>
#include <QGraphicsSceneContextMenuEvent>
#include <QToolButton>
#include <QGraphicsWidget>
#include <QKeyEvent>
#include <QFocusEvent>
#include <QSettings>
#include <QRegExp>
#include <QClipboard>
#include <QPrinter>
#include <QPair>
#include "entryitem.h"
#include "searchoptionswidget.h"
#ifndef qStrip
#define qStrip qDebug()
#endif
//class LaneGraphicsView;
class Place;
class GraphicsEntry : public QWidget {
  Q_OBJECT

 public:
  GraphicsEntry(QWidget * parent = 0);
  ~GraphicsEntry();
  enum SearchType { RootSearch, WordSearch, NodeSearch, PageSearch };
    Place getXmlForRoot(const Place &);
    Place showPlace(const Place &,bool thisPageOnly,bool createTab,bool activateTab);
    Place getPage(const Place & );
    Place getPlace(int index=-1) const;

    QGraphicsView * getView() const;
    int getPageNumber(int which=0) const;
    int hasPlace(const Place & ,int searchtype,bool focus = true);
    void setPagingForward() {
      m_pagingDir = 0;
    }
    void setPagingBackward() {
      m_pagingDir = 1;
    }
    QString getPageInfo(bool sys,bool summary);
    qreal getScale() { return m_scale;}
    void setScale(qreal v, bool use = true);
    int getTextWidth() const { return m_textWidth; }
    void setTextWidth(int w) { m_textWidth = w;}
    QGraphicsScene * getScene() { return m_scene;}
    int search();
    int getFindCount() const;
    //    int getCurrentFind() const;
    //    int getHighlightCount() const;
    bool hasHighlights() const;
    bool hasMoreFinds() const;
    void searchNext();
    bool hasNode(const QString &) const;
    bool focusNode(const QString &);
    void focusLast();
    void home();
    QString getHome() const;
    void setHome(const QString &);
    void shiftFocus();
    void setCurrentItem(QGraphicsItem *);
    static QString getXsltFileName();
    QString userTitle() const;
  public slots:
    void setUserTitle(const QString &);
    void anchorClicked(const QUrl &);
    void linkActivated(const QString &);
    void linkHovered(const QString &);
    void anchorTest();
    qreal onZoomIn();
    qreal onZoomOut();
    void onZoom(double);
    void onReload();
    void onReload(const QString & css,const QString & xslt);
    void onClearScene();
    void showPerseus(const Place &);
    void showHtml();
    void updateCurrentPlace(const Place &);
    void selectAll();
    void clearAll();
    void selectEntry();
    void copy();
    void onWiden();
    void onNarrow();
    void clearHighlights(bool keepResults = true);
    void notesButtonPressed();
    void addButtonDecoration(bool);
    void focusPlace();
    void showSelections();
    void print(QPrinter &,const QString & node = QString());
    void showLinkDetails(const QString & );
    void fixLink(const QStringList &,bool);
 private:
    int m_findCount;
    int m_highlightCount;
    int m_currentFind;
    void centerOnSearchResult(int,int);
    QString lastRoot();
    QString firstRoot();
    QString getOutputFilename(const QString &,bool, const QString &,const QString & node = QString());
    void readSettings();
    void writeDefaultSettings();
    QString fixHtml(const QString &);
    QString readCssFromFile(const QString &name);
    bool saveLink(const QString &,const QString &);
    void dumpInfo(EntryItem *,const QString & node);
    void setItemPlace(EntryItem * item,const QSqlQuery & query);

    void reposition();
    void appendEntries(int);
    void prependEntries(int);
    void moveFocusUp();
    void moveFocusDown();
    void moveForward();
    void moveBackward();
    EntryItem * createEntry(const QString & xml);
    QString transform(int type,const QString & xsl,const QString & xml);
    void checkLink(const QString);
    SearchOptions m_currentSearchOptions;

    QList<int> m_searchItemIndexes;
    int m_searchItemPtr;
    int m_searchIndex;


    bool m_showLinkWarning;
    bool m_linksUseCurrentTab;
    bool m_activateLink;
    bool m_notesEnabled;
    bool m_clearScene;
    bool m_debug;
    bool m_dumpXml;
    bool m_dumpHtml;
    bool m_dumpOutputHtml;
    bool m_offPageMovement;
    bool m_linkCheckMode;

    int m_pagingDir;
    int m_entryMargin;
    int m_widenStep;
    int m_defaultWidth;
    int m_textWidth;

    QString m_moveFocusUpKey;
    QString m_moveFocusDownKey;
    QString m_zoomInKey;
    QString m_zoomOutKey;
    QString m_moveForwardKey;
    QString m_moveBackwardKey;
    QString m_widenKey;
    QString m_narrowKey;
    QString m_searchKey;
    QString m_searchNextKey;
    QString m_clearKey;
    QString m_showKey;
    QString m_markKey;
    QString m_homeKey;
    QString m_reloadKey;
    QString m_currentSearchTarget;
    QString m_focusNode;
    QString m_currentCss;
    QString m_printCss;
    QString m_entryXslt;
    QString m_nodeXslt;
    QString m_standardCSS;
    QString m_currentHtml;
    QString m_pattern;
    QString m_highlightColorName;
    QString m_userTitle;
    QRegExp m_currentSearchRx;

    QList<EntryItem *> m_items;

    //    const XalanCompiledStylesheet * m_compiledXsl;

    qreal m_scale;

    QTransform m_transform;

    QTextOption m_textOption;

    QGraphicsView * m_view;
    QGraphicsScene * m_scene;
    QGraphicsTextItem * m_item;


    Place m_place;


    //    XalanTransformer * m_xalan;

 protected:
    void keyPressEvent(QKeyEvent *);
    void focusInEvent(QFocusEvent *);
    void closeEvent(QCloseEvent *);
 signals:
    void focusItemChanged(QGraphicsItem *, QGraphicsItem *, Qt::FocusReason);
    void nextRoot(const QString &);
    void prevRoot(const QString &);
    void prevHead(const Place &);
    void next(const Place &);
    void prev(const Place &);
    void cssChanged();
    void placeChanged(const Place &);
    void historyPositionChanged(int);
    void historyAddition(const Place &);
    void bookmarkAdd(const QString & id,const Place &);
    void clearPage();
    void searchPage();
    void findCount(int);
    void gotoNode(const Place & p,bool,bool);
    void printNode(const QString &);
    void printPage();
    void searchStarted();
    void searchFoundNext();
    void searchFinished();
};
/*
class LaneGraphicsView : public QGraphicsView {
  Q_OBJECT
 public:
  LaneGraphicsView(QGraphicsScene *,GraphicsEntry * parent = 0);
 protected:
  virtual void scrollContentsBy(int,int);
  void keyPressEvent(QKeyEvent *);
  void focusInEvent(QFocusEvent *);
  void focusOutEvent(QFocusEvent *);
 signals:
  void nextPage();
  void backPage();
};
*/
#endif

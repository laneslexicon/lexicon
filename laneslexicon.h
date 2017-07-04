#ifndef __LANESLEXICON_H__
#define __LANESLEXICON_H__
#include <QApplication>
#include <QMainWindow>
#include <QTabWidget>
#include <QTableWidget>
#include <QTreeWidgetItem>
#include <QGridLayout>
#include <QFormLayout>
#include <QComboBox>
#include <QToolBar>
#include <QStatusBar>
#include <QMenu>
#include <QMenuBar>
#include <QAction>
#include <QStatusBar>
#include <QDebug>
#include <QSplitter>
#include <QLineEdit>
#include <QTextEdit>
#include <QStackedLayout>
#include <QSplitter>
#include <QVBoxLayout>
#include <QPushButton>
#include <QSettings>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsItem>
#include <QTreeView>
#include <QSqlDatabase>
#include <QFile>
#include <QIcon>
#include <QLabel>
#include <QDialog>
#include <QKeySequenceEdit>
#include <QSettings>
#include <QDateTime>
#include <QShortcut>
#include <QSignalMapper>
#include <QMessageBox>
#include <QDockWidget>
#include <QInputDialog>
#include <QActionGroup>
#include <QTimer>
#include <QUrl>
#include <QPrinter>
#include <QPrintDialog>
#include <QPainter>
#include <QDomDocument>
#include "place.h"
#include "searchoptionswidget.h"
#include "searchoptions.h"
class ArabicSearchDialog;
class NoteMaster;
class HistoryMaster;
class Place;
class NodeSearchDialog;
class PageSearchDialog;
class AppMenu;
class InputMapper;
class GraphicsEntry;
class ContentsWidget;
class TabWidget;
class EditView;
class LogViewer;
class HelpWidget;
class OptionsDialog;
class HelpView;
class ContentsPanel;
class LanesLexicon : public QMainWindow
{
  Q_OBJECT

 public:
  explicit LanesLexicon(QWidget *parent = 0);
  ~LanesLexicon();

  QSize sizeHint() const;
  void readSettings();
  void writeSettings();
  bool isOk() { return m_ok;}
  QString convertString(const QString &) const;
  QString getActiveKeymap() const;
  QString getKeymapFileName(const QString &) const;
  bool keymapsEnabled() const { return m_keymapsEnabled;}
  QMapIterator<QString,QString> getMapIterator();
  HistoryMaster * history();
  NoteMaster * notes();
  QTabWidget * tabwidget() const;
  QWidget * currentTab();
  bool sanityCheck(int type = 0);
  enum NewTabBehaviour { InsertTab, AppendTab };
  enum NavigationMode { ByRoot , ByPage };
  static SearchOptions options(int);
  public slots:
    int hasPlace(const Place & p,int searchtype,bool setFocus = false);
    GraphicsEntry * showPlace(const Place &,bool newTab,bool activate);
    void setStatus(const QString &);
      void enableKeymaps(bool);
    //    void saveNote(Note *);
      void importXml(const QString & filename);

      void onSavePageSet();
      void onLoadPageSet();
      void onEditPageSet();
    private slots:
      void onMessageTimeout();
      void onAllowDuplicates();
      void onFocusContent();
      void onTabList();
      void onOptionsChanged(bool);
      void onReady();
      void onLogViewer();
      void onLinkChanged();
      void onXrefMode();
      void testSlot();
      void sync();
      void showNoteBrowser();
      void onNotesDeleted(const QStringList &);
      void shortcut(const QString &);
      void localSearch();
      void localSearchNext();
      void localSearchClear();
      void localSearchShow();
      void tabsChanged();

      void reloadEntry(const QString &,const QString &);
      void revertEntry();
      void findNextRoot(const QString &);
      void findPrevRoot(const QString &);

      void moveNext(const Place &);
      void movePrevious(const Place &);
      void movePreviousHead(const Place &);

      void onCloseTab(int) ;
      void onCloseOtherTabs(int) ;
      void onDuplicateTab(int);


      void onGoToPage(const Place &);
      void onExit();
      void onTest();

      void onShowContents();
      void onShowToolbar();
      void onShowMenubar();
      void onShowStatusbar();
      void onNavigationMenuChanged(QAction *);       // change navigation by menu
      void onNavNext();
      void onNavPrev();
      void onNavFirst();
      void onNavLast();
      void onNextRoot();
      void onPrevRoot();
      void onFirstRoot();
      void onLastRoot();
      void onNextPage();
      void onPrevPage();
      void onFirstPage();
      void onLastPage();

      void onDefaultScale();
      void onDefaultWidth();

      void onSelectTheme();
      void onEditTheme();
      void onCreateTheme();

      void searchForText();
      void searchForPage();
      void searchForNode();
      void searchForRoot();
      void searchForEntry();

      void onClearHistory();
      void onShowHistory();
      void onHistorySelection();

      void onKeymapChanged();
      void onKeymapHelp(const QString & map = QString());

      void onEditView();

      void showHistory(const Place &p);

      void onOptions();
      void showHelp(const QString &,const QString & = QString());
      void onHelpLoaded(bool);
      void onSetInterface(bool triggered=false);
      void treeItemDoubleClicked(QTreeWidgetItem * , int);
      void entryActivated(QTreeWidgetItem * , int);
      void focusItemChanged(QGraphicsItem *, QGraphicsItem *, Qt::FocusReason);

      void rootChanged(const QString & root,const QString & node);
      void placeChanged(const Place &);
      void historyPositionChanged(int);
      void historyAddition(const Place &);

      void bookmarkShortcut(const QString &);
      void bookmarkInput();
      void bookmarkAdd(const QString & id,const Place & p);


      void onDocs();
      void onAbout();
      void onImportXml();
      void bookmarkRebuildMenu();
      void addShortcut(const QString & name,const QString & key,bool update = false);
      void currentTabChanged(int ix = -1);
      /// these apply to the current graphicsentry
      void pageZoomIn();
      void pageZoomOut();
      void pageWiden();
      void pageNarrow();
      void pagePrint();
      void pageSearch();
      void pageClear();
      void pageSearchComplete();
      void pageSearchStart();
      /// from FullSearchWidget
      //      void showSearchNode(const QString &);
      void convertToEntry();
      void printNode(const QString &);
      void printNodeView(GraphicsEntry *,const QString &);
      void syncFromContents();
      void syncFromEntry();

      void onDeleteTheme();
      void onChangeArabicFont();
      void onExportLinks();
      void onImportLinks();
      void showSearchNode(const QString &,bool forceNewTab = false);

 protected:
      void closeEvent(QCloseEvent *);
 private:
      int addTab(bool create,QWidget * ,const QString & );
      bool deleteWidget(QWidget * );
      void showStartupEntry();
      void printCurrentPage(const QString & node = QString());
      void restoreSavedState();
      void cleanup();
      void search(int searchType,ArabicSearchDialog *,const QString &);
      //      Place showPlace(const Place &,int);
      void closeAllTabs();
      void updateStatusBar(const Place & p = Place());
      void updateMenu();
      int searchTabs(const QString & node,bool activate = false);
      int searchTabs(const Place & ,bool activate = false);
      void restoreBookmarks();
      void setupBookmarkShortcuts();
      void bookmarkJump(const QString & id,bool,bool);
      void setupShortcuts();
      int m_historyPos;
      void bookmarkClear();
      void setSignals(GraphicsEntry *);
      void loadStyleSheet();
      void createActions();
      void setIcon(QAction *,const QString &,const QString &);
      void setIcons(const QString & theme = QString());
      void createToolBar();
      Place setupHistory(int startPos = -1);
      void createMenus();
      void createStatusBar();
      bool deleteTheme(const QString &) ;

      bool m_keymapsEnabled;
      bool eventFilter(QObject * target, QEvent *);
      bool openDatabase(const QString &);
      void getFirstAndLast();
      void restoreTabs();
      Place getCurrentPlace();
      int getSearchCount();
      void addBookmarkMenuItem(const QString & id);

      void enableForPage(bool);

      bool removeDirectory(const QString &);
      void activateTheme(const QString &);

      bool copyRecursively(const QString &,const QString &,int *);

      //    QAction * createIconAction(const QString imgdir,const QString & iconfile,const QString & text);
      /// look through all tabs for the given node, -1 if not found, else tab index

      //      SearchOptions m_defaultSearchOptions;
      //      SearchOptions m_searchOptions;

      QString m_configFile;
      QString m_interface;    // "default","minimal"
      QString m_applicationCssFile;
      QString m_printPdfLocation;
      QString m_currentTheme;

      QString m_currentMap;
      QString m_dbName;
      QString m_notesDbName;
      QString m_historyDbName;
      QString m_startupNode;
      QString m_startupRoot;
      QString m_firstRoot;
      QString m_lastRoot;

      QString m_historyMenuFormat;
      QString m_bookmarkMenuFormat;
      QPrinter m_printer;

      NavigationMode m_navMode;
      int m_firstPage;
      int m_lastPage;
      int m_messageInterval;

      QLabel * m_placeIndicator;
      QLabel * m_statusMessage;
      QTimer * m_messageTimer;

      ArabicSearchDialog * m_rootSearchDialog;
      ArabicSearchDialog * m_wordSearchDialog;
      ArabicSearchDialog * m_headSearchDialog;
      NodeSearchDialog   * m_nodeSearchDialog;
      PageSearchDialog   * m_pageSearchDialog;

      bool m_allowNavMode;
      bool m_toolbarText;
      bool m_linkContents;
      bool m_printToPdf;
      bool m_printerReUse;
      bool m_ok;
      bool m_docked;          // whether to use docked widget
      bool m_valgrind;
      bool m_useNotes;
      bool m_historyEnabled;
      bool m_saveTabs;
      bool m_saveSettings;
      bool m_restoreTabs;
      bool m_searchNewTab;
      bool m_searchSwitchTab;
      bool m_interfaceWarning;
      bool m_saveBookmarks;
      bool m_restoreBookmarks;
      bool m_revertEnabled;
      bool m_menuWarning;
      bool m_allowDuplicates;
      bool m_allowDuplicatesPermanent;
      bool m_historyNewTab;
      bool m_historyGoTab;
      bool m_rootActivateTab;
      bool m_searchAgainDefault;     // true = Yes, false = No
      QToolBar * m_mainbar;
      QToolBar * m_navigation;
      QToolBar * m_entrybar;

      //      QFont arFont;
      QFont m_historyMenuFont;

      InputMapper * m_mapper;

      QMap<QString,QString> m_maps;
      QMap<QString,QString> m_mapHelp;

      QSignalMapper * m_shortcutMap;
      QSignalMapper * m_bookmarkMap;

      QMap<QString, Place> m_bookmarks;

      QMenu * m_bookmarkMenu;
      QMenu * m_historyMenu;
      QMenu * m_fileMenu;
      QMenu * m_searchMenu;
      QMenu * m_navigationModeMenu;
      QMenu * m_moveMenu;
      QMenu * m_viewMenu;
      QMenu * m_pageMenu;
      QMenu * m_toolMenu;
      QMenu * m_helpMenu;
      QMenu * m_themeMenu;
      QMenu * m_pageSetMenu;
      AppMenu * m_mainmenu;

      QSize m_toolbarIconSize;

      Place m_place;

      ContentsWidget * m_tree;
      TabWidget * m_tabs;
      QDockWidget * m_treeDock;
      EditView * m_editView;
      LogViewer * m_logview;
      HelpView * m_helpview;


      QSqlDatabase m_db;
      QSqlDatabase m_notesDb;

      NewTabBehaviour m_tabStyle;

      QToolButton * m_bookmarkButton;
      QToolButton * m_exitButton;
      QToolButton * m_searchButton;
      QToolButton * m_docButton;
      QToolButton * m_aboutButton;
      QToolButton * m_optionsButton;
      QToolButton * m_logButton;
      QToolButton * m_zoomInButton;
      QToolButton * m_zoomOutButton;
      QToolButton * m_widenButton;
      QToolButton * m_narrowButton;
      QToolButton * m_printButton;
      QToolButton * m_localSearchButton;
      QToolButton * m_localSearchNextButton;
      QToolButton * m_clearButton;
      QToolButton * m_syncLeftButton;
      QToolButton * m_syncRightButton;
      QToolButton * m_navFirstButton;
      QToolButton * m_navLastButton;
      QToolButton * m_navNextButton;
      QToolButton * m_navPrevButton;
      QToolButton * m_navigationButton;
      QToolButton * m_historyButton;
      QToolButton * m_keymapsButton;
      QToolButton * m_linkButton;

      QActionGroup * m_moveGroup;


      QAction * m_tablistAction;
      QAction * m_minimalAction;
      QAction * m_logViewerAction;
      QAction * m_optionsAction;
      QAction * m_bookmarkListAction;
      QAction * m_bookmarkAddAction;
      QAction * m_bookmarkJumpAction;
      QAction * m_bookmarkClearAction;
      QAction * m_bookmarkRevertAction;
      QAction * m_docAction;
      QAction * m_exitAction;
      QAction * m_testAction;
      QAction * m_editViewAction;
      QAction * m_historyAction;
      QAction * m_bookmarkAction;
      QAction * m_searchAction;
      QAction * m_searchWordAction;
      QAction * m_textSearchAction;
      QAction * m_searchPageAction;
      QAction * m_searchRootAction;
      QAction * m_searchNodeAction;
      QAction * m_searchEntryAction;
      QAction * m_navNextAction;
      QAction * m_navPrevAction;
      QAction * m_navFirstAction;
      QAction * m_navLastAction;
      QAction * m_navModeRootAction;
      QAction * m_navModePageAction;
      QAction * m_clearHistoryAction;
      QAction * m_showHistoryAction;
      QAction * m_zoomInAction;
      QAction * m_zoomOutAction;
      QAction * m_widenAction;
      QAction * m_narrowAction;
      QAction * m_printAction;
      QAction * m_localSearchAction;
      QAction * m_localSearchNextAction;
      QAction * m_clearAction;                     // clear the highlights
      QAction * m_convertToEntryAction;      // convert HeadSearch to GraphicsEntry
      QAction * m_keymapsAction;
      QAction * m_linkAction;
      QAction * m_syncFromContentsAction;
      QAction * m_syncFromEntryAction;
      QAction * m_aboutAction;
      QAction * m_defaultScaleAction;
      QAction * m_defaultWidthAction;
      QAction * m_selectThemeAction;
      QAction * m_editThemeAction;
      QAction * m_deleteThemeAction;
      QAction * m_createThemeAction;
      QAction * m_changeArabicFontAction;
      QAction * m_showContentsAction;
      QAction * m_showMenuAction;
      QAction * m_showStatusBarAction;
      QAction * m_showToolbarAction;
      QAction * m_exportLinksAction;
      QAction * m_importLinksAction;
      QAction * m_importXmlAction;
      QAction * m_loadPageSetAction;
      QAction * m_savePageSetAction;
      QAction * m_editPageSetAction;
      QAction * m_showNoteBrowserAction;
      HistoryMaster * m_history;
      NoteMaster * m_notes;

      QShortcut * m_helpRequested;
      QMap<QString,QString> m_siteMap;
      ContentsPanel * m_contentsPanel;
 signals:
      void nodeActivated(const QString & node,const QString & word);
};
#endif

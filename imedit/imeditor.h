#ifndef _IMEDITOR_H_
#define _IMEDITOR_H_
#include <QApplication>
#include <QMainWindow>
#include <QTabWidget>
#include <QTableWidget>
#include <QGridLayout>
#include <QFormLayout>
#include <QToolBar>
#include <QStatusBar>
#include <QMenu>
#include <QMenuBar>
#include <QAction>
#include <QStatusBar>
#include <QDebug>
#include <QLineEdit>
#include <QSqlDatabase>
#include <QTextEdit>
#include <QTextStream>
#include <QStackedLayout>
#include <QSplitter>
#include <QVBoxLayout>
#include <QPushButton>
#include <QSettings>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QShortcut>
#include <QKeySequence>
#include <QGraphicsItem>
#include <QLabel>
#include <QFile>
#include <QFontDialog>
#include <QSignalMapper>
#include <QTextDocumentWriter>
#include <QFileDialog>
#include <QPrinter>
#include <QPrintDialog>
#include <QMessageBox>
#include <QPrinter>
#include <QPrintDialog>
#include <QPrintPreviewDialog>
#include <QPageSetupDialog>
#include <QTextBlock>
#include <QTextBlockFormat>
#include <QPrinterInfo>
#include <QInputDialog>
#include <QComboBox>
class ImEdit;
class ShowMapWidget;
//class EditInfoWidget;
class ImEditor : public QWidget {
  Q_OBJECT

 public:
  ImEditor(QWidget * parent = 0);
  ImEdit * edit() { return m_edit;}
  QAction * saveAction() { return m_saveAction; }

  ~ImEditor();
  public slots:
    void hideHelp();
    void hideMaps(bool hide = false);
    void hidePrint(bool hide = false);
    void onInsertChar();
    void logMessage(const QString &);
    void onPrinterSetup();
    void onOptions();
    void onMap(const QString &);
    void onFont();
    void onOpen();
    void onImport();
    void onSave();
    void onSaveAs();
    void onPrint();
    void onPrinters();
    void charInserted(int,int);
    void onInsertBlock();
    void scan();
    void cursorPositionChanged();
    void onMapHelp();
    void onShowHelp(const QString & map = QString());
    void onChangeMap(const QString &);
 private:
    void createActions();
    void setCharFormat(const QString & map = QString());
    void createToolbar();
    void save(const QString & fileName);
    ImEdit * m_edit;
    QToolBar * m_toolbar;
    void readSettings();
    QAction * m_fontAction;
    QAction * m_openAction;
    QAction * m_importAction;
    QAction * m_saveAction;
    QAction * m_saveAsAction;
    QAction * m_printAction;
    QAction * m_optionsAction;
    QAction * m_printOptionsAction;
    QAction * m_insertBlockAction;
    QAction * m_insertCharAction;
    QAction * m_mapBoxAction;
    QAction * m_mapHelpAction;
    QSignalMapper * m_mapSignals;
    QStringList m_formats;
    bool m_autoSaveSettings;
    bool m_interactiveFont;
    bool m_toolbarVisible;
    bool m_pdfPrint;
    QString m_documentFileName;
    QString m_currentScript;
    QString m_mapOffValue;
    QPrinter * m_printer;
    QComboBox * m_mapBox;
    QComboBox * m_helpBox;
    QList<QShortcut *>  m_shortcuts;
    QMap<QString,QString> m_scriptFonts;
    QMap<QString,QString> m_mapToScript;
    QMap<QString,QString> m_mapHelp;
 signals:
    void save();
    void editFocus(int);
};
#endif

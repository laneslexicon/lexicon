#ifndef __NODEVIEW_H__
#define __NODEVIEW_H__
#include <QDialog>
#include <QString>
#include <QTextBrowser>
#include <QVBoxLayout>
#include <QDialogButtonBox>

class NodeView : public QDialog {
  Q_OBJECT

 public:
  NodeView(const QString & html,QWidget * parent = 0);
  public slots:
    void accept();
    void reject();
 private:
  QTextBrowser * m_browser;
};
#endif

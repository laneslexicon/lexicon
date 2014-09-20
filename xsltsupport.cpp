#include "xsltsupport.h"
#include "QsLog.h"
#ifdef USE_XALAN
static XalanTransformer * m_xalan;
const XalanCompiledStylesheet * m_compiledXsl;

int compileStylesheet(const QString & xsl) {
  if (m_compiledXsl == 0) {
    std::istringstream iss(xsl.toStdString());
    int r = m_xalan->compileStylesheet(xsl.toLocal8Bit().data(),m_compiledXsl);
    return r;
  }
  return 0;
}
const char * getCompileError() {
  return m_xalan->getLastError();
}
void initXslt() {
  static bool m_init = false;
  if (! m_init){
    XMLPlatformUtils::Initialize();
    XalanTransformer::initialize();
    m_init = true;
    m_xalan = new XalanTransformer;
  }
}
QString xsltTransform(const QString & xml) {
  std::istringstream iss(xml.toStdString());
  std::stringstream ostream;
  m_xalan->transform(iss,m_compiledXsl,ostream);
  return QString::fromStdString(ostream.str());
}

void freeXslt() {
  /// TODO free xalan code
}
#endif
#ifdef USE_LIBXSLT
static xsltStylesheetPtr cur;
static xsltStylesheetPtr curNode;
static xsltStylesheetPtr curTest = NULL;
xmlDocPtr xsltTest = NULL;
QStringList xmlParseErrors;
void initXslt() {
   static bool m_init = false;
   if (! m_init) {
      xmlSubstituteEntitiesDefault(1);
      xmlLoadExtDtdDefaultValue = 1;
      xmlGenericErrorFunc handler = (xmlGenericErrorFunc)parseErrorHandler;
      initGenericErrorDefaultFunc(&handler);
   }
}
/**
 * this is called every time, but only actually compiled once
 *
 * @type  0   use cur as the stylesheet (usually entry.xslt)
 *        1   use curNode as the stylesheet (usually node.xslt)
 *        2   reload stylesheet cur (ie reload entry.xslt)
 *        3   compile stylesheet string to curTest
 * @xsl   the name of the stylesheet file when type = 0,1,2
 *        the actual stylesheet when type = 3
 *
 * @return 0 if ok, otherwise the number of errors
 */
int compileStylesheet(int type,const QString & xsl) {
  switch(type) {
  case ENTRY_XSLT: {
    if (cur == 0) {
      /// if errors in xslt they will be xmlParseErrors
      cur = xsltParseStylesheetFile((const xmlChar *)xsl.toUtf8().data());
      return xmlParseErrors.size();
    }
   return 0;
  }
  case NODE_XSLT: {
   if (curNode == 0) {
      /// if errors in xslt they will be xmlParseErrors
      curNode = xsltParseStylesheetFile((const xmlChar *)xsl.toUtf8().data());
      return xmlParseErrors.size();
   }
   return 0;
  }
  case ENTRY_XSLT_RECOMPILE : {
    if (cur != 0) {
      xsltFreeStylesheet(cur);
    }
    /// if errors in xslt they will be xmlParseErrors
    cur = xsltParseStylesheetFile((const xmlChar *)xsl.toUtf8().data());
    return xmlParseErrors.size();
  }
  case TEST_XSLT: {
    if (curTest != 0) {
      xsltFreeStylesheet(curTest);
    }
    QByteArray ar = xsl.toUtf8();
    xsltTest = xmlParseMemory((const char *)ar.data(), ar.size());
    curTest = xsltParseStylesheetDoc(xsltTest);
    //    xmlFreeDoc(xsltTest);
    return xmlParseErrors.size();
  }
  default : {
    QLOG_WARN() << Q_FUNC_INFO << QObject::tr("compile of unknown type requested") << type;
    break;
  }
  }
  return 0;
}

QString xsltTransform(int type,const QString & xml) {
  QString result;
  xmlDocPtr doc, res;
  const char *params[16 + 1];
  memset(params,0x00,sizeof(params));
  QByteArray ba = xml.toUtf8();
  doc = xmlParseMemory(ba.data(),ba.size());
  /// if errors in xml they will be xmlParseErrors
  if (doc == 0) {
     return QString();
  }

  xmlChar * buf;
  int sz;

  switch(type) {
  case ENTRY_XSLT:
  case ENTRY_XSLT_RECOMPILE: {
    res = xsltApplyStylesheet(cur, doc, params);
    xsltSaveResultToString(&buf,&sz, res,cur);
    break;
  }
  case NODE_XSLT:{
    res = xsltApplyStylesheet(curNode, doc, params);
    xsltSaveResultToString(&buf,&sz, res,curNode);
    break;
  }
  case TEST_XSLT: {
    res = xsltApplyStylesheet(curTest, doc, params);
    xsltSaveResultToString(&buf,&sz, res,curTest);
    break;
  }
  default : {
    QLOG_WARN() << Q_FUNC_INFO << QObject::tr("transform of unknown type requested") << type;
    break;
  }
  }
//  xsltSaveResultToFile(stdout, res, cur);

  xmlFreeDoc(res);
  xmlFreeDoc(doc);
  QString html = QString((char *) buf);
  /// this causes link errors in Window
  //  xmlFree(buf);
  // the link below recommends replacing xmlFree with free and explains why it is ok
  //https://stackoverflow.com/questions/4984853/mingw-libxml2-issue
  free(buf);
  return html;
}
void freeXslt() {
  xsltFreeStylesheet(cur);
  xsltFreeStylesheet(curNode);
  xsltFreeStylesheet(curTest);
  xsltCleanupGlobals();
  xmlCleanupParser();
}
#define TMP_BUF_SIZE 512
void parseErrorHandler(void * /* ctx*/, const char *msg, ...) {
  static QStringList errors;
  char string[TMP_BUF_SIZE];
   va_list arg_ptr;

   va_start(arg_ptr, msg);
   vsnprintf(string, TMP_BUF_SIZE, msg, arg_ptr);
   va_end(arg_ptr);
   QString s = string;
   xmlParseErrors << s.trimmed();
  return;
}
QStringList getParseErrors() {
  return xmlParseErrors;
}
void clearParseErrors() {
  xmlParseErrors.clear();
}
#endif
/**
   xsl - name of xslt file
*/
/*
  QString xalanTransform(const QString & xsl,const QString & xml,QMap<QString,QString> & params) {
  std::istringstream iss(xml.toStdString());
  //  std::cout << xml.toStdString();
  std::string ss;
  //  iss >> ss;
  //  std::cout  << ss;
  std::stringstream ostream;
  QByteArray ba = xsl.toLocal8Bit();
  XalanTransformer * xalan;
  xalan =  getXalan();
  QMapIterator<QString,QString> i(params);

  while(i.hasNext()) {
  i.next();
  QByteArray ka = i.key().toLocal8Bit();
  QByteArray va = i.value().toLocal8Bit();
  xalan->setStylesheetParam(ka.data(), va.data());
  }
  int theResult =
  //    x.transform("../tmp/test.xml","../xslt/tei.xsl",xmlOut)
  // x.transform("../tmp/test.xml","../xslt/tei.xsl",ostream);
  xalan->transform(iss,ba.data(),ostream);
  // x.transform(xml.toStdString(),"../xslt/tei.xsl",ostream);
  //  std::cout << ostream.str();
  QString t;
  t = QString::fromStdString(ostream.str());
  return t;
  }
*/
/*
  const XalanDOMString key("param1");
  const XalanDOMString expression("'Hello World'");
  theXalanTransformer.setStylesheetParam(key, expression);
*/

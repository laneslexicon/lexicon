#include "xsltsupport.h"
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
int compileStylesheet(const QString & xsl) {
   if (cur == 0) {
      /// if errors in xslt they will be xmlParseErrors
      cur = xsltParseStylesheetFile((const xmlChar *)xsl.toLocal8Bit().data());
   }
   return 0;
}
QString xsltTransform(const QString & xml) {
  QString result;
  xmlDocPtr doc, res;
  const char *params[16 + 1];
  memset(params,0x00,sizeof(params));
  QByteArray ba = xml.toLocal8Bit();
  doc = xmlParseMemory(ba.data(),ba.size());
  /// if errors in xml they will be xmlParseErrors
  if (doc == 0) {
     return QString();
  }
  res = xsltApplyStylesheet(cur, doc, params);
  xmlChar * buf;
  int sz;
  xsltSaveResultToString(&buf,&sz, res,cur);
//  xsltSaveResultToFile(stdout, res, cur);

  xmlFreeDoc(res);
  xmlFreeDoc(doc);
  return QString((char *) buf);
}
void freeXslt() {
  xsltFreeStylesheet(cur);
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
   qDebug() << s.trimmed();
   xmlParseErrors << s.trimmed();
  return;
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

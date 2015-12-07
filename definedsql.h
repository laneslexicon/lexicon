#ifndef _DEFINEDSQL_H_
#define _DEFINEDSQL_H_
#define SQL_FIRST_ROOT "select root,page from entry where datasource = 1 and type = 0 order by nodenum limit 5;"
#define SQL_LAST_ROOT "select root,page from entry where datasource = 1 and type = 0 order by nodenum desc limit 5;"
#define SQL_PAGE_FIND "select root,nodeid from entry where page = %1 order by nodenum asc"
#define SQL_LINKTO_NODE "select tonode from links where orthid = ?"
#define SQL_LINK_ROOT_FIND "select word from root where bword = ?"
#define SQL_FIND_ENTRY_BY_NODE "select * from entry where datasource = 1 and nodeid = ?"
#define SQL_FIND_ENTRY_FOR_ROOT "select root,broot,word,bword,xml,page,itype,nodeid,supplement,headword from entry where datasource = 1  and root = ? order by nodenum"
#define SQL_QUASI_FIND_BY_WORD "select quasi from root where word = ? and datasource = 1"
#define SQL_ROOT_FOR_PAGE "select root,broot,word,bword,xml,page,itype,nodeid,supplement from entry where datasource = 1 and page = ? order by nodenum asc"
#define SQL_FIND_VERSION "select createversion,createdate,xmlversion,dbid from lexicon"

#define SQL_FIND_NODE_FOR_LINK "select id,fromnode,tonode,link from links where datasource = 1 and orthid = ?"
#define SQL_UPDATE_LINK_TO_NODE "update links set tonode = ?, matchtype = 100 where orthid = ?"
#define SQL_GET_XML_FOR_NODE "select xml from entry where nodeid = ?"
#define SQL_ENTRY_FOR_NODE "select * from entry where nodeid = ?"
#define SQL_UPDATE_XML_FOR_NODE "update entry set xml = ? where nodeid = ?"
/*
  contentswidget.cpp
 */
#define SQL_FIND_ENTRIES_FOR_ROOT "select word,itype,bword,nodeid,supplement,headword from entry where datasource = 1 and root = ? order by nodenum asc"

#define SQL_FIND_LETTERS "select distinct letter from root where datasource = 1 order by letter"
#define SQL_FIND_WORDS_FOR_ROOT "select word,supplement from root where letter = ? and datasource = 1 order by word,supplement"

/* fullsearch.cpp */

#define SQL_FIND_XREF_ENTRIES "select id,word,root,entry,node,nodenum,page from xref where datasource = 1 order by nodenum asc"

#define SQL_FIND_ENTRY_DETAILS "select root,word,xml,page from entry where datasource = 1 and nodeid = ?"
#define SQL_FIND_MAXIMUM "select id from %1 order by id desc limit 1"

#define SQL_REGEX_FIND_ENTRY_DETAILS "select root,word,nodeid,xml,page from entry where datasource = 1 order by nodenum asc"

/* headsearch.cpp */
#define SQL_FIND_ENTRY_HEADWORD "select id,word,root,nodeid,nodenum,headword,page from entry where datasource = 1 order by nodenum asc"

/* history */

#define SQL_ADD_HISTORY "insert into history (datasource,node,word,root,supplement,page,vol,timewhen) values (1,?,?,?,?,?,?,?)"
#define SQL_PREV_HISTORY "select %1 from history where id <= ? order by id desc"
#define SQL_NEXT_HISTORY "select %1 from history where id > ? order by id asc"
#define SQL_LAST_HISTORY "select %1 from history where id = (select max(id) from history)"
#define SQL_FIRST_HISTORY "select %1 from history order by id asc limit 1"
#define SQL_LIST_HISTORY "select %1 from history order by id desc limit %2"
#define SQL_GET_HISTORY "select %1 from history where id = ?"
#define SQL_GET_MAX_HISTORY "select max(id) from history"
#define SQL_DELETE_HISTORY "delete from history"

/* about.cpp */
#define SQL_GET_INFO "select sourceid,description,createversion,createdate,xmlversion,dbid from lexicon where sourceid = 1"

/* notebrowser.cpp */
#define SQL_LIST_NOTES "select id,word,subject,created,amended,substr(note,1,%1),place from notes"
#define SQL_GET_NODE_FOR_NOTE "select root,word,xml from entry where datasource = 1 and nodeid = ?"
#define SQL_GET_WORD_FOR_NOTE "select root,word,xml from entry where datasource = 1 and word = ?"

/* notes.cpp */

#define SQL_GET_NOTES_FOR_WORD "select id,word,place,subject,note,created,amended,type from notes where word = ?"
#define SQL_GET_NOTES_FOR_NODE "select id,word,place,subject,note,created,amended,type,node from notes where node = ?"
#define SQL_GET_NOTE_BY_ID "select word,place,subject,note,created,amended,type from notes where id = ?"

#define SQL_INSERT_NOTE "insert into notes (datasource,word,place,subject,note,created,type,node) values (:datasource,:word,:place,:subject,:note,:created,:type,:node)"
#define SQL_UPDATE_NOTE "update notes set subject = ?, note = ?, amended = ? where id = ?"

/* noteswidget.cpp */
#define SQL_GET_NOTES "select id,nodeid,word,note,tag from notes"
#define SQL_UPDATE_NOTE_TEXT "update notes set note = ? where id = ?"
#define SQL_INSERT_NOTE_DETAILS "insert into notes (nodeid,word,note,tag) values (?,?,?,?)"
#define SQL_UPDATE_NOTE_TAG "update notes set tag = ? where id = ?"

#define SQL_FIXED_LINKS "select orthid,fromnode,tonode from links where (datasource = 1) and (matchtype = 100)"

#define SQL_LINK_TYPE "select * from links where orthid = ?"
#define SQL_LINK_UPDATE_STATUS "update links set status = ?,tonode = ?,note = ? where orthid = ?"
#define SQL_UPDATE_ENTRY_XML "update entry set xml = ? where nodeid = ?"
//
#define SQL_PAGESET_HEADERS "select * from pageset order by title asc"
#define SQL_PAGESET_DETAIL "select * from page where pageset = ? order by pagenum asc"
#define SQL_PAGESET_DETAIL_BY_ID "select * from page where id = ?"
#define SQL_PAGESET_PAGE_COUNT "select count(id) from page where pageset = ?"
#define SQL_PAGESET_ADD_HEADER "insert into pageset (title,accessed) values (?,?)"
#define SQL_PAGESET_ADD_PAGE "insert into page (pageset,place,userdata,pagenum) values (?,?,?,?)"
#define SQL_PAGESET_DELETE  "delete from pageset where id = ?"
#define SQL_PAGESET_DELETE_ALL_PAGES "delete from page where pageset = ?"
#define SQL_PAGESET_DELETE_PAGE "delete from page where id = ?"
#define SQL_PAGESET_FIND_ORPHANED "select pageset.id from pageset left join page on page.pageset = pageset.id where page.id is NULL"
#endif

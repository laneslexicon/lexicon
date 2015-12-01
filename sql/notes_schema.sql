CREATE TABLE notes(id integer primary key,datasource integer,word text,place text,subject text,note text,created text,amended text,data text, type int,node text);
CREATE INDEX 'notes_word' on notes (word asc);
CREATE INDEX 'notes_node' on notes (node asc);
CREATE TABLE pageset(id integer primary key, title text,accessed text);
CREATE TABLE page(id integer primary key, pageset integer,place text,pagenum integer,userdata text);

CREATE TABLE history(id integer primary key,node text,word text,root text,supplement int,page int,vol int,timewhen text, datasource integer);
CREATE TABLE notes(id integer primary key,datasource integer,type text,keyfield text,keyvalue text,note text,created text,amended text,data text);

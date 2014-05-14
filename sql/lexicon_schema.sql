CREATE TABLE root (id integer primary key,datasource integer,word text,bword text,letter text,bletter text,xml text,supplement integer,quasi integer,alternates integer,page integer);
CREATE TABLE alternate (id integer primary key,datasource integer,word text,bword text,letter text,bletter text,xml text,supplement integer,quasi integer,alternate integer);
CREATE TABLE itype (id integer primary key,datasource integer,itype integer,root text,broot text,nodeId text,word text,xml text, bareword text);
CREATE TABLE entry (id integer primary key,datasource integer,root text,broot text,word text,bword text,itype text,nodeId text,xml text,supplement integer,file text,page integer,nodenum real, bareword text);
CREATE TABLE orth (id integer primary key, datasource integer,entryid integer,form text,bform text,nodeid text,root text,broot text);
CREATE INDEX 'word_index' on entry (word asc);
CREATE INDEX 'broot_index' on entry (broot asc);
CREATE INDEX 'root_index' on entry (root asc);
CREATE TABLE xref (id INTEGER primary key,datasource integer,word TEXT,bword text,node TEXT,type INTEGER,page integer, bentry text, entry text, broot text, root text, bareword text);
CREATE INDEX 'letter_index' on root (letter asc);
CREATE INDEX 'nodenum_index' on entry (nodenum asc);
CREATE INDEX 'nodenum_index_desc' on entry (nodenum desc);
CREATE INDEX 'supp_letter_index' on root (supplement asc,letter asc);
CREATE INDEX 'supp_word_index' on root (supplement asc,word asc);
CREATE INDEX 'node_index' on entry (nodeId asc);
CREATE INDEX 'xref_bword' on xref (bword asc);
CREATE INDEX 'xref_word' on xref (word asc);
CREATE INDEX 'bareword_index' on entry (bareword asc);
CREATE INDEX page_index on entry (page asc);
CREATE TABLE source (integer id primary key, sourceid integer,description text,version integer);
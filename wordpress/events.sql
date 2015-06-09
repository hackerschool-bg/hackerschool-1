
CREATE TABLE IF NOT EXISTS events (
  id varchar(32) NOT NULL,
  name varchar(256) NOT NULL,
  description text NOT NULL,
  startTime datetime NOT NULL,
  url varchar(256) NOT NULL,
  latitude double NOT NULL,
  longitude double NOT NULL,
  city varchar(128) NOT NULL,
  region varchar(128) NOT NULL,
  country varchar(128) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

ALTER TABLE events
  ADD PRIMARY KEY (id),
  ADD FULLTEXT KEY city (city);
ALTER TABLE events
  ADD FULLTEXT KEY region (region);

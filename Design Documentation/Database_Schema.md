# Schema of Database

This is the SQLite schema of the database and is based on the Database Design (see Database_Design.md).  This is currently a **Work In Progress**!


```sqlite
CREATE TABLE Person (
  PersonID INTEGER PRIMARY KEY NOT NULL,   -- (RowID) ID
  Bio TEXT NOT NULL DEFAULT '',            -- Biography of person
  Ethnicity TEXT DEFAULT '',               -- Description of Ethnicity (or NULL if unknown)
  Sex TEXT NOT NULL DEFAULT '?' CHECK (Sex IN ('M', 'F', '?')), -- Chromosonal Sex ('M', 'F' or '?' if unknown)
  Gender TEXT DEFAULT '?',                 -- Description of Gender (or '?' if unknown), will often match sex
  Notes TEXT DEFAULT NULL,                 -- Any notes that aren't necessarily official but should nonetheless be recorded about a person
  ChildOfRelID INTEGER DEFAULT NULL REFERENCES Relationship (RelID), -- Biological child of this relationship (or NULL if unknown)
  IsAdopted INTEGER DEFAULT NULL CHECK (IsAdopted IN (0, 1, NULL)),  -- Specifies that the person was adopted.  Not required as this data is also included in the AdoptFoster table
  BirthDate INTEGER NOT NULL REFERENCES Date (DateID), -- Date of Birth
  BirthPlace TEXT NOT NULL DEFAULT '?',    -- Birth Place Name (or '?' if unknown)
  BirthAddrID INTEGER DEFAULT NULL REFERENCES Address (AddrID), -- Birth Place Address ID (or NULL if not applicable, linked value is '?' if unknown)
  DeathDate INTEGER DEFAULT NULL REFERENCES Date (DateID), -- Death Date (or NULL if still alive)
  DeathPlace TEXT DEFAULT NULL,            -- Death Place Name (or NULL if still alive, '?' if unknown)
  DeathAddrID INTEGER DEFAULT NULL REFERENCES Address (AddrID), -- Death Place Address ID (or NULL if not applicable or still alive, Linked value is '?' if unknown)
  DeathAge INTEGER DEFAULT NULL REFERENCES Age (AgeID), -- Age at Death (or NULL if still alive, linked value is NULL:NULL:NULL if unknown)
  SourceID INTEGER DEFAULT NULL REFERENCES Source (SourceID) -- Source (or NULL if none)
);


```
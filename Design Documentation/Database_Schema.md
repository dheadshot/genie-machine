# Schema of Database

This is the SQLite schema of the database and is based on the Database Design (see Database_Design.md).  This is currently a **Work In Progress**!

## Table Definitions

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

CREATE TABLE Date (
  DateID INTEGER PRIMARY KEY NOT NULL,     -- (RowID) ID
  DateTypeID TEXT DEFAULT 'AT' NOT NULL REFERENCES DateType (DateTypeID), -- Type of Date.  If 'BET', use '2' fields as well
  InclusiveDate1 TEXT DEFAULT (CASE WHEN DateTypeID IS 'AT' THEN NULL ELSE 'N' END) CHECK ((InclusiveDate1 = NULL and DateTypeID = 'AT') or InclusiveDate1 = 0 or InclusiveDate1 = 1), -- Does the date range include the date specified, Yes or No?  (NULL for "At" dates)
  Year1 TEXT NOT NULL DEFAULT '????',      -- At least 4 digits, replace any digit with '?' if unknown (e.g. 194? for between 1940 and 1949)
  Month1 INTEGER DEFAULT NULL,             -- Month number or NULL if unknown
  Day1 INTEGER DEFAULT NULL,               -- Day number or NULL if unknown
  Cal1TypeID INTEGER NOT NULL DEFAULT 1 REFERENCES CalendarType (CalTypeID), -- Type of Calendar specified
  Comment1 TEXT DEFAULT '',                -- Comment on the Date
  Source1ID INTEGER DEFAULT NULL REFERENCES Source (SourceID), -- Source for Date 1 (or NULL if none)
  InclusiveDate2 TEXT DEFAULT (CASE WHEN DateTypeID IS NOT 'BET' THEN NULL ELSE 'N' END) CHECK ((InclusiveDate2 = NULL and DateTypeID IS NOT 'BET') or (DateTypeID = 'BET' and (InclusiveDate2 = 0 or InclusiveDate2 = 1))), -- Only used if DateTypeID is 'BET', otherwise NULL, Does the date range include the date specified, Yes or No?  (NULL also for "At" dates)
  Year2 TEXT DEFAULT (CASE WHEN DateTypeID IS NOT 'BET' THEN NULL ELSE '????' END) CHECK (Year2 ISNULL or DateTypeID = 'BET'), -- Only used if DateTypeID is 'BET', otherwise NULL, At least 4 digits, replace any digit with '?' if unknown (e.g. 194? for between 1940 and 1949)
  Month2 INTEGER DEFAULT NULL CHECK (Month2 ISNULL or DateTypeID = 'BET'), -- Only used if DateTypeID is 'BET', otherwise NULL, Month number or NULL also if unknown
  Day2 INTEGER DEFAULT NULL CHECK (Day2 ISNULL or DateTypeID = 'BET'), -- Only used if DateTypeID is 'BET', otherwise NULL, Day number or NULL also if unknown
  Cal2TypeID INTEGER DEFAULT (CASE WHEN DateTypeID IS NOT 'BET' THEN NULL ELSE 1 END) REFERENCES CalendarType (CalTypeID) CHECK (Cal2TypeID ISNULL or DateTypeID = 'BET'), -- Only used if DateTypeID is 'BET', otherwise NULL, Type of Calendar specified
  Comment2 TEXT DEFAULT (CASE WHEN DateTypeID IS NOT 'BET' THEN NULL ELSE '' END), -- Only used if DateTypeID is 'BET', otherwise NULL, Comment on the Date
  Source2ID INTEGER DEFAULT NULL REFERENCES Source (SourceID) CHECK (Source2ID ISNULL or DateTypeID = 'BET') -- Only used if DateTypeID is 'BET', otherwise NULL, Source for Date 2 (or NULL also if none)
);



```

## Values

```sqlite

```
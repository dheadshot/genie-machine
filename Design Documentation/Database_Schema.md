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
  InclusiveDate1 TEXT DEFAULT (CASE DateTypeID WHEN 'AT' THEN NULL WHEN 'ABT' THEN NULL ELSE 'N' END) CHECK ((InclusiveDate1 ISNULL and (DateTypeID = 'AT' or DateTypeID = 'ABT')) or InclusiveDate1 = 'N' or InclusiveDate1 = 'Y'), -- Does the date range include the date specified, Yes or No?  (NULL for "At" dates)
  Year1 TEXT NOT NULL DEFAULT '????',      -- At least 4 digits, replace any digit with '?' if unknown (e.g. 194? for between 1940 and 1949)
  Month1 INTEGER DEFAULT NULL,             -- Month number or NULL if unknown
  Day1 INTEGER DEFAULT NULL,               -- Day number or NULL if unknown
  Cal1TypeID INTEGER NOT NULL DEFAULT 1 REFERENCES CalendarType (CalTypeID), -- Type of Calendar specified
  Comment1 TEXT DEFAULT '',                -- Comment on the Date
  Source1ID INTEGER DEFAULT NULL REFERENCES Source (SourceID), -- Source for Date 1 (or NULL if none)
  InclusiveDate2 TEXT DEFAULT (CASE WHEN DateTypeID IS NOT 'BET' THEN NULL ELSE 'N' END) CHECK ((InclusiveDate2 = NULL and DateTypeID IS NOT 'BET') or (DateTypeID = 'BET' and (InclusiveDate2 = 'N' or InclusiveDate2 = 'Y'))), -- Only used if DateTypeID is 'BET', otherwise NULL, Does the date range include the date specified, Yes or No?  (NULL also for "At" dates)
  Year2 TEXT DEFAULT (CASE WHEN DateTypeID IS NOT 'BET' THEN NULL ELSE '????' END) CHECK (Year2 ISNULL or DateTypeID = 'BET'), -- Only used if DateTypeID is 'BET', otherwise NULL, At least 4 digits, replace any digit with '?' if unknown (e.g. 194? for between 1940 and 1949)
  Month2 INTEGER DEFAULT NULL CHECK (Month2 ISNULL or DateTypeID = 'BET'), -- Only used if DateTypeID is 'BET', otherwise NULL, Month number or NULL also if unknown
  Day2 INTEGER DEFAULT NULL CHECK (Day2 ISNULL or DateTypeID = 'BET'), -- Only used if DateTypeID is 'BET', otherwise NULL, Day number or NULL also if unknown
  Cal2TypeID INTEGER DEFAULT (CASE WHEN DateTypeID IS NOT 'BET' THEN NULL ELSE 1 END) REFERENCES CalendarType (CalTypeID) CHECK (Cal2TypeID ISNULL or DateTypeID = 'BET'), -- Only used if DateTypeID is 'BET', otherwise NULL, Type of Calendar specified
  Comment2 TEXT DEFAULT (CASE WHEN DateTypeID IS NOT 'BET' THEN NULL ELSE '' END), -- Only used if DateTypeID is 'BET', otherwise NULL, Comment on the Date
  Source2ID INTEGER DEFAULT NULL REFERENCES Source (SourceID) CHECK (Source2ID ISNULL or (DateTypeID = 'BET' and Source2ID ISNULL) or (DateTypeID = 'BET' and Source2ID IS NOT NULL)) -- Only used if DateTypeID is 'BET', otherwise NULL, Source for Date 2 (or NULL also if none)
);

CREATE TABLE DateType (
  DateTypeID TEXT PRIMARY KEY NOT NULL,    -- Indexes the types
  DateTypeTxt TEXT NOT NULL UNIQUE DEFAULT '' -- Full date types
);

CREATE TABLE CalendarType (
  CalTypeID INTEGER PRIMARY KEY NOT NULL,  -- (RowID) Index of the types
  CalTypeName TEXT NOT NULL UNIQUE DEFAULT '' -- Names of the types
);

CREATE TABLE Age (
  AgeID INTEGER PRIMARY KEY NOT NULL,      -- (RowID) ID
  AgeTypeID TEXT NOT NULL DEFAULT 'AT' REFERENCES AgeType (AgeTypeID), -- Type of Age.  If 'BET', use '2' fields as well
  InclusiveAge1 TEXT DEFAULT (CASE AgeTypeID WHEN 'AT' THEN NULL WHEN 'ABT' THEN NULL ELSE 'Y' END) CHECK ((InclusiveAge1 ISNULL and (AgeTypeID = 'AT' or AgeTypeID = 'ABT')) or InclusiveAge1 = 'Y' or InclusiveAge1 = 'N'), -- Does the age range include the age specified, Yes or No?  (NULL for "AT" and "ABT" ages, otherwise default Yes)
  UseMonths1 INTEGER DEFAULT 1 NOT NULL CHECK (UseMonths1 IN (0, 1)), -- Are we using the Months field?  Specifically, the age could only be known in the days:years form.
  NumYears1 INTEGER DEFAULT NULL CHECK (NumYears1 >= 0 or NumYears1 ISNULL), -- Number of Years (or NULL if unknown)
  NumMonths1 INTEGER DEFAULT NULL CHECK (NumMonths1 >= 0 or NumMonths1 ISNULL or (NumMonths1 ISNULL and UseMonths1 = 0)), -- Number of Months (or NULL if unknown or not applicable (see UseMonths1 field))
  NumDays1 INTEGER DEFAULT NULL CHECK (NumDays1 >= 0 or NumDays1 ISNULL), -- Number of Days (or NULL if unknown)
  Comment1 TEXT DEFAULT NULL,              -- Comment on the Age
  Source1ID INTEGER DEFAULT NULL REFERENCES Source (SourceID), -- Source for Age 1 (or NULL if none)
  InclusiveAge2 TEXT DEFAULT (CASE WHEN AgeTypeID IS NOT 'BET' THEN NULL ELSE 'Y' END) CHECK ((InclusiveAge2 ISNULL and AgeType IS NOT 'BET') or (AgeType IS 'BET' and (InclusiveAge2 = 'Y' or InclusiveAge2 = 'N'))), -- Only used if AgeTypeID is 'BET', otherwise NULL, Does the age range include the age specified, Yes or No?  (NULL default for most AgeTypeIDs, otherwise default Yes)
  UseMonths2 INTEGER DEFAULT (CASE WHEN AgeTypeID IS NOT 'BET' THEN NULL ELSE 1 END) CHECK ((AgeTypeID IS NOT 'BET' and UseMonths2 IS NULL) or UseMonths2 IN (0, 1)), -- Only used if AgeTypeID is 'BET', otherwise NULL, Are we using the Months field?  Specifically, the age could only be known in the days:years form.
  NumYears2 INTEGER DEFAULT NULL CHECK ((AgeTypeID IS NOT 'BET' and NumYears2 IS NULL) or NumYears2 ISNULL or NumYears2 >= 0), -- Only used if AgeTypeID is 'BET', otherwise NULL, Number of Years (or NULL also if unknown)
  NumMonths2 INTEGER DEFAULT NULL CHECK ((AgeTypeID IS NOT 'BET' and NumMonths2 ISNULL) or (UseMonths2 = 0 and NumMonths2 ISNULL) or NumMonths2 ISNULL or NumMonths2 >= 0), -- Only used if AgeTypeID is 'BET', otherwise NULL, Number of Months (or NULL also if unknown or not applicable (see UseMonths2 field))
  NumDays2 INTEGER DEFAULT NULL CHECK ((AgeTypeID IS NOT 'BET' and NumDays2 ISNULL) or NumDays2 ISNULL or NumDays2 >= 0), -- Only used if AgeTypeID is 'BET', otherwise NULL, Number of Days (or NULL also if unknown)
  Comment2 TEXT DEFAULT NULL CHECK ((AgeTypeID IS NOT 'BET' and Comment2 ISNULL) or Comment2 ISNULL or Comment2 IS NOT NULL), -- Only used if AgeTypeID is 'BET', otherwise NULL, Comment on the Age
  Source2ID INTEGER DEFAULT NULL REFERENCES Source (SourceID) CHECK (Source2ID ISNULL or (AgeTypeID = 'BET' and Source2ID ISNULL) or (AgeTypeID = 'BET' and Source2ID IS NOT NULL)) -- Only used if AgeTypeID is 'BET', otherwise NULL, Source for Age 2 (or NULL also if none)
);

CREATE TABLE AgeType (
  AgeTypeID TEXT PRIMARY KEY NOT NULL,     -- Indexes the types
  AgeTypeTxt TEXT NOT NULL UNIQUE DEFAULT '' -- Full Age types
);

CREATE TABLE PersonName (
  NameID INTEGER PRIMARY KEY NOT NULL,     -- (RowID) ID
  PersonID INTEGER NOT NULL REFERENCES Person (PersonID), -- The person whose name this is
  UseStartDateID INTEGER NOT NULL REFERENCES Date (DateID), -- The date at which the person started using the name
  UseEndDateID INTEGER DEFAULT NULL REFERENCES Date (DateID), -- The date at which the person stopped using this name, this can be NULL if they are still using it.
  FamilyName TEXT DEFAULT '?',             -- Family Name(s) (usually Surname): can be NULL if not applicable, or '?' if unknown.  Use '???' for illegible letters. 
  GivenName TEXT DEFAULT '?',              -- Given Name(s) (not middle names though - just those you'd address them by): can be NULL if not applicable, or '?' if unknown.  Use '???' for illegible letters.
  Patronymic TEXT DEFAULT NULL,            -- Patronymic if used: can be NULL if not applicable, or '?' if unknown.  Use '???' for illegible letters.
  OtherNames TEXT DEFAULT NULL,            -- Middle names etc.: can be NULL if not applicable, or '?' if unknown.  Use '???' for illegible letters.
  TitlePrefix TEXT DEFAULT NULL,           -- Title part used at the beginning of the name: can be NULL if not applicable, or '?' if unknown.  Use '???' for illegible letters.
  TitleSuffix TEXT DEFAULT NULL,           -- Title part used at the end of the name: can be NULL if not applicable, or '?' if unknown.  Use '???' for illegible letters.
  TitleInterPart TEXT DEFAULT NULL,        -- Title part used during the name (e.g. 'of'): can be NULL if not applicable, or '?' if unknown.  Use '???' for illegible letters.
  NameFormat TEXT NOT NULL DEFAULT 'G F' CHECK ((FamilyName IS NOT NULL and NameFormat LIKE '%F%') or (GivenName IS NOT NULL and NameFormat LIKE '%G%') or (Patronymic IS NOT NULL and NameFormat LIKE '%P%') or (OtherNames IS NOT NULL and NameFormat LIKE '%O%') or (TitlePrefix IS NOT NULL and NameFormat LIKE '%T%') or (TitleSuffix IS NOT NULL and NameFormat LIKE '%S%') or (TitleInterPart IS NOT NULL and NameFormat LIKE '%I%') or NameFormat IS '?'), -- Order in which the names go: 'G' for Given Name, 'F' for Family Name, 'P' for Patronymic, 'O' for other names, 'T' for Title Prefix, 'S' for Title Suffix, 'I' for Title InterPart.  Can be '?' if unknown.
  PhoneticName TEXT DEFAULT NULL,          -- How to pronounce the name: can be NULL if not applicable, or '?' if unknown.
  PartsGuessed INTEGER NOT NULL DEFAULT 0 CHECK (PartsGuessed = 0 or (PartsGuessed = 1 and (FamilyName LIKE '%???%' or GivenName LIKE '%???%' or Patronymic LIKE '%???%' or OtherNames LIKE '%???%' or TitlePrefix LIKE '%???%' or TitleSuffix LIKE '%???%' or TitleInterPart LIKE '%???%')) or PartsGuessed = 1), -- If 1, name contains guessed or illegible parts.
  Comments TEXT DEFAULT NULL,              -- Any comments on this name
  SourceID INTEGER DEFAULT NULL REFERENCES Source (SourceID) -- Source (or NULL if none)
);

CREATE TABLE PhysDesc (
  PhysDescID INTEGER PRIMARY KEY NOT NULL, -- (RowID) ID of Physical Description
  PersonID INTEGER NOT NULL REFERENCES Person (PersonID), -- The person that this is the physical description of
  PhysDescTxt TEXT NOT NULL DEFAULT '',    -- The Physical Description of the person
  DescDate INTEGER NOT NULL REFERENCES Date (DateID), -- The date of the Physical Description
  Notes TEXT DEFAULT NULL,                 -- Any notes, however unofficial, about the description
  SourceID INTEGER DEFAULT NULL REFERENCES Source (SourceID) -- Source (or NULL if none)
);

CREATE TABLE Beliefs (
  BeliefsID INTEGER PRIMARY KEY NOT NULL,  -- (RowID) ID of Belief System
  PersonID INTEGER NOT NULL REFERENCES Person (PersonID), -- The Person for whom this is a belief system
  BeliefTypeID INTEGER NOT NULL DEFAULT 1 REFERENCES BeliefType (BeliefTypeID), -- The type of Belief System
  Description TEXT NOT NULL DEFAULT '',    -- Description of Belief System
  StartDate INTEGER NOT NULL REFERENCES Date (DateID), -- When the person started believing this (links to '?' values if unknown)
  EndDate INTEGER DEFAULT NULL REFERENCES Date (DateID), -- When the person stopped believing this, or NULL if they never stopped (links to '?' values if unknown)
  Comments TEXT DEFAULT NULL,              -- Comments
  SourceID INTEGER DEFAULT NULL REFERENCES Source (SourceID) -- Source (or NULL if none)
);

CREATE TABLE BeliefType (
  BeliefTypeID INTEGER PRIMARY KEY NOT NULL, -- (RowID) ID of the type of belief system
  BeliefTypeTxt TEXT NOT NULL DEFAULT '' UNIQUE -- The belief system type
);

```

## Values

```sqlite
INSERT INTO DateType (DateTypeID, DateTypeTxt) VALUES ('AT', 'At'), ('ABT', 'About'), ('BEF', 'Before'), ('AFT', 'After'), ('BET', 'Between');

INSERT INTO CalendarType (CalTypeID, CalTypeName) VALUES (1, 'Gregorian'), (2, 'Julian'), (3, 'Hebrew'), (4, 'French'), (5, 'Roman'), (6, 'Unknown'), (7, 'Other');

INSERT INTO AgeType (AgeTypeID, AgeTypeTxt) VALUES ('AT', 'At'), ('ABT', 'About'), ('BEF', 'Before'), ('AFT', 'After'), ('BET', 'Between');

INSERT INTO BeliefType (BeliefTypeID, BeliefTypeTxt) VALUES (1, 'Religion'), (2, 'Cult'), (3, 'Political'), (4, 'Other Formal'), (5, 'Informal Attribute'), (6, 'Other');

```
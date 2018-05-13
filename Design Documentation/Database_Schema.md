# Schema of Database

This is the SQLite schema of the database and is based on the Database Design (see Database_Design.md).

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
  InclusiveDate1 TEXT DEFAULT NULL CHECK ((InclusiveDate1 ISNULL and (DateTypeID = 'AT' or DateTypeID = 'ABT')) or InclusiveDate1 = 'N' or InclusiveDate1 = 'Y'), -- Does the date range include the date specified, Yes or No?  (NULL for "At" dates)
  Year1 TEXT NOT NULL DEFAULT '????',      -- At least 4 digits, replace any digit with '?' if unknown (e.g. 194? for between 1940 and 1949)
  Month1 INTEGER DEFAULT NULL,             -- Month number or NULL if unknown
  Day1 INTEGER DEFAULT NULL,               -- Day number or NULL if unknown
  Cal1TypeID INTEGER NOT NULL DEFAULT 1 REFERENCES CalendarType (CalTypeID), -- Type of Calendar specified
  Comment1 TEXT DEFAULT '',                -- Comment on the Date
  Source1ID INTEGER DEFAULT NULL REFERENCES Source (SourceID), -- Source for Date 1 (or NULL if none)
  InclusiveDate2 TEXT DEFAULT NULL CHECK ((InclusiveDate2 = NULL and DateTypeID IS NOT 'BET') or (DateTypeID = 'BET' and (InclusiveDate2 = 'N' or InclusiveDate2 = 'Y'))), -- Only used if DateTypeID is 'BET', otherwise NULL, Does the date range include the date specified, Yes or No?  (NULL also for "At" dates)
  Year2 TEXT DEFAULT NULL CHECK (Year2 ISNULL or DateTypeID = 'BET'), -- Only used if DateTypeID is 'BET', otherwise NULL, At least 4 digits, replace any digit with '?' if unknown (e.g. 194? for between 1940 and 1949)
  Month2 INTEGER DEFAULT NULL CHECK (Month2 ISNULL or DateTypeID = 'BET'), -- Only used if DateTypeID is 'BET', otherwise NULL, Month number or NULL also if unknown
  Day2 INTEGER DEFAULT NULL CHECK (Day2 ISNULL or DateTypeID = 'BET'), -- Only used if DateTypeID is 'BET', otherwise NULL, Day number or NULL also if unknown
  Cal2TypeID INTEGER DEFAULT NULL REFERENCES CalendarType (CalTypeID) CHECK (Cal2TypeID ISNULL or DateTypeID = 'BET'), -- Only used if DateTypeID is 'BET', otherwise NULL, Type of Calendar specified
  Comment2 TEXT DEFAULT NULL, -- Only used if DateTypeID is 'BET', otherwise NULL, Comment on the Date
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
  InclusiveAge1 TEXT DEFAULT 'Y' CHECK ((InclusiveAge1 ISNULL and (AgeTypeID = 'AT' or AgeTypeID = 'ABT')) or InclusiveAge1 = 'Y' or InclusiveAge1 = 'N'), -- Does the age range include the age specified, Yes or No?  (NULL for "AT" and "ABT" ages, otherwise default Yes)
  UseMonths1 INTEGER DEFAULT 1 NOT NULL CHECK (UseMonths1 IN (0, 1)), -- Are we using the Months field?  Specifically, the age could only be known in the days:years form.
  NumYears1 INTEGER DEFAULT NULL CHECK (NumYears1 >= 0 or NumYears1 ISNULL), -- Number of Years (or NULL if unknown)
  NumMonths1 INTEGER DEFAULT NULL CHECK (NumMonths1 >= 0 or NumMonths1 ISNULL or (NumMonths1 ISNULL and UseMonths1 = 0)), -- Number of Months (or NULL if unknown or not applicable (see UseMonths1 field))
  NumDays1 INTEGER DEFAULT NULL CHECK (NumDays1 >= 0 or NumDays1 ISNULL), -- Number of Days (or NULL if unknown)
  Comment1 TEXT DEFAULT NULL,              -- Comment on the Age
  Source1ID INTEGER DEFAULT NULL REFERENCES Source (SourceID), -- Source for Age 1 (or NULL if none)
  InclusiveAge2 TEXT DEFAULT NULL CHECK ((InclusiveAge2 ISNULL and AgeTypeID IS NOT 'BET') or (AgeTypeID IS 'BET' and (InclusiveAge2 = 'Y' or InclusiveAge2 = 'N'))), -- Only used if AgeTypeID is 'BET', otherwise NULL, Does the age range include the age specified, Yes or No?  (NULL default for most AgeTypeIDs, otherwise default Yes)
  UseMonths2 INTEGER DEFAULT NULL CHECK ((AgeTypeID IS NOT 'BET' and UseMonths2 IS NULL) or UseMonths2 IN (0, 1)), -- Only used if AgeTypeID is 'BET', otherwise NULL, Are we using the Months field?  Specifically, the age could only be known in the days:years form.
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

CREATE TABLE Nationality (
  NatID INTEGER PRIMARY KEY NOT NULL,      -- (RowID) ID of Nationality/Tribal Affiliation
  PersonID INTEGER NOT NULL REFERENCES Person (PersonID), -- The person for whom this Nationality/Tribal Affiliation applies
  AffilTypeID INTEGER NOT NULL DEFAULT 2 REFERENCES AffilType (AffilTypeID), -- Type of Affiliation
  Description TEXT NOT NULL DEFAULT '',    -- Description of Affiliation
  IDCode TEXT DEFAULT NULL,                -- Identity Code (or NULL if not applicable, or '?' if unknown)
  StartDate INTEGER NOT NULL REFERENCES Date (DateID), -- The start date of the Affiliation
  EndDate INTEGER DEFAULT NULL REFERENCES Date (DateID), -- The end date of the affiliation, or NULL if is hasn't ended.
  ConvertedToID INTEGER DEFAULT NULL REFERENCES Nationality (NatID), -- The ID of the nationality it changed into if a nationality/affiliation type was converted to another, otherwise NULL.
  Comments TEXT DEFAULT NULL,              -- Any comments about this
  SourceID INTEGER DEFAULT NULL REFERENCES Source (SourceID) -- Source (or NULL if none)
);

CREATE TABLE AffilType (
  AffilTypeID INTEGER PRIMARY KEY NOT NULL, -- (RowID) ID of the Affilliation Type
  AffilTypeTxt TEXT NOT NULL DEFAULT '' UNIQUE -- Affilliation Type in full
);

CREATE TABLE AdoptFoster (
  AdoptFosterID INTEGER PRIMARY KEY NOT NULL, -- (RowID) Adoption/Fosterhood ID
  ByRelationship INTEGER DEFAULT NULL REFERENCES Relationship (RelID), -- Adopted by Relationship ID (or NULL if unknown)
  PersonID INTEGER NOT NULL REFERENCES Person (PersonID), -- The person who was adopted
  AdFosDate INTEGER NOT NULL REFERENCES Date (DateID), -- The date of the Adoption/Fosterhood.  Linked values are '?' if unknown
  AdFosEndDate INTEGER DEFAULT NULL REFERENCES Date (DateID), -- The date the Adoption/Fosterhood ended or NULL if Not Applicable.  Linked values are '?' if unknown
  AdoptTypeID INTEGER NOT NULL DEFAULT 1 REFERENCES AdoptType (AdoptTypeID), -- The type of adoption or fosterhood
  Description TEXT NOT NULL DEFAULT '',    -- Description of the Adoption/Fosterhood
  PersonTypeID INTEGER DEFAULT 0 NOT NULL REFERENCES AdoptPersonType (AdPersonTypeID), -- Code for the person in the relationship adopting/fostering this person.
  SourceID INTEGER DEFAULT NULL REFERENCES Source (SourceID) -- Source (or NULL if none)
);

CREATE TABLE AdoptPersonType (
  AdPersonTypeID UNSIGNED INT PRIMARY KEY NOT NULL UNIQUE, -- ID for person adopted type (NOT RowID!)
  AdPersonTypeDesc TEXT NOT NULL DEFAULT '' UNIQUE -- Description of person adopted type
);

CREATE TABLE AdoptType (
  AdoptTypeID INTEGER PRIMARY KEY NOT NULL, -- (RowID) Adoption/Fosterhood Type ID
  AdoptTypeTxt TEXT NOT NULL DEFAULT '' UNIQUE -- Full Adoption/Fosterhood Type
);

CREATE TABLE Relationship (
  RelID INTEGER PRIMARY KEY NOT NULL,      -- (RowID) ID of the relationship
  Person1ID INTEGER NOT NULL REFERENCES Person (PersonID), -- First person in relationship
  Person2ID INTEGER DEFAULT NULL REFERENCES Person (PersonID) CHECK (Person2ID ISNULL or Person2ID IS NOT NULL or (Person2ID ISNULL and RelTypeID = 14)), -- Second person in relationship or NULL if unknown (or also NULL if N/A in adoption)
  RelTypeID INTEGER NOT NULL DEFAULT 7 REFERENCES RelType (RelTypeID), -- The type of relationship
  IsRomantic INTEGER DEFAULT 1 CHECK (IsRomantic IN (NULL, 0, 1)), -- Is the relationship a romantic or a platonic one? (NULL if unknown)
  Description TEXT NOT NULL DEFAULT '',    -- Description of the Relationship
  RelEndTypeID INTEGER NOT NULL DEFAULT 4 REFERENCES RelEndType (RelEndTypeID), -- The way the relationship ended
  RelEndDesc TEXT DEFAULT '' NOT NULL,     -- Description of how the relationship ended
  ConvRelID INTEGER DEFAULT NULL REFERENCES Relationship (RelID) CHECK (ConvRelID ISNULL or (ConvRelID IS NOT NULL and RelEndTypeID = 3)), -- If the Relationship End Type ID is 3 (converted), this links to the relationship it converted into.  May need a placeholder?
  Notes TEXT DEFAULT NULL,                 -- Any notes, however unofficial, on this relationship
  SourceID INTEGER DEFAULT NULL REFERENCES Source (SourceID) -- Source (or NULL if none)
);

CREATE TABLE RelType (
  RelTypeID INTEGER PRIMARY KEY NOT NULL,  -- (RowID) ID of the Relationship Type
  RelTypeTxt TEXT NOT NULL DEFAULT '' UNIQUE, -- Full Relationship Type
  CanHaveChild INTEGER NOT NULL DEFAULT 1 CHECK (CanHaveChild IN (0, 1)) -- Can this relationship type give birth to a child?
);

CREATE TABLE RelEndType (
  RelEndTypeID INTEGER PRIMARY KEY NOT NULL, -- (RowID) ID of the Relationship End Type
  RelEndTypeTxt TEXT NOT NULL DEFAULT '' UNIQUE -- Full Relationship End Type
);

CREATE TABLE Job (
  JobID INTEGER PRIMARY KEY NOT NULL,      -- (RowID) ID of Job
  PersonID INTEGER NOT NULL REFERENCES Person (PersonID), -- Person whose job this is
  JobTitle TEXT NOT NULL DEFAULT '',       -- Title of Job
  JobDescription TEXT NOT NULL DEFAULT '', -- Description of Job
  PayDescription TEXT NOT NULL DEFAULT '?', -- Description of Pay or '?' if pay is unknown
  StartDate INTEGER NOT NULL REFERENCES Date (DateID), -- Date the person started the Job
  EndDate INTEGER DEFAULT NULL REFERENCES Date (DateID), -- Date the person ended the job (or NULL if the job is still ongoing)
  Comments TEXT DEFAULT NULL,              -- Comments on the Job
  SourceID INTEGER DEFAULT NULL REFERENCES Source(SourceID) -- Source (or NULL if none)
);

CREATE TABLE Address (
  AddrID INTEGER PRIMARY KEY NOT NULL,     -- (RowID) ID of Address
  Description TEXT NOT NULL DEFAULT '',    -- Description of Address
  AddrNameNum TEXT NOT NULL DEFAULT '?',   -- Address Name or Number (or '?' if unknown)
  AddrLine2 TEXT DEFAULT NULL,             -- Address Line 2 (or NULL if not applicable)
  AddrRoad TEXT DEFAULT NULL,              -- Address Road (or '?' if unknown, or NULL if not applicable)
  AddrSettlement TEXT DEFAULT NULL,        -- Settlement (or '?' if unknown, or NULL if not applicable)
  AddrBorough TEXT DEFAULT NULL,           -- Borough  (or '?' if unknown, or NULL if not applicable)
  AddrCounty TEXT DEFAULT NULL,            -- County  (or '?' if unknown, or NULL if not applicable)
  AddrState TEXT DEFAULT NULL,             -- State  (or '?' if unknown, or NULL if not applicable)
  AddrPostCode TEXT DEFAULT NULL,          -- Postal Code or similar  (or '?' if unknown, or NULL if not applicable)
  AddrCountry TEXT DEFAULT NULL,           -- Country  (or '?' if unknown, or NULL if not applicable)
  Latitude REAL DEFAULT NULL,              -- Latitude (or NULL if unknown)
  Longitude REAL DEFAULT NULL,             -- Longitude (or NULL if unknown)
  FixTelNum TEXT DEFAULT NULL,             -- Fixed (land-line) Telephone Number  (or '?' if unknown, or NULL if not applicable)
  Notes TEXT DEFAULT NULL,                 -- Any notes, even if unofficial
  SourceID INTEGER DEFAULT NULL REFERENCES Source(SourceID) -- Source (or NULL if none)
);

CREATE TABLE LivingAddr (
  LivingAddrID INTEGER PRIMARY KEY NOT NULL, -- (RowID) ID of Living Address
  PersonID INTEGER NOT NULL REFERENCES Person (PersonID), -- Person whose address this is
  AddrID INTEGER NOT NULL REFERENCES Address (AddrID), -- The Address
  ArrivedDate INTEGER NOT NULL REFERENCES Date (DateID), -- Date the person arrived at the address
  LeftDate INTEGER DEFAULT NULL REFERENCES Date (DateID), -- Date the person left the address, or NULL if they haven't left yet
  Notes TEXT DEFAULT NULL,                 -- Any notes, however unofficial, on this residence
  SourceID INTEGER DEFAULT NULL REFERENCES Source(SourceID) -- Source (or NULL if none)
);

CREATE TABLE WorkingAddr (
  WorkingAddrID INTEGER PRIMARY KEY NOT NULL, -- (RowID) ID of Working Address
  JobID INTEGER NOT NULL REFERENCES Job (JobID), -- Job at this address
  AddrID INTEGER NOT NULL REFERENCES Address (AddrID), -- The Address
  ArrivedDate INTEGER NOT NULL REFERENCES Date (DateID), -- Date the job arrived at the address
  LeftDate INTEGER DEFAULT NULL REFERENCES Date (DateID), -- Date the job left the address, or NULL if it hasn't left yet
  Description TEXT NOT NULL DEFAULT '',    -- Description of this job being at this address
  Notes TEXT DEFAULT NULL,                 -- Any notes, however unofficial, about this job at this address
  SourceID INTEGER DEFAULT NULL REFERENCES Source(SourceID) -- Source (or NULL if none)
);

CREATE TABLE OtherContact (
  OtherContactID INTEGER PRIMARY KEY NOT NULL, -- (RowID) ID of the Other Contact-type Address
  PersonID INTEGER NOT NULL REFERENCES Person (PersonID), -- Person whose contact details these are
  StartDate INTEGER NOT NULL REFERENCES Date (DateID), -- Date the person gained these contact details
  EndDate INTEGER DEFAULT NULL REFERENCES Date (DateID), -- Date the person lost these contact details, or NULL if they are still valid
  ContactTypeID INTEGER NOT NULL DEFAULT 1 REFERENCES ContactType (ContactTypeID), -- Type of Contact Details
  Description TEXT NOT NULL DEFAULT '',    -- Description of contact details
  Data TEXT NOT NULL DEFAULT '',           -- Contact Details
  Notes TEXT DEFAULT NULL,                 -- Any notes, however unofficial, on these contact details
  SourceID INTEGER DEFAULT NULL REFERENCES Source(SourceID) -- Source (or NULL if none)
);

CREATE TABLE ContactType (
  ContactTypeID INTEGER PRIMARY KEY NOT NULL, -- (RowID) ID of the Contact Type
  ContactTypeTxt TEXT NOT NULL DEFAULT ''  -- Full Contact Type
);

CREATE TABLE EducAchieve (
  EducAchieveID INTEGER PRIMARY KEY NOT NULL, -- (RowID) Educational Achievement ID
  PersonID INTEGER NOT NULL REFERENCES Person (PersonID), -- Person who achieved it
  AchieveDate INTEGER NOT NULL REFERENCES Date (DateID), -- Date they achieved it
  QualName TEXT NOT NULL DEFAULT '',       -- Qualification Name
  QualType TEXT NOT NULL DEFAULT '',       -- Qualification Type
  Subject TEXT DEFAULT NULL,               -- Subject of Qualification, or NULL if too general
  Description TEXT NOT NULL DEFAULT '',    -- Description of Qualification
  PlaceName TEXT DEFAULT NULL,             -- Place qualification was achieved or NULL if not applicable
  PlaceAddrID INTEGER DEFAULT NULL REFERENCES Address (AddrID), -- Address of the place (or NULL if Unknown, also NULL if unapplicable (above will be NULL too in this case)).
  Notes TEXT DEFAULT NULL,                 -- Any notes, however unofficial, about this qualification
  SourceID INTEGER DEFAULT NULL REFERENCES Source(SourceID) -- Source (or NULL if none)
);

CREATE TABLE Event (
  EventID INTEGER PRIMARY KEY NOT NULL,    -- (RowID) Event ID
  PersonID INTEGER NOT NULL REFERENCES Person (PersonID), -- Person to whom the event occurred
  EventDate INTEGER NOT NULL REFERENCES Date (DateID), -- Date of the Event
  AgeAtEvent INTEGER DEFAULT NULL REFERENCES Age (AgeID), -- Person's age at event
  Description TEXT NOT NULL DEFAULT '',    -- Short description of event
  Details TEXT DEFAULT NULL,               -- Event Details (or NULL if not applicable)
  PlaceName TEXT DEFAULT NULL,             -- Place the where the event occurred (or NULL if not applicable)
  PlaceAddr INTEGER DEFAULT NULL REFERENCES Address (AddrID), -- Address of place the where the event occurred (or NULL if not applicable)
  Notes TEXT DEFAULT NULL,                 -- Any notes, however unofficial, about this event
  SourceID INTEGER DEFAULT NULL REFERENCES Source(SourceID) -- Source (or NULL if none)
);

CREATE TABLE Source (
  SourceID INTEGER PRIMARY KEY NOT NULL,   -- (RowID) Source ID
  SourceTypeID INTEGER NOT NULL REFERENCES SourceType (SourceTypeID), -- The type of source
  Description TEXT NOT NULL DEFAULT '',    -- Description of the source
  FromDate INTEGER NOT NULL REFERENCES Date (DateID), -- The date from which the work, well, dates.  Links to '?' if unknown.
  FromWork TEXT DEFAULT NULL,              -- Work the source is from (e.g. name of a book, etc).  If not applicable, is NULL.
  SectPagePara TEXT DEFAULT NULL,          -- Section, Page and Paragraph reference or at least some way of locating source in work, or NULL if not applicable.
  Location TEXT DEFAULT NULL,              -- Location of source (e.g. a library) or NULL if not applicable.
  LocationAddr INTEGER DEFAULT NULL REFERENCES Address (AddrID) CHECK (LocationAddr ISNULL or LocationAddr IS NOT NULL or (LocationAddr ISNULL and Location ISNULL)), -- Address of source location, or NULL if not applicable or unknown (if unknown, above field won't be NULL).
  CallNum TEXT DEFAULT NULL,               -- Call Number or other reference form for work in location, or '?' if unknown, or NULL if not applicable.
  LangCodeMajor TEXT DEFAULT NULL REFERENCES ISO6393 (Code3), -- ISO 639-3 code (Major part), or NULL if not applicable.
  LangCodeMinor TEXT DEFAULT NULL CHECK (LangCodeMinor ISNULL or LangCodeMinor IS NOT NULL or (LangCodeMinor ISNULL and LangCodeMajor ISNULL)), -- ISO 639-3 code (Minor/National part), or NULL if not applicable.
  AccessedDate INTEGER NOT NULL REFERENCES Date (DateID), -- The date the source was accessed and/or details added to the database.
  InternalData TEXT DEFAULT NULL,          -- Typed Data from source, or NULL if this cannot be done.
  ExternalData TEXT DEFAULT NULL,          -- Path of external file containing source data, or NULL if not applicable
  ExtDataMIME TEXT DEFAULT NULL CHECK ((ExternalData ISNULL and ExtDataMIME ISNULL) or (ExternalData IS NOT NULL and ExtDataMIME IS NOT NULL)), -- MIME type of External Data file, or NULL if not applicable.
  Reliability INTEGER DEFAULT NULL CHECK (Reliability IN (0, 1, 2, 3, NULL)), -- Reliability assessment of data in a scale of 0 to 3, or NULL if unknown.
  Notes TEXT DEFAULT NULL                  -- Any notes, however unofficial, about the source.
);

CREATE TABLE SourceType (
  SourceTypeID INTEGER PRIMARY KEY NOT NULL, -- (RowID) Source Type ID
  SourceTypeTxt TEXT NOT NULL DEFAULT '' UNIQUE -- The full Source Type
);

CREATE TABLE Multimedia (
  MultimediaID INTEGER PRIMARY KEY NOT NULL, -- (RowID) ID of Other Multimedia
  Description TEXT NOT NULL DEFAULT '',    -- Description of the Multimedia
  OriginDate INTEGER NOT NULL REFERENCES Date (DateID), -- Original date of the Multimedia (Linked values are '?' if unknown)
  Data TEXT NOT NULL DEFAULT '',           -- Path of external data file containing Multimedia
  DataMIME TEXT NOT NULL DEFAULT 'application/octet-stream', -- MIME type of the external data file
  Notes TEXT DEFAULT NULL                  -- Any notes, however unofficial, on the Multimedia
);

CREATE TABLE InMultimedia (
  InMultimediaID INTEGER PRIMARY KEY NOT NULL, -- (RowID) ID of Multimedia Assignment
  MultimediaID INTEGER NOT NULL REFERENCES Multimedia (MultimediaID), -- ID of the Other Multimedia
  PersonID INTEGER NOT NULL REFERENCES Person (PersonID), -- ID of the person featured in the Multimedia
  Notes TEXT DEFAULT NULL                  -- Any notes, however unofficial, about the person's role in the Multimedia
);

CREATE TABLE ISO6393 (
  Code3 TEXT PRIMARY KEY NOT NULL UNIQUE,  -- The ISO-639-3 code
  Code2B TEXT DEFAULT NULL,                -- The ISO-639-2 Bibliographic code (or NULL if not available)
  Code2T TEXT DEFAULT NULL,                -- The ISO-639-2 Terminology code (or NULL if not available)
  Code1 TEXT DEFAULT NULL,                 -- The ISO-639-1 code (or NULL if not available)
  ScopeCode TEXT NOT NULL DEFAULT 'I' CHECK (ScopeCode IN ('I', 'M', 'S')), -- Single letter specifying the scope of the language code: I(ndividual), M(acrolanguage), S(pecial)
  TypeCode TEXT NOT NULL DEFAULT 'L' CHECK (TypeCode IN ('A', 'C', 'E', 'H', 'L', 'S')), -- Single letter specifying the type of the language code: A(ncient), C(onstructed), E(xtinct), H(istorical), L(iving), S(pecial)
  RefName TEXT NOT NULL DEFAULT '',        -- The reference name of the language
  Comment TEXT DEFAULT NULL                -- Any comments about the language.
);

CREATE TABLE MetaInfo (
  Element TEXT PRIMARY KEY NOT NULL UNIQUE, -- Meta Information Element
  Value TEXT DEFAULT NULL                  -- Meta Information Value
);
```

## Values

```sqlite
INSERT INTO DateType (DateTypeID, DateTypeTxt) VALUES ('AT', 'At'), ('ABT', 'About'), ('BEF', 'Before'), ('AFT', 'After'), ('BET', 'Between');

INSERT INTO CalendarType (CalTypeID, CalTypeName) VALUES (1, 'Gregorian'), (2, 'Julian'), (3, 'Hebrew'), (4, 'French'), (5, 'Roman'), (6, 'Unknown'), (7, 'Other');

INSERT INTO AgeType (AgeTypeID, AgeTypeTxt) VALUES ('AT', 'At'), ('ABT', 'About'), ('BEF', 'Before'), ('AFT', 'After'), ('BET', 'Between');

INSERT INTO BeliefType (BeliefTypeID, BeliefTypeTxt) VALUES (1, 'Religion'), (2, 'Cult'), (3, 'Political'), (4, 'Other Formal'), (5, 'Informal Attribute'), (6, 'Other');

INSERT INTO AffilType (AffilTypeID, AffilTypeTxt) VALUES (1, 'Tribe'), (2, 'National Citizen'), (3, 'Freedom to Remain'), (4, 'Working Visa'), (5, 'Permanent Residency'), (6, 'Other');

INSERT INTO AdoptPersonType (AdPersonTypeID, AdPersonTypeDesc) VALUES (0, 'Unknown'), (1, 'Person 1 Only'), (2, 'Person 2 Only'), (3, 'Both people in relationship');

INSERT INTO AdoptType (AdoptTypeID, AdoptTypeTxt) VALUES (1, 'Legal Adoption'), (2, 'Informal Adoption'), (3, 'Fosterhood'), (4, 'Other');

INSERT INTO RelType (RelTypeID, RelTypeTxt, CanHaveChild) VALUES (1, 'Sperm/Egg donor', 1), (2, 'Surrogate', 1), (3, 'Very Short Term Relationship', 1), (4, 'Short Term Relationship', 1), (5, 'Long Term Relationship', 1), (6, 'Formal Engagement', 1), (7, 'Marriage', 1), (8, 'Civil Partnership', 1), (9, 'Friends', 0), (10, 'Sexual Friendship', 1), (11, 'Godparent/Godchild', 0), (12, 'Witness at marriage', 0), (13, 'Other', 1), (14, '(Dummy)', 0);

INSERT INTO RelEndType (RelEndTypeID, RelEndTypeTxt) VALUES (1, 'Split'), (2, 'Death'), (3, 'Converted'), (4, 'Unknown');

INSERT INTO ContactType (ContactTypeID, ContactTypeTxt) VALUES (1, 'Fixed Telephone Number'), (2, 'Mobile Telephone Number'), (3, 'Facsimile Number'), (4, 'Telex/Teleprinter Number'), (5, 'E-Mail Address'), (6, 'Web Address'), (7, 'Instant Messaging Address'), (8, 'VOIP Number/Address'), (9, 'Social Media Account'), (10, 'Other');

INSERT INTO SourceType (SourceTypeID, SourceTypeTxt) VALUES (1, 'Memory'), (2, 'Record'), (3, 'Image'), (4, 'Audio'), (5, 'Video'), (6, 'Other');

INSERT INTO MetaInfo (Element, Value) VALUES ('Version', ''), ('Last_Edit', strftime('%Y-%m-%dT%H:%M:%S.%f'));
```

See also the file ISO_639-3_Values.txt for the INSERT statement for the values of table ISO6393.

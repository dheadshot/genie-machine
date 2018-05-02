# Database Design

The following is the design of the database where genealogy data will be stored.  It is presented as a list of tables, some with a description of their preset content.  Since the database will use SQLite3, the datatypes and constraints reflect this.

## Person Table

The "Person" table stores information about a specific person.  Please note that a person can have many names over the course of their life and thus names are held in the "PersonName" table.

Field Name   | Data Type | Key Type | Default Value | Additional Constraints          | Description 
------------ | --------- | -------- | ------------- | ------------------------------- | -----------
PersonID     | INTEGER   | Primary  | N/A           | NOT NULL                        | (RowID) ID
Bio          | TEXT      | N/A      | ''            | NOT NULL                        | Biography of person
Ethnicity    | TEXT      | N/A      | ''            |                                 | Description of Ethnicity (or NULL if unknown)
Sex          | TEXT      | N/A      | '?'           | NOT NULL, 'M' or 'F' or '?'     | Chromosonal Sex ('M', 'F' or '?' if unknown)
Gender       | TEXT      | N/A      | '?'           |                                 | Description of Gender (or '?' if unknown), will often match sex
Notes        | TEXT      | N/A      | NULL          |                                 | Any notes that aren't necessarily official but should nonetheless be recorded about a person
ChildOfRelID | INTEGER   | Foreign  | NULL          | Links to Relationship(RelID)    | Biological child of this relationship (or NULL if unknown)
IsAdopted    | INTEGER   | N/A      | NULL          | 0 or 1 or NULL                  | Specifies that the person was adopted.  Not required as this data is also included in the AdoptFoster table
BirthDate    | INTEGER   | Foreign  | N/A           | NOT NULL, Links to Date(DateID) | Date of Birth
BirthPlace   | TEXT      | N/A      | '?'           | NOT NULL                        | Birth Place Name (or '?' if unknown)
BirthAddrID  | INTEGER   | Foreign  | NULL          | Links to Address(AddrID)        | Birth Place Address ID (or NULL if not applicable, linked value is '?' if unknown)
DeathDate    | INTEGER   | Foreign  | NULL          | Links to Date(DateID)           | Death Date (or NULL if still alive)
DeathPlace   | TEXT      | N/A      | NULL          |                                 | Death Place Name (or NULL if still alive, '?' if unknown)
DeathAddrID  | INTEGER   | Foreign  | NULL          | Links to Address(AddrID)        | Death Place Address ID (or NULL if not applicable or still alive, Linked value is '?' if unknown)
DeathAge     | INTEGER   | Foreign  | NULL          | Links to Age(AgeID)             | Age at Death (or NULL if still alive, linked value is NULL:NULL:NULL if unknown)
SourceID     | INTEGER   | Foreign  | NULL          | Links to Source(SourceID)       | Source (or NULL if none)


## Date Table

The "Date" table specifies a date or date range that an event took place.

Field Name     | Data Type | Key Type | Default Value | Additional Constraints                     | Description 
-------------- | --------- | -------- | ------------- | ------------------------------------------ | -----------
DateID         | INTEGER   | Primary  | N/A           | NOT NULL                                   | (RowID) ID
DateTypeID     | TEXT      | Foreign  | 'AT'          | NOT NULL, Links to DateType(DateTypeID)    | Type of Date.  If 'BET', use '2' fields as well
InclusiveDate1 | TEXT      | N/A      | NULL          | 'Y' or 'N' or NULL                         | Does the date range include the date specified, Yes or No?  (NULL for "At" dates)
Year1          | TEXT      | N/A      | '????'        | NOT NULL                                   | 4 digits, replace any digit with '?' if unknown (e.g. 194? for between 1940 and 1949)
Month1         | INTEGER   | N/A      | NULL          |                                            | Month number or NULL if unknown
Day1           | INTEGER   | N/A      | NULL          |                                            | Day number or NULL if unknown
Cal1TypeID     | INTEGER   | Foreign  | 1             | NOT NULL, Links to CalendarType(CalTypeID) | Type of Calendar specified
Comment1       | TEXT      | N/A      | ''            |                                            | Comment on the Date
Source1ID      | INTEGER   | Foreign  | NULL          | Links to Source(SourceID)                  | Source for Date 1 (or NULL if none)
InclusiveDate2 | TEXT      | N/A      | NULL          | 'Y' or 'N' or NULL                         | Only used if DateTypeID is 'BET', otherwise NULL; Does the date range include the date specified, Yes or No?  (NULL also for "At" dates)
Year2          | TEXT      | N/A      | NULL          |                                            | Only used if DateTypeID is 'BET', otherwise NULL; 4 digits, replace any digit with '?' if unknown (e.g. 194? for between 1940 and 1949)
Month2         | INTEGER   | N/A      | NULL          |                                            | Only used if DateTypeID is 'BET', otherwise NULL; Month number or NULL also if unknown
Day2           | INTEGER   | N/A      | NULL          |                                            | Only used if DateTypeID is 'BET', otherwise NULL; Day number or NULL also if unknown
Cal2TypeID     | INTEGER   | Foreign  | NULL          | Links to CalendarType(CalTypeID)           | Only used if DateTypeID is 'BET', otherwise NULL; Type of Calendar specified
Comment2       | TEXT      | N/A      | NULL          |                                            | Only used if DateTypeID is 'BET', otherwise NULL; Comment on the Date
Source2ID      | INTEGER   | Foreign  | NULL          | Links to Source(SourceID)                  | Only used if DateTypeID is 'BET', otherwise NULL; Source for Date 2 (or NULL also if none)

Note that "FROM" dates are implemented as "AFTER" with "InclusiveDate1" set to 'Y', "TO" dates are "BEFORE" with "InclusiveDate1" set to 'Y' and "FROM ... TO ..." dates are implemented as "BETWEEN" dates with both "InclusiveDate" fields set to 'Y'.


## DateType Table

The "DateType" table details the type of date specified in the "Date" table.

Field Name   | Data Type | Key Type | Default Value | Additional Constraints | Description 
------------ | --------- | -------- | ------------- | ---------------------- | -----------
DateTypeID   | TEXT      | Primary  | N/A           | NOT NULL               | Indexes the types
DateTypeTxt  | TEXT      | N/A      | ''            | NOT NULL, UNIQUE       | Full date types

### Predefined Contents

This table has predefined contents as follows:

DateTypeID | DateTypeTxt
---------- | -----------
AT         | At
ABT        | About
BEF        | Before
AFT        | After
BET        | Between


## CalendarType Table

The "CalendarType" table details the type of calendar used for the date specified in the "Date" table.

Field Name   | Data Type | Key Type | Default Value | Additional Constraints | Description 
------------ | --------- | -------- | ------------- | ---------------------- | -----------
CalTypeID    | INTEGER   | Primary  | N/A           | NOT NULL               | (RowID) Index of the types
CalTypeName  | TEXT      | N/A      | ''            | NOIT NULL, UNIQUE      | Names of the types

### Predefined Contents

This table has predefined contents as follows:

CalTypeID  | CalTypeName
---------- | -----------
1          | Gregorian
2          | Julian
3          | Hebrew
4          | French
5          | Roman
6          | Unknown
7          | Other

In the case of type 7 ("Other"), the calendar type should be specified in the comment in the "Date" table.


## Age

The "Age" table specifies the age or age range of a person when an event took place.

Field Name     | Data Type | Key Type | Default Value | Additional Constraints                     | Description 
-------------- | --------- | -------- | ------------- | ------------------------------------------ | -----------
AgeID          | INTEGER   | Primary  | N/A           | NOT NULL                                   | (RowID) ID
AgeTypeID      | TEXT      | Foreign  | 'AT'          | NOT NULL, Links to AgeType(AgeTypeID)      | Type of Age.  If 'BET', use '2' fields as well
InclusiveAge1  | TEXT      | N/A      | 'Y'           | 'Y' or 'N' or NULL                         | Does the age range include the age specified, Yes or No?  (NULL for "AT" ages, otherwise default Yes)
UseMonths1     | INTEGER   | N/A      | 1             | NOT NULL, 0 or 1                           | Are we using the Months field?  Specifically, the age could only be known in the days:years form.
NumYears1      | INTEGER   | N/A      | NULL          |                                            | Number of Years (or NULL if unknown)
NumMonths1     | INTEGER   | N/A      | NULL          |                                            | Number of Months (or NULL if unknown or not applicable (see UseMonths1 field))
NumDays1       | INTEGER   | N/A      | NULL          |                                            | Number of Days (or NULL if unknown)
Comment1       | TEXT      | N/A      | NULL          |                                            | Comment on the Age
Source1ID      | INTEGER   | Foreign  | NULL          | Links to Source(SourceID)                  | Source for Age 1 (or NULL if none)
InclusiveAge2  | TEXT      | N/A      | NULL          | 'Y' or 'N' or NULL                         | Only used if AgeTypeID is 'BET', otherwise NULL; Does the age range include the age specified, Yes or No?  (NULL also for "At" ages, otherwise default Yes)
UseMonths2     | INTEGER   | N/A      | NULL          | 0 or 1 or NULL                             | Only used if AgeTypeID is 'BET', otherwise NULL; Are we using the Months field?  Specifically, the age could only be known in the days:years form.
NumYears2      | INTEGER   | N/A      | NULL          |                                            | Only used if AgeTypeID is 'BET', otherwise NULL; Number of Years (or NULL also if unknown)
NumMonths2     | INTEGER   | N/A      | NULL          |                                            | Only used if AgeTypeID is 'BET', otherwise NULL; Number of Months (or NULL also if unknown or not applicable (see UseMonths2 field))
NumDays2       | INTEGER   | N/A      | NULL          |                                            | Only used if AgeTypeID is 'BET', otherwise NULL; Number of Days (or NULL also if unknown)
Comment2       | TEXT      | N/A      | NULL          |                                            | Only used if AgeTypeID is 'BET', otherwise NULL; Comment on the Age
Source2ID      | INTEGER   | Foreign  | NULL          | Links to Source(SourceID)                  | Only used if AgeTypeID is 'BET', otherwise NULL; Source for Age 2 (or NULL also if none)


## AgeType Table

The "AgeType" table details the type of age specified in the "Age" table.

Field Name | Data Type | Key Type | Default Value | Additional Constraints | Description 
---------- | --------- | -------- | ------------- | ---------------------- | -----------
AgeTypeID  | TEXT      | Primary  | N/A           | NOT NULL               | Indexes the types
AgeTypeTxt | TEXT      | N/A      | ''            | UNIQUE, NOT NULL       | Full Age types

### Predefined Contents

This table has predefined contents as follows:

AgeTypeID | AgeTypeTxt
--------- | ----------
AT        | At
ABT       | About
BEF       | Before
AFT       | After
BET       | Between


## PersonName Table

The "PersonName" table stores the various names that a person uses over the course of their lifetime.

Field Name     | Data Type | Key Type | Default Value | Additional Constraints                     | Description 
-------------- | --------- | -------- | ------------- | ------------------------------------------ | -----------
NameID         | INTEGER   | Primary  | N/A           | NOT NULL                                   | (RowID) ID
PersonID       | INTEGER   | Foreign  | N/A           | NOT NULL, Links to Person(PersonID)        | The person whose name this is
UseStartDateID | INTEGER   | Foreign  | N/A           | NOT NULL, Links to Date(DateID)            | The date at which the person started using the name
UseEndDateID   | INTEGER   | Foreign  | NULL          | Links to Date(DateID)                      | The date at which the person stopped using this name, this can be NULL if they are still using it.
FamilyName     | TEXT      | N/A      | '?'           |                                            | Family Name(s) (usually Surname): can be NULL if not applicable, or '?' if unknown.  Use '???' for illegible letters. 
GivenName      | TEXT      | N/A      | '?'           |                                            | Given Name(s) (not middle names though - just those you'd address them by): can be NULL if not applicable, or '?' if unknown.  Use '???' for illegible letters.
Patronymic     | TEXT      | N/A      | NULL          |                                            | Patronymic if used: can be NULL if not applicable, or '?' if unknown.  Use '???' for illegible letters.
OtherNames     | TEXT      | N/A      | NULL          |                                            | Middle names etc.: can be NULL if not applicable, or '?' if unknown.  Use '???' for illegible letters.
TitlePrefix    | TEXT      | N/A      | NULL          |                                            | Title part used at the beginning of the name: can be NULL if not applicable, or '?' if unknown.  Use '???' for illegible letters.
TitleSuffix    | TEXT      | N/A      | NULL          |                                            | Title part used at the end of the name: can be NULL if not applicable, or '?' if unknown.  Use '???' for illegible letters.
TitleInterPart | TEXT      | N/A      | NULL          |                                            | Title part used during the name (e.g. 'of'): can be NULL if not applicable, or '?' if unknown.  Use '???' for illegible letters.
NameFormat     | TEXT      | N/A      | 'G F'         | NOT NULL                                   | Order in which the names go: 'G' for Given Name, 'F' for Family Name, 'P' for Patronymic, 'O' for other names, 'T' for Title Prefix, 'S' for Title Suffix, 'I' for Title InterPart.  Can be '?' if unknown.
PhoneticName   | TEXT      | N/A      | NULL          |                                            | How to pronounce the name: can be NULL if not applicable, or '?' if unknown.
PartsGuessed   | INTEGER   | N/A      | 0             | NOT NULL, 0 or 1                           | If 1, name contains guessed or illegible parts.
Comments       | TEXT      | N/A      | NULL          |                                            | Any comments on this name
SourceID       | INTEGER   | Foreign  | NULL          | Links to Source(SourceID)                  | Source (or NULL if none)


## PhysDesc Table

The "PhysDesc" table stores any physical descriptions of a person that have been documented.

Field Name  | Data Type | Key Type | Default Value | Additional Constraints              | Description 
----------- | --------- | -------- | ------------- | ----------------------------------- | -----------
PhysDescID  | INTEGER   | Primary  | N/A           | NOT NULL                            | (RowID) ID of Physical Description
PersonID    | INTEGER   | Foreign  | N/A           | NOT NULL, Links to Person(PersonID) | The person that this is the physical description of
PhysDescTxt | TEXT      | N/A      | ''            | NOT NULL                            | The Physical Description of the person
DescDate    | INTEGER   | Foreign  | N/A           | NOT NULL, Links to Date(DateID)     | The date of the Physical Description
Notes       | TEXT      | N/A      | NULL          |                                     | Any notes, however unofficial, about the description
SourceID    | INTEGER   | Foreign  | NULL          | Links to Source(SourceID)           | Source (or NULL if none)


## Beliefs

The "Beliefs" table stores the belief systems that a person subscribes to, whether a religious or political affiliation or something else.

Field Name     | Data Type | Key Type | Default Value | Additional Constraints                      | Description 
-------------- | --------- | -------- | ------------- | ------------------------------------------- | -----------
BeliefsID      | INTEGER   | Primary  | N/A           | NOT NULL                                    | (RowID) ID of Belief System
PersonID       | INTEGER   | Foreign  | N/A           | NOT NULL, Links to Person(PersonID)         | The Person for whom this is a belief system
BeliefTypeID   | INTEGER   | Foreign  | 1             | NOT NULL, Links to BeliefType(BeliefTypeID) | The type of Belief System
Description    | TEXT      | N/A      | ''            | NOT NULL                                    | Description of Belief System
StartDate      | INTEGER   | Foreign  | N/A           | NOT NULL, Links to Date(DateID)             | When the person started believing this (links to '?' values if unknown)
EndDate        | INTEGER   | Foreign  | NULL          | Links to Date(DateID)                       | When the person stopped believeing this, or NULL if they never stopped (links to '?' values if unknown)
Comments       | TEXT      | N/A      | NULL          |                                             | Comments
SourceID       | INTEGER   | Foreign  | NULL          | Links to Source(SourceID)                   | Source (or NULL if none)


## BeliefType Table

The "BeliefType" table stores the types of belief system that a person can subscribe to, such as "Religious" or "Political".

Field Name    | Data Type | Key Type | Default Value | Additional Constraints | Description 
------------- | --------- | -------- | ------------- | ---------------------- | -----------
BeliefTypeID  | INTEGER   | Primary  | N/A           | NOT NULL               | (RowID) ID of the type of belief system
BeliefTypeTxt | TEXT      | N/A      | ''            | NOT NULL, UNIQUE       | The belief system type

### Predefined Contents

This table has predefined contents as follows:

BeliefTypeID | BeliefTypeTxt
------------ | ----------
1            | Religion
2            | Cult
3            | Political
4            | Other Formal
5            | Informal Attribute
6            | Other

Please note that "athiest" is an attribute of a belief system, so would fall under category 5 ("Informal Attribute").


## Nationality Table

The "Nationality" table stores the nationality or tribal affiliation of a person.  A person can have many overlapping nationalities or tribal affiliations.

Field Name     | Data Type | Key Type | Default Value | Additional Constraints                      | Description 
-------------- | --------- | -------- | ------------- | ------------------------------------------- | -----------
NatID          | INTEGER   | Primary  | N/A           | NOT NULL                                    | (RowID) ID of Nationality/Tribal Affiliation
PersonID       | INTEGER   | Foreign  | N/A           | NOT NULL, Links to Person(PersonID)         | The person for whom this Nationality/Tribal Affiliation applies
AffilTypeID    | INTEGER   | Foreign  | 2             | NOT NULL, Links to AffilType(AffilTypeID)   | Type of Affiliation
Description    | TEXT      | N/A      | ''            | NOT NULL                                    | Description of Affiliation
IDCode         | TEXT      | N/A      | NULL          |                                             | Identity Code (or NULL if not applicable, or '?' if unknown)
StartDate      | INTEGER   | Foreign  | N/A           | NOT NULL, Links to Date(DateID)             | The start date of the Affiliation
EndDate        | INTEGER   | Foreign  | NULL          | Links to Date(DateID)                       | The end date of the affiliation, or NULL if is hasn't ended.
ConvertedToID  | INTEGER   | Foreign  | NULL          | Links to Nationality(NatID)                 | The ID of the nationality it changed into if a nationality/affiliation type was converted to another, otherwise NULL.
Comments       | TEXT      | N/A      | NULL          |                                             | Any comments about this
SourceID       | INTEGER   | Foreign  | NULL          | Links to Source(SourceID)                   | Source (or NULL if none)


## AffilType Table

The "AffilType" table stores the type of affiliation detailed in the "Nationality" table.

Field Name   | Data Type | Key Type | Default Value | Additional Constraints | Description 
------------ | --------- | -------- | ------------- | ---------------------- | -----------
AffilTypeID  | INTEGER   | Primary  | N/A           | NOT NULL               | (RowID) ID of the Affilliation Type
AffilTypeTxt | TEXT      | N/A      | ''            | NOT NULL, UNIQUE       | Affilliation Type in full

### Predefined Contents

This table has predefined contents as follows:

AffilTypeID | AffilTypeTxt
----------- | ------------
1           | Tribe
2           | National Citizen
3           | Freedom to Remain
4           | Working Visa
5           | Permanent Residency
6           | Other


## AdoptFoster Table

The "AdoptFoster" table details the adoption or fosterhood of a person by a relationship.  Note that this can be a dummy relationship for adoption by single people not in relationships.

Field Name     | Data Type | Key Type | Default Value | Additional Constraints                             | Description 
-------------- | --------- | -------- | ------------- | -------------------------------------------------- | -----------
AdoptFosterID  | INTEGER   | Primary  | N/A           | NOT NULL                                           | (RowID) Adoption/Fosterhood ID
ByRelationship | INTEGER   | Foreign  | NULL          | Links to Relationship(RelID)                       | Adopted by Relationship ID (or NULL if unknown)
PersonID       | INTEGER   | Foreign  | N/A           | NOT NULL, Links to Person(PersonID)                | The person who was adopted
AdFosDate      | INTEGER   | Foreign  | N/A           | NOT NULL, Links to Date(DateID)                    | The date of the Adoption/Fosterhood.  Linked values are '?' if unknown
AdoptTypeID    | INTEGER   | Foreign  | 1             | NOT NULL, Links to AdoptType(AdoptTypeID)          | The type of adoption or fosterhood
Description    | TEXT      | N/A      | ''            | NOT NULL                                           | Description of the Adoption/Fosterhood
PersonTypeID   | INTEGER   | Foreign  | 0             | NOT NULL, Links to AdoptPersonType(AdPersonTypeID) | Code for the person in the relationship adopting/fostering this person.
SourceID       | INTEGER   | Foreign  | NULL          | Links to Source(SourceID)                          | Source (or NULL if none)


## AdoptPersonType Table

The "AdoptPersonType" table details the who in a relationship is adopting/fostering a child, as descripted in the "AdoptFoster" table.

Field Name       | Data Type | Key Type | Default Value | Additional Constraints | Description 
---------------- | --------- | -------- | ------------- | ---------------------- | -----------
AdPersonTypeID   | INT       | Primary  | N/A           | NOT NULL, UNIQUE       | ID for person adopted type
AdPersonTypeDesc | TEXT      | N/A      | ''            | NOT NULL, UNIQUE       | Description of person adopted type

Note that since AdPersonTypeID is *NOT* the RowID, to avoid it being declared as such, the datatype *MUST* be written as `INT`.

### Predefined Contents

This table has predefined contents as follows:

AdPersonTypeID | AdPersonTypeDesc
-------------- | ----------------
0              | Unknown
1              | Person 1 Only
2              | Person 2 Only
3              | Both people in relationship


## AdoptType Table

The "AdoptType" table describes the type of adoption detailed in the "AdoptFoster" table.

Field Name   | Data Type | Key Type | Default Value | Additional Constraints | Description 
------------ | --------- | -------- | ------------- | ---------------------- | -----------
AdoptTypeID  | INTEGER   | Primary  | N/A           | NOT NULL               | (RowID) Adoption/Fosterhood Type ID
AdoptTypeTxt | TEXT      | N/A      | ''            | NOT NULL, UNIQUE       | Full Adoption/Fosterhood Type

### Predefined Contents

This table has predefined contents as follows:

AdoptTypeID | AdoptTypeTxt
----------- | ------------
1           | Legal Adoption
2           | Informal Adoption
3           | Fosterhood
4           | Other



## Relationship Table

The "Relationship" table describes details of the relationship between two people.  If a relationship includes more than two people (e.g. a polyamorous romantic relationship), multiple records will need to be entered in the "Relationship" table.

Field Name   | Data Type | Key Type | Default Value | Additional Constraints                      | Description 
------------ | --------- | -------- | ------------- | ------------------------------------------- | -----------
RelID        | INTEGER   | Primary  | N/A           | NOT NULL                                    | (RowID) ID of the relationship
Person1ID    | INTEGER   | Foreign  | N/A           | NOT NULL, Links to Person(PersonID)         | First person in relationship
Person2ID    | INTEGER   | Foreign  | NULL          | Links to Person(PersonID)                   | Second person in relationship or NULL if unknown (or also NULL if N/A in adoption)
RelTypeID    | INTEGER   | Foreign  | 7             | NOT NULL, Links to RelType(RelTypeID)       | The type of relationship
IsRomantic   | INTEGER   | N/A      | 1             | 0 or 1 or NULL                              | Is the relationship a romantic or a platonic one? (NULL if unknown)
Description  | TEXT      | N/A      | ''            | NOT NULL                                    | Description of the Relationship
RelEndTypeID | INTEGER   | Foreign  | 4             | NOT NULL, Links to RelEndType(RelEndTypeID) | The way the relationship ended
RelEndDesc   | TEXT      | N/A      | ''            | NOT NULL                                    | Description of how the relationship ended
ConvRelID    | INTEGER   | Foreign  | NULL          | Links to Relationship(RelID)                | If the Relationship End Type ID is 3 (converted), this links to the relationship it converted into.
Notes        | TEXT      | N/A      | NULL          |                                             | Any notes, however unofficial, on this relationship
SourceID     | INTEGER   | Foreign  | NULL          | Links to Source(SourceID)                   | Source (or NULL if none)


## RelType Table

The "RelType" table describes the type of relationship detailed in the "Relationship" table.  It includes a "Dummy" type for use in single-person adoptions.

Field Name   | Data Type | Key Type | Default Value | Additional Constraints | Description 
------------ | --------- | -------- | ------------- | ---------------------- | -----------
RelTypeID    | INTEGER   | Primary  | N/A           | NOT NULL               | (RowID) ID of the Relationship Type
RelTypeTxt   | TEXT      | N/A      | ''            | NOT NULL, UNIQUE       | Full Relationship Type
CanHaveChild | INTEGER   | N/A      | 1             | NOT NULL, 0 or 1       | Can this relationship type give birth to a child?

### Predefined Contents

This table has predefined contents as follows:

RelTypeID | RelTypeTxt                   | CanHaveChild | *Comment*
--------- | ---------------------------- | ------------ | ---------
1         | Sperm/Egg donor              | 1            | 
2         | Surrogate                    | 1            | 
3         | Very Short Term Relationship | 1            | *(e.g. one-night's stand)*
4         | Short Term Relationship      | 1            | 
5         | Long Term Relationship       | 1            | 
6         | Formal Engagement            | 1            | 
7         | Marriage                     | 1            | 
8         | Civil Partnership            | 1            | 
9         | Friends                      | 0            | 
10        | Sexual Friendship            | 1            | *(Friends with benefits)*
11        | Godparent/Godchild           | 0            | *(Person 1 is Godparent, Person 2 is Godchild)*
12        | Witness at marriage          | 0            | *(Person 1 is wed, Person 2 is Witness)*
13        | Other                        | 1            | 
14        | (Dummy)                      | 0            | *(Dummy relationship for single-person adoptions)*


## RelEndType Table

The "RelEndType" table describes the way a relationship (detailed in the "Relationship" table) has ended.  If the type is 3 ("Converted"), the relationship table includes the relationship the current one converted into.

Field Name    | Data Type | Key Type | Default Value | Additional Constraints | Description 
------------- | --------- | -------- | ------------- | ---------------------- | -----------
RelEndTypeID  | INTEGER   | Primary  | N/A           | NOT NULL               | (RowID) ID of the Relationship End Type
RelEndTypeTxt | TEXT      | N/A      | ''            | NOT NULL, UNIQUE       | Full Relationship End Type

### Predefined Contents

This table has predefined contents as follows:

RelEndTypeID | RelEndTypeTxt | *Comment*
------------ | ------------- | ---------
1            | Split         | *(e.g. Divorce, Anullment, etc)*
2            | Death         | *(of a Participant)*
3            | Converted     | *(into another relationship type)*
4            | Unknown       |   


## Job Table

The "Job" table details the form of employment a person has.  This employment may be paid or unpaid.

Field Name     | Data Type | Key Type | Default Value | Additional Constraints              | Description 
-------------- | --------- | -------- | ------------- | ----------------------------------- | -----------
JobID          | INTEGER   | Primary  | N/A           | NOT NULL                            | (RowID) ID of Job
PersonID       | INTEGER   | Foreign  | N/A           | NOT NULL, Links to Person(PersonID) | Person whose job this is
JobTitle       | TEXT      | N/A      | ''            | NOT NULL                            | Title of Job
JobDescription | TEXT      | N/A      | ''            | NOT NULL                            | Description of Job
PayDescription | TEXT      | N/A      | '?'           | NOT NULL                            | Description of Pay or '?' if pay is unknown
StartDate      | INTEGER   | Foreign  | N/A           | NOT NULL, Links to Date(DateID)     | Date the person started the Job
EndDate        | INTEGER   | Foreign  | NULL          | Links to Date(DateID)               | Date the person ended the job (or NULL if the job is still ongoing)
Comments       | TEXT      | N/A      | NULL          |                                     | Comments on the Job
SourceID       | INTEGER   | Foreign  | NULL          | Links to Source(SourceID)           | Source (or NULL if none)


## Address Table

The "Address" table details the addresses of various locations.

Field Name     | Data Type | Key Type | Default Value | Additional Constraints    | Description 
-------------- | --------- | -------- | ------------- | ------------------------- | -----------
AddrID         | INTEGER   | Primary  | N/A           | NOT NULL                  | (RowID) ID of Address
Description    | TEXT      | N/A      | ''            | NOT NULL                  | Description of Address
AddrNameNum    | TEXT      | N/A      | '?'           | NOT NULL                  | Address Name or Number (or '?' if unknown)
AddrLine2      | TEXT      | N/A      | NULL          |                           | Address Line 2 (or NULL if not applicable)
AddrRoad       | TEXT      | N/A      | NULL          |                           | Address Road (or '?' if unknown, or NULL if not applicable)
AddrSettlement | TEXT      | N/A      | NULL          |                           | Settlement (or '?' if unknown, or NULL if not applicable)
AddrBorough    | TEXT      | N/A      | NULL          |                           | Borough  (or '?' if unknown, or NULL if not applicable)
AddrCounty     | TEXT      | N/A      | NULL          |                           | County  (or '?' if unknown, or NULL if not applicable)
AddrState      | TEXT      | N/A      | NULL          |                           | State  (or '?' if unknown, or NULL if not applicable)
AddrPostCode   | TEXT      | N/A      | NULL          |                           | Postal Code or similar  (or '?' if unknown, or NULL if not applicable)
AddrCountry    | TEXT      | N/A      | NULL          |                           | Country  (or '?' if unknown, or NULL if not applicable)
Latitude       | REAL      | N/A      | NULL          |                           | Latitude (or NULL if unknown)
Longitude      | REAL      | N/A      | NULL          |                           | Longitude (or NULL if unknown)
FixTelNum      | TEXT      | N/A      | NULL          |                           | Fixed (land-line) Telephone Number  (or '?' if unknown, or NULL if not applicable)
Notes          | TEXT      | N/A      | NULL          |                           | Any notes, even if unofficial
SourceID       | INTEGER   | Foreign  | NULL          | Links to Source(SourceID) | Source (or NULL if none)


## LivingAddr Table

The "LivingAddr" table details the residency of a person (from the "Person" table) at an address (from the "Address" table).

Field Name   | Data Type | Key Type | Default Value | Additional Constraints              | Description 
------------ | --------- | -------- | ------------- | ----------------------------------- | -----------
LivingAddrID | INTEGER   | Primary  | N/A           | NOT NULL                            | (RowID) ID of Living Address
PersonID     | INTEGER   | Foreign  | N/A           | NOT NULL, Links to Person(PersonID) | Person whose address this is
AddrID       | INTEGER   | Foreign  | N/A           | NOT NULL, Links to Address(AddrID)  | The Address
ArrivedDate  | INTEGER   | Foreign  | N/A           | NOT NULL, Links to Date(DateID)     | Date the person arrived at the address
LeftDate     | INTEGER   | Foreign  | NULL          | Links to Date(DateID)               | Date the person left the address, or NULL if they haven't left yet
Notes        | TEXT      | N/A      | NULL          |                                     | Any notes, however unofficial, on this residence
SourceID     | INTEGER   | Foreign  | NULL          | Links to Source(SourceID)           | Source (or NULL if none)


## WorkingAddr Table

The "WorkingAddr" table details the addresses at which a person's employment is situated.  A person's job can change addresses over the course of their employment in the same role with the same firm.  Additionally, some jobs may move regularly (such as construction work) and/or be situated in multiple sites at once.  The separation of this table from the "Job" table allows for all these conditions.

Field Name     | Data Type | Key Type | Default Value | Additional Constraints             | Description 
-------------- | --------- | -------- | ------------- | ---------------------------------- | -----------
WorkingAddrID  | INTEGER   | Primary  | N/A           | NOT NULL                           | (RowID) ID of Working Address
JobID          | INTEGER   | Foreign  | N/A           | NOT NULL, Links to Job(JobID)      | Job at this address
AddrID         | INTEGER   | Foreign  | N/A           | NOT NULL, Links to Address(AddrID) | The Address
ArrivedDate    | INTEGER   | Foreign  | N/A           | NOT NULL, Links to Date(DateID)    | Date the job arrived at the address
LeftDate       | INTEGER   | Foreign  | NULL          | Links to Date(DateID)              | Date the job left the address, or NULL if it hasn't left yet
Description    | TEXT      | N/A      | ''            | NOT NULL                           | Description of this job being at this address
Notes          | TEXT      | N/A      | NULL          |                                    | Any notes, however unofficial, about this job at this address
SourceID       | INTEGER   | Foreign  | NULL          | Links to Source(SourceID)          | Source (or NULL if none)


## OtherContact Table

The "OtherContact" table stores additional contact details for a person.

Field Name     | Data Type | Key Type | Default Value | Additional Constraints                        | Description 
-------------- | --------- | -------- | ------------- | --------------------------------------------- | -----------
OtherContactID | INTEGER   | Primary  | N/A           | NOT NULL                                      | (RowID) ID of the Other Contact-type Address
PersonID       | INTEGER   | Foreign  | N/A           | NOT NULL, Links to Person(PersonID)           | Person whose contact details these are
StartDate      | INTEGER   | Foreign  | N/A           | NOT NULL, Links to Date(DateID)               | Date the person gained these contact details
EndDate        | INTEGER   | Foreign  | NULL          | Links to Date(DateID)                         | Date the person lost these contact details, or NULL if they are still valid
ContactTypeID  | INTEGER   | Foreign  | 1             | NOT NULL, Links to ContactType(ContactTypeID) | Type of Contact Details
Description    | TEXT      | N/A      | ''            | NOT NULL                                      | Description of contact details
Data           | TEXT      | N/A      | ''            | NOT NULL                                      | Contact Details
Notes          | TEXT      | N/A      | NULL          |                                               | Any notes, however unofficial, on these contact details
SourceID       | INTEGER   | Foreign  | NULL          | Links to Source(SourceID)                     | Source (or NULL if none)


## ContactType Table

The "ContactType" table details the types of contact that are stored in the "OtherContact" table.

Field Name     | Data Type | Key Type | Default Value | Additional Constraints | Description 
-------------- | --------- | -------- | ------------- | ---------------------- | -----------
ContactTypeID  | INTEGER   | Primary  | N/A           | NOT NULL               | (RowID) ID of the Contact Type
ContactTypeTxt | TEXT      | N/A      | ''            | NOT NULL               | Full Contact Type

### Predefined Contents

This table has predefined contents as follows:

ContactTypeID | ContactTypeTxt
------------- | --------------
1             | Fixed Telephone Number
2             | Mobile Telephone Number
3             | Facsimile Number
4             | Telex/Teleprinter Number
5             | E-Mail Address
6             | Web Address
7             | Instant Messaging Address
8             | VOIP Number/Address
9             | Social Media Account
10            | Other


## EducAchieve Table

The "EducAchieve" table describes the Educational Achievements (qualifications, etc) of a person.

Field Name    | Data Type | Key Type | Default Value | Additional Constraints              | Description 
------------- | --------- | -------- | ------------- | ----------------------------------- | -----------
EducAchieveID | INTEGER   | Primary  | N/A           | NOT NULL                            | (RowID) Educational Achievement ID
PersonID      | INTEGER   | Foreign  | N/A           | NOT NULL, Links to Person(PersonID) | Person who achieved it
AchieveDate   | INTEGER   | Foreign  | N/A           | NOT NULL, Links to Date(DateID)     | Date they achieved it
QualName      | TEXT      | N/A      | ''            | NOT NULL                            | Qualification Name
QualType      | TEXT      | N/A      | ''            | NOT NULL                            | Qualification Type
Subject       | TEXT      | N/A      | NULL          |                                     | Subject of Qualification, or NULL if too general
Description   | TEXT      | N/A      | ''            | NOT NULL                            | Description of Qualification
PlaceName     | TEXT      | N/A      | NULL          |                                     | Place qualification was achieved or NULL if not applicable
PlaceAddrID   | INTEGER   | Foreign  | NULL          | Links to Address(AddrID)            | Address of the place (or NULL if Unknown, also NULL if unapplicable (above will be NULL too in this case)).
Notes         | TEXT      | N/A      | NULL          |                                     | Any notes, however unofficial, about this qualification
SourceID      | INTEGER   | Foreign  | NULL          | Links to Source(SourceID)           | Source (or NULL if none)


## Event Table

The "Event" table details the events that take place throughout a person's life that aren't covered by the other tables.

Field Name    | Data Type | Key Type | Default Value | Additional Constraints              | Description 
------------- | --------- | -------- | ------------- | ----------------------------------- | -----------
EventID       | INTEGER   | Primary  | N/A           | NOT NULL                            | (RowID) Event ID
PersonID      | INTEGER   | Foreign  | N/A           | NOT NULL, Links to Person(PersonID) | Person to whom the event occurred
EventDate     | INTEGER   | Foreign  | N/A           | NOT NULL, Links to Date(DateID)     | Date of the Event
AgeAtEvent    | INTEGER   | Foreign  | NULL          | Links to Age(AgeID)                 | Person's age at event
Description   | TEXT      | N/A      | ''            | NOT NULL                            | Short description of event
Details       | TEXT      | N/A      | NULL          |                                     | Event Details (or NULL if not applicable)
PlaceName     | TEXT      | N/A      | NULL          |                                     | Place the where the event occurred (or NULL if not applicable)
PlaceAddr     | INTEGER   | Foreign  | NULL          | Links to Address(AddrID)            | Address of place the where the event occurred (or NULL if not applicable)
Notes         | TEXT      | N/A      | NULL          |                                     | Any notes, however unofficial, about this event
SourceID      | INTEGER   | Foreign  | NULL          | Links to Source(SourceID)           | Source (or NULL if none)


## Source Table

The "Source" table stores details of the sources referred to by other tables.  Note that some data will be stored externally in files whose path is stored in this table.

Field Name    | Data Type | Key Type | Default Value | Additional Constraints                      | Description 
------------- | --------- | -------- | ------------- | ------------------------------------------- | -----------
SourceID      | INTEGER   | Primary  | N/A           | NOT NULL                                    | (RowID) Source ID
SourceTypeID  | INTEGER   | Foreign  | N/A           | NOT NULL, Links to SourceType(SourceTypeID) | The type of source
Description   | TEXT      | N/A      | ''            | NOT NULL                                    | Description of the source
FromDate      | INTEGER   | Foreign  | N/A           | NOT NULL, Links to Date(DateID)             | The date from which the work, well, dates.  Links to '?' if unknown.
FromWork      | TEXT      | N/A      | NULL          |                                             | Work the source is from (e.g. a book, etc).  If not applicable, is NULL.
SectPagePara  | TEXT      | N/A      | NULL          |                                             | Section, Page and Paragraph reference or at least some way of locating source in work, or NULL if not applicable.
Location      | TEXT      | N/A      | NULL          |                                             | Location of source (e.g. a library) or NULL if not applicable.
LocationAddr  | INTEGER   | Foreign  | NULL          | Links to Address(AddrID)                    | Address of source location, or NULL if not applicable or unknown (if unknown, above field won't be NULL).
CallNum       | TEXT      | N/A      | NULL          |                                             | Call Number or other reference form for work in location, or '?' if unknown, or NULL if not applicable.
LangCodeMajor | TEXT      | Foreign  | NULL          | Links to ISO6393(Code3)                     | ISO 639-3 code (Major part), or NULL if not applicable.
LangCodeMinor | TEXT      | N/A      | NULL          |                                             | ISO 639-3 code (Minor/National part), or NULL if not applicable.
AccessedDate  | INTEGER   | Foreign  | N/A           | NOT NULL, Links to Date(DateID)             | The date the source was accessed and/or details added to the database.
InternalData  | TEXT      | N/A      | NULL          |                                             | Typed Data from source, or NULL if this cannot be done.
ExternalData  | TEXT      | N/A      | NULL          |                                             | Path of external file containing source data, or NULL if not applicable
ExtDataMIME   | TEXT      | N/A      | NULL          |                                             | MIME type of External Data file, or NULL if not applicable.
Reliability   | INTEGER   | N/A      | NULL          |                                             | Reliability assessment of data in a scale of 0 to 3, or NULL if unknown.
Notes         | TEXT      | N/A      | NULL          |                                             | Any notes, however unofficial, about the source.


## SourceType Table

The "SourceType" table details the type of information stored in the "Source" table.

Field Name     | Data Type | Key Type | Default Value | Additional Constraints | Description 
-------------- | --------- | -------- | ------------- | ---------------------- | -----------
SourceTypeID   | INTEGER   | Primary  | N/A           | NOT NULL               | (RowID) Source Type ID
SourceTypeTxt  | TEXT      | N/A      | ''            | NOT NULL, UNIQUE       | The full Source Type

### Predefined Contents

This table has predefined contents as follows:

SourceTypeID | SourceTypeTxt
------------ | -------------
1            | Memory
2            | Record
3            | Image
4            | Audio
5            | Video
6            | Other


## Multimedia Table

The "Multimedia" table details external multimedia files that aren't being used as sources.

Field Name   | Data Type | Key Type | Default Value              | Additional Constraints          | Description 
------------ | --------- | -------- | -------------------------- | ------------------------------- | -----------
MultimediaID | INTEGER   | Primary  | N/A                        | NOT NULL                        | (RowID) ID of Other Multimedia
Description  | TEXT      | N/A      | ''                         | NOT NULL                        | Description of the Multimedia
OriginDate   | INTEGER   | Foreign  | N/A                        | NOT NULL, Links to Date(DateID) | Original date of the Multimedia
Data         | TEXT      | N/A      | ''                         | NOT NULL                        | Path of external data file containing Multimedia
DataMIME     | TEXT      | N/A      | 'application/octet-stream' | NOT NULL                        | MIME type of the external data file
Notes        | TEXT      | N/A      | NULL                       |                                 | Any notes, however unofficial, on the Multimedia


## InMultimedia Table

The "InMultimedia" table Describes the people (in the "Person" table) appearing in a particular piece of multimedia (in the "Multimedia" table).  This table gets over the Many-To-Many relationship of this situation.

Field Name     | Data Type | Key Type | Default Value | Additional Constraints                      | Description 
-------------- | --------- | -------- | ------------- | ------------------------------------------- | -----------
InMultimediaID | INTEGER   | Primary  | N/A           | NOT NULL                                    | (RowID) ID of Multimedia Assignment
MultimediaID   | INTEGER   | Foreign  | N/A           | NOT NULL, Links to Multimedia(MultimediaID) | ID of the Other Multimedia
PersonID       | INTEGER   | Foreign  | N/A           | NOT NULL, Links to Person(PersonID)         | ID of the person featured in the Multimedia
Notes          | TEXT      | N/A      | NULL          |                                             | Any notes, however unofficial, about the person's role in the Multimedia


## ISO6393 Table

The "ISO6393" table stores possible ISO-639-3 Language codes that can be assigned to sources.

Field Name | Data Type | Key Type | Default Value | Additional Constraints                           | Description 
---------- | --------- | -------- | ------------- | ------------------------------------------------ | -----------
Code3      | TEXT      | Primary  | N/A           | NOT NULL, UNIQUE                                 | The ISO-639-3 code
Code2B     | TEXT      | N/A      | NULL          |                                                  | The ISO-639-2 Bibliographic code (or NULL if not available)
Code2T     | TEXT      | N/A      | NULL          |                                                  | The ISO-639-2 Terminology code (or NULL if not available)
Code1      | TEXT      | N/A      | NULL          |                                                  | The ISO-639-1 code (or NULL if not available)
ScopeCode  | TEXT      | N/A      | 'I'           | NOT NULL, 'I' or 'M' or 'S'                      | Single letter specifying the scope of the language code: I(ndividual), M(acrolanguage), S(pecial)
TypeCode   | TEXT      | N/A      | 'L'           | NOT NULL, 'A' or 'C' or 'E' or 'H' or 'L' or 'S' | Single letter specifying the type of the language code: A(ncient), C(onstructed), E(xtinct), H(istorical), L(iving), S(pecial)
RefName    | TEXT      | N/A      | ''            | NOT NULL                                         | The reference name of the language
Comment    | TEXT      | N/A      | NULL          |                                                  | Any comments about the language.

### Predefined Contents

This table has predefined contents.  See the external file 'iso-6393-3.tab' for the data.


## MetaInfo Table

The "MetaInfo" stores metadata about the database.

Field Name | Data Type | Key Type | Default Value | Additional Constraints                           | Description 
---------- | --------- | -------- | ------------- | ------------------------------------------------ | -----------
Element    | TEXT      | Primary  | N/A           | NOT NULL, UNIQUE                                 | Meta Information Element
Value      | TEXT      | N/A      | NULL          |                                                  | Meta Information Value

### Predefined Contents

This table has predefined contents as follows:

Element   | Value | *Comment*
--------- | ----- | ---------
Version   |       | *Program version that made the database*
Last_Edit |       | *Date/time of last edit in YYYY-MM-DD HH:MM:SS +ZZZZ format*



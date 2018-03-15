# genie-machine
The Genie Machine is/will be a simple cross-platform Genealogy Database program written in C.

## Motivations

### Why Both Windows and Linux?

While Windows is the most popular Operating System, the Genie Machine will also work on Linux.  A large proportion of those documenting their family-history will be retired as age tends to focus the mind on these things.  They will thus have a reduced income.  A decent Windows PC is a substantial investment for those on a budget.  However, most people already have a TV and therefore the Raspberry Pi is a reasonably priced alternative.  The Raspberry Pi primarily runs Linux, thus it makes sense to use technologies for the Genie Machine that can run on both platforms.

## Compilation Dependencies

```
IUP
->CD
  ->IM
    ->Lua (Development files)
->iupmatrixex.h from previous version
->srcmatrixex/* from previous version
->Webkit Development files?
SQLite3
```
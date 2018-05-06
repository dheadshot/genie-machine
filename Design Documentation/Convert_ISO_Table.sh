#!/bin/bash
# This script converts the ISO 639-3 Table into an SQL "INSERT" statement.
cat iso-639-3.tab | cut -f1 | sed "s/.*/\(\'&\'\,/" > isot1.txt
cat iso-639-3.tab | cut -f2 | sed "s/.*/\'&\'\,/" | sed "s/'',/NULL,/" > isot2.txt
cat iso-639-3.tab | cut -f3 | sed "s/.*/\'&\'\,/" | sed "s/'',/NULL,/" > isot3.txt
cat iso-639-3.tab | cut -f4 | sed "s/.*/\'&\'\,/" | sed "s/'',/NULL,/" > isot4.txt
cat iso-639-3.tab | cut -f5 | sed "s/.*/\'&\'\,/" > isot5.txt
cat iso-639-3.tab | cut -f6 | sed "s/.*/\'&\'\,/" > isot6.txt
cat iso-639-3.tab | cut -f7 | sed "s/'/''/g" | sed "s/.*/\'&\'\,/" > isot7.txt
cat iso-639-3.tab | cut -f8 | sed "s/'/''/g" | sed "s/.*/\'&\'\)\,/" | sed "$ s/,$/;/" | tr -d '\r' > isot8.txt
echo 'INSERT INTO ISO6393' > isoval.txt
echo '(Code3, Code2B, Code2T, Code1, ScopeCode, TypeCode, RefName, Comment)' >> isoval.txt
echo 'VALUES' >> isoval.txt
paste -d ' ' isot1.txt isot2.txt isot3.txt isot4.txt isot5.txt isot6.txt isot7.txt isot8.txt | tail -n +2 >> isoval.txt
mv isoval.txt ISO_639-3_Values.txt
rm isot1.txt isot2.txt isot3.txt isot4.txt isot5.txt isot6.txt isot7.txt isot8.txt
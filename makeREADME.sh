#!/bin/sh

sed '/^```$/d
/badge\.svg/{N;d;}
/^\*\*Attention/d
s/DWITH\\_TESTS/DWITH_TESTS/
1 {
	s/^# //
}
' README.md > README

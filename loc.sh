#/bin/sh
# much improved by Jonathan Leffler on stackoverflow

git diff --stat `git hash-object -t tree /dev/null` |
# not my code
grep -v "^ kazmath" |
grep -E -v "\.(png|gbo|obj|txt|md|blend) " |
# not my code
grep -v "obj2opengl\.pl " |
sed -e 's/+*$//' -e '$d' |
cut -d '|' -f 2 |
awk '{sum += $1 } END { print "lines of code " sum }'


rm -r build
rm -r  out

rm -r lex.yy.c
rm -r parser.tab.c
rm -r parser.tab.h



cd test
cd test
find . -type f -name "*.dot" -exec rm -f {} +
find . -type f -name "*.png" -exec rm -f {} +
find . -type f -name "*.s" -exec rm -f {} +

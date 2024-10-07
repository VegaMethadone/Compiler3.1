#!/bin/bash





echo "test-1"
./out/parser.exe ./test/test1.txt
dot -Gcharset=latin1 -Tpng ./test/test1.dot -o ./test/graph1.png


echo "test-2"
./out/parser.exe ./test/test2.txt
dot -Gcharset=latin1 -Tpng ./test/test2.dot -o ./test/graph2.png


echo "test-3"
./out/parser.exe ./test/test3.txt
dot -Gcharset=latin1 -Tpng ./test/test3.dot -o ./test/graph3.png


echo "test-4"
./out/parser.exe ./test/test4.txt
dot -Gcharset=latin1 -Tpng ./test/test4.dot -o ./test/graph4.png


echo "test-5"
./out/parser.exe ./test/test5.txt
dot -Gcharset=latin1 -Tpng ./test/test5.dot -o ./test/graph5.png


echo "test-6"
./out/parser.exe ./test/test6.txt
dot -Gcharset=latin1 -Tpng ./test/test6.dot -o ./test/graph6.png

echo "test-7"
./out/parser.exe ./test/test7.txt
dot -Gcharset=latin1 -Tpng ./test/test7.dot -o ./test/graph7.png



cd test
#find . -type f -name "*.dot" -exec rm -f {} +
#find . -type f -name "*.png" -exec rm -f {} +

echo "Done"
sleep 15

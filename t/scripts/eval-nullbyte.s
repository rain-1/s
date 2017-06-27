eval X 'echo -n 0 | tr 0 \'\\\\0\''
echo $X
eval X 'echo -n 00000000000000 | tr 0 \'\\\\0\''
echo $X
eval X 'echo -n 10200300040000500006 | tr 0 \'\\\\0\''
echo $X
eval X 'echo -n 10200300040000500006000000000007 | tr 0 \'\\\\0\''
echo $X
eval X 'echo -n 010101010101010 | tr 0 \'\\\\0\''
echo $X
eval X 'echo -n 101010101010101 | tr 0 \'\\\\0\''
echo $X

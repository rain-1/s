echo 'echo $0 $1 $2 $#' | > tmp.s
source tmp.s foo bar
source tmp.s cat dog fish
rm -f tmp.s

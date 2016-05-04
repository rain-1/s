true && echo ok1
false && echo bad1
true && true && echo ok2
true && false && echo bad2
false || echo ok3
true || echo bad3
false || false || echo ok4
false || true || echo bad4
false && echo bad5 || echo ok5

// test case p6
// scalar and tensor

type T1 : [3 3]
type T2 : [3 3]
type scalar : []

var in a : T1
var in b : T2
var c : [3 3]
var d : scalar

c = a + b
c = a + d


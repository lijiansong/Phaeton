// test case 5

type tensor : [1 1 1 2]
type tensor3 : [12 34 56]
type tensor4 : [34 12 27 43]
type tensor6 : [12 13 14 24 28 42]

var in a : tensor3
var in b : tensor3
var in c : tensor4
var in d : tensor6
var in out x : tensor
var in out A : tensor
var out v : tensor
var W : [32 3 96 48]

v = a * b * c . [[2 3] [6 8]]
v = d * b * c . [[2 3] [6 8]]

a = a + b
a = a - b
a = a / b
a = a # b
a = a # b . [[1 2] [1 2]]

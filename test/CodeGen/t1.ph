// tensor product

type tensor3  : [4 5 6]
type tensor4  : [1 2 3 4]
type tensor10 : [4 5 6 4 5 6 1 2 3 4]

var in a : tensor3
var in b : tensor3
var in c : tensor4
var out d : tensor10

d = a # b # c

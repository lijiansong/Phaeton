// tensor product and contraction

type tensor3 : [12 24 11]
type tensor4 : [24 11 22 64]
type tensor6 : [11 24 22 64]

var a : tensor3
var b : tensor3
var c : tensor4
var d : tensor6

d = (a # b # c) . [[1 6] [5 7] [0 3]]


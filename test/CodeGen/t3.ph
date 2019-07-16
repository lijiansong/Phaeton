// tensor product and contraction

type tensor3 : [12 34 17]
type tensor4 : [34 17 28 42]
type tensor6 : [17 34 28 42]

var a : tensor3
var b : tensor3
var c : tensor4
var d : tensor6

d = a # b # c . [[1 6] [5 7] [0 3]]


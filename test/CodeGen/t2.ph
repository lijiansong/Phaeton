// tensor product and contraction

type tensor3 : [12 34 17]
type tensor4 : [34 17 4 8]

var a : tensor3
var b : tensor4
var c : [12 4 8]

c = (a # b) . [[2 4] [1 3]]


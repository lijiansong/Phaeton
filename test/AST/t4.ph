// test case 4

type scalar : []
type vector : [11]
type matrix : [12 34]
type tensor : [32 3 96 96]

var in a : scalar
var in b : vector
var in c : matrix
var in d : tensor
var out v : tensor
var in W : [32 3 96 48]

b = b + b
c = c * c

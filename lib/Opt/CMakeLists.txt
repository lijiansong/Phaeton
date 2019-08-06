add_library(Opt STATIC
            ExprTree.cpp
            ExprTreeTransformer.cpp
            ExprTreeVisitor.cpp
            ENBuilder.cpp
            IdentifierCopier.cpp
            StackExprRemover.cpp
            ExprTreeLoopLifter.cpp)

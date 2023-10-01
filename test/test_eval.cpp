#include <test/scheme_test.h>

TEST_CASE_METHOD(SchemeTest, "Quote") {
    ExpectEq("(quote (1 2))", "(1 2)");
    ExpectEq("'(1 2)", "(1 2)");

    ExpectEq("(quote (1 . 2))", "(1 . 2)");
    ExpectEq("(quote '1)", "(quote 1)");
    ExpectEq("'(+ 1 2)", "(+ 1 2)");
    ExpectEq("(quote 1)", "1");
    ExpectEq("(quote (1 2))", "(1 2)");
    ExpectEq("(quote (quote 1))", "(quote 1)");

    ExpectEq("'()", "()");
    ExpectEq("'(1)", "(1)");

    ExpectEq("'(1 2 . 3)", "(1 2 . 3)");
    ExpectEq("'(1 2 . ())", "(1 2)");
    ExpectEq("'(1 . (2 . ()))", "(1 2)");
}

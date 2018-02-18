
/*
 * apply same function n times
 * applyN(3, f, x) => f(f(f(x)))
 */
let rec applyN = (n, f, x) => n <= 0 ? x : applyN(n - 1, f, f(x));
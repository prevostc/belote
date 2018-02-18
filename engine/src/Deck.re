type color = Spades | Hearts | Diamonds | Clubs;
type motif = Value(int);
type card = {
    color: color,
    motif: motif,
};

let king = Value(13);
let queen = Value(12);
let jack = Value(11);
let ace = Value(1);

let newDeck = () => ListUtil.product(
    [Spades, Hearts, Diamonds, Clubs],
    [king, queen, jack, ace, Value(7), Value(8), Value(9), Value(10)])
    |> List.map(((color, motif)) => { color, motif })
;

let cut = (position, deck) => ListUtil.slice(position, List.length(deck), deck) @ ListUtil.slice(0, position - 1, deck);
let cardEquals = (cA, cB) => cA.color === cB.color && cA.motif === cB.motif;

/* todo: compare with trumps */
let motifCompare = (mA, mB) => switch ((mA, mB)) {
    | (Value(a), Value(b)) => compare(a, b)
};
let motifGreaterThan = (mA, mB) => switch ((mA, mB)) {
    | (Value(a), Value(b)) => b > a
};
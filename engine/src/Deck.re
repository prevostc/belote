
type color = Spades | Hearts | Diamonds | Clubs;
/* I used all cards just because it makes debugging easy */
type motif = Joker | Ace | V2 | V3 | V4 | V5 | V6 | V7 | V8 | V9 | V10 | Jack | Queen | King;
type card = {
    color: color,
    motif: motif,
};

let newDeck = () => ListUtil.product(
    [Spades, Hearts, Diamonds, Clubs],
    [King, Queen, Jack, Ace, V7, V8, V9, V10])
    |> List.map(((color, motif)) => { color, motif })
;

let cut = (position, deck) => ListUtil.slice(position, List.length(deck), deck) @ ListUtil.slice(0, position - 1, deck);
let cardEquals = (cA, cB) => cA.color === cB.color && cA.motif === cB.motif;

/* A K Q J 10 9 8 7 */
let pokerOrder = [Ace, King, Queen, Jack, V10, V9, V8, V7];

/* J 9 A 10 K Q 8 7 */
let trumpOrder = [Jack, V9, Ace, V10, King, Queen, V8, V7];

/* A 10 K Q J 9 8 7 */
let nonTrumpOrder = [Ace, V10, King, Queen, Jack, V9, V8, V7];

let motifCompare = (order, motifA, motifB): int => {
    let getIdx = (motif, order) => order |> ListUtil.indexOf(motif, (===));
    let length = order |> List.length;
    let indexes = (order |> getIdx(motifA), order |> getIdx(motifB));
    switch (indexes) {
        | (Some(idxA), Some(idxB)) => compare(length - idxA, length - idxB)
        | (None, Some(_)) => 1
        | (Some(_), None) => -1
        | (None, None) => 0
    }
};

let sortMotifs = (order, lst) => lst |> List.sort(motifCompare(order));

/* true if B is strictly greater than A */
let motifGreaterThan = (order, mA, mB) => motifCompare(order, mA, mB) < 0;
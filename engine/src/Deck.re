
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

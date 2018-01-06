module Deck {
    include Util;

    type color = Spades | Hearts | Diamonds | Clubs;
    type motif = King | Queen | Jack | Ace | Value(int);
    type card = {
        color: color,
        motif: motif,
    };

    let newDeck = () => Util.listProduct(
        [Spades, Hearts, Diamonds, Clubs], 
        [King, Queen, Jack, Ace, Value(7), Value(8), Value(9), Value(10)])
        |> List.map(((color, motif)) => { color, motif })
    ;

    let cut = (position, deck) => Util.listSlice(position, List.length(deck), deck) @ Util.listSlice(0, position - 1, deck);
    let cmpCard = (cA, cB) => cA.color === cB.color && cA.motif === cB.motif;
}
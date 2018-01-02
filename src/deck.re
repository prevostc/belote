module Deck {
    type color = Spades | Hearts | Diamonds | Clubs;
    type motif = King | Queen | Jack | Ace | Value(int);
    type card = {
        color: color,
        motif: motif,
    };

    let newDeck = () => {
        List.map((color) => {
            List.map((motif) => {
                { color, motif };
            }, [King, Queen, Jack, Ace, Value(7), Value(8), Value(9), Value(10)]);
        }, [Spades, Hearts, Diamonds, Clubs])
        |> List.flatten
    };
}
open Jest;
include Deck;
include List;

describe("Deck", () => {
  open ExpectJs;

  test("creates a new deck", () => {
    Deck.newDeck() |> List.length |> expect |> toEqual(32);
  });
  test("creates a new deck with cards", () => {
    Deck.newDeck() |> List.hd |> expect |> toEqual(Deck.{motif: Deck.King, color: Deck.Spades});
  });
});
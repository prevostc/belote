open Jest;
include Deck;
include List;

describe("Deck", () => {
  open ExpectJs;

  test("creates a new deck", () => {
    expect(List.length(Deck.newDeck())) |> toEqual(32);
  });
  test("creates a new deck with cards", () => {
    expect(List.nth(Deck.newDeck(), 0)) |> toEqual(Deck.{motif: Deck.King, color: Deck.Spades});
  });
});
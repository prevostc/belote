open Jest;

describe("Deck", () => {
  open ExpectJs;

  test("creates a new deck", () => {
    Deck.newDeck() |> List.length |> expect |> toEqual(32);
  });
  test("creates a new deck with cards", () => {
    Deck.newDeck() |> List.hd |> expect |> toEqual(Deck.{motif: Deck.king, color: Deck.Spades});
  });
  test("cuts a list at given position", () => {
    [1, 2, 3, 4, 5, 6] |> Deck.cut(3) |> expect |> toEqual([4, 5, 6, 1, 2, 3])
  });
});

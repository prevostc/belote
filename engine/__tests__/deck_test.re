open Jest;

describe("Deck", () => {
  open ExpectJs;

  test("creates a new deck", () => {
    Deck.newDeck() |> List.length |> expect |> toEqual(32);
  });
  test("creates a new deck with cards", () => {
    Deck.newDeck() |> List.hd |> expect |> toEqual(Deck.{motif: Deck.King, color: Deck.Spades});
  });
  test("cuts a list at given position", () => {
    [1, 2, 3, 4, 5, 6] |> Deck.cut(3) |> expect |> toEqual([4, 5, 6, 1, 2, 3])
  });
});


describe("Card Order", () => {
  open ExpectJs;
  open Deck;

  test("compares motifs according to poker order", () => {
    (
        motifCompare(pokerOrder, Ace, King),
        motifCompare(pokerOrder, King, Queen),
        motifCompare(pokerOrder, Queen, Jack),
        motifCompare(pokerOrder, Queen, Queen),
        motifCompare(pokerOrder, V9, V10),
    ) |> expect |> toEqual((
        1, 1, 1, 0, -1
    ));
  });

  test("compares motifs according to trump order", () => {
    (
        motifCompare(trumpOrder, Jack, Ace),
        motifCompare(trumpOrder, V9, Ace),
        motifCompare(trumpOrder, Jack, V9),
        motifCompare(trumpOrder, V10, Queen),
        motifCompare(trumpOrder, Queen, Queen),
        motifCompare(trumpOrder, V7, V10),
    ) |> expect |> toEqual((
        1, 1, 1, 1, 0, -1
    ));
  });

  test("compares motifs according to non trump order", () => {
    (
        motifCompare(nonTrumpOrder, Ace, Jack),
        motifCompare(nonTrumpOrder, V10, Jack),
        motifCompare(nonTrumpOrder, V10, V9),
        motifCompare(nonTrumpOrder, V10, Queen),
        motifCompare(nonTrumpOrder, Queen, Queen),
        motifCompare(nonTrumpOrder, V7, V10),
    ) |> expect |> toEqual((
        1, 1, 1, 1, 0, -1
    ));
  });

  test("compares motifs with helper function", () => {
    (
        motifGreaterThan(nonTrumpOrder, Ace, Jack),
        motifGreaterThan(nonTrumpOrder, V10, Jack),
        motifGreaterThan(nonTrumpOrder, V10, V9),
        motifGreaterThan(nonTrumpOrder, V10, Queen),
        motifGreaterThan(nonTrumpOrder, Queen, Queen),
        motifGreaterThan(nonTrumpOrder, V7, V10),
    ) |> expect |> toEqual((
        false, false, false, false, false, true
    ));
  });

  test("sorts motifs", () => {
    let motifs = [Ace, King, Queen, Jack, V10, V9, V8, V7];
    (
        motifs |> sortMotifs(pokerOrder),
        motifs |> sortMotifs(trumpOrder),
        motifs |> sortMotifs(nonTrumpOrder),
        motifs |> sortMotifs(pokerOrder) |> List.rev,
        motifs |> sortMotifs(trumpOrder) |> List.rev,
        motifs |> sortMotifs(nonTrumpOrder) |> List.rev,
    ) |> expect |> toEqual((
        [V7, V8, V9, V10, Jack, Queen, King, Ace],
        [V7, V8, Queen, King, V10, Ace, V9, Jack],
        [V7, V8, V9, Jack, Queen, King, V10, Ace],
        pokerOrder,
        trumpOrder,
        nonTrumpOrder,
    ));
  });
});

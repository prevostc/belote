open Jest;

describe("Card order", () => {
  open ExpectJs;
  open CardOrder;

  test("compares motifs according to poker order", () => {
    (
        motifCompare(pokerOrder, Deck.Ace, Deck.King),
        motifCompare(pokerOrder, Deck.King, Deck.Queen),
        motifCompare(pokerOrder, Deck.Queen, Deck.Jack),
        motifCompare(pokerOrder, Deck.Queen, Deck.Queen),
        motifCompare(pokerOrder, Deck.V9, Deck.V10),
    ) |> expect |> toEqual((
        1, 1, 1, 0, -1
    ));
  });

  test("compares motifs according to trump order", () => {
    (
        motifCompare(trumpOrder, Deck.Jack, Deck.Ace),
        motifCompare(trumpOrder, Deck.V9, Deck.Ace),
        motifCompare(trumpOrder, Deck.Jack, Deck.V9),
        motifCompare(trumpOrder, Deck.V10, Deck.Queen),
        motifCompare(trumpOrder, Deck.Queen, Deck.Queen),
        motifCompare(trumpOrder, Deck.V7, Deck.V10),
    ) |> expect |> toEqual((
        1, 1, 1, 1, 0, -1
    ));
  });

  test("compares motifs according to non trump order", () => {
    (
        motifCompare(plainOrder, Deck.Ace, Deck.Jack),
        motifCompare(plainOrder, Deck.V10, Deck.Jack),
        motifCompare(plainOrder, Deck.V10, Deck.V9),
        motifCompare(plainOrder, Deck.V10, Deck.Queen),
        motifCompare(plainOrder, Deck.Queen, Deck.Queen),
        motifCompare(plainOrder, Deck.V7, Deck.V10),
    ) |> expect |> toEqual((
        1, 1, 1, 1, 0, -1
    ));
  });

  test("compares motifs with helper function", () => {
    (
        motifGreaterThan(plainOrder, Deck.Ace, Deck.Jack),
        motifGreaterThan(plainOrder, Deck.V10, Deck.Jack),
        motifGreaterThan(plainOrder, Deck.V10, Deck.V9),
        motifGreaterThan(plainOrder, Deck.V10, Deck.Queen),
        motifGreaterThan(plainOrder, Deck.Queen, Deck.Queen),
        motifGreaterThan(plainOrder, Deck.V7, Deck.V10),
    ) |> expect |> toEqual((
        false, false, false, false, false, true
    ));
  });

  test("sorts motifs", () => {
    let motifs = [Deck.Ace, Deck.King, Deck.Queen, Deck.Jack, Deck.V10, Deck.V9, Deck.V8, Deck.V7];
    (
        motifs |> sortMotifs(pokerOrder),
        motifs |> sortMotifs(trumpOrder),
        motifs |> sortMotifs(plainOrder),
        motifs |> sortMotifs(pokerOrder) |> List.rev,
        motifs |> sortMotifs(trumpOrder) |> List.rev,
        motifs |> sortMotifs(plainOrder) |> List.rev,
    ) |> expect |> toEqual((
        [Deck.V7, Deck.V8, Deck.V9, Deck.V10, Deck.Jack, Deck.Queen, Deck.King, Deck.Ace],
        [Deck.V7, Deck.V8, Deck.Queen, Deck.King, Deck.V10, Deck.Ace, Deck.V9, Deck.Jack],
        [Deck.V7, Deck.V8, Deck.V9, Deck.Jack, Deck.Queen, Deck.King, Deck.V10, Deck.Ace],
        pokerOrder,
        trumpOrder,
        plainOrder,
    ));
  });

  test("Finds highest of motif", () => {
      let table = [
        Deck.{color: Deck.Spades, motif: Deck.V8},
        Deck.{color: Deck.Spades, motif: Deck.V9},
        Deck.{color: Deck.Spades, motif: Deck.Ace},
        Deck.{color: Deck.Clubs, motif: Deck.V9},
        Deck.{color: Deck.Clubs, motif: Deck.V10},
      ];
      (
        table |> getHighestTrump(Deck.Spades),
        table |> getHighestNonTrump(Deck.Spades),
      ) |> expect |> toEqual((
        Some(Deck.V9),
        Some(Deck.Ace),
      ));
  });

  test("Account for card order, trump and non trump", () => {
      let table = [
        Deck.{color: Deck.Spades, motif: Deck.Ace},
        Deck.{color: Deck.Spades, motif: Deck.V10},
        Deck.{color: Deck.Spades, motif: Deck.King},
        Deck.{color: Deck.Spades, motif: Deck.Jack},
        Deck.{color: Deck.Spades, motif: Deck.V9}
      ];
      (
        table |> getHighestTrump(Deck.Spades),
        table |> getHighestNonTrump(Deck.Spades),
      ) |> expect |> toEqual((
        Some(Deck.Jack),
        Some(Deck.Ace),
      ));
  });

  test("Finds winning card in table", () => {
      let table = [
        Deck.{color: Deck.Spades, motif: Deck.Ace},
        Deck.{color: Deck.Spades, motif: Deck.V9},
        Deck.{color: Deck.Clubs, motif: Deck.Jack},
        Deck.{color: Deck.Clubs, motif: Deck.V10},
      ];
      (
        table |> getWinningCard(Deck.Spades),
        table |> getWinningCard(Deck.Clubs),
        table |> getWinningCard(Deck.Diamonds),
      ) |> expect |> toEqual((
        Some(Deck.{color: Deck.Spades, motif: Deck.V9}),
        Some(Deck.{color: Deck.Clubs, motif: Deck.Jack}),
        Some(Deck.{color: Deck.Spades, motif: Deck.Ace}),
      ))
  });
  test("Finds winning card in table with high trumps", () => {
      let table = [
        Deck.{color: Deck.Spades, motif: Deck.V9},
        Deck.{color: Deck.Spades, motif: Deck.Jack},
        Deck.{color: Deck.Clubs, motif: Deck.King},
        Deck.{color: Deck.Clubs, motif: Deck.V10},
      ];
      (
        table |> getWinningCard(Deck.Spades),
        table |> getWinningCard(Deck.Clubs),
        table |> getWinningCard(Deck.Diamonds),
      ) |> expect |> toEqual((
        Some(Deck.{color: Deck.Spades, motif: Deck.Jack}),
        Some(Deck.{color: Deck.Clubs, motif: Deck.V10}),
        Some(Deck.{color: Deck.Spades, motif: Deck.Jack}),
      ))
  });
});

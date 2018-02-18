open Jest;

describe("Dealer", () => {
  open ExpectJs;

  let initialDeck = Deck.newDeck();
  let mkCard = (c, m): Deck.card => { color: c, motif: m };

  test("deal cards", () => {
    let dealtMap = Dealer.dealHands(Player.East, initialDeck);
    dealtMap |> Player.PlayerMap.find(South) |> Array.of_list |> expect |> toEqual([
      mkCard(Spades, Deck.King), mkCard(Spades, Deck.Queen), mkCard(Spades, Deck.Jack),
      mkCard(Hearts, V7), mkCard(Hearts, V8),
      mkCard(Diamonds, V7), mkCard(Diamonds, V8), mkCard(Diamonds, V9),
    ] |> Array.of_list);
  });
});
open Jest;

describe("Dealer", () => {
  open ExpectJs;

  let initialDeck = Deck.newDeck();
  let mkCard = (c, m): Deck.card => { color: c, motif: m };

  test("deal cards", () => {
    let dealtMap = Dealer.dealHands(Player.East, initialDeck);
    dealtMap |> Player.PlayerMap.find(South) |> Array.of_list |> expect |> toEqual([
      mkCard(Spades, Deck.king), mkCard(Spades, Deck.queen), mkCard(Spades, Deck.jack),
      mkCard(Hearts, Value(7)), mkCard(Hearts, Value(8)),
      mkCard(Diamonds, Value(7)), mkCard(Diamonds, Value(8)), mkCard(Diamonds, Value(9)),
    ] |> Array.of_list);
  });
});
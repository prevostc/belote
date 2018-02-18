open Jest;

describe("Card play helpers", () => {
  open ExpectJs;
  open CardPlay;

  let trump = Deck.Spades;

  test("Finds highest of motif", () => {
      let table = [
        Deck.{color: Deck.Spades, motif: Deck.Value(8)},
        Deck.{color: Deck.Spades, motif: Deck.Value(9)},
        Deck.{color: Deck.Clubs, motif: Deck.Value(9)},
        Deck.{color: Deck.Clubs, motif: Deck.Value(10)},
      ];
      table |> getHighestMotifOfColor(Deck.Spades) |> expect |> toEqual(Some(Deck.Value(9)))
  });

  test("Card order is different for trump", () => {
    /* @todo */
    true |> expect |> toEqual(true)
  });

  test("Finds winning card in table", () => {
      let table = [
        Deck.{color: Deck.Spades, motif: Deck.Value(8)},
        Deck.{color: Deck.Spades, motif: Deck.Value(9)},
        Deck.{color: Deck.Clubs, motif: Deck.Value(9)},
        Deck.{color: Deck.Clubs, motif: Deck.Value(10)},
      ];
      (
        table |> getWinningCard(Deck.Spades),
        table |> getWinningCard(Deck.Clubs),
        table |> getWinningCard(Deck.Diamonds),
      ) |> expect |> toEqual((
        Some(Deck.{color: Deck.Spades, motif: Deck.Value(9)}),
        Some(Deck.{color: Deck.Clubs, motif: Deck.Value(10)}),
        Some(Deck.{color: Deck.Spades, motif: Deck.Value(9)}),
      ))
  });
});


describe("Card play validation", () => {
  open ExpectJs;
  open CardPlay;

  let first = Player.North;
  let trump = Deck.Spades;

  test("First to play, can play anything you have", () => {
    let card = Deck.{color: Deck.Spades, motif: Deck.ace};
    let table = [];
    let hand = [card];
    let spot = Player.North;
    card |> cardPlayValidation(first, trump, table, hand, spot) |> expect |> toEqual(ValidCardPlay);
  });
  test("Can only play a card you have in hand", () => {
    let card = Deck.{color: Deck.Spades, motif: Deck.ace};
    let table = [];
    let hand = [Deck.{color: Deck.Spades, motif: Deck.king}, Deck.{color: Deck.Diamonds, motif: Deck.ace}];
    let spot = Player.North;
    card |> cardPlayValidation(first, trump, table, hand, spot) |> expect |> toEqual(InvalidCardPlay(CardNotInHand(card)));
  });
  test("Can not play if not your turn", () => {
    let card = Deck.{color: Deck.Spades, motif: Deck.ace};
    let table = [];
    let hand = [card];
    let spot = Player.South;
    card |> cardPlayValidation(first, trump, table, hand, spot) |> expect |> toEqual(InvalidCardPlay(NotYourTurn(Player.North, Player.South)));
  });
  test("Can not play if table is full", () => {
    let card = Deck.{color: Deck.Spades, motif: Deck.ace};
    let table = [card, card, card, card];
    let hand = [card];
    let spot = Player.North;
    card |> cardPlayValidation(first, trump, table, hand, spot) |> expect |> toEqual(InvalidCardPlay(TableIsFull));
  });
  test("Must play asked color if you have it", () => {
    let card = Deck.{color: Deck.Clubs, motif: Deck.ace};
    let table = [Deck.{color: Deck.Diamonds, motif: Deck.ace}];
    let hand = [card, Deck.{color: Deck.Diamonds, motif: Deck.king}];
    let spot = Player.East;
    card |> cardPlayValidation(first, trump, table, hand, spot) |> expect |> toEqual(InvalidCardPlay(MustPlayColor(Deck.Diamonds, Deck.Clubs)));
  });

  test("You do not have the asked color, must play trump if you have some", () => {
    let card = Deck.{color: Deck.Clubs, motif: Deck.ace};
    let table = [Deck.{color: Deck.Diamonds, motif: Deck.ace}, Deck.{color: Deck.Diamonds, motif: Deck.king}];
    let hand = [card, Deck.{color: Deck.Spades, motif: Deck.king}];
    let spot = Player.South;
    card |> cardPlayValidation(first, trump, table, hand, spot) |> expect |> toEqual(InvalidCardPlay(MustPlayTrump));
  });

  test("You do not have the asked color, must play a higher trump if you have one", () => {
    let card = Deck.{color: Deck.Spades, motif: Deck.Value(7)};
    let table = [Deck.{color: Deck.Diamonds, motif: Deck.ace}, Deck.{color: Deck.Spades, motif: Deck.Value(8)}];
    let hand = [card, Deck.{color: Deck.Spades, motif: Deck.king}];
    let spot = Player.South;
    card |> cardPlayValidation(first, trump, table, hand, spot) |> expect |> toEqual(
        InvalidCardPlay(MustPlayHigherTrump(Deck.Value(7), Deck.Value(8), Deck.king))
    );
  });

  test("You do not have the asked color, must play trump exception: if your team wins the hand, you can keep your trumps", () => {
    /* @todo */
    true |> expect |> toEqual(true)
  });
});

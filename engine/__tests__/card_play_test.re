open Jest;

describe("Card play helpers", () => {
  open ExpectJs;
  open CardPlay;

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

  test("Test if team is winning the table, buddy has a higher trump", () => {
    let table = [
      Deck.{color: Deck.Spades, motif: Deck.Ace},
      Deck.{color: Deck.Spades, motif: Deck.Jack},
      Deck.{color: Deck.Clubs, motif: Deck.King},
    ];
    (
      table |> isTeamWinningTable(Deck.Spades),
      table |> isTeamWinningTable(Deck.Clubs),
      table |> isTeamWinningTable(Deck.Diamonds),
    ) |> expect |> toEqual((
        true,
        false,
        false,
    ))
  });
  test("Test if team is winning the table, buddy has a higher non trump", () => {
    let table = [
      Deck.{color: Deck.Spades, motif: Deck.Jack},
      Deck.{color: Deck.Spades, motif: Deck.Ace},
      Deck.{color: Deck.Clubs, motif: Deck.King},
    ];
    (
      table |> isTeamWinningTable(Deck.Spades),
      table |> isTeamWinningTable(Deck.Clubs),
      table |> isTeamWinningTable(Deck.Diamonds),
    ) |> expect |> toEqual((
        false,
        false,
        true,
    ))
  });

  test("Test if team is winning the table, not enough rounds", () => {
    (
      [ Deck.{color: Deck.Spades, motif: Deck.Jack} ] |> isTeamWinningTable(Deck.Spades),
      [] |> isTeamWinningTable(Deck.Clubs),
    ) |> expect |> toEqual((
        false,
        false,
    ))
  });
});

describe("Card play validation", () => {
  open ExpectJs;
  open CardPlay;

  let first = Player.North;
  let trump = Deck.Spades;

  test("First to play, can play anything you have", () => {
    let card = Deck.{color: Deck.Spades, motif: Deck.Ace};
    let table = [];
    let hand = [card];
    let spot = Player.North;
    card |> cardPlayValidation(first, trump, table, hand, spot) |> expect |> toEqual(ValidCardPlay);
  });
  test("Can only play a card you have in hand", () => {
    let card = Deck.{color: Deck.Spades, motif: Deck.Ace};
    let table = [];
    let hand = [Deck.{color: Deck.Spades, motif: Deck.King}, Deck.{color: Deck.Diamonds, motif: Deck.Ace}];
    let spot = Player.North;
    card |> cardPlayValidation(first, trump, table, hand, spot) |> expect |> toEqual(InvalidCardPlay(CardNotInHand(card)));
  });
  test("Can not play if not your turn", () => {
    let card = Deck.{color: Deck.Spades, motif: Deck.Ace};
    let table = [];
    let hand = [card];
    let spot = Player.South;
    card |> cardPlayValidation(first, trump, table, hand, spot) |> expect |> toEqual(InvalidCardPlay(NotYourTurn(Player.North, Player.South)));
  });
  test("Can not play if table is full", () => {
    let card = Deck.{color: Deck.Spades, motif: Deck.Ace};
    let table = [card, card, card, card];
    let hand = [card];
    let spot = Player.North;
    card |> cardPlayValidation(first, trump, table, hand, spot) |> expect |> toEqual(InvalidCardPlay(TableIsFull));
  });
  test("Must play asked color if you have it", () => {
    let card = Deck.{color: Deck.Clubs, motif: Deck.Ace};
    let table = [Deck.{color: Deck.Diamonds, motif: Deck.Ace}];
    let hand = [card, Deck.{color: Deck.Diamonds, motif: Deck.King}];
    let spot = Player.East;
    card |> cardPlayValidation(first, trump, table, hand, spot) |> expect |> toEqual(InvalidCardPlay(MustPlayColor(Deck.Diamonds, Deck.Clubs)));
  });

  test("Must play a higher trump if it's the asked color", () => {
    let card = Deck.{color: Deck.Spades, motif: Deck.V7};
    let table = [
        Deck.{color: Deck.Spades, motif: Deck.Queen},
        Deck.{color: Deck.Spades, motif: Deck.Ace},
        Deck.{color: Deck.Diamonds, motif: Deck.King},
    ];
    let hand = [card, Deck.{color: Deck.Spades, motif: Deck.Jack}];
    let spot = Player.West;
    card |> cardPlayValidation(first, trump, table, hand, spot) |> expect |> toEqual(
        InvalidCardPlay(MustPlayHigherTrump(Deck.V7, Deck.Ace, Deck.Jack))
    );
  });

  test("Can play a higher trump if it's the asked color", () => {
    let card = Deck.{color: Deck.Spades, motif: Deck.V9};
    let table = [
        Deck.{color: Deck.Spades, motif: Deck.Queen},
        Deck.{color: Deck.Spades, motif: Deck.Ace},
        Deck.{color: Deck.Diamonds, motif: Deck.King},
    ];
    let hand = [card, Deck.{color: Deck.Spades, motif: Deck.Jack}];
    let spot = Player.West;
    card |> cardPlayValidation(first, trump, table, hand, spot) |> expect |> toEqual(ValidCardPlay);
  });

  test("You do not have the asked color, must play trump if you have some", () => {
    let card = Deck.{color: Deck.Clubs, motif: Deck.Ace};
    let table = [Deck.{color: Deck.Diamonds, motif: Deck.King}, Deck.{color: Deck.Diamonds, motif: Deck.Ace}];
    let hand = [card, Deck.{color: Deck.Spades, motif: Deck.King}];
    let spot = Player.South;
    card |> cardPlayValidation(first, trump, table, hand, spot) |> expect |> toEqual(InvalidCardPlay(MustPlayTrump));
  });

  test("You do not have the asked color, must play a higher trump if you have one", () => {
    let card = Deck.{color: Deck.Spades, motif: Deck.V7};
    let table = [Deck.{color: Deck.Diamonds, motif: Deck.Ace}, Deck.{color: Deck.Spades, motif: Deck.V8}];
    let hand = [card, Deck.{color: Deck.Spades, motif: Deck.King}];
    let spot = Player.South;
    card |> cardPlayValidation(first, trump, table, hand, spot) |> expect |> toEqual(
        InvalidCardPlay(MustPlayHigherTrump(Deck.V7, Deck.V8, Deck.King))
    );
  });
  test("You do not have the asked color, must play a lower trump if you do not have higher", () => {
    let card = Deck.{color: Deck.Spades, motif: Deck.V7};
    let table = [Deck.{color: Deck.Diamonds, motif: Deck.Ace}, Deck.{color: Deck.Spades, motif: Deck.V8}];
    let hand = [card, Deck.{color: Deck.Clubs, motif: Deck.King}];
    let spot = Player.South;
    card |> cardPlayValidation(first, trump, table, hand, spot) |> expect |> toEqual(ValidCardPlay);
  });

  test("You do not have the asked color, must play trump exception: if your team wins the hand with a non trump, you can keep your trumps", () => {
    let card = Deck.{color: Deck.Clubs, motif: Deck.Ace};
    let table = [
        Deck.{color: Deck.Diamonds, motif: Deck.Queen},
        Deck.{color: Deck.Diamonds, motif: Deck.Ace},
        Deck.{color: Deck.Diamonds, motif: Deck.King},
    ];
    let hand = [card, Deck.{color: Deck.Spades, motif: Deck.King}];
    let spot = Player.West;
    card |> cardPlayValidation(first, trump, table, hand, spot) |> expect |> toEqual(ValidCardPlay);
  });

  test("You do not have the asked color, must play trump exception: if your team wins the hand with a trump, you can keep your trumps", () => {
    let card = Deck.{color: Deck.Clubs, motif: Deck.Ace};
    let table = [
        Deck.{color: Deck.Diamonds, motif: Deck.Queen},
        Deck.{color: Deck.Spades, motif: Deck.Ace},
        Deck.{color: Deck.Diamonds, motif: Deck.King},
    ];
    let hand = [card, Deck.{color: Deck.Spades, motif: Deck.King}];
    let spot = Player.West;
    card |> cardPlayValidation(first, trump, table, hand, spot) |> expect |> toEqual(ValidCardPlay);
  });
});

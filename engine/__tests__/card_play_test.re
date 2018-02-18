open Jest;

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

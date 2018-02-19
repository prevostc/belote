open Jest;

describe("Score", () => {
  open ExpectJs;
  open Score;

  test("Test score is computed properly", () => {
    let table = [
      Deck.{color: Deck.Spades, motif: Deck.Ace},
      Deck.{color: Deck.Spades, motif: Deck.Jack},
      Deck.{color: Deck.Clubs, motif: Deck.King},
    ];
    (
      table |> getCardsScore(Deck.Spades),
      table |> getCardsScore(Deck.Clubs),
      table |> getCardsScore(Deck.Diamonds),
    ) |> expect |> toEqual((
        35,
        17,
        17,
    ))
  });


    let baseGraveyard = Player.PlayerMap.empty
        |> Player.PlayerMap.add(North, [])
        |> Player.PlayerMap.add(East, [])
        |> Player.PlayerMap.add(South, [])
        |> Player.PlayerMap.add(West, []);

  test("Create score based on contract and graveyard", () => {
    let graveyard = baseGraveyard
        |>  Player.PlayerMap.add(Player.East, [Deck.{color: Deck.Spades, motif: Deck.Ace}])
        |>  Player.PlayerMap.add(Player.West, [Deck.{color: Deck.Spades, motif: Deck.King}])
        |>  Player.PlayerMap.add(Player.North, [Deck.{color: Deck.Spades, motif: Deck.Jack}])
        |>  Player.PlayerMap.add(Player.South, [Deck.{color: Deck.Spades, motif: Deck.V9}])
    ;
    (
      graveyard |> contractToScore(Deck.Spades, 10, Player.East),
      graveyard |> contractToScore(Deck.Spades, 20, Player.East),
      graveyard |> contractToScore(Deck.Diamonds, 10, Player.West),
      graveyard |> contractToScore(Deck.Spades, 10, Player.North),
      graveyard |> contractToScore(Deck.Diamonds, 10, Player.South),
    ) |> expect |> toEqual((
        Score.{team: Player.EastWest, score: 10},
        Score.{team: Player.NorthSouth, score: 20},
        Score.{team: Player.EastWest, score: 10},
        Score.{team: Player.NorthSouth, score: 10},
        Score.{team: Player.EastWest, score: 10},
    ))
  });

  test("Create score based on contract and graveyard: exception capot", () => {
    (
      baseGraveyard |> Player.PlayerMap.add(Player.East, [Deck.{color: Deck.Spades, motif: Deck.Ace}]) |> contractToScore(Deck.Spades, Bid.capot, Player.East),
      baseGraveyard |> Player.PlayerMap.add(Player.West, [Deck.{color: Deck.Spades, motif: Deck.Ace}]) |> contractToScore(Deck.Spades, Bid.capot, Player.East),
      baseGraveyard |> Player.PlayerMap.add(Player.East, [Deck.{color: Deck.Spades, motif: Deck.Ace}]) |> contractToScore(Deck.Spades, Bid.capot, Player.North),
      baseGraveyard |> Player.PlayerMap.add(Player.North, [Deck.{color: Deck.Spades, motif: Deck.Ace}]) |> contractToScore(Deck.Spades, Bid.capot, Player.East),
    ) |> expect |> toEqual((
        Score.{team: Player.EastWest, score: Bid.capot},
        Score.{team: Player.EastWest, score: Bid.capot},
        Score.{team: Player.EastWest, score: Bid.capot},
        Score.{team: Player.NorthSouth, score: Bid.capot},
    ))
  });

  test("Create score based on contract and graveyard: exception general", () => {
    (
      baseGraveyard |> Player.PlayerMap.add(Player.East, [Deck.{color: Deck.Spades, motif: Deck.Ace}]) |> contractToScore(Deck.Spades, Bid.general, Player.East),
      baseGraveyard |> Player.PlayerMap.add(Player.West, [Deck.{color: Deck.Spades, motif: Deck.Ace}]) |> contractToScore(Deck.Spades, Bid.general, Player.East),
      baseGraveyard |> Player.PlayerMap.add(Player.East, [Deck.{color: Deck.Spades, motif: Deck.Ace}]) |> contractToScore(Deck.Spades, Bid.general, Player.North),
      baseGraveyard |> Player.PlayerMap.add(Player.North, [Deck.{color: Deck.Spades, motif: Deck.Ace}]) |> contractToScore(Deck.Spades, Bid.general, Player.East),
    ) |> expect |> toEqual((
        Score.{team: Player.EastWest, score: Bid.general},
        Score.{team: Player.NorthSouth, score: Bid.general},
        Score.{team: Player.EastWest, score: Bid.general},
        Score.{team: Player.NorthSouth, score: Bid.general},
    ))
  });
});

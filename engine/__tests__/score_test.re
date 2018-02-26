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
      graveyard |> contractToScore(Deck.Spades, Player.North, 10, Player.East),
      graveyard |> contractToScore(Deck.Spades, Player.North, 20, Player.East),
      graveyard |> contractToScore(Deck.Diamonds, Player.North, 10, Player.West),
      graveyard |> contractToScore(Deck.Spades, Player.West, 10, Player.North),
      graveyard |> contractToScore(Deck.Diamonds, Player.West, 10, Player.South),
    ) |> expect |> toEqual((
        Score.{trump: Deck.Spades, lastTrickWinner: Player.North, winner: Player.EastWest, score: 10, contractValue: 10, contractPlayer: Player.East},
        Score.{trump: Deck.Spades, lastTrickWinner: Player.North, winner: Player.NorthSouth, score: 20, contractValue: 20, contractPlayer: Player.East},
        Score.{trump: Deck.Diamonds, lastTrickWinner: Player.North, winner: Player.EastWest, score: 10, contractValue: 10, contractPlayer: Player.West},
        Score.{trump: Deck.Spades, lastTrickWinner: Player.West, winner: Player.NorthSouth, score: 10, contractValue: 10, contractPlayer: Player.North},
        Score.{trump: Deck.Diamonds, lastTrickWinner: Player.West, winner: Player.EastWest, score: 10, contractValue: 10, contractPlayer: Player.South},
    ))
  });

  test("Create score based on contract and graveyard: last trick winner takes 10 points", () => {
    let graveyard = baseGraveyard
        |>  Player.PlayerMap.add(Player.East, [Deck.{color: Deck.Spades, motif: Deck.Ace}])
        |>  Player.PlayerMap.add(Player.West, [Deck.{color: Deck.Spades, motif: Deck.King}])
        |>  Player.PlayerMap.add(Player.North, [Deck.{color: Deck.Spades, motif: Deck.Jack}])
        |>  Player.PlayerMap.add(Player.South, [Deck.{color: Deck.Spades, motif: Deck.V9}])
    ;
    (
      graveyard |> contractToScore(Deck.Spades, Player.East, 20, Player.East),
      graveyard |> contractToScore(Deck.Spades, Player.South, 20, Player.East),
      graveyard |> contractToScore(Deck.Spades, Player.West, 20, Player.East),
      graveyard |> contractToScore(Deck.Spades, Player.North, 20, Player.East),
    ) |> expect |> toEqual((
        Score.{trump: Deck.Spades, lastTrickWinner: Player.East, winner: Player.EastWest, score: 20, contractValue: 20, contractPlayer: Player.East},
        Score.{trump: Deck.Spades, lastTrickWinner: Player.South, winner: Player.NorthSouth, score: 20, contractValue: 20, contractPlayer: Player.East},
        Score.{trump: Deck.Spades, lastTrickWinner: Player.West, winner: Player.EastWest, score: 20, contractValue: 20, contractPlayer: Player.East},
        Score.{trump: Deck.Spades, lastTrickWinner: Player.North, winner: Player.NorthSouth, score: 20, contractValue: 20, contractPlayer: Player.East},
    ))
  });

  test("Create score based on contract and graveyard: exception capot", () => {
    (
      baseGraveyard |> Player.PlayerMap.add(Player.East, [Deck.{color: Deck.Spades, motif: Deck.Ace}]) |> contractToScore(Deck.Spades, Player.East, Bid.capot, Player.East),
      baseGraveyard |> Player.PlayerMap.add(Player.West, [Deck.{color: Deck.Spades, motif: Deck.Ace}]) |> contractToScore(Deck.Spades, Player.East, Bid.capot, Player.East),
      baseGraveyard |> Player.PlayerMap.add(Player.East, [Deck.{color: Deck.Spades, motif: Deck.Ace}]) |> contractToScore(Deck.Spades, Player.East, Bid.capot, Player.North),
      baseGraveyard |> Player.PlayerMap.add(Player.North, [Deck.{color: Deck.Spades, motif: Deck.Ace}]) |> contractToScore(Deck.Spades, Player.East, Bid.capot, Player.East),
    ) |> expect |> toEqual((
        Score.{trump: Deck.Spades, lastTrickWinner: Player.East, winner: Player.EastWest, score: Bid.capot, contractValue: Bid.capot, contractPlayer: Player.East},
        Score.{trump: Deck.Spades, lastTrickWinner: Player.East, winner: Player.EastWest, score: Bid.capot, contractValue: Bid.capot, contractPlayer: Player.East},
        Score.{trump: Deck.Spades, lastTrickWinner: Player.East, winner: Player.EastWest, score: Bid.capot, contractValue: Bid.capot, contractPlayer: Player.North},
        Score.{trump: Deck.Spades, lastTrickWinner: Player.East, winner: Player.NorthSouth, score: Bid.capot, contractValue: Bid.capot, contractPlayer: Player.East},
    ))
  });

  test("Create score based on contract and graveyard: exception general", () => {
    (
      baseGraveyard |> Player.PlayerMap.add(Player.East, [Deck.{color: Deck.Spades, motif: Deck.Ace}]) |> contractToScore(Deck.Spades, Player.East, Bid.general, Player.East),
      baseGraveyard |> Player.PlayerMap.add(Player.West, [Deck.{color: Deck.Spades, motif: Deck.Ace}]) |> contractToScore(Deck.Spades, Player.East, Bid.general, Player.East),
      baseGraveyard |> Player.PlayerMap.add(Player.East, [Deck.{color: Deck.Spades, motif: Deck.Ace}]) |> contractToScore(Deck.Spades, Player.East, Bid.general, Player.North),
      baseGraveyard |> Player.PlayerMap.add(Player.North, [Deck.{color: Deck.Spades, motif: Deck.Ace}]) |> contractToScore(Deck.Spades, Player.East, Bid.general, Player.East),
    ) |> expect |> toEqual((
        Score.{trump: Deck.Spades, lastTrickWinner: Player.East, winner: Player.EastWest, score: Bid.general, contractValue: Bid.general, contractPlayer: Player.East},
        Score.{trump: Deck.Spades, lastTrickWinner: Player.East, winner: Player.NorthSouth, score: Bid.general, contractValue: Bid.general, contractPlayer: Player.East},
        Score.{trump: Deck.Spades, lastTrickWinner: Player.East, winner: Player.EastWest, score: Bid.general, contractValue: Bid.general, contractPlayer: Player.North},
        Score.{trump: Deck.Spades, lastTrickWinner: Player.East, winner: Player.NorthSouth, score: Bid.general, contractValue: Bid.general, contractPlayer: Player.East},
    ))
  });
});

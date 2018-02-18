open Jest;

describe("Turn winner", () => {
  open ExpectJs;
  open TurnWinner;

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

  test("Finds the winning player and team", () => {
    let table = [
        Deck.{color: Deck.Spades, motif: Deck.Queen},
        Deck.{color: Deck.Spades, motif: Deck.Ace},
        Deck.{color: Deck.Clubs, motif: Deck.Ace},
        Deck.{color: Deck.Spades, motif: Deck.Jack},
    ];
    (
        table |> getWinningPlayer(Player.North, Deck.Spades),
        table |> getWinningPlayer(Player.North, Deck.Clubs),
    ) |> expect |> toEqual((
        Player.West,
        Player.South,
    ))
  });
});

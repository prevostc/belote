open Jest;
open Game;

describe("Engine API tests", () => {
  open ExpectJs;

  let game = Engine.createGame("1234");

  let (p1, g1) = Engine.joinGame(game, "1", "a", Game.North);
  let (p2, g2) = Engine.joinGame(g1, "2", "b", Game.South);
  let (p3, g3) = Engine.joinGame(g2, "3", "c", Game.East);
  let (p4, g4) = Engine.joinGame(g3, "4", "d", Game.West);

  test("Creates a game state", () => {
    game.players |> expect |> toEqual([]);
  });
  test("Creates a game state", () => {
    game.uuid |> expect |> toEqual("1234");
  });

  test("Join a game", () => {
    let (p, g) = Engine.joinGame(game, "1", "john", Game.North);
    g.players |> expect |> toEqual([p]);
  });

  test("Multiple join a game", () => {
    let (_, g1) = Engine.joinGame(game, "1", "john", Game.North);
    let (_, g2) = Engine.joinGame(g1, "2", "john", Game.South);
    g2.players |> List.length |> expect |> toEqual(2);
  });

  test("Cannot join a if spot taken", () => {
    let (p1, g1) = Engine.joinGame(game, "1", "john", Game.North);
    let (_, g2) = Engine.joinGame(g1, "2", "john", Game.North);
    g2.players |> expect |> toEqual([ p1 ]);
  });

  test("Move spot if uuid taken", () => {
    let (_, g1) = Engine.joinGame(game, "1", "john", Game.North);
    let (p2, g2) = Engine.joinGame(g1, "1", "john", Game.South);
    g2.players |> expect |> toEqual([ p2 ]);
  });

  test("Move spot if uuid taken", () => {
    let (_, g1) = Engine.joinGame(game, "1", "john", Game.North);
    let (p2, g2) = Engine.joinGame(g1, "1", "john", Game.South);
    g2.players |> expect |> toEqual([ p2 ]);
  });

  test("Join scenario", () => {
    let (p1, g1) = Engine.joinGame(game, "1", "john", Game.North);
    let (p2, g2) = Engine.joinGame(g1, "2", "doe", Game.North);
    let (p3, g3) = Engine.joinGame(g2, "2", "doe", Game.South);
    let (p4, g4) = Engine.joinGame(g3, "1", "john", Game.West);
    let (p5, g5) = Engine.joinGame(g4, "2", "doe", Game.North);
    let (p6, g6) = Engine.joinGame(g5, "1", "john", Game.North);

    let format = ((p: Engine.playerState, players: list(Engine.playerState))) => ((p.uuid, p.spot), List.map((p: Engine.playerState) => (p.uuid, p.spot), players));
    (
        (p1, g1.players) |> format,
        (p2, g2.players) |> format,
        (p3, g3.players) |> format,
        (p4, g4.players) |> format,
        (p5, g5.players) |> format,
        (p6, g6.players) |> format
    ) |> expect |> toEqual((
        (("1", Game.North), [("1", Game.North)]),
        (("1", Game.North), [("1", Game.North)]),
        (("2", Game.South), [("1", Game.North), ("2", Game.South)]),
        (("1", Game.West),  [("2", Game.South), ("1", Game.West)]),
        (("2", Game.North), [("1", Game.West), ("2", Game.North)]),
        (("1", Game.West),  [("1", Game.West), ("2", Game.North)])
    ));
  });

  test("Starts the game when 4 people joined", () => {
    let (p1, g1) = Engine.joinGame(game, "1", "a", Game.North);
    let (p2, g2) = Engine.joinGame(g1, "2", "b", Game.South);
    let (p3, g3) = Engine.joinGame(g2, "3", "c", Game.East);
    let (p4, g4) = Engine.joinGame(g3, "4", "d", Game.West);
    (
        game.gameState.phase, g1.gameState.phase, g2.gameState.phase, g3.gameState.phase, g4.gameState.phase
    ) |> expect |> toEqual((
        Game.Initial, Game.Initial, Game.Initial, Game.Initial, Game.Bidding
    ));
  });

  test("Checks who is the dealer", () => {
    (
        Engine.isDealer("1", g4), Engine.isDealer("2", g4), Engine.isDealer("3", g4), Engine.isDealer("4", g4)
    ) |> expect |> toEqual((
        true, false, false, false
    ));
  });
});
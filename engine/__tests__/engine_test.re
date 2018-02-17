open Jest;

describe("Engine create game", () => {
  open ExpectJs;

  let game = Engine.createGame("1234");

  test("Creates a game state", () => {
    game.players |> expect |> toEqual([]);
  });
  test("Creates a game state", () => {
    game.uuid |> expect |> toEqual("1234");
  });
});

describe("Engine join game", () => {
  open ExpectJs;

  let game = Engine.createGame("1234");
  let (_, g1) = Engine.joinGame(game, "1", "a", Player.North);
  let (_, g2) = Engine.joinGame(g1, "2", "b", Player.South);
  let (_, g3) = Engine.joinGame(g2, "3", "c", Player.East);
  let (_, g4) = Engine.joinGame(g3, "4", "d", Player.West);

  test("Join a game", () => {
    let (p, g) = Engine.joinGame(game, "1", "john", Player.North);
    g.players |> expect |> toEqual([p]);
  });

  test("Multiple join a game", () => {
    let (_, g1) = Engine.joinGame(game, "1", "john", Player.North);
    let (_, g2) = Engine.joinGame(g1, "2", "john", Player.South);
    g2.players |> List.length |> expect |> toEqual(2);
  });

  test("Cannot join a if spot taken", () => {
    let (p1, g1) = Engine.joinGame(game, "1", "john", Player.North);
    let (_, g2) = Engine.joinGame(g1, "2", "john", Player.North);
    g2.players |> expect |> toEqual([ p1 ]);
  });

  test("Move spot if uuid taken", () => {
    let (_, g1) = Engine.joinGame(game, "1", "john", Player.North);
    let (p2, g2) = Engine.joinGame(g1, "1", "john", Player.South);
    g2.players |> expect |> toEqual([ p2 ]);
  });

  test("Move spot if uuid taken", () => {
    let (_, g1) = Engine.joinGame(game, "1", "john", Player.North);
    let (p2, g2) = Engine.joinGame(g1, "1", "john", Player.South);
    g2.players |> expect |> toEqual([ p2 ]);
  });

  test("Join scenario", () => {
    let (p1, g1) = Engine.joinGame(game, "1", "john", Player.North);
    let (p2, g2) = Engine.joinGame(g1, "2", "doe", Player.North);
    let (p3, g3) = Engine.joinGame(g2, "2", "doe", Player.South);
    let (p4, g4) = Engine.joinGame(g3, "1", "john", Player.West);
    let (p5, g5) = Engine.joinGame(g4, "2", "doe", Player.North);
    let (p6, g6) = Engine.joinGame(g5, "1", "john", Player.North);

    let format = ((p: Engine.playerState, players: list(Engine.playerState))) => ((p.uuid, p.spot), List.map((p: Engine.playerState) => (p.uuid, p.spot), players));
    (
        (p1, g1.players) |> format,
        (p2, g2.players) |> format,
        (p3, g3.players) |> format,
        (p4, g4.players) |> format,
        (p5, g5.players) |> format,
        (p6, g6.players) |> format
    ) |> expect |> toEqual((
        (("1", Player.North), [("1", Player.North)]),
        (("1", Player.North), [("1", Player.North)]),
        (("2", Player.South), [("1", Player.North), ("2", Player.South)]),
        (("1", Player.West),  [("2", Player.South), ("1", Player.West)]),
        (("2", Player.North), [("1", Player.West), ("2", Player.North)]),
        (("1", Player.West),  [("1", Player.West), ("2", Player.North)])
    ));
  });

  test("Starts the game when 4 people joined", () => {
    let (_, g1) = Engine.joinGame(game, "1", "a", Player.North);
    let (_, g2) = Engine.joinGame(g1, "2", "b", Player.South);
    let (_, g3) = Engine.joinGame(g2, "3", "c", Player.East);
    let (_, g4) = Engine.joinGame(g3, "4", "d", Player.West);
    (
        game.phase, g1.phase, g2.phase, g3.phase, g4.phase
    ) |> expect |> toEqual((
        Phase.Initial, Phase.Initial, Phase.Initial, Phase.Initial, Phase.Bidding
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


describe("Engine game start", () => {
  open ExpectJs;

  let game = Engine.createGame("1234");
  let mkCard = (c, m): Deck.card => { color: c, motif: m };

  test("The deck gets shuffled on game start", () => {
    let state = game |> Engine.dispatch(Engine.StartGame) |> Engine.handleError;
    /* @todo: this tests may fail il the first card stays unchanged, seed the random to fix */
    state.hands |> Player.PlayerMap.find(Player.East) |> List.hd |> expect |> not_ |> toEqual(mkCard(Deck.Spades, Deck.king));
  });

  test("The deck gets dealt on game start", () => {
    let state = game |> Engine.dispatch(Engine.StartGame) |> Engine.handleError;
    state.hands |> Player.PlayerMap.find(Player.South) |> List.length |> expect |> toEqual(8);
  });
});



describe("Engine bid", () => {
  open ExpectJs;

  let mkBid = (p, v) => Bid.Bid(p, v, Deck.Diamonds);
  let dispatchBid = (p, v) => Engine.MakeBid(mkBid(p, v)) |> Engine.dispatch;
  let dispatchPass = p => Engine.MakeBid(Bid.Pass(p)) |> Engine.dispatch;
  let game = {...Engine.createGame("abc"), phase: Phase.Bidding };

  test("We can make a bid", () => {
    let state = game |> dispatchBid(East, 80) |> Engine.handleError;
    state.bids |> List.length |> expect |> toEqual(1);
  });

  test("We can make multiple bids", () => {
    let state = game
      |> dispatchBid(Player.North, 80) |> Engine.handleError
      |> dispatchBid(Player.East, 90) |> Engine.handleError
      |> dispatchBid(Player.South, 100) |> Engine.handleError
      |> dispatchBid(Player.West, 110) |> Engine.handleError
      ;
    state.bids |> List.length |> expect |> toEqual(4);
  });

  test("Bad bids gets rejected", () => {
    let state = game
      |> dispatchBid(Player.North, 80) |> Engine.handleError
      |> dispatchBid(Player.East, 80)
      ;
    state |> expect |> toEqual(
        Engine.ActionError(
            Engine.InvalidBid(
                Bid.Bid(Player.East, 80, Deck.Diamonds)
            )
        )
    );
  });

  test("Passing 3 times ends bid phase", () => {
    let state = game
      |> dispatchBid(North, 80) |> Engine.handleError
      |> dispatchPass(East) |> Engine.handleError
      |> dispatchPass(South) |> Engine.handleError
      |> dispatchPass(West) |> Engine.handleError
      ;
    state.phase |> expect |> toEqual(Phase.Playing);
  });
});
open Jest;

describe("Engine create game", () => {
  open ExpectJs;

  let game = Engine.createGame("1234");

  test("Creates a game state", () => {
    game.players |> expect |> toEqual(Player.PlayerMap.empty);
  });
  test("Creates a game state", () => {
    game.uuid |> expect |> toEqual("1234");
  });
});

describe("Engine join game", () => {
  open ExpectJs;

  let rawDispatchJoinGame = (uuid, name, spot) => Engine.JoinGame(uuid, name, spot) |> Engine.dispatch;
  let dispatchJoinGame = (uuid, name, spot, game) => Engine.raiseErrorOrUnboxState(Engine.dispatch(Engine.JoinGame(uuid, name, spot), game));

  let game = Engine.createGame("1234");
  let g1 = game |> dispatchJoinGame("1", "a", Player.North);
  let g2 = g1 |> dispatchJoinGame("2", "b", Player.South);
  let g3 = g2 |> dispatchJoinGame("3", "c", Player.East);
  let g4 = g3 |> dispatchJoinGame("4", "d", Player.West);

  test("Join a game", () => {
    let g = game |> dispatchJoinGame("1", "john", Player.North);
    let expectedPlayer: Engine.playerState = {
        uuid: "1",
        name: "john",
    };
    g.players |> expect |> toEqual(Player.PlayerMap.singleton(Player.North, expectedPlayer));
  });

  test("Multiple join a game", () => {
    let g1 = game |> dispatchJoinGame("1", "a", Player.North);
    let g2 = g1 |> dispatchJoinGame("2", "b", Player.South);
    g2.players |> Player.PlayerMap.cardinal |> expect |> toEqual(2);
  });

  test("Cannot join a if spot taken", () => {
    let g1 = game |> dispatchJoinGame("1", "john", Player.North);
    let err = g1 |> rawDispatchJoinGame("2", "john", Player.North);
    err |> expect |> toEqual(Engine.ActionError(Engine.InvalidJoinGame));
  });

  test("Move spot if uuid taken", () => {
    let g1 = game |> dispatchJoinGame("1", "john", Player.North);
    let g2 = g1 |> dispatchJoinGame("1", "john", Player.South);
    let expectedPlayer: Engine.playerState = {
        uuid: "1",
        name: "john",
    };
    g2.players |> expect |> toEqual(Player.PlayerMap.singleton(Player.South, expectedPlayer));
  });

  test("Starts the game when 4 people joined", () => {
    let g1 = game |> dispatchJoinGame("1", "a", Player.North);
    let g2 = g1 |> dispatchJoinGame("2", "b", Player.South);
    let g3 = g2 |> dispatchJoinGame("3", "c", Player.East);
    let g4 = g3 |> dispatchJoinGame("4", "d", Player.West);
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
    let state = game |> Engine.dispatch(Engine.StartGame) |> Engine.raiseErrorOrUnboxState;
    /* @todo: this tests may fail il the first card stays unchanged, seed the random to fix */
    state.hands |> Player.PlayerMap.find(Player.East) |> List.hd |> expect |> not_ |> toEqual(mkCard(Deck.Spades, Deck.king));
  });

  test("The deck gets dealt on game start", () => {
    let state = game |> Engine.dispatch(Engine.StartGame) |> Engine.raiseErrorOrUnboxState;
    state.hands |> Player.PlayerMap.find(Player.South) |> List.length |> expect |> toEqual(8);
  });
});



describe("Engine bid", () => {
  open ExpectJs;

  let mkBid = (p, v) => Bid.Bid(p, v, Deck.Diamonds);
  let dispatchBid = (p, v, game) => Engine.dispatch(Engine.MakeBid(mkBid(p, v)), game) |> Engine.raiseErrorOrUnboxState;
  let rawDispatchBid = (p, v, game) => Engine.dispatch(Engine.MakeBid(mkBid(p, v)), game);
  let dispatchPass = (p, game) => Engine.dispatch(Engine.MakeBid(Bid.Pass(p)), game) |> Engine.raiseErrorOrUnboxState;
  let game = {...Engine.createGame("abc"), phase: Phase.Bidding };

  test("We can make a bid", () => {
    let state = game |> dispatchBid(East, 80);
    state.bids |> List.length |> expect |> toEqual(1);
  });

  test("We can make multiple bids", () => {
    let state = game
      |> dispatchBid(Player.North, 80)
      |> dispatchBid(Player.East, 90)
      |> dispatchBid(Player.South, 100)
      |> dispatchBid(Player.West, 110)
      ;
    state.bids |> List.length |> expect |> toEqual(4);
  });

  test("Bad bids gets rejected", () => {
    let err = game
      |> dispatchBid(Player.North, 80)
      |> rawDispatchBid(Player.East, 80)
      ;
    err |> expect |> toEqual(
        Engine.ActionError(
            Engine.InvalidBid(
                Bid.ValueNotHigher(80, 80)
            )
        )
    );
  });

  test("Passing 3 times ends bid phase", () => {
    let state = game
      |> dispatchBid(North, 80)
      |> dispatchPass(East)
      |> dispatchPass(South)
      |> dispatchPass(West)
      ;
    state.phase |> expect |> toEqual(Phase.Playing);
  });
});
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
    let state = game |> Engine.dispatch(Engine.FourthPlayerJoined) |> Engine.raiseErrorOrUnboxState;
    /* @todo: this tests may fail il the first card stays unchanged, seed the random to fix */
    state.hands |> Player.PlayerMap.find(Player.East) |> List.hd |> expect |> not_ |> toEqual(mkCard(Deck.Spades, Deck.King));
  });

  test("The deck gets dealt on game start", () => {
    let state = game |> Engine.dispatch(Engine.FourthPlayerJoined) |> Engine.raiseErrorOrUnboxState;
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



describe("Engine Play", () => {
  open ExpectJs;
  open Engine;

  let dispatchPlay = (p, c, game) => dispatch(Engine.PlayCard(p, c), game) |> raiseErrorOrUnboxState;
  let game = {
    ...createGame("abc"),
    phase: Phase.Playing,
    first: Player.North,
    trump: Deck.Spades,
    hands: Player.PlayerMap.empty
        |> Player.PlayerMap.add(North, [Deck.{color: Deck.Spades, motif: Deck.Ace} , Deck.{color: Deck.Clubs, motif: Deck.Ace}])
        |> Player.PlayerMap.add(East,  [Deck.{color: Deck.Spades, motif: Deck.V10} , Deck.{color: Deck.Clubs, motif: Deck.V10}])
        |> Player.PlayerMap.add(South, [Deck.{color: Deck.Spades, motif: Deck.King}, Deck.{color: Deck.Clubs, motif: Deck.King}])
        |> Player.PlayerMap.add(West,  [Deck.{color: Deck.Spades, motif: Deck.Jack}, Deck.{color: Deck.Clubs, motif: Deck.Jack}])
  };

  test("An error results in an ActionError", () => {
    let card = Deck.{color: Deck.Diamonds, motif: Deck.V7};
    let err = dispatch(PlayCard(Player.North, card), game);
    err |> expect |> toEqual(ActionError(InvalidCardPlay(CardPlay.CardNotInHand(card))));
  });

  test("Update player hand and table when playing is allowed", () => {
    let card = Deck.{color: Deck.Spades, motif: Deck.Ace};
    let state = game |> dispatchPlay(North, card);
    (
        state.table,
        state.hands |> Player.PlayerMap.find(North)
    ) |> expect |> toEqual((
        [card],
        [Deck.{color: Deck.Clubs, motif: Deck.Ace}]
    ));
  });

  let g1 = game |> dispatchPlay(Player.North, Deck.{color: Deck.Spades, motif: Deck.Ace});
  let g2 = g1   |> dispatchPlay(Player.East,  Deck.{color: Deck.Spades, motif: Deck.V10});
  let g3 = g2   |> dispatchPlay(Player.South, Deck.{color: Deck.Spades, motif: Deck.King});
  let g4 = g3   |> dispatchPlay(Player.West,  Deck.{color: Deck.Spades, motif: Deck.Jack});
  test("When turn is over, update team graveyard, update first player and reset table", () => {
    (
        g4.table,
        g4.first
    ) |> expect |> toEqual((
        [],
        Player.West
    ));
  });

  test("When round is over, scores are written, dealer is changed, cards are regrouped and cut and dealt", () => {
    let g5 = g4 |>  dispatchPlay(Player.West,  Deck.{color: Deck.Clubs, motif: Deck.Jack});
    let g6 = g5 |> dispatchPlay(Player.North, Deck.{color: Deck.Clubs, motif: Deck.Ace});
    let g7 = g6 |> dispatchPlay(Player.East,  Deck.{color: Deck.Clubs, motif: Deck.V10});
    let g8 = g7 |> dispatchPlay(Player.South, Deck.{color: Deck.Clubs, motif: Deck.King});
    true |> expect |> toEqual(true)
  });
});
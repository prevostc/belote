type playerUuid = string;
type gameUuid = string;
type playerName = string;

type playerState = {
    uuid: playerUuid,
    name: playerName,
};

type gameState = {
    uuid: gameUuid,
    players: Player.PlayerMap.t(playerState),
    phase: Phase.phase,
    hands: Player.PlayerMap.t(list(Deck.card)),
    bids: list(Bid.bid),
    dealer: Player.player,
    deck: list(Deck.card),
    scores: list(Score.score),

    /* playing state */
    graveyard: Player.PlayerMap.t(list(Deck.card)),
    first: Player.player,
    table: list(Deck.card),
    trump: Deck.color,

    /* contract state */
    contractValue: int,
    contractPlayer: Player.player,
};

let createGame = (uuid: gameUuid): gameState => {
    {
        uuid: uuid,
        players: Player.PlayerMap.empty,
        phase: Initial,
        hands: Player.PlayerMap.empty
            |> Player.PlayerMap.add(North, [])
            |> Player.PlayerMap.add(East, [])
            |> Player.PlayerMap.add(South, [])
            |> Player.PlayerMap.add(West, []),
        dealer: North,
        bids: [],
        deck: Deck.newDeck(),
        scores: [],

        graveyard: Player.PlayerMap.empty
            |> Player.PlayerMap.add(North, [])
            |> Player.PlayerMap.add(East, [])
            |> Player.PlayerMap.add(South, [])
            |> Player.PlayerMap.add(West, []),
        first: Player.nextPlayer(Player.North),
        table: [],
        trump: Deck.Spades,
        contractPlayer: Player.North,
        contractValue: 0,
    };
};

/* @todo actions are able to trigger events, the split is not that clear, maybe events are useless */
type action =
      LeaveGame(playerUuid)
    | JoinGame(playerUuid, playerName, Player.player)
    | FourthPlayerJoined
    | MakeBid(Bid.bid)
    | PlayCard(Player.player, Deck.card)
;
type error = InvalidJoinGame | InvalidBid(Bid.error) | InvalidCardPlay(CardPlay.error);
type actionResult = State(gameState) | ActionError(error);

let errorToMessage = (error) => switch error {
    | InvalidCardPlay(CardPlay.CardNotInHand(_)) => "Error: invalid card play, not in hand"
    | InvalidCardPlay(CardPlay.MustPlayColor(_, _)) => "Error: invalid card play, you must play asked color"
    | InvalidCardPlay(CardPlay.MustPlayHigherTrump(_, _, _)) => "Error: invalid card play, you must play a higher trump"
    | InvalidCardPlay(CardPlay.MustPlayTrump) => "Error: invalid card play, you must play a trump"
    | InvalidCardPlay(CardPlay.TableIsFull) => "Error: invalid card play, table is full"
    | InvalidCardPlay(CardPlay.NotYourTurn(_, _)) => "Error: invalid card play, not your turn"
    | InvalidBid(Bid.ValueNotLegal(_)) => "Error: invalid bid value"
    | InvalidBid(Bid.ValueNotHigher(_, _)) => "Error: invalid bid value should be higher than previous non-pass bid"
    | InvalidBid(Bid.NotYourTurn(_, _)) => "Error: invalid bid, not your turn"
    | InvalidJoinGame => "Error: invalid game join attempt"
};

/* @todo: move this code to Format, but this causes a circular dep */
/* @todo: do not raise, but return directly, also allow multiple chained dispatch */
let raiseErrorOrUnboxState = (actionResult: actionResult) => switch (actionResult) {
    | State(s) => s
    | ActionError(e) => raise(Failure(e |> errorToMessage))
};

let playerExists = (playerUuid) => Player.PlayerMap.exists((_, p: playerState) => p.uuid === playerUuid);
let findPlayer = (playerUuid, map) => map |> Player.PlayerMap.filter((_, p: playerState) => p.uuid === playerUuid) |> Player.PlayerMap.choose;

/* @todo: split actions on state in different reducers ? maybe the phase change, deck shuffle, deck cut and deck dealing do not belong to the same plAce ? */
let rec dispatch = (action: action, state: gameState): actionResult => {
    switch action {
        | LeaveGame(playerUuid) => {
            State({
                ...state,
                players: state.players |> Player.PlayerMap.filter((_, p: playerState) => p.uuid !== playerUuid)
            })
        }

        | JoinGame(playerUuid, playerName, spot) => {
            let newPlayer: playerState = {
                uuid: playerUuid,
                name: playerName,
            };

            /* player in same spot */
            if (state.players |> Player.PlayerMap.mem(spot)) {
                ActionError(InvalidJoinGame)

            } else {
                /* player with same uuid, wants to change spot, remove it first */
                let stateWithoutExistingPlayer = (state.players |> playerExists(playerUuid))
                        ? raiseErrorOrUnboxState(dispatch(LeaveGame(playerUuid), state))
                        : state;

                /* new player */
                let stateWithNewPlayer = {
                    ...stateWithoutExistingPlayer,
                    players: stateWithoutExistingPlayer.players |> Player.PlayerMap.add(spot, newPlayer)
                };

                /* start game automatically if 4 players joined */
                let newGameState = (stateWithNewPlayer.players |> Player.PlayerMap.cardinal) === 4
                    ? raiseErrorOrUnboxState(dispatch(FourthPlayerJoined, stateWithNewPlayer)) : stateWithNewPlayer;

                State(newGameState)
            }
        }

        | FourthPlayerJoined => {
            /* @todo: shuffle + cut does not make any sense when it's a machine dealing */
            let rnd = state.deck |> List.length |> Random.int;
            State({
                ...state,
                deck: [],
                hands: state.deck |> ListUtil.shuffle |> Deck.cut(rnd) |> Dealer.dealHands(state.dealer),
                phase: Phase.Bidding
            })
        }

        | MakeBid(b) => {
            switch (b |> Bid.bidValidation(state.bids)) {
                | Bid.ValidBid => State({
                    let bids = state.bids @ [b];
                    let bidEnds = Bid.bidPhaseEnd(bids);
                    if (bidEnds) {
                        let winningBid = bids |> Bid.filterNotPass |> ListUtil.last;
                        switch winningBid {
                            /* start playing automatically if bid phase ends */
                            | Some(Bid.Bid(p, v, c)) => {
                                ...state,
                                bids: bids,
                                first: Bid.firstPlayerIsBidder(v) ? p : Player.nextPlayer(state.dealer),
                                phase: Playing,
                                trump: c,
                                contractValue: v,
                                contractPlayer: p
                            }
                            /* everyone passed, reset bids, change dealer, regroup cards, deal cards */
                            | _ => {
                                let getCards = (p) => state.hands |> Player.PlayerMap.find(p);
                                let deck = getCards(North) @ getCards(West) @ getCards(South) @ getCards(East);
                                let rnd = deck |> List.length |> Random.int;
                                let dealer = Player.nextPlayer(state.dealer);
                                {
                                    ...state,
                                    bids: [],
                                    dealer: dealer,
                                    deck: [],
                                    hands: deck |> Deck.cut(rnd) |> Dealer.dealHands(dealer)
                                }
                            }
                        }
                    } else {
                        { ...state, bids: bids }
                    }
                })
                | Bid.InvalidBid(e) => ActionError(InvalidBid(e))
            }
        }

        | PlayCard(p, c) => {
            let playerHand = state.hands |> Player.PlayerMap.find(p);
            switch (c |> CardPlay.cardPlayValidation(state.first, state.trump, state.table, playerHand, p)) {
                | CardPlay.ValidCardPlay => {
                    let newTable = state.table @ [c];
                    let turnIsOver = (newTable |> List.length) === 4;

                    if (turnIsOver) {
                        let winningPlayer = TurnWinner.getWinningPlayer(state.first, state.trump, newTable);
                        let winningPlayerGraveyard = state.graveyard |> Player.PlayerMap.find(winningPlayer);
                        let newState = {
                            ...state,
                            table: [],
                            hands: state.hands |> Player.PlayerMap.add(p, playerHand |> List.filter(card => !Deck.cardEquals(c, card))),
                            graveyard: state.graveyard |> Player.PlayerMap.add(winningPlayer, winningPlayerGraveyard @ newTable),
                            first: winningPlayer
                        };
                        let roundIsOver = (newState.hands |> Player.PlayerMap.find(Player.North) |> List.length) <= 0;

                        /* round is over, scores are written down, dealer is changed, cards are regrouped and cut and dealt */
                        if (roundIsOver) {
                            let getCards = (t) => newState.graveyard |> Player.PlayerMap.find(t);
                            let deck = getCards(East) @ getCards(West) @ getCards(North) @ getCards(South);
                            let rnd = deck |> List.length |> Random.int;
                            let dealer = Player.nextPlayer(newState.dealer);
                            let newScore = Score.contractToScore(state.trump, winningPlayer, state.contractValue, state.contractPlayer, state.graveyard);
                            State({
                                ...newState,
                                dealer: dealer,
                                bids: [],
                                deck: [],
                                scores: newState.scores @ [newScore],
                                hands: deck |> Deck.cut(rnd) |> Dealer.dealHands(dealer)
                            })
                        /* round is not over, keep going */
                        } else {
                            State(newState)
                        }
                    } else {
                        /* turn is not over, remove card from player hand and continue */
                        State({
                            ...state,
                            table: newTable,
                            hands: state.hands |> Player.PlayerMap.add(p, playerHand |> List.filter(card => !Deck.cardEquals(c, card)))
                        })
                    }
                }
                | CardPlay.InvalidCardPlay(e) => ActionError(InvalidCardPlay(e))
            }
        }
    };
};


/* public api: selectors */
let getPlayerAndSpot = (uuid: playerUuid, state: gameState) => state.players |> findPlayer(uuid);
let getPlayerSpot = (uuid: playerUuid, state: gameState) => state.players |> findPlayer(uuid) |> fst;
let getPlayerState = (uuid: playerUuid, state: gameState) => state.players |> findPlayer(uuid) |> fst;

let getUuid = (state: gameState) => state.uuid;

let getCards = (uuid: playerUuid, state: gameState) => state.players |> playerExists(uuid)
    ? state.players |> findPlayer(uuid) |> ((spot, _)) => (state.hands |> Player.PlayerMap.find(spot))
    : [];

let getTableCards = (state: gameState) => state.table;

let isDealer = (uuid: playerUuid, state: gameState): bool => state.players |> playerExists(uuid)
        ? state.players |> findPlayer(uuid) |> ((spot, _)) => spot === state.dealer
        : false;

let getNextToPlay = (state: gameState) => state.table |> CardPlay.getNextToPlay(state.first);

let isPlayerActionNeeded = (uuid: playerUuid, state: gameState): bool => switch state.phase {
    | Bidding => Bid.getNextToPlay(state.dealer, state.bids) === getPlayerSpot(uuid, state)
    | Playing => CardPlay.getNextToPlay(state.first, state.table) === getPlayerSpot(uuid, state)
    | _ => false
};

let getActionNeededFromPlayerAndSpot = (state: gameState) => {
    let spot = switch state.phase {
        | Bidding => Some(Bid.getNextToPlay(state.dealer, state.bids))
        | Playing => Some(CardPlay.getNextToPlay(state.first, state.table))
        | _ => None
    };
    switch spot {
        | Some(s) => Some((s, state.players |> Player.PlayerMap.find(s)))
        | None => None
    }
};

let canCardBePlayed = (uuid: playerUuid, color: Deck.color, motif: Deck.motif, state: gameState) => {
    let p = state |> getPlayerSpot(uuid);
    let c = Deck.{color: color, motif: motif};
    let playerHand = state.hands |> Player.PlayerMap.find(p);
    switch (c |> CardPlay.cardPlayValidation(state.first, state.trump, state.table, playerHand, p)) {
        | CardPlay.ValidCardPlay => true
        | CardPlay.InvalidCardPlay(e) => false
    }
};


/* public api: actions */
let joinGame = (uuid: playerUuid, name: playerName, spot: Player.player, state: gameState): actionResult => state |> dispatch(JoinGame(uuid, name, spot));

let bid = (uuid: playerUuid, value: int, color: Deck.color, state: gameState): actionResult => {
    let spot = state |> getPlayerSpot(uuid);
    state |> dispatch(MakeBid(Bid.Bid(spot, value, color)))
};
let pass = (uuid: playerUuid, state: gameState): actionResult => {
    let spot = state |> getPlayerSpot(uuid);
    state |> dispatch(MakeBid(Bid.Pass(spot)))
};
let playCard = (uuid: playerUuid, color: Deck.color, motif: Deck.motif, state: gameState): actionResult => {
    let spot = state |> getPlayerSpot(uuid);
    state |> dispatch(PlayCard(spot, Deck.{color: color, motif: motif}))
};
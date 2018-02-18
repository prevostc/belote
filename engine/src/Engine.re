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
    };
};

type action = LeaveGame(playerUuid) | JoinGame(playerUuid, playerName, Player.player) | StartGame | MakeBid(Bid.bid);
type error = InvalidJoinGame | InvalidBid(Bid.bid);
type actionResult = State(gameState) | ActionError(error);

/* @todo: move this code to Format, but this causes a circular dep */
/* @todo: do not raise, but return directly, also allow multiple chained dispatch */
let raiseErrorOrUnboxState = (actionResult: actionResult) => switch (actionResult) {
    | State(s) => s
    | ActionError(InvalidBid(_)) => raise(Failure("Error: invalid bid"))
    | ActionError(InvalidJoinGame) => raise(Failure("Error: invalid game join attempt"))
};

let playerExists = (playerUuid) => Player.PlayerMap.exists((_, p: playerState) => p.uuid === playerUuid);
let findPlayer = (playerUuid, map) => map |> Player.PlayerMap.filter((_, p: playerState) => p.uuid === playerUuid) |> Player.PlayerMap.choose;

/* @todo: split actions on state in different reducers ? maybe the phase change, deck shuffle, deck cut and deck dealing do not belong to the same place ? */
let rec dispatch = (action: action, state: gameState): actionResult => {
    let validBid = state.bids |> Bid.validBid;
    switch action {
        | LeaveGame(playerUuid) => {
        Js.log(state.players |> Player.PlayerMap.filter((_, p: playerState) => p.uuid !== playerUuid));
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
                    ? raiseErrorOrUnboxState(dispatch(StartGame, stateWithNewPlayer)) : stateWithNewPlayer;

                State(newGameState)
            }
        }

        | StartGame => {
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
            validBid(b)
                ? State({
                    let bids = state.bids @ [b];
                    { ...state, bids: bids, phase: Bid.bidPhaseEnd(bids) ? Playing : state.phase }
                })
                : ActionError(InvalidBid(b))
        }
    };
};

/* public api */
let joinGame = (uuid: playerUuid, name: playerName, spot: Player.player, state: gameState): actionResult => state |> dispatch(JoinGame(uuid, name, spot));

let getPlayerAndSpot = (uuid: playerUuid, state: gameState) => state.players |> findPlayer(uuid);

let getUuid = (state: gameState) => state.uuid;

let getCards = (uuid: playerUuid, state: gameState) => {
    state.players |> playerExists(uuid)
        ? state.players |> findPlayer(uuid) |> ((spot, _)) => (state.hands |> Player.PlayerMap.find(spot))
        : []
};

let isDealer = (uuid: playerUuid, state: gameState): bool => {
    state.players |> playerExists(uuid)
        ? state.players |> findPlayer(uuid) |> ((spot, _)) => spot === state.dealer
        : false
};
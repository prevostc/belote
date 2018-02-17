type uuid = string;

type playerState = {
    uuid: uuid,
    name: string,
    spot: Player.player,
    gameUuid: uuid
};

type gameState = {
    uuid: uuid,
    players: list(playerState),
    phase: Phase.phase,
    hands: Player.PlayerMap.t(list(Deck.card)),
    bids: list(Bid.bid),
    dealer: Player.player,
    deck: list(Deck.card),
    scores: list(Score.score),
};

type action = StartGame | MakeBid(Bid.bid);
type error = InvalidBid(Bid.bid);
type actionResult = State(gameState) | ActionError(error);

/* @todo: move this code to Format, but this causes a circular dep */
/* @todo: do not raise, but return directly, also allow multiple chained dispatch */
let handleError = (actionResult: actionResult) => switch (actionResult) {
    | State(s) => s
    | ActionError(InvalidBid(_)) => raise(Failure("Error: invalid bid"))
};

let dispatch = (action: action, state: gameState): actionResult => {
    let validBid = state.bids |> Bid.validBid;
    switch action {
        | StartGame => {
            /*
                @todo: split actions on state in different reducers ?
                       maybe the phase change, deck shuffle, deck cut and deck dealing do
                       not belong to the same place ?
                @todo: shuffle + cut does not make any sense when it's a machine dealing
            */
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

let createGame = (uuid: uuid): gameState => {
    {
        uuid: uuid,
        players: [],
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


let joinGame = (gameState: gameState, playerUuid: uuid, playerName: string, spot: Player.player): (playerState, gameState) => {
    let newPlayer: playerState = {
        uuid: playerUuid,
        name: playerName,
        spot: spot,
        gameUuid: gameState.uuid
    };

    let sameSpot = gameState.players |> ListUtil.find((p: playerState) => p.spot === spot);
    let sameUuid = gameState.players |> ListUtil.find((p: playerState) => p.uuid === playerUuid);

    let (player, players) = switch (sameSpot, sameUuid) {
        /* already someone there, do nothing */
        | (Some(p), None) => (p, gameState.players)
        | (Some(_), Some(o)) => (o, gameState.players)
        /* player wants to move spot */
        | (None, Some(p)) => (newPlayer, List.filter((e: playerState) => e.uuid !== p.uuid, gameState.players) @ [newPlayer])
        /* new player */
        | _ => (newPlayer, gameState.players @ [newPlayer])
    };

    /* start game automatically if 4 players joined */
    let newGameState = (players |> List.length) === 4 ? handleError(dispatch(StartGame, gameState)) : gameState;

    (
        player,
        {
            ...newGameState,
            players: players
        }
    );
};

let getCards = (playerUuid: uuid, gameState: gameState) => {
    let player = gameState.players |> ListUtil.find((p: playerState) => p.uuid === playerUuid);
    switch (player) {
        | Some(p) => gameState.hands |> Player.PlayerMap.find(p.spot)
        | None => []
    };
};
let isDealer = (playerUuid: uuid, gameState: gameState) => {
    let player = gameState.players |> ListUtil.find((p: playerState) => p.uuid === playerUuid);
    switch (player) {
        | Some(p) => p.spot === gameState.dealer
        | None => false
    };
};
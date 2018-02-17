open Game;
open Util;

type uuid = string;

type playerState = {
    uuid: uuid,
    name: string,
    spot: Game.player,
    gameUuid: uuid
};

type gameState = {
    uuid: uuid,
    players: list(playerState),
    gameState: Game.state,
};

let createGame = (uuid: uuid): gameState => {
    uuid: uuid,
    players: [],
    gameState: Game.initialState()
};


let joinGame = (game: gameState, playerUuid: uuid, playerName: string, spot: Game.player): (playerState, gameState) => {
    let newPlayer: playerState = {
        uuid: playerUuid,
        name: playerName,
        spot: spot,
        gameUuid: game.uuid
    };

    let sameSpot = game.players |> Util.listFind((p: playerState) => p.spot === spot);
    let sameUuid = game.players |> Util.listFind((p: playerState) => p.uuid === playerUuid);

    let (player, players) = switch (sameSpot, sameUuid) {
        /* already someone there, do nothing */
        | (Some(p), None) => (p, game.players)
        | (Some(p), Some(o)) => (o, game.players)
        /* player wants to move spot */
        | (None, Some(p)) => (newPlayer, List.filter((e: playerState) => e.uuid !== p.uuid, game.players) @ [newPlayer])
        /* new player */
        | _ => (newPlayer, game.players @ [newPlayer])
    };

    (
        player,
        {
            ...game,
            gameState: (players |> List.length) === 4 ? Game.dispatch(Game.StartGame, game.gameState) : game.gameState,
            players: players
        }
    );
};

let getCards = (playerUuid: uuid, game: gameState) => {
    let player = game.players |> Util.listFind((p: playerState) => p.uuid === playerUuid);
    switch (player) {
        | Some(p) => game.gameState.hands |> Game.PlayerMap.find(p.spot)
        | None => []
    };
};
let isDealer = (playerUuid: uuid, game: gameState) => {
    let player = game.players |> Util.listFind((p: playerState) => p.uuid === playerUuid);
    switch (player) {
        | Some(p) => p.spot === game.gameState.dealer
        | None => false
    };
};
open Game;

type playerState = Js.t({.
    uuid: string,
    name: string,
    spot: string,
    gameUuid: string
});

type gameState = Js.t({.
    uuid: string,
    players: array((string, int)),
    gameState: Game.state,
});

let createGame = (uuid): gameState => [%bs.obj {
    uuid: uuid,
    players: [| |],
    gameState: Game.initialState()
}];

let joinGame = (game, playerUuid, spot): gameState => [%bs.obj {
    uuid: game##uuid,
    gameState: game##gameState,
    players: Array.append(game##players, [| (playerUuid, spot) |])
}]

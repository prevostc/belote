open Game;

/*
@todo: refactor this
this file links the low level reasonml world and high level js world
ReasonML works with arrays of ids and ids, that's not a great api
We should create a proper api mapping file that does not embeed business logic
*/

type playerState = Js.t({.
    uuid: string,
    name: string,
    spot: string,
    gameUuid: string
});

type gameState = Js.t({.
    uuid: string,
    players: array(playerState),
    gameState: Game.state,
});

let createGame = (uuid): gameState => [%bs.obj {
    uuid: uuid,
    players: [| |],
    gameState: Game.initialState()
}];

let joinGame = (game, playerUuid, playerName, spot): (playerState, gameState) => {
    let player = [%bs.obj {
        uuid: playerUuid,
        name: playerName,
        spot: spot,
        gameUuid: game##uuid
    }];
    (
        player,
        [%bs.obj {
            uuid: game##uuid,
            gameState: game##gameState,
            players: Array.append(game##players, [| player |])
        }]
    )
}

open Game;
open Util;

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
    let validSpot = spot === "SOUTH" || spot === "WEST" || spot === "NORTH" || spot === "EAST";

    let newPlayer = [%bs.obj {
        uuid: playerUuid,
        name: playerName,
        spot: spot,
        gameUuid: game##uuid
    }];

    let sameSpot = game##players |> Util.arrayFind(p => p##spot === spot);
    let sameUuid = game##players |> Util.arrayFind(p => p##uuid === playerUuid);

    let (player, players) = switch (sameSpot, sameUuid) {
        /* already someone there, do nothing */
        | (Some(p), None) => (p, game##players)
        | (Some(p), Some(o)) => (o, game##players)
        /* player wants to move spot */
        | (None, Some(p)) => (newPlayer, game##players |> Util.arrayFilter(e => e##uuid !== p##uuid) |> Array.append([|newPlayer|]))
        /* new player */
        | _ => (newPlayer, Array.append(game##players, [| newPlayer |]))
    };

    validSpot
        ? (
            player,
            [%bs.obj {
                uuid: game##uuid,
                gameState: game##gameState,
                players: players
            }]
        )
        :( player, game )
}

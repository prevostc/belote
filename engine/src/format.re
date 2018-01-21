open Game;
open Util;

/*
@todo: refactor this
this file links the low level reasonml world and high level js world
ReasonML works with arrays of ids and ids, that's not a great api
We should create a proper api mapping file that does not embeed business logic
*/
[@bs.val] external json_stringify : 'a => string = "JSON.stringify";

let consts = [%bs.obj {
    "SPOT": [%bs.obj {
        "NORTH": Game.North,
        "EAST": Game.East,
        "SOUTH": Game.South,
        "WEST": Game.West
    }],
    "PHASE": [%bs.obj {
         "INITIAL": Game.Initial,
         "DEALING": Game.Dealing,
         "BIDDING": Game.Bidding,
         "PLAYING": Game.Playing,
         "END": Game.End
     }]
}];

let formatSpot = (s: Game.player) => switch s {
    | Game.North => "NORTH"
    | Game.East => "EAST"
    | Game.South => "SOUTH"
    | Game.West => "WEST"
};

let formatPhase = (p: Game.phase) => switch p {
    | Game.Initial => "INITIAL"
    | Game.Dealing => "DEALING"
    | Game.Bidding => "BIDDING"
    | Game.Playing => "PLAYING"
    | Game.End => "END"
};

let getUuid = (o: Engine.gameState) => o.uuid;

let formatPlayer = (p: Engine.playerState) => [%bs.obj {
    "uuid": p.uuid,
    "spot": formatSpot(p.spot),
    "name": p.name,
    "gameUuid": p.gameUuid
}];
let formatGame = (g: Engine.gameState) => {
    [%bs.obj {
        "uuid": g.uuid,
        "gameState": g.gameState |> json_stringify,
        "players": g.players |> List.map(formatPlayer) |> Array.of_list,
        "phase": g.gameState.phase |> formatPhase
    }];
};
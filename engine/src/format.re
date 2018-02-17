open Game;
open Util;
open Deck;

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
    }],
    "CARD_COLOR": [%bs.obj {
        "SPADES": Deck.Spades,
        "HEARTS": Deck.Hearts,
        "DIAMONDS": Deck.Diamonds,
        "CLUBS": Deck.Clubs
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

let formatCardColor = (c: Deck.color) => switch c {
    | Deck.Spades => "SPADES"
    | Deck.Hearts => "HEARTS"
    | Deck.Diamonds => "DIAMONDS"
    | Deck.Clubs => "CLUBS"
};

let getUuid = (o: Engine.gameState) => o.uuid;

let formatPlayer = (p: Engine.playerState) => [%bs.obj {
    "uuid": p.uuid,
    "spot": p.spot |> formatSpot,
    "name": p.name,
    "gameUuid": p.gameUuid
}];
let formatGame = (g: Engine.gameState) => {
    [%bs.obj {
        "uuid": g.uuid,
        "gameState": g.gameState |> json_stringify,
        "players": g.players |> List.map(formatPlayer) |> Array.of_list,
        "phase": g.gameState.phase |> formatPhase,
        "dealer": g.gameState.dealer |> formatSpot
    }];
};
let formatCards = cards => cards |> List.map((c: Deck.card) => {
    [%bs.obj {
       "color": formatCardColor(c.color),
       "motif": 12
    }]
}) |> Array.of_list;
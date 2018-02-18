
/*
@todo: refactor this
this file links the low level reasonml world and high level js world
ReasonML works with arrays of ids and ids, that's not a great api
We should create a proper api mapping file that does not embeed business logic
*/
let consts = [%bs.obj {
    "SPOT": [%bs.obj {
        "NORTH": Player.North,
        "EAST": Player.East,
        "SOUTH": Player.South,
        "WEST": Player.West
    }],
    "PHASE": [%bs.obj {
        "INITIAL": Phase.Initial,
        "DEALING": Phase.Dealing,
        "BIDDING": Phase.Bidding,
        "PLAYING": Phase.Playing,
        "END": Phase.End
    }],
    "CARD_COLOR": [%bs.obj {
        "SPADES": Deck.Spades,
        "HEARTS": Deck.Hearts,
        "DIAMONDS": Deck.Diamonds,
        "CLUBS": Deck.Clubs
    }]
}];

let formatSpot = (s: Player.player) => switch s {
    | Player.North => "NORTH"
    | Player.East => "EAST"
    | Player.South => "SOUTH"
    | Player.West => "WEST"
};

let formatPhase = (p: Phase.phase) => switch p {
    | Phase.Initial => "INITIAL"
    | Phase.Dealing => "DEALING"
    | Phase.Bidding => "BIDDING"
    | Phase.Playing => "PLAYING"
    | Phase.End => "END"
};

let formatCardColor = (c: Deck.color) => switch c {
    | Deck.Spades => "SPADES"
    | Deck.Hearts => "HEARTS"
    | Deck.Diamonds => "DIAMONDS"
    | Deck.Clubs => "CLUBS"
};

let formatPlayer = (gameUuid, spot: Player.player, p: Engine.playerState) => [%bs.obj {
    "uuid": p.uuid,
    "gameUuid": gameUuid,
    "name": p.name,
    "spot": spot |> formatSpot
}];

let formatBid = (gameUuid, playerUuid, bid: Bid.bid) => switch (bid) {
    | Bid.Pass(_) => [%bs.obj {
        "isPass": true,
        "value": Js.Nullable.null,
        "color": Js.Nullable.null,
        "playerUuid": playerUuid,
        "gameUuid": gameUuid
    }]
    | Bid.Bid(_, v, c) => [%bs.obj {
        "isPass": false,
        "value": Js.Nullable.return(v),
        "color": Js.Nullable.return(formatCardColor(c)),
        "playerUuid": playerUuid,
        "gameUuid": gameUuid
    }]
};

/* @todo: format hands, bids, deck and scores */
let formatGame = (g: Engine.gameState) => {
    [%bs.obj {
        "uuid": g.uuid,
        "players": g.players
            |> Player.PlayerMap.bindings
            |> List.map(((k: Player.player, v: Engine.playerState)) => formatPlayer(g.uuid, k, v))
            |> Array.of_list,
        "bids": g.bids
            |> List.map((b: Bid.bid) => {
                let player = switch (b) {
                    | Bid.Bid(p, _, _) | Pass(p) => g.players |> Player.PlayerMap.find(p)
                };
                formatBid(g.uuid, player.uuid, b)
            })
            |> Array.of_list,
        "phase": g.phase |> formatPhase,
        "dealer": g.dealer |> formatSpot
    }];
};
let formatCards = cards => cards |> List.map((c: Deck.card) => {
    [%bs.obj {
       "color": formatCardColor(c.color),
       "motif": 12
    }]
}) |> Array.of_list;

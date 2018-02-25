
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
    "TEAM": [%bs.obj {
        "NORTH_SOUTH": Player.NorthSouth,
        "EAST_WEST": Player.EastWest
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
    }],
    "CARD_MOTIF": [%bs.obj {
        "ACE": Deck.Ace,
        "V10": Deck.V10,
        "KING": Deck.King,
        "QUEEN": Deck.Queen,
        "JACK": Deck.Jack,
        "V9": Deck.V9,
        "V8": Deck.V8,
        "V7": Deck.V7
    }]
}];

let formatSpot = (s: Player.player) => switch s {
    | Player.North => "NORTH"
    | Player.East => "EAST"
    | Player.South => "SOUTH"
    | Player.West => "WEST"
};

let formatTeam = (s: Player.team) => switch s {
    | Player.NorthSouth => "NORTH_SOUTH"
    | Player.EastWest => "EAST_WEST"
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

let formatCardMotif = (m: Deck.motif) => switch m {
    | Deck.Ace => "ACE"
    | Deck.V10 => "V10"
    | Deck.King => "KING"
    | Deck.Queen => "QUEEN"
    | Deck.Jack => "JACK"
    | Deck.V9 => "V9"
    | Deck.V8 => "V8"
    | Deck.V7 => "V7"
    | _ => "OTHER"
};

let formatPlayer = (gameUuid, spot: Player.player, p: Engine.playerState) => [%bs.obj {
    "uuid": p.uuid,
    "gameUuid": gameUuid,
    "name": p.name,
    "spot": spot |> formatSpot,
    "team": spot |> Player.getTeam |> formatTeam
}];

let formatPlayers = (g: Engine.gameState) => g.players
    |> Player.PlayerMap.bindings
    |> List.map(((k: Player.player, v: Engine.playerState)) => formatPlayer(g.uuid, k, v))
    |> Array.of_list;

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

let formatBids = (g: Engine.gameState) => g.bids
    |> List.map((b: Bid.bid) => {
        let player = switch (b) {
            | Bid.Bid(p, _, _) | Pass(p) => g.players |> Player.PlayerMap.find(p)
        };
        formatBid(g.uuid, player.uuid, b)
    })
    |> Array.of_list;


let formatCard = (c: Deck.card) => [%bs.obj {
   "color": formatCardColor(c.color),
   "motif": formatCardMotif(c.motif)
}];

let formatCards = (cards) => cards
    |> List.map(formatCard)
    |> Array.of_list;

let formatPlayerCards = (playerUuid, g: Engine.gameState) => g
    |> Engine.getCards(playerUuid)
    |> formatCards;

let formatContract = (g: Engine.gameState) => {
    switch g.phase {
        | Phase.Playing => Js.Nullable.return([%bs.obj {
           "player": g.players |> Player.PlayerMap.find(g.contractPlayer) |> formatPlayer(g.uuid, g.contractPlayer),
           "value": g.contractValue,
           "trump": g.trump |> formatCardColor
       }])
       | _ => Js.Nullable.null
    }
};


let formatScore = (g: Engine.gameState, s: Score.score) => [%bs.obj {
    "winner": formatTeam(s.winner),
    "contract": [%bs.obj {
        "player": g.players |> Player.PlayerMap.find(s.contractPlayer) |> formatPlayer(g.uuid, s.contractPlayer),
        "value": s.contractValue,
        "trump": s.trump |> formatCardColor
    }],
    "contractValue": s.contractValue,
    "contractPlayer": g.players |> Player.PlayerMap.find(s.contractPlayer) |> formatPlayer(g.uuid, s.contractPlayer),
    "score": s.score
}];

let formatScores = (g: Engine.gameState) => g.scores
    |> List.map(formatScore(g))
    |> Array.of_list;


/* @todo: format hands, bids, deck and scores */
let formatGame = (g: Engine.gameState) => {
    [%bs.obj {
        "uuid": g.uuid,
        "phase": g.phase |> formatPhase,
        "dealer": g.dealer |> formatSpot,
        "contract": g |> formatContract
    }];
};

let formatOptional = (o) => switch o {
    | Some(v) => Js.Nullable.return(v)
    | None => Js.Nullable.null
}
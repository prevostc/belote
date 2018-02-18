type player = North | East | South | West;
type team = NorthSouth | EastWest;

let nextPlayer = (player) => switch player {
    | North => East
    | East => South
    | South => West
    | West => North
};

let getTeam = (player) => switch player {
    | North => NorthSouth
    | East => EastWest
    | South => NorthSouth
    | West => EastWest
};

/* @todo: using a map is kind of annoying when you sometimes want to search by uuid and sometimes by spot */
module PlayerMap = Map.Make({
    type t = player;
    let playerToNum = player => switch player {
        | North => 0
        | East => 1
        | South => 2
        | West => 3
    };
    let compare = (a, b) => IntUtil.compare(playerToNum(a), playerToNum(b));
});


/* @todo: using a map is kind of annoying when you sometimes want to search by uuid and sometimes by spot */
module TeamMap = Map.Make({
    type t = team;
    let teamToNum = player => switch player {
        | EastWest => 0
        | NorthSouth => 1
    };
    let compare = (a, b) => IntUtil.compare(teamToNum(a), teamToNum(b));
});

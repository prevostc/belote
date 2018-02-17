type player = North | East | South | West;
type team = NorthSouth | EastWest;

let nextPlayer = (player) => switch player {
    | North => East
    | East => South
    | South => West
    | West => North
};

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
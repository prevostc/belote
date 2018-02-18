
type bid = Bid(Player.player, int, Deck.color) | Pass(Player.player);

let filterNotPass = List.filter(bid => switch bid {
    | Bid(_) => true
    | _ => false
});

let capot = 250;
let general = 400;
/* @todo: annonces (belotte/rebelotte/...), coinche */
let firstPlayerIsBidder = (v: int): bool => v === general;

type error = ValueNotLegal(int) | ValueNotHigher(int, int) | NotYourTurn(Player.player, Player.player);
type bidResult = ValidBid | InvalidBid(error);

let bidValidation = (lastBids: list(bid), bid: bid): bidResult => {
    let validValue = v =>
        v mod 10 === 0
        && v >= 80
        && (v <= 160 || v === capot || v === general);

    let bidCount = lastBids |> filterNotPass |> List.length;

    /* check if it's this player to speak */
    let checkValidPlayer = p => lastBids |> ListUtil.last |> b => switch b {
        /* check if this is indeed the next player speaKing */
        | Some(Bid(prevP, _, _) | Pass(prevP)) => Player.nextPlayer(prevP) === p ? ValidBid : InvalidBid(NotYourTurn(prevP, p))
        /* first player is always valid */
        | None => ValidBid
    };

    let checkValidBid = bid => switch bid {
       | Bid(p, v, _) => validValue(v) ? checkValidPlayer(p) : InvalidBid(ValueNotLegal(v))
       | Pass(p) => checkValidPlayer(p)
    };

    if (bidCount === 0) {
        checkValidBid(bid);
    } else {
        let lastBid = lastBids |> filterNotPass |> ListUtil.last;

        switch (lastBid, bid) {
            | (Some(Bid(_, _, _)), Pass(p)) => checkValidPlayer(p)
            | (Some(Bid(_, prev, _)), Bid(p, v, _)) => {
                if (! validValue(v)) {
                    InvalidBid(ValueNotLegal(v))
                } else if (v <= prev) {
                    InvalidBid(ValueNotHigher(prev, v))
                } else {
                    checkValidPlayer(p)
                }
            }
            | (_, bid) => checkValidBid(bid);
        };
    }
};

let bidPhaseEnd = (lastBids: list(bid)): bool => {
    let length = lastBids |> List.length;
    if ((lastBids |> List.length) < 4) {
        false
    } else {
        let isPass = i => switch (List.nth(lastBids, i)) {
            | Pass(_) => true
            | _ => false
        };
        isPass(length - 1) && isPass(length - 2) && isPass(length - 3);
    }
};

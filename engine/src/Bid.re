
type bid = Bid(Player.player, int, Deck.color) | Pass(Player.player);

let filterNotPass = List.filter(bid => switch bid {
    | Bid(_) => true
    | _ => false
});

let validBid = (lastBids: list(bid), bid: bid): bool => {
    let validValue = v =>
        v mod 10 === 0
        && v >= 80
        && (v <= 160 || v === 250 || v === 400);

    let bidCount = lastBids |> filterNotPass |> List.length;
    let isValidPlayer = p => lastBids |> ListUtil.last |> b => switch b {
        | Some(Bid(prevP, _, _) | Pass(prevP)) => Player.nextPlayer(prevP) === p
        | None => true
    };
    if (bidCount === 0) {
        switch bid {
            | Bid(_, v, _) => validValue(v)
            | Pass(_) => true
        };
    } else {
        let lastBid = lastBids |> filterNotPass |> ListUtil.last;

        switch (lastBid, bid) {
            | (Some(Bid(_, _, _)), Pass(p)) => isValidPlayer(p)
            | (Some(Bid(_, prev, _)), Bid(p, v, _)) => validValue(v) && v > prev && isValidPlayer(p)
            | _ => false
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

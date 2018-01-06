

module Game {
    include Deck;
    include Util;

    type player = North | East | South | West;
    type team = NorthSouth | EastWest;

    let nextPlayer = (player) => switch player {
        | North => East
        | East => South
        | South => West
        | West => North
    };
    let playerToNum = player => switch player {
        | North => 0
        | East => 1
        | South => 2
        | West => 3
    };

    module PlayerMap = Map.Make({
        type t = player;
        let compare = (a, b) => Util.intCompare(playerToNum(a), playerToNum(b));
    });

    module Bid {
        type bid = Bid(player, int, Deck.color) | Pass(player);

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
            if (bidCount === 0) {
                switch bid {
                    | Bid(_, v, _) => validValue(v)
                    | Pass(_) => true
                };
            } else {
                let lastBid = lastBids |> filterNotPass |> Util.listLast;
                switch (lastBid, bid) {
                    | (Some(Bid(prevP, _, _) | Pass(prevP)), Pass(p)) => nextPlayer(prevP) === p
                    | (Some(Bid(prevP, prev, _)), Bid(p, v, _)) => validValue(v) && v > prev && nextPlayer(prevP) === p
                    | _ => false
                };
            }
        };
    };

    module Score {
        type score = {
            team: team,
            score: int,
        };
    };

    type phase = Dealing | Bidding | Play | End;

    type action = StartGame | DealCards | MakeBid(Bid.bid);
    type error = InvalidBid(Bid.bid);

    type state = {
        error: option(error),
        phase: phase,
        hands: PlayerMap.t(list(Deck.card)),
        bids: list(Bid.bid),
        dealer: player,
        deck: list(Deck.card),
        scores: list(Score.score),
    };
    
    let initialState = () : state => {
        {
            error: None,
            phase: Dealing,
            hands: PlayerMap.empty 
                |> PlayerMap.add(North, [])
                |> PlayerMap.add(East, [])
                |> PlayerMap.add(South, [])
                |> PlayerMap.add(West, []),
            dealer: North,
            bids: [],
            deck: Deck.newDeck(),
            scores: [],
        };
    };

    let dealHands = (dealer, deck) => {
        let sliceCurry = (deck, start, stop) => deck |> Util.listSlice(start, stop);
        let slc = sliceCurry(deck);
        let rec nextNPlayer = (n, p) => n <= 0 ? p : nextNPlayer(n - 1, nextPlayer(p));
        PlayerMap.empty 
            |> PlayerMap.add(nextNPlayer(1, dealer), slc(0, 2)  @ slc(12, 13) @ slc(20, 22))
            |> PlayerMap.add(nextNPlayer(2, dealer), slc(3, 5)  @ slc(14, 15) @ slc(23, 25))
            |> PlayerMap.add(nextNPlayer(3, dealer), slc(6, 8)  @ slc(16, 17) @ slc(26, 28))
            |> PlayerMap.add(nextNPlayer(4, dealer), slc(9, 11) @ slc(18, 19) @ slc(29, 31))
            ;
    };

    let stateErr = (error, state) => {...state, error: Some(error)};

    let reducer = (action: action, state: state) => {
        let validBid = state.bids |> Bid.validBid;
        switch action {
            | StartGame => { ...state, deck: state.deck |> Util.listShuffle }
            | DealCards => { 
                let rnd = state.deck |> List.length |> Random.int;
                {
                    ...state, 
                    deck: [], 
                    hands: state.deck |> Deck.cut(rnd) |> dealHands(state.dealer),
                    phase: Bidding
                }
            }
            | MakeBid(b) => {
                validBid(b)
                    ? {...state, bids: state.bids @ [b]}
                    : state |> stateErr(InvalidBid(b))
            }
        };
    };

    let dispatch = reducer;
}
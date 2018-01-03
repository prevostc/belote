include Deck;

module Coinche {
    open Util;

    type player = North | East | South | West;
    type team = NorthSouth | EastWest;

    let nextPlayer = (player) => switch player {
        | North => East
        | East => South
        | South => West
        | West => North
    };

    
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

    type state = {
        phase: phase,
        hands: list((player, list(Deck.card))),
        bids: list(Bid.bid),
        deck: list(Deck.card),
        scores: list(Score.score),
    };
    
    let initialState = () : state => {
        {
            phase: Dealing,
            hands: [
                (North, []),
                (East, []),
                (South, []),
                (West, []),
            ],
            bids: [],
            deck: Deck.newDeck(),
            scores: [],
        };
    };


    type request('a, 'b) = Success('a) | Failure('b);
    let bindReducer = (reducer) => ((action, state)) => switch state {
        | Success(state) => Success((action, reducer(action, state)))
        | Failure(m) => Failure(m)
    };

    type action = MakeBid(Bid.bid);
    type error = InvalidBid(Bid.bid);

    let reducer = (action: action, state: state) => {
        let validBid = state.bids |> Bid.validBid;
        switch action {
            | MakeBid(b) => {
                validBid(b)
                    ? Success({...state, bids: state.bids @ [b]})
                    : Failure(InvalidBid(b))
            }
        };
    };

    let applyAction = (res) => switch res {
        | Success((state, action)) => Success((reducer(action, state), action))
        | Failure(m) => Failure(m)
    };
}
include Deck;

module Coinche {

    type req('a, 'b) = Success('a) | Failure('b);
    let bind = (next, input) => switch input {
        | Success(s) => next(s)
        | Failure(f) => Failure(f)
    };

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

        let validBid = (~lastBid: option(bid), ~bid: bid): bool => {
            let validValue = v => 
                v mod 10 === 0 
                && v >= 80 
                && (v <= 160 || v === 250 || v === 400);

            switch (lastBid, bid) {
                | (Some(Bid(prevP, _, _)), Pass(p)) => nextPlayer(prevP) === p
                | (None, Pass(p)) => true
                | (Some(Bid(prevP, prev, _)), Bid(p, v, _)) => validValue(v) && v > prev && nextPlayer(prevP) === p
                | (None, Bid(p, v, _)) => validValue(v)
                | _ => false
            };
        };

        let lastBid = (bids: list(bid)): option(bid) => {
            List.length(bids) === 0 
                ? None
                : Some(List.nth(bids, List.length(bids)))
        }
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

    type action = MakeBid(Bid.bid);
    type error = InvalidBid(Bid.bid);

    let reducer = (~state: state, ~action: action) => {
        switch action {
            | MakeBid(b) => {
                Bid.validBid(~lastBid=Bid.lastBid(state.bids), ~bid=b)
                    ? Success({...state, bids: state.bids @ [b]})
                    : Failure(InvalidBid(b))
            }
        };
    };

    let applyAction = (res) => switch res {
        | Success((state, action)) => Success((reducer(~state=state, ~action=action), action))
        | Failure(m) => Failure(m)
    };
}
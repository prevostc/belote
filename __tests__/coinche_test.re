open Jest;
include Coinche;
include Deck;
include List;

describe("Bid", () => {
  open ExpectJs;
  open Coinche.Bid;

  let isValidBid = validBid([Bid(North, 80, Deck.Spades)]);
  let multiEqTrue = lst => {
    lst |> List.fold_left((&&), true) |> expect |> toEqual(true)
  };
  let multiEqFalse = lst => {
    lst |> List.fold_left((||), false) |> expect |> toEqual(false)
  };

  test("Pass is valid when next", () => {
    Pass(East) |> validBid([Pass(North)]) |> expect |> toEqual(true);
  });
  test("Pass is invalid when not next", () => {
    Pass(South) |> validBid([Pass(North)]) |> expect |> toEqual(true);
  });
  test("A valid bid specials", () => {
    [
      Bid(East, 80, Deck.Spades) |> validBid([]),
      Bid(East, 100, Deck.Spades) |> validBid([]),
      Pass(East) |> isValidBid,
    ] |> multiEqTrue;
  });
  test("A valid bid values", () => {
    [90, 100, 160, 250, 400] 
      |> List.map(v => Bid(East, v, Deck.Spades)) 
      |> List.map(isValidBid) 
      |> multiEqTrue;
  });
  test("An invalid bid", () => {
    [70, 91, 170, 260, 800] 
      |> List.map(v => Bid(East, v, Deck.Spades)) 
      |> List.map(isValidBid) 
      |> multiEqFalse;
  });
});

/*
describe("Game", () => {
  open ExpectJs;

  test("We can make a bid", () => {
    let res = Coinche.applyAction((Coinche.initialState(), MakeBid(Bid(East, 80, Deck.Diamonds))));
    
    switch(res) {
      | Success((s, _)) => expect(List.length(s.bids))
      | Failure(_) => expect(-1)
    } |> toEqual(1)
  });
  test("We can make multiple bids", () => {
    let res = Coinche.initialState() 
    |> (state) => Coinche.applyAction(~action=MakeBid(Bid(East, 80, Deck.Diamonds)), ~state)
    |> (state) => Coinche.applyAction(~action=MakeBid(Bid(East, 80, Deck.Diamonds)), ~state)
    |> (state) => Coinche.applyAction(~action=MakeBid(Bid(East, 80, Deck.Diamonds)), ~state)
    ;
    
    switch(res) {
      | Success(s) => expect(List.length(s.bids))
      | Failure(_) => expect(-1)
    } |> toEqual(1)
  });
});*/
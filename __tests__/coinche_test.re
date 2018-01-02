open Jest;
include Coinche;
include Deck;
include List;

describe("Coinche", () => {
  open ExpectJs;
  open Coinche.Bid;

  let isValidBid = validBid(~lastBid=Some(Bid(North, 80, Deck.Spades)));

  test("Pass is always valid", () => {
    expect(validBid(~lastBid=Some(Pass(North)), ~bid=Pass(East))) |> toEqual(true)
  });
  test("A valid bid", () => {
    expect(List.fold_left((&&), true, [
      validBid(~lastBid=None, ~bid=Bid(East, 80, Deck.Spades)),
      validBid(~lastBid=None, ~bid=Bid(East, 100, Deck.Spades)),
      isValidBid(~bid=Pass(East)),
      isValidBid(~bid=Bid(East, 90, Deck.Spades)),
      isValidBid(~bid=Bid(East, 100, Deck.Spades)),
      isValidBid(~bid=Bid(East, 160, Deck.Spades)),
      isValidBid(~bid=Bid(East, 250, Deck.Spades)),
      isValidBid(~bid=Bid(East, 400, Deck.Spades)),
    ])) |> toEqual(true)
  });
  test("An invalid bid", () => {
    expect(List.fold_left((||), false, [
      isValidBid(~bid=Bid(East, 70, Deck.Spades)),
      isValidBid(~bid=Bid(East, 91, Deck.Spades)),
      isValidBid(~bid=Bid(East, 170, Deck.Spades)),
      isValidBid(~bid=Bid(East, 260, Deck.Spades)),
      isValidBid(~bid=Bid(East, 800, Deck.Spades)),
    ])) |> toEqual(false)
  });
});


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
});
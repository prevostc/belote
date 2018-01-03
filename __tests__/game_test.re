open Jest;
include Game;
include Deck;
include List;

describe("Bid", () => {
  open ExpectJs;
  open Game.Bid;

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

describe("Game", () => {
  open ExpectJs;
  open Game;
  open Game.Bid;

  let mkBid = (p, v) => Bid(p, v, Deck.Diamonds);
  let dspBid = (p, v) => MakeBid(mkBid(p, v)) |> dispatch;

  test("We can make a bid", () => {
    let state = initialState() |> dspBid(East, 80);
    state.bids |> List.length |> expect |> toEqual(1);
  });

  test("We can make multiple bids", () => {
    let state = initialState() 
      |> dspBid(North, 80)
      |> dspBid(East, 90)
      |> dspBid(South, 100)
      |> dspBid(West, 110)
      ;
    state.bids |> List.length |> expect |> toEqual(4);
  });

  test("Bad bids gets rejected", () => {
    let state = initialState() 
      |> dspBid(North, 80)
      |> dspBid(East, 80)
      ;
    state.bids |> List.length |> expect |> toEqual(1);
  });

  test("Bad bids gets rejected", () => {
    let state = initialState() 
      |> dspBid(North, 80)
      |> dspBid(East, 80)
      ;
    state.error |> expect |> toEqual(Some(InvalidBid(mkBid(East, 80))));
  });
});
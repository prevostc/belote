open Jest;
include List;
include Array;
include Deck;
include Game;
include Util;

describe("Bid", () => {
  open ExpectJs;
  open Game.Bid;

  let isValidBid = validBid([Bid(North, 80, Deck.Spades)]);
  let multiEqBuilder = (valueMap, expectedMap) => lst => lst 
    |> List.map(valueMap) |> Array.of_list 
    |> expect |> toEqual(
      lst |> List.map(expectedMap) |> Array.of_list
    );
  let multiEqFalse = multiEqBuilder(v => v, (_) => false);
  let multiEqTrue = multiEqBuilder(v => v, (_) => true);

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

describe("Game bid", () => {
  open ExpectJs;
  open Game.Bid;

  let mkBid = (p, v) => Bid(p, v, Deck.Diamonds);
  let dispatchBid = (p, v) => Game.MakeBid(mkBid(p, v)) |> Game.dispatch;

  test("We can make a bid", () => {
    let state = Game.initialState() |> dispatchBid(East, 80);
    state.bids |> List.length |> expect |> toEqual(1);
  });

  test("We can make multiple bids", () => {
    let state = Game.initialState() 
      |> dispatchBid(North, 80)
      |> dispatchBid(East, 90)
      |> dispatchBid(South, 100)
      |> dispatchBid(West, 110)
      ;
    state.bids |> List.length |> expect |> toEqual(4);
  });

  test("Bad bids gets rejected", () => {
    let state = Game.initialState() 
      |> dispatchBid(North, 80)
      |> dispatchBid(East, 80)
      ;
    state.bids |> List.length |> expect |> toEqual(1);
  });

  test("Bad bids gets rejected", () => {
    let state = Game.initialState() 
      |> dispatchBid(North, 80)
      |> dispatchBid(East, 80)
      ;
    state.error |> expect |> toEqual(Some(Game.InvalidBid(mkBid(East, 80))));
  });
});


describe("Game Dealing", () => {
  open ExpectJs;

  let initialState = Game.initialState();

  test("deal cards", () => {
    let mkCard = (c, m): Deck.card => { color: c, motif: m };
    let dealt = Game.dealHands(Game.East, initialState.deck);
    dealt |> Game.PlayerMap.find(South) |> Array.of_list |> expect |> toEqual([
      mkCard(Spades, King), mkCard(Spades, Queen), mkCard(Spades, Jack),
      mkCard(Hearts, Value(7)), mkCard(Hearts, Value(8)),
      mkCard(Diamonds, Value(7)), mkCard(Diamonds, Value(8)), mkCard(Diamonds, Value(9)),
    ] |> Array.of_list);
  });

  test("The deck gets shuffled", () => {
    let state = initialState |> Game.dispatch(Game.StartGame);
    Util.listEq(Deck.cmpCard, state.deck, initialState.deck) |> expect |> toEqual(false);
  });

  test("The deck gets dealt", () => {
    let state = initialState |> Game.dispatch(Game.DealCards);
    state.hands |> Game.PlayerMap.find(Game.South) |> List.length |> expect |> toEqual(8);
  });
});
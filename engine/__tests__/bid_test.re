open Jest;

describe("Bid validation", () => {
  open ExpectJs;
  open Bid;

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
  test("Pass is valid when next to pass", () => {
    Pass(East) |> validBid([Bid(West, 80, Deck.Spades), Pass(North)]) |> expect |> toEqual(true);
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

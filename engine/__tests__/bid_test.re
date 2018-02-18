open Jest;

describe("Bid validation", () => {
  open ExpectJs;
  open Bid;

  let multiEqBuilder = (valueMap, expectedMap) => lst => lst
    |> List.map(valueMap) |> Array.of_list 
    |> expect |> toEqual(
      lst |> List.map(expectedMap) |> Array.of_list
    );
  let multiEqValid = multiEqBuilder(v => v, (_) => ValidBid);

  test("Pass is valid when next", () => {
    Pass(East) |> bidValidation([Pass(North)]) |> expect |> toEqual(ValidBid);
  });
  test("Pass is valid when next to pass", () => {
    Pass(East) |> bidValidation([Bid(West, 80, Deck.Spades), Pass(North)]) |> expect |> toEqual(ValidBid);
  });
  test("A valid bid specials", () => {
    [
      Bid(East, 80, Deck.Spades) |> bidValidation([]),
      Bid(East, 100, Deck.Spades) |> bidValidation([]),
      Pass(East) |> bidValidation([]),
    ] |> multiEqValid;
  });
  test("A valid bid values", () => {
    [90, 100, 160, 250, 400] 
      |> List.map(v => Bid(East, v, Deck.Spades)) 
      |> List.map(bidValidation([]))
      |> multiEqValid;
  });

  test("An invalid bid", () => {
    [70, 91, 170, 260, 800]
      |> List.map(v => Bid(East, v, Deck.Spades))
      |> List.map(bidValidation([]))
      |> expect |> toEqual(
        [70, 91, 170, 260, 800]
           |> List.map(v => InvalidBid(ValueNotLegal(v)))
      );
  });
  test("Bid is NOT valid value is not higher", () => {
    Bid(East, 80, Deck.Spades) |> bidValidation([Bid(North, 80, Deck.Spades)]) |> expect |> toEqual(InvalidBid(ValueNotHigher(80, 80)));
  });
  test("Pass is NOT valid when not next", () => {
    Pass(South) |> bidValidation([Pass(North)]) |> expect |> toEqual(InvalidBid(NotYourTurn(North, South)));
  });
  test("Bid is NOT valid when not next, last is pass", () => {
    Bid(South, 100, Deck.Spades) |> bidValidation([Pass(North)]) |> expect |> toEqual(InvalidBid(NotYourTurn(North, South)));
  });
  test("Bid is NOT valid when not next, last is bid", () => {
    Bid(South, 100, Deck.Spades) |> bidValidation([Bid(North, 80, Deck.Spades)]) |> expect |> toEqual(InvalidBid(NotYourTurn(North, South)));
  });
});

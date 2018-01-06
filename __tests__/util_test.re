open Jest;
include Util;
include List;

describe("Util.listLast", () => {
  open ExpectJs;

  test("Returns last element", () => {
    [1, 2, 3, 4] |> Util.listLast |> expect |> toEqual(Some(4));
  });
  test("Handles empty lists", () => {
    [] |> Util.listLast |> expect |> toEqual(None);
  });
});

describe("Util.listProduct", () => {
  open ExpectJs;

  test("Create carthesian product of 2 lists", () => {
    Util.listProduct([1, 2, 3], ["a", "b"]) |> List.length |> expect |> toEqual(6);
  });
  test("Handles empty lists", () => {
    Util.listProduct([], []) |> List.length |> expect |> toEqual(0);
  });
});

describe("Util.listShuffle", () => {
  open ExpectJs;

  test("The list gets shuffled", () => {
    Util.listShuffle([1, 2, 3, 4, 5, 6, 7, 8, 9, 10]) |> Util.listEq((===), [1, 2, 3, 4, 5, 6, 7, 8, 9, 10]) |> expect |> toEqual(false);
  });
});


describe("Util.listEq", () => {
  open ExpectJs;

  test("Compare equal lists", () => {
    Util.listEq((===), [1, 2, 3, 4, 5], [1, 2, 3, 4, 5]) |> expect |> toEqual(true);
  });
  test("Compare equal lists of tuples", () => {
    let cmpTuples = (a, b) => switch (a, b) {
      | ((al, ar), (bl, br)) => al === bl && ar === br
    };
    Util.listEq(cmpTuples, [(1, 2)], [(1, 2)]) |> expect |> toEqual(true);
  });
  test("Compare not equal lists", () => {
    Util.listEq((===), [1, 2, 3, 4, 5], [1, 2, 4, 4, 5]) |> expect |> toEqual(false);
  });
});


describe("Util.listSlice", () => {
  open ExpectJs;

  test("Slices a list", () => {
    [1, 2, 3, 4, 5] |> Util.listSlice(0, 2) |> expect |> toEqual([1, 2, 3]);
  });
});
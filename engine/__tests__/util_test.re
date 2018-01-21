open Jest;
include Util;
include List;


describe("Util.intCompare", () => {
  open ExpectJs;

  test("Compares two values", () => {
    Util.intCompare(10, 20) |> expect |> toEqual(1);
  });
  test("Compares two values", () => {
    Util.intCompare(20, 10) |> expect |> toEqual(-1);
  });
  test("Compares two values", () => {
    Util.intCompare(10, 10) |> expect |> toEqual(0);
  });
});

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

describe("Util.listFind", () => {
  open ExpectJs;

  test("Finds None", () => {
    [1, 2, 3, 4, 5] |> Util.listFind(e => e === 12) |> expect |> toEqual(None);
  });
  test("Finds an element in an array", () => {
    [1, 2, 3, 4, 5] |> Util.listFind(e => e === 4) |> expect |> toEqual(Some(4));
  });
  test("Finds the first element in an array", () => {
    [(1, 1), (1, 2), (1, 3)] |> Util.listFind(((one, i)) => one === 1) |> expect |> toEqual(Some((1, 1)));
  });
});


describe("Util.arrayFind", () => {
  open ExpectJs;

  test("Finds None", () => {
    [|1, 2, 3, 4, 5|] |> Util.arrayFind(e => e === 12) |> expect |> toEqual(None);
  });
  test("Finds an element in an array", () => {
    [|1, 2, 3, 4, 5|] |> Util.arrayFind(e => e === 4) |> expect |> toEqual(Some(4));
  });
  test("Finds the first element in an array", () => {
    [|(1, 1), (1, 2), (1, 3)|] |> Util.arrayFind(((one, i)) => one === 1) |> expect |> toEqual(Some((1, 1)));
  });
});


describe("Util.arrayFilter", () => {
  open ExpectJs;

  test("Filters nothing", () => {
    [|1, 2, 3, 4, 5|] |> Util.arrayFilter(e => e !== 12) |> expect |> toEqual([|1, 2, 3, 4, 5|]);
  });
  test("Filters one element", () => {
    [|1, 2, 3, 4, 5|] |> Util.arrayFilter(e => e !== 4) |> expect |> toEqual([|1, 2, 3, 5|]);
  });
  test("Filters multiple elements", () => {
    [|(1, 1), (1, 2), (2, 3)|] |> Util.arrayFilter(((a, b)) => a !== 1) |> expect |> toEqual([|(2, 3)|]);
  });
})
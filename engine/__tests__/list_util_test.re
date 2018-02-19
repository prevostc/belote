open Jest;

describe("ListUtil.last", () => {
  open ExpectJs;
  open ListUtil;

  test("Returns last element", () => {
    [1, 2, 3, 4] |> last |> expect |> toEqual(Some(4));
  });
  test("Handles empty lists", () => {
    [] |> last |> expect |> toEqual(None);
  });
});

describe("ListUtil.product", () => {
  open ExpectJs;
  open ListUtil;

  test("Create carthesian product of 2 lists", () => {
    product([1, 2, 3], ["a", "b"]) |> List.length |> expect |> toEqual(6);
  });
  test("Handles empty lists", () => {
    product([], []) |> List.length |> expect |> toEqual(0);
  });
});

describe("ListUtil.shuffle", () => {
  open ExpectJs;
  open ListUtil;

  test("The list gets shuffled", () => {
    shuffle([1, 2, 3, 4, 5, 6, 7, 8, 9, 10]) |> equals((===), [1, 2, 3, 4, 5, 6, 7, 8, 9, 10]) |> expect |> toEqual(false);
  });
});


describe("ListUtil.equals", () => {
  open ExpectJs;
  open ListUtil;

  test("Compare equal lists", () => {
    equals((===), [1, 2, 3, 4, 5], [1, 2, 3, 4, 5]) |> expect |> toEqual(true);
  });
  test("Compare equal lists of tuples", () => {
    let cmpTuples = (a, b) => switch (a, b) {
      | ((al, ar), (bl, br)) => al === bl && ar === br
    };
    equals(cmpTuples, [(1, 2)], [(1, 2)]) |> expect |> toEqual(true);
  });
  test("Compare not equal lists", () => {
    equals((===), [1, 2, 3, 4, 5], [1, 2, 4, 4, 5]) |> expect |> toEqual(false);
  });
});


describe("ListUtil.slice", () => {
  open ExpectJs;
  open ListUtil;

  test("Slices a list", () => {
    [1, 2, 3, 4, 5] |> slice(0, 2) |> expect |> toEqual([1, 2, 3]);
  });
});

describe("ListUtil.find", () => {
  open ExpectJs;
  open ListUtil;

  test("Finds None", () => {
    [1, 2, 3, 4, 5] |> find(e => e === 12) |> expect |> toEqual(None);
  });
  test("Finds an element in an array", () => {
    [1, 2, 3, 4, 5] |> find(e => e === 4) |> expect |> toEqual(Some(4));
  });
  test("Finds the first element in an array", () => {
    [(1, 1), (1, 2), (1, 3)] |> find(((one, _)) => one === 1) |> expect |> toEqual(Some((1, 1)));
  });
});


describe("ListUtil.indexOf", () => {
  open ExpectJs;
  open ListUtil;

  test("Index of simple existing element", () => {
    [10, 20, 30, 40, 50] |> indexOf(20, (===)) |> expect |> toEqual(Some(1));
  });
  test("Index of simple non existing element", () => {
    [10, 20, 30, 40, 50] |> indexOf(100, (===)) |> expect |> toEqual(None);
  });
  test("Index of complex existing element", () => {
    [(1, 1), (1, 2), (1, 3)] |> indexOf((1, 2), ((a1, a2), (b1, b2)) => a1 === b1 && a2 === b2) |> expect |> toEqual(Some(1));
  });
  test("Index of complex non existing element", () => {
    [(1, 1), (1, 2), (1, 3)] |> indexOf((10, 20), ((a1, a2), (b1, b2)) => a1 === b1 && a2 === b2) |> expect |> toEqual(None);
  });
});


describe("ListUtil.flatMap", () => {
  open ExpectJs;
  open ListUtil;

  test("Index of simple existing element", () => {
    [1, 2, 3, 4, 5] |> flatMap(e => [e, e * 10]) |> expect |> toEqual([1, 10, 2, 20, 3, 30, 4, 40, 5, 50]);
  });
});
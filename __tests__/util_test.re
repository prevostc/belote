open Jest;
include Util;
include List;

describe("Util", () => {
  open ExpectJs;

  test("listLast: Returns last element", () => {
    [1, 2, 3, 4] |> Util.listLast |> expect |> toEqual(Some(4));
  });
  test("listLast: Handles empty lists", () => {
    [] |> Util.listLast |> expect |> toEqual(None);
  });

  test("listProduct: Create carthesian product of 2 lists", () => {
    Util.listProduct([1, 2, 3], ["a", "b"]) |> List.length |> expect |> toEqual(6);
  });
  test("listProduct: Handles empty lists", () => {
    Util.listProduct([], []) |> List.length |> expect |> toEqual(0);
  });
});
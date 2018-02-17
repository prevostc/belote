open Jest;

describe("Util.intCompare", () => {
  open ExpectJs;

  test("Compares two values", () => {
    compare(10, 20) |> expect |> toEqual(-1);
  });
  test("Compares two values", () => {
    compare(20, 10) |> expect |> toEqual(1);
  });
  test("Compares two values", () => {
    compare(10, 10) |> expect |> toEqual(0);
  });
});

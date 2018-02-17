open Jest;

describe("ArrayUtil.find", () => {
  open ExpectJs;
  open ArrayUtil;

  test("Finds None", () => {
    [|1, 2, 3, 4, 5|] |> find(e => e === 12) |> expect |> toEqual(None);
  });
  test("Finds an element in an array", () => {
    [|1, 2, 3, 4, 5|] |> find(e => e === 4) |> expect |> toEqual(Some(4));
  });
  test("Finds the first element in an array", () => {
    [|(1, 1), (1, 2), (1, 3)|] |> find(((one, _)) => one === 1) |> expect |> toEqual(Some((1, 1)));
  });
});


describe("ArrayUtil.filter", () => {
  open ExpectJs;
  open ArrayUtil;

  test("Filters nothing", () => {
    [|1, 2, 3, 4, 5|] |> filter(e => e !== 12) |> expect |> toEqual([|1, 2, 3, 4, 5|]);
  });
  test("Filters one element", () => {
    [|1, 2, 3, 4, 5|] |> filter(e => e !== 4) |> expect |> toEqual([|1, 2, 3, 5|]);
  });
  test("Filters multiple elements", () => {
    [|(1, 1), (1, 2), (2, 3)|] |> filter(((a, _)) => a !== 1) |> expect |> toEqual([|(2, 3)|]);
  });
})
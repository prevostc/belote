open Jest;

describe("Engine API tests", () => {
  open ExpectJs;

  let game = Engine.createGame("1234");

  test("Creates a game state", () => {
    game##players |> expect |> toEqual([| |]);
  });
  test("Creates a game state", () => {
    game##uuid |> expect |> toEqual("1234");
  });

  test("Join a game", () => {
    let (p, g) = Engine.joinGame(game, "1", "john", "NORTH");
    g##players |> expect |> toEqual([| p |]);
  });

  test("Multiple join a game", () => {
    let (_, g1) = Engine.joinGame(game, "1", "john", "NORTH");
    let (_, g2) = Engine.joinGame(g1, "2", "john", "SOUTH");
    g2##players |> Array.length |> expect |> toEqual(2);
  });

  test("Cannot join a if spot taken", () => {
    let (p1, g1) = Engine.joinGame(game, "1", "john", "NORTH");
    let (_, g2) = Engine.joinGame(g1, "2", "john", "NORTH");
    g2##players |> expect |> toEqual([| p1 |]);
  });

  test("Move spot if uuid taken", () => {
    let (_, g1) = Engine.joinGame(game, "1", "john", "NORTH");
    let (p2, g2) = Engine.joinGame(g1, "1", "john", "SOUTH");
    g2##players |> expect |> toEqual([| p2 |]);
  });


});
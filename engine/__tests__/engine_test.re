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

  test("Cannot join if spot invalid", () => {
    let (p1, g1) = Engine.joinGame(game, "1", "john", "WAT");
    g1##players |> expect |> toEqual([| |]);
  });

  test("Move spot if uuid taken", () => {
    let (_, g1) = Engine.joinGame(game, "1", "john", "NORTH");
    let (p2, g2) = Engine.joinGame(g1, "1", "john", "SOUTH");
    g2##players |> expect |> toEqual([| p2 |]);
  });

  test("Move spot if uuid taken", () => {
    let (_, g1) = Engine.joinGame(game, "1", "john", "NORTH");
    let (p2, g2) = Engine.joinGame(g1, "1", "john", "SOUTH");
    g2##players |> expect |> toEqual([| p2 |]);
  });

  test("Join scenario", () => {
    let (p1, g1) = Engine.joinGame(game, "1", "john", "NORTH");
    let (p2, g2) = Engine.joinGame(g1, "2", "doe", "SOUTH");
    let (p3, g3) = Engine.joinGame(g2, "1", "john", "WEST");
    let (p4, g4) = Engine.joinGame(g3, "2", "doe", "NORTH");
    let (p5, g5) = Engine.joinGame(g4, "1", "john", "NORTH");

    let format = ((players, p)) => (Array.map(p => (p##uuid, p##spot), players), (p##uuid, p##spot));
    (
        (g1##players, p1) |> format,
        (g2##players, p2) |> format,
        (g3##players, p3) |> format,
        (g4##players, p4) |> format,
        (g5##players, p5) |> format
    ) |> expect |> toEqual((
        ([|("1", "NORTH")|], ("1", "NORTH")),
        ([|("1", "NORTH"), ("2", "SOUTH")|], ("2", "SOUTH")),
        ([|("1", "WEST"), ("2", "SOUTH")|], ("1", "WEST")),
        ([|("2", "NORTH"), ("1", "WEST")|], ("2", "NORTH")),
        ([|("2", "NORTH"), ("1", "WEST")|], ("1", "WEST"))
    ));
  });


});
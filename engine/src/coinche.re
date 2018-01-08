open Game;

type coinche = Js.t({.
    uuid: string,
    state: Game.state,
});

let createGame = (uuid): coinche => [%bs.obj {
    uuid: uuid,
    state: Game.initialState()
}]

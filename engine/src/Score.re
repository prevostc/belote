
type score = {
    winner: Player.team,
    trump: Deck.color,
    contractValue: int,
    contractPlayer: Player.player,
    score: int,
};

let getCardScore = (trump: Deck.color, card: Deck.card) => {
    switch ((trump === card.color, card.motif)) {
        | (true, Deck.Jack) => 20
        | (true, Deck.V9) => 14
        | (_, Deck.Ace) => 11
        | (_, Deck.V10) => 10
        | (_, Deck.King) => 4
        | (_, Deck.Queen) => 3
        | (false, Deck.Jack) => 2
        | _ => 0
    }
};

/* @todo: 10 de der, belotte */
let getCardsScore = (trump: Deck.color, cards: list(Deck.card)) => cards |> List.fold_left((sum, v) => sum + getCardScore(trump, v), 0);

/* @todo: handle special case where you made the contract but made less points than the adversary */
let contractToScore = (trump, contractValue, contractPlayer, graveyard) => {
    let playerTeam = Player.getTeam(contractPlayer);
    let getCards = (filter) => [Player.North, Player.South, Player.East, Player.West]
        |> List.filter(filter)
        |> ListUtil.flatMap(p => Player.PlayerMap.find(p, graveyard));

    /* @todo: use strategy pattern to factorise this code duplication */

    /* contract player must win every cards */
    if (contractValue === Bid.general) {
        let othersCards = getCards(p => p !== contractPlayer);
        let contractWon = (othersCards |> List.length) === 0;
        { trump: trump, winner: contractWon ? playerTeam : Player.getOtherTeam(playerTeam), score: contractValue, contractValue: contractValue, contractPlayer: contractPlayer }

    /* contract team must win every cards */
    } else if (contractValue === Bid.capot) {
        let othersCards = getCards(p => Player.getTeam(p) !== playerTeam);
        let contractWon = (othersCards |> List.length) === 0;
        { trump: trump, winner: contractWon ? playerTeam : Player.getOtherTeam(playerTeam), score: contractValue, contractValue: contractValue, contractPlayer: contractPlayer }

    /* contract team must make more points than contractValue */
    } else {
        let teamCards = getCards(p => Player.getTeam(p) === playerTeam);
        let contractWon = getCardsScore(trump, teamCards) >= contractValue;
        { trump: trump, winner: contractWon ? playerTeam : Player.getOtherTeam(playerTeam), score: contractValue, contractValue: contractValue, contractPlayer: contractPlayer }
    }
};
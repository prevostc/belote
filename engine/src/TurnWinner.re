
/* true if the current player team is currently winning the table */
let isTeamWinningTable = (trump: Deck.color, table: list(Deck.card)) => {
    let l = table |> List.length;
    if (l < 2) {
        false
    } else {
        let winningCard = table |> CardOrder.getWinningCard(trump);
        let teamCard = List.nth(table, l - 2);
        switch (winningCard) {
            /* team is winning */
            | Some(c) => Deck.cardEquals(c, teamCard)
            | None => false
        }
    }
};

let getWinningPlayer = (first: Player.player, trump: Deck.color, table: list(Deck.card)): Player.player => {
    let winningCard = table |> CardOrder.getWinningCard(trump);
    switch winningCard {
        | Some(card) => {
            let cardIdx = table |> ListUtil.indexOf(card, Deck.cardEquals);
            switch cardIdx {
                | Some(idx) => first |> FnUtil.applyN(idx, Player.nextPlayer)
                | None => raise(Failure("Could not find idx of winning card"))
            }
        }
        | None => raise(Failure("Could not find winning card"))
    }
};


type error =
    CardNotInHand(Deck.card)
    | MustPlayColor(Deck.color, Deck.color)
    | MustPlayTrump
    | MustPlayHigherTrump(Deck.motif, Deck.motif, Deck.motif)
    | TableIsFull
    | NotYourTurn(Player.player, Player.player)
;
type cardPlayResult = ValidCardPlay | InvalidCardPlay(error);

/*
 * utility function: apply same function n times
 * applyN(3, f, x) => f(f(f(x)))
 */
let rec applyN = (n, f, x) => n <= 0 ? x : applyN(n - 1, f, f(x));

/* asked color is the first color played */
let getAskedColor = (table: list(Deck.card)) => (table |> List.hd).color;

/* find the highest motif of a particular color */
let getHighestMotifOfColor = (color: Deck.color, table: list(Deck.card)): option(Deck.motif) => {
    let sortedTableTrumps = table
        |> List.filter((c: Deck.card) => c.color === color)
        |> List.map((c: Deck.card) => c.motif)
        |> List.sort(Deck.motifCompare)
        |> List.rev
    ;
    if ((sortedTableTrumps |> List.length) <= 0) {
        None
    } else {
        Some(sortedTableTrumps |> List.hd)
    }
};

/* find out who is currently winning */
let getWinningCard = (trump: Deck.color, table: list(Deck.card)): option(Deck.card) => {
    if ((table |> List.length) <= 0) {
        None
    } else {
        let askedColor = table |> getAskedColor;
        let highestAsked = table |> getHighestMotifOfColor(askedColor);
        let highestTrump = table |> getHighestMotifOfColor(trump);
        switch ((highestTrump, highestAsked)) {
            /* biggest trump is winning */
            | (Some(m), _) => Some(Deck.{ color: trump, motif: m })
            /* biggest of asked color is winning */
            | (None, Some(m)) => Some(Deck.{ color: askedColor, motif: m })
            | _ => None
        }
    }
};

/* true if the player team is currently winning the table */
let isTeamWinningTable = (trump: Deck.color, table: list(Deck.card), spot: Player.player) => {
    let l = table |> List.length;
    if (l < 2) {
        false
    } else {
        let winningCard = table |> getWinningCard(trump);
        let teamCard = List.nth(table, l - 2);
        switch (winningCard) {
            /* team is winning */
            | Some(c) => Deck.cardEquals(c, teamCard)
            | None => false
        }
    }
};

/* card playing business rules, yes it's that complex... */
let cardPlayValidation = (
    first: Player.player,
    trump: Deck.color,
    table: list(Deck.card),
    hand: list(Deck.card),
    spot: Player.player,
    card: Deck.card
): cardPlayResult => {
    let cardInHand = hand |> List.exists(c => Deck.cardEquals(c, card));
    let tableIsFull = (table |> List.length) === 4;
    let tableIsEmpty = (table |> List.length) === 0;
    let nextToPlay = applyN(table |> List.length, Player.nextPlayer, first);

    if (! cardInHand) {
        InvalidCardPlay(CardNotInHand(card))
    } else if (nextToPlay !== spot) {
        InvalidCardPlay(NotYourTurn(nextToPlay, spot))
    } else if (tableIsFull) {
        InvalidCardPlay(TableIsFull)
    } else if (tableIsEmpty) {
        ValidCardPlay
    } else {
        /* is next to play, table is not empty but not full */
        let askedColor = table |> getAskedColor;
        let hasAskedColor = hand |> List.exists((c: Deck.card) => c.color === askedColor);
        let hasTrump = hand |> List.exists((c: Deck.card) => c.color === trump);

        /* trump asked */
        if (askedColor === trump) {
            ValidCardPlay

        /* non trump asked */
        } else {
            /* you have the asked color and must play it */
            if (hasAskedColor) {
                card.color === askedColor ? ValidCardPlay : InvalidCardPlay(MustPlayColor(askedColor, card.color))

            /* you have some trump, rules apply*/
            } else if (hasTrump) {
                /* team is winning, can play anything */
                if (spot |> isTeamWinningTable(trump, table)) {
                    ValidCardPlay

                /* team is not winning, you have trump and must play it*/
                } else if (card.color !== trump) {
                    InvalidCardPlay(MustPlayTrump)

                /* you rightfully played trump */
                } else {
                    let highestTrump = table |> getHighestMotifOfColor(trump);
                    let highestTrumpInHand = hand |> getHighestMotifOfColor(trump);

                    switch ((highestTrump, highestTrumpInHand)) {
                        | (Some(m), Some(mh)) => {
                            Js.log(("HERE", m, mh, card.motif));
                            /* you player a higher trump */
                            if (Deck.motifGreaterThan(m, card.motif)) {
                                ValidCardPlay

                            /* you had a higher trump, but did not played it */
                            } else if (Deck.motifGreaterThan(m, mh)) {
                                InvalidCardPlay(MustPlayHigherTrump(card.motif, m, mh))

                            /* played a lower trump intentionnaly */
                            } else {
                                ValidCardPlay
                            }
                        }
                        | _ => ValidCardPlay
                    }
                }

            /* you do not have trumps and you do not have the asked color */
            } else {
                ValidCardPlay
            }
        }
    }
}
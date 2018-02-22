
type error =
    CardNotInHand(Deck.card)
    | MustPlayColor(Deck.color, Deck.color)
    | MustPlayTrump
    | MustPlayHigherTrump(Deck.motif, Deck.motif, Deck.motif)
    | TableIsFull
    | NotYourTurn(Player.player, Player.player)
;
type cardPlayResult = ValidCardPlay | InvalidCardPlay(error);

/* asked color is the first color played */
let getAskedColor = (table: list(Deck.card)) => (table |> List.hd).color;

let getNextToPlay = (first, table) => FnUtil.applyN(table |> List.length, Player.nextPlayer, first);

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
    let nextToPlay = table |> getNextToPlay(first);

    let validateMustPlayTrump = () => {
        let highestTrump = table |> CardOrder.getHighestTrump(trump);
        let highestTrumpInHand = hand |> CardOrder.getHighestTrump(trump);

        switch ((highestTrump, highestTrumpInHand)) {
            | (Some(m), Some(mh)) => {
                let hadHigherMotif = CardOrder.motifGreaterThan(CardOrder.trumpOrder, m);
                /* you player a higher trump */
                if (hadHigherMotif(card.motif)) {
                    ValidCardPlay

                /* you had a higher trump, but did not played it */
                } else if (hadHigherMotif(mh)) {
                    InvalidCardPlay(MustPlayHigherTrump(card.motif, m, mh))

                /* played a lower trump intentionnaly */
                } else {
                    ValidCardPlay
                }
            }
            | _ => ValidCardPlay
        }
    };

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

            /* if trump is asked and you have some, play higher */
            if (hasTrump) {
                validateMustPlayTrump()

            } else {
                ValidCardPlay
            }

        /* non trump asked */
        } else {
            if (hasAskedColor) {
                /* you have the asked color and must play it */
                if (card.color !== askedColor) {
                    InvalidCardPlay(MustPlayColor(askedColor, card.color))

                /* everything else is permitted*/
                } else {
                    ValidCardPlay
                }

            /* you have some trump, rules apply*/
            } else if (hasTrump) {
                /* team is winning, can play anything */
                if (table |> TurnWinner.isTeamWinningTable(trump)) {
                    ValidCardPlay

                /* team is not winning, you have trump and must play it*/
                } else if (card.color !== trump) {
                    InvalidCardPlay(MustPlayTrump)

                /* you rightfully played trump */
                } else {
                    validateMustPlayTrump()
                }

            /* you do not have trumps and you do not have the asked color */
            } else {
                ValidCardPlay
            }
        }
    }
}
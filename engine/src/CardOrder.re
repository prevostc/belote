
/* A K Q J 10 9 8 7 */
let pokerOrder = [Deck.Ace, Deck.King, Deck.Queen, Deck.Jack, Deck.V10, Deck.V9, Deck.V8, Deck.V7];

/* J 9 A 10 K Q 8 7 */
let trumpOrder = [Deck.Jack, Deck.V9, Deck.Ace, Deck.V10, Deck.King, Deck.Queen, Deck.V8, Deck.V7];

/* A 10 K Q J 9 8 7 */
let plainOrder = [Deck.Ace, Deck.V10, Deck.King, Deck.Queen, Deck.Jack, Deck.V9, Deck.V8, Deck.V7];

let motifCompare = (order, motifA, motifB): int => {
    let getIdx = (motif, order) => order |> ListUtil.indexOf(motif, (===));
    let length = order |> List.length;
    let indexes = (order |> getIdx(motifA), order |> getIdx(motifB));
    switch (indexes) {
        | (Some(idxA), Some(idxB)) => compare(length - idxA, length - idxB)
        | (None, Some(_)) => 1
        | (Some(_), None) => -1
        | (None, None) => 0
    }
};

let sortMotifs = (order, lst) => lst |> List.sort(motifCompare(order));

/* true if B is strictly greater than A */
let motifGreaterThan = (order, mA, mB) => motifCompare(order, mA, mB) < 0;
/* asked color is the first color played */
let getAskedColor = (table: list(Deck.card)) => (table |> List.hd).color;

/* find the highest motif of a particular color */
let getHighestMotifOfColor = (isTrump: bool, color: Deck.color, table: list(Deck.card)): option(Deck.motif) => {
    let sortedCardsOfColor = table
        |> List.filter((c: Deck.card) => c.color === color)
        |> List.map((c: Deck.card) => c.motif)
        |> sortMotifs(isTrump ? trumpOrder : plainOrder)
        |> List.rev
    ;

    if ((sortedCardsOfColor |> List.length) <= 0) {
        None
    } else {
        Some(sortedCardsOfColor |> List.hd)
    }
};

let getHighestNonTrump = getHighestMotifOfColor(false);
let getHighestTrump = getHighestMotifOfColor(true);

/* find out who is currently winning */
let getWinningCard = (trump: Deck.color, table: list(Deck.card)): option(Deck.card) => {
    if ((table |> List.length) <= 0) {
        None
    } else {
        let askedColor = table |> getAskedColor;
        let highestAsked = table |> getHighestNonTrump(askedColor);
        let highestTrump = table |> getHighestTrump(trump);
        switch ((highestTrump, highestAsked)) {
            /* biggest trump is winning */
            | (Some(m), _) => Some(Deck.{ color: trump, motif: m })
            /* biggest of asked color is winning */
            | (None, Some(m)) => Some(Deck.{ color: askedColor, motif: m })
            | _ => None
        }
    }
};

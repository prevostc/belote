
let dealHands = (dealer: Player.player, deck: list(Deck.card)) => {
    let sliceCurry = (deck, start, stop) => deck |> ListUtil.slice(start, stop);
    let slc = sliceCurry(deck);
    let rec nextNPlayer = (n, p) => n <= 0 ? p : nextNPlayer(n - 1, Player.nextPlayer(p));
    Player.PlayerMap.empty
        |> Player.PlayerMap.add(nextNPlayer(1, dealer), slc(0, 2)  @ slc(12, 13) @ slc(20, 22))
        |> Player.PlayerMap.add(nextNPlayer(2, dealer), slc(3, 5)  @ slc(14, 15) @ slc(23, 25))
        |> Player.PlayerMap.add(nextNPlayer(3, dealer), slc(6, 8)  @ slc(16, 17) @ slc(26, 28))
        |> Player.PlayerMap.add(nextNPlayer(4, dealer), slc(9, 11) @ slc(18, 19) @ slc(29, 31))
        ;
};
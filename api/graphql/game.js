const uuid = require('uuid/v4');
const { PubSub, withFilter } = require('graphql-subscriptions');
const { engine, format } = require('../engine');
const SPOT = format.consts.SPOT;
const CARD_COLOR = format.consts.CARD_COLOR;
const CARD_MOTIF = format.consts.CARD_MOTIF;
const gameStore = require('../store/gameStore');

const pubsub = new PubSub();
const GAME_UPDATE_TOPIC = 'GAME_TOPIC';
const publishGameChange = (game) => {
    pubsub.publish(GAME_UPDATE_TOPIC, {gameStateChanged: format.formatGame(game)});
};

// temp test game
(function(){
    const gameUuid = "test-game";
    let game = engine.createGame(gameUuid);
    game = engine.raiseErrorOrUnboxState(engine.joinGame('1', 'north', SPOT.NORTH, game));
    game = engine.raiseErrorOrUnboxState(engine.joinGame('2', 'east', SPOT.EAST, game));
    game = engine.raiseErrorOrUnboxState(engine.joinGame('3', 'south', SPOT.SOUTH, game));
    game = engine.raiseErrorOrUnboxState(engine.joinGame('4', 'west', SPOT.WEST, game));
    game = engine.raiseErrorOrUnboxState(engine.bid('1', 80, CARD_COLOR['HEARTS'], game));
    game = engine.raiseErrorOrUnboxState(engine.pass('2', game));
    game = engine.raiseErrorOrUnboxState(engine.pass('3', game));
    game = engine.raiseErrorOrUnboxState(engine.pass('4', game));
    gameStore.save(game);
})();

const schema = `
    enum Team {
        NORTH_SOUTH
        EAST_WEST
    }

    enum PlayerSpot {
      NORTH
      SOUTH
      EAST
      WEST
    }
    
    enum CardColor {
        SPADES
        HEARTS
        DIAMONDS 
        CLUBS
    }
    
    enum CardMotif {
        ACE
        V10
        KING
        QUEEN
        JACK
        V9
        V8
        V7
    }
    
    type Contract {
        player: Player!
        trump: CardColor!
        value: Int!
    }

    type Game {
        uuid: ID!
        players: [Player]!
        phase: String!
        bids: [Bid]!
        cards(playerUuid: ID!): [Card]!
        actionNeededFrom: Player
        table: [Card]!
        contract: Contract
    }
    
    type Bid {
        isPass: Boolean!
        value: Int
        color: CardColor
        player: Player!
    }
    
    type Card {
        color: CardColor!
        motif: CardMotif!
        canBePlayed(gameUuid: ID!, playerUuid: ID!): Boolean!
    }
    
    type Player {
        uuid: ID!
        name: String!
        spot: PlayerSpot!
        
        team: Team!
        game: Game!
        isDealer: Boolean!
        actionNeeded: Boolean!
        cards: [Card]!
    }
    
    type Query {
        game(uuid: ID!): Game!
    }
    
    type Mutation {
        createGame(name: String): Game!
        joinGame(gameUuid: ID!, playerUuid: ID, playerName: String!, spot: PlayerSpot!): Player!
        bid(gameUuid: ID!, playerUuid: ID!, value: Int!, color: CardColor!): Game!
        pass(gameUuid: ID!, playerUuid: ID!): Game!
        playCard(gameUuid: ID!, playerUuid: ID!, color: CardColor!, motif: CardMotif!): Game!
    }
    
    type Subscription {
        gameStateChanged(uuid: String!): Game!
    }
`;

const resolvers = {
    Player: {
        game: async (player) => {
            const game = await gameStore.get(player.gameUuid);
            return format.formatGame(game);
        },
        isDealer: async (player) => {
            const game = await gameStore.get(player.gameUuid);
            return engine.isDealer(player.uuid, game);
        },
        cards: async (player) => {
            const game = await gameStore.get(player.gameUuid);
            return format.formatPlayerCards(player.uuid, game);
        },
        actionNeeded: async (player) => {
            const game = await gameStore.get(player.gameUuid);
            return engine.isPlayerActionNeeded(player.uuid, game);
        },
    },
    Bid: {
        player: async (bid) => {
            const { gameUuid, playerUuid } = bid;
            const gameObj = await gameStore.get(gameUuid);
            const [spot, player] = engine.getPlayerAndSpot(playerUuid, gameObj);
            return format.formatPlayer(gameUuid, spot, player);
        },
    },
    Card: {
        canBePlayed: async (card, args) => {
            const { gameUuid, playerUuid } = args;
            const { color, motif } = card;
            let gameObj = await gameStore.get(gameUuid);

            const res = engine.canCardBePlayed(playerUuid, CARD_COLOR[color], CARD_MOTIF[motif], gameObj);
            return res;
        },
    },
    Game: {
        bids: async (game) => {
            const gameObj = await gameStore.get(game.uuid);
            return format.formatBids(gameObj);
        },
        players: async (game) => {
            const gameObj = await gameStore.get(game.uuid);
            return format.formatPlayers(gameObj);
        },
        cards: async (game, args) => {
            const gameObj  = await gameStore.get(game.uuid);
            return format.formatPlayerCards(args.playerUuid, gameObj);
        },
        actionNeededFrom: async (game) => {
            const gameObj  = await gameStore.get(game.uuid);
            const res = format.formatOptional(engine.getActionNeededFromPlayerAndSpot(gameObj));
            if (res === null) {
                return null;
            }
            const [spot, player] = res;
            return format.formatPlayer(game.uuid, spot, player);
        },
        table: async (game) => {
            const gameObj  = await gameStore.get(game.uuid);
            return format.formatCards(engine.getTableCards(gameObj));
        },
    },
    Query: {
        game: async(ctx, args) => {
            const game = await gameStore.get(args.uuid);
            // @todo: hide sensitive info (other players cards, deck)
            return format.formatGame(game);
        },
    },
    Mutation: {
        createGame: async() => {
            const game = engine.createGame(uuid());
            // @todo: limit number of playing games
            await gameStore.save(game);
            publishGameChange(game);
            return format.formatGame(game)
        },
        joinGame: async(ctx, args) => {
            // @todo: input check
            const { gameUuid, playerName, spot } = args;
            const playerUuid = args.playerUuid || uuid();
            let gameObj = await gameStore.get(gameUuid);
            const gameSpot = SPOT[spot];

            const res = engine.joinGame(playerUuid, playerName, gameSpot, gameObj);
            gameObj = engine.raiseErrorOrUnboxState(res);

            await gameStore.save(gameObj);
            publishGameChange(gameObj);

            // return the new player object
            const [newSpot, player] = engine.getPlayerAndSpot(playerUuid, gameObj);
            return format.formatPlayer(gameUuid, newSpot, player);
        },
        bid: async(ctx, args) => {
            // @todo: input check
            const { gameUuid, playerUuid, value, color} = args;
            let gameObj = await gameStore.get(gameUuid);
            const gameColor = CARD_COLOR[color];

            const res = engine.bid(playerUuid, value, gameColor, gameObj);
            gameObj = engine.raiseErrorOrUnboxState(res);

            await gameStore.save(gameObj);
            publishGameChange(gameObj);

            return format.formatGame(gameObj);
        },
        pass: async(ctx, args) => {
            // @todo: input check
            const { gameUuid, playerUuid } = args;
            let gameObj = await gameStore.get(gameUuid);

            const res = engine.pass(playerUuid, gameObj);
            gameObj = engine.raiseErrorOrUnboxState(res);

            await gameStore.save(gameObj);
            publishGameChange(gameObj);

            return format.formatGame(gameObj);
        },
        playCard: async(ctx, args) => {
            // @todo: input check
            const { gameUuid, playerUuid, color, motif } = args;
            let gameObj = await gameStore.get(gameUuid);

            const res = engine.playCard(playerUuid, CARD_COLOR[color], CARD_MOTIF[motif], gameObj);
            gameObj = engine.raiseErrorOrUnboxState(res);

            await gameStore.save(gameObj);
            publishGameChange(gameObj);

            return format.formatGame(gameObj);
        }
    },
    Subscription: {
        gameStateChanged: {
            subscribe: withFilter(
                () => pubsub.asyncIterator(GAME_UPDATE_TOPIC),
                (payload, variables) => {
                    const game = payload.gameStateChanged;
                    return game.uuid === variables.uuid;
                }
            )
        }
    },
}

module.exports = { schema, resolvers }

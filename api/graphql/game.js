const uuid = require('uuid/v4');
const { PubSub, withFilter } = require('graphql-subscriptions');
const { engine, format } = require('../engine');
const SPOT = format.consts.SPOT;
const CARD_COLOR = format.consts.CARD_COLOR;
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
    game = engine.raiseErrorOrUnboxState(engine.joinGame('1', 'n', SPOT.WEST, game));
    game = engine.raiseErrorOrUnboxState(engine.joinGame('2', 's', SPOT.SOUTH, game));
    game = engine.raiseErrorOrUnboxState(engine.joinGame('3', 'e', SPOT.EAST, game));
    gameStore.save(game);
})();

const schema = `
    enum PlayerSpot {
      NORTH
      SOUTH
      EAST
      WEST
    }

    type Game {
        uuid: String!
        players: [Player]!
        phase: String!
        bids: [Bid]!
    }
    
    enum CardColor {
        SPADES
        HEARTS
        DIAMONDS 
        CLUBS
    }
    
    type Bid {
        isPass: Boolean!
        value: Int
        color: CardColor
        player: Player!
    }
    
    type Card {
        color: CardColor!
        motif: Int!
    }
    
    type Player {
        uuid: String!
        name: String!
        spot: PlayerSpot!
        
        game: Game!
        isDealer: Boolean!
        cards: [Card]!
    }
    
    type Query {
        game(uuid: String!): Game!
    }
    
    type Mutation {
        createGame(name: String): Game!
        joinGame(gameUuid: String!, playerUuid: String, playerName: String!, spot: PlayerSpot!): Player!
        bid(gameUuid: String!, playerUuid: String, value: Int!, color: CardColor!): Game!
        pass(gameUuid: String!, playerUuid: String): Game!
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
            const cards = engine.getCards(player.uuid, game);
            return format.formatCards(cards);
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

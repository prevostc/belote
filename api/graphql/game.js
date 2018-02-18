const uuid = require('uuid/v4');
const { PubSub, withFilter } = require('graphql-subscriptions');
const { engine, format } = require('../engine');
const SPOT = format.consts.SPOT;
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
    game = engine.raiseErrorOrUnboxState(engine.joinGame('1', 'n', SPOT.NORTH, game));
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
    }
    
    enum CardColor {
        SPADES
        HEARTS
        DIAMONDS 
        CLUBS
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
            const gameUuid = args.gameUuid;
            let gameObj = await gameStore.get(gameUuid);
            const playerUuid = args.playerUuid || uuid();
            const gameSpot = SPOT[args.spot];
            if (gameSpot === undefined) {
                throw new Error('Invalid spot');
            }
            // @todo: input check
            const res = engine.joinGame(playerUuid, args.playerName, gameSpot, gameObj);
            gameObj = engine.raiseErrorOrUnboxState(res);

            // @todo: detect if game changed to save and publish only when necessary
            await gameStore.save(gameObj);
            publishGameChange(gameObj);

            // return the new player object
            const [spot, player] = engine.getPlayerAndSpot(playerUuid, gameObj);
            return format.formatPlayer(gameUuid, spot, player);
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

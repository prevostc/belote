const uuid = require('uuid/v4');
const { PubSub, withFilter } = require('graphql-subscriptions');
const engine = require('../engine');
const gameStore = require('../store/gameStore');

const pubsub = new PubSub();
const GAME_UPDATE_TOPIC = 'GAME_TOPIC';
const publishGameChange = (game) => {
    pubsub.publish(GAME_UPDATE_TOPIC, {gameStateChanged: gameFormatter(game)});
};

// temp test game
(function(){
    const gameUuid = "test-game";
    let game = engine.createGame(gameUuid);
    [p, game] = engine.joinGame(game, '1', 'n', "NORTH");
    [p, game] = engine.joinGame(game, '2', 's', "SOUTH");
    [p, game] = engine.joinGame(game, '3', 'e', "EAST");
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
        gameState: String!
        players: [Player]!
    }
    
    type Player {
        uuid: String!
        name: String!
        spot: PlayerSpot!
        game: Game!
    }
    
    type Query {
        game(uuid: String!): Game!
    }
    
    type Mutation {
        createGame(name: String): Game!
        joinGame(gameUuid: String!, playerUuid: String, playerName: String!, spot: PlayerSpot!): Player
    }
    
    type Subscription {
        gameStateChanged(uuid: String!): Game!
    }
`

const gameFormatter = game => ({ ...game, gameState: JSON.stringify(game.gameState)});

const resolvers = {
    Player: {
        game: async (player) => {
            const game = await gameStore.get(player.gameUuid);
            return gameFormatter(game);
        }
    },
    Query: {
        game: async(ctx, args) => {
            const game = await gameStore.get(args.uuid);
            // @todo: hide sensitive info (other players cards, deck)
            return gameFormatter(game);
        },
    },
    Mutation: {
        createGame: async() => {
            const game = engine.createGame(uuid());
            // @todo: limit number of playing games
            await gameStore.save(game);
            publishGameChange(game);
            return gameFormatter(game)
        },
        joinGame: async(ctx, args) => {
            let gameObj = await gameStore.get(args.gameUuid);
            const playerUuid = args.playerUuid || uuid();
            // @todo: input check
            const [ player, game ] = engine.joinGame(gameObj, playerUuid, args.playerName, args.spot);
            // @todo: detect if game changed to save and publish only when necessary
            await gameStore.save(game);
            publishGameChange(game);
            // already a player there
            return player.uuid === playerUuid ? player : null;
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

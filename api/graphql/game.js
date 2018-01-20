const uuid = require('uuid/v4');
const { PubSub, withFilter } = require('graphql-subscriptions');
const engine = require('../engine');
const gameStore = require('../store/gameStore');

const pubsub = new PubSub();
const GAME_UPDATE_TOPIC = 'GAME_TOPIC';
const publishGameChange = (game) => {
    pubsub.publish(GAME_UPDATE_TOPIC, {gameStateChanged: gameFormatter(game)});
};

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
        joinGame(gameUuid: String!, playerName: String!, spot: PlayerSpot!): Player!
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
            // @todo: input check
            const [ player, game ] = engine.joinGame(gameObj, uuid(), args.playerName, args.spot);
            await gameStore.save(game);
            publishGameChange(game);
            return player;
        }
    },
    Subscription: {
        gameStateChanged: {
            subscribe: withFilter(
                () => pubsub.asyncIterator(GAME_UPDATE_TOPIC),
                (payload, variables) => {
                    const game = payload.gameStateChanged;
                    console.log('game.uuid === variables.uuid', game.uuid === variables.uuid);
                    return game.uuid === variables.uuid;
                }
            )
        }
    },
}

module.exports = { schema, resolvers }
